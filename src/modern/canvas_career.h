#ifndef CANVAS_CAREER_H
#define CANVAS_CAREER_H

/* The squadron's books: the parts of the campaign that outlive one sortie,
 * kept apart from the game so the host tests can read and write them too.
 * Nothing here knows about SDL or the screen; it is plain arithmetic and
 * plain text, as the adjutant would keep it. */

#include <stdio.h>
#include <string.h>

/* ---- The flight's health ------------------------------------------------
 *
 * Eccles, Gridpipe Thyne and Bluebottle each carry a health from 0 to 100
 * from one sortie to the next.  At the end of every sortie, flown with
 * them or not, each man first has the rest he has had since the last one
 * (WINGMAN_REST, never above 100), and then the day's damage is taken off:
 * WINGMAN_HIT_COST for every round that struck his machine, and a man sent
 * home wounded goes to the sick bay at no better than WINGMAN_WOUNDED.  A
 * wingman below WINGMAN_FIT is not in the flight that morning.  So a man
 * shot out of the fight sits out exactly one sortie and is back for the
 * one after, a little the worse for it.
 *
 * Nought means lost, and only the written story loses a man: Gridpipe
 * Thyne after his last turn.  The arithmetic of the war never takes a man
 * below one, so a pilot in the sick bay is never mistaken for one lost. */
#define WINGMAN_FULL 100
#define WINGMAN_REST 25
#define WINGMAN_FIT 35
#define WINGMAN_HIT_COST 20
#define WINGMAN_WOUNDED 10

static int career_wingman_after_sortie(int health, int hits, int wounded)
{
    if (health <= 0) return 0;      /* lost: the story's, not ours */
    health += WINGMAN_REST;
    if (health > WINGMAN_FULL) health = WINGMAN_FULL;
    if (hits > 0) health -= hits * WINGMAN_HIT_COST;
    if (wounded && health > WINGMAN_WOUNDED) health = WINGMAN_WOUNDED;
    if (health < 1) health = 1;
    return health;
}

static int career_wingman_fit(int health)
{
    return health >= WINGMAN_FIT;
}

/* The written story on top of the arithmetic.  Gridpipe Thyne is gone
 * once his last turn is behind us; before it he is at his place, so a
 * chapter begun again after the last life brings him back as the story
 * has him.  Nobody else can be lost, so a nought anywhere else (which only
 * a damaged or hand-edited save could hold) is read as a man fit again.
 * Health the war has left them with is otherwise kept exactly. */
static void career_story_wingmen(int health[3], int thyne_gone)
{
    int pilot;
    for (pilot = 0; pilot < 3; ++pilot)
        if (health[pilot] <= 0) health[pilot] = WINGMAN_FULL;
    if (thyne_gone) health[1] = 0;
}

/* ---- The order of sorties ------------------------------------------------
 *
 * After a sortie that did its work the war moves to the next one, and on
 * past it when he is grounded: the squadron flies that sortie without him,
 * so it is struck off his book as well.  Grounding never strikes off the
 * last sortie of a chapter; it waits, as it always has, for the next
 * sortie that can be flown without him.  The same rule decides both what
 * the save says comes next and where the evening's diary turns to, so the
 * two can never disagree. */
enum { CAREER_NEXT_MISSION, CAREER_NEXT_CHAPTER, CAREER_WAR_OVER };

static int career_advance(int *chapter, int *mission, int *complete,
                          int *grounded, int chapters, int missions)
{
    if (*mission + 1 < missions) {
        ++*mission;
        if (*grounded && *mission + 1 < missions) {
            ++*mission;
            *grounded = 0;
        }
        return CAREER_NEXT_MISSION;
    }
    if (*chapter + 1 < chapters) {
        ++*chapter;
        *mission = 0;
        return CAREER_NEXT_CHAPTER;
    }
    *complete = 1;
    return CAREER_WAR_OVER;
}

/* ---- The save, as text ---------------------------------------------------
 *
 * One record of the campaign, line by line, each version adding to the
 * end of the one before so that an old save is always the first part of
 * a new one:
 *
 *   CANVAS_SKIES_SAVE 6
 *   chapter mission
 *   eccles thyne bluebottle            (health, 0..100)
 *   complete                           (version 2)
 *   lives                              (version 3)
 *   victories sorties awards rank fly gun engine   (version 4)
 *   morale offences hold penalty disgraced         (version 5)
 *   grounded                           (version 6)
 *
 * A save older than a field gives that field its peacetime value: three
 * lives, a clean record, a cheerful mess and nobody grounded.  The health
 * of the flight is in every version; before version 6 it only ever held
 * what the story said (all fit, Thyne lost late in 1917), which is just
 * what a fresh flight would have, so it is taken as it stands. */
#define CAREER_SAVE_MAGIC "CANVAS_SKIES_SAVE"
#define CAREER_SAVE_VERSION 6

typedef struct {
    int version;            /* as read; a write is always the newest */
    int chapter, mission;
    int wingman[3];
    int complete, lives;
    int victories, sorties;
    unsigned awards;
    int rank;
    int skill[3];           /* flying, gunnery, engine, in hundredths */
    int conduct[5];         /* morale, offences, hold, penalty, disgraced */
    int grounded;           /* a grounding still owed to the next sortie */
} CareerRecord;

static void career_record_defaults(CareerRecord *r)
{
    memset(r, 0, sizeof(*r));
    r->version = CAREER_SAVE_VERSION;
    r->wingman[0] = r->wingman[1] = r->wingman[2] = WINGMAN_FULL;
    r->lives = 3;
    r->conduct[0] = 70;
}

/* Reads a record and checks every field against what the game can hold.
 * Anything out of order refuses the whole record, so the caller can fall
 * back to the .bak rather than continue a war from nonsense. */
static int career_record_read(FILE *file, CareerRecord *out, int chapters,
                              int missions)
{
    CareerRecord r;
    char magic[24];
    int *c = r.conduct;
    career_record_defaults(&r);
    if (fscanf(file, "%23s %d\n%d %d\n%d %d %d", magic, &r.version,
               &r.chapter, &r.mission, &r.wingman[0], &r.wingman[1],
               &r.wingman[2]) != 7 ||
        strcmp(magic, CAREER_SAVE_MAGIC) != 0 ||
        r.version < 1 || r.version > CAREER_SAVE_VERSION ||
        r.chapter < 0 || r.chapter >= chapters ||
        r.mission < 0 || r.mission >= missions ||
        r.wingman[0] < 0 || r.wingman[0] > 100 ||
        r.wingman[1] < 0 || r.wingman[1] > 100 ||
        r.wingman[2] < 0 || r.wingman[2] > 100)
        return 0;
    if (r.version >= 2 &&
        (fscanf(file, "%d", &r.complete) != 1 ||
         (r.complete != 0 && r.complete != 1)))
        return 0;
    if (r.version >= 3 &&
        (fscanf(file, "%d", &r.lives) != 1 || r.lives < 0 || r.lives > 3))
        return 0;
    /* Version 4 adds the pilot's record.  Older saves start it at zero. */
    if (r.version >= 4 &&
        (fscanf(file, "%d %d %u %d %d %d %d", &r.victories, &r.sorties,
                &r.awards, &r.rank, &r.skill[0], &r.skill[1],
                &r.skill[2]) != 7 ||
         r.victories < 0 || r.sorties < 0 || r.rank < 0 || r.rank > 3 ||
         r.skill[0] < 0 || r.skill[0] > 100 ||
         r.skill[1] < 0 || r.skill[1] > 100 ||
         r.skill[2] < 0 || r.skill[2] > 100))
        return 0;
    /* Version 5 adds conduct.  Older saves start with a cheerful mess. */
    if (r.version >= 5 &&
        (fscanf(file, "%d %d %d %d %d", &c[0], &c[1], &c[2], &c[3],
                &c[4]) != 5 ||
         c[0] < 0 || c[0] > 100 || c[1] < 0 || c[1] > 3 || c[2] < 0 ||
         c[3] < 0 || c[3] > 3 || c[4] < 0 || c[4] > 1))
        return 0;
    /* Version 6 adds a grounding not yet served.  Older saves owe none. */
    if (r.version >= 6 &&
        (fscanf(file, "%d", &r.grounded) != 1 ||
         (r.grounded != 0 && r.grounded != 1)))
        return 0;
    *out = r;
    return 1;
}

/* Writes the newest version.  Returns 0 if the text did not go out. */
static int career_record_write(FILE *file, const CareerRecord *r)
{
    return fprintf(file, CAREER_SAVE_MAGIC " %d\n%d %d\n%d %d %d\n%d\n%d\n"
                   "%d %d %u %d %d %d %d\n%d %d %d %d %d\n%d\n",
                   CAREER_SAVE_VERSION, r->chapter, r->mission,
                   r->wingman[0], r->wingman[1], r->wingman[2],
                   r->complete, r->lives,
                   r->victories, r->sorties, r->awards, r->rank,
                   r->skill[0], r->skill[1], r->skill[2],
                   r->conduct[0], r->conduct[1], r->conduct[2],
                   r->conduct[3], r->conduct[4], r->grounded) > 0;
}

#endif
