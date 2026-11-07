/* Host checks of the campaign's books (src/modern/canvas_career.h): the
 * save's text, old saves read into the new one, the wingmen's wounds and
 * rest, and a grounding that must survive a quit after the debrief.
 * Everything is deterministic; nothing touches the real save. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "canvas_career.h"

#define CHAPTERS 8
#define MISSIONS 12

static int failures, checks;

#define CHECK(cond) do { \
    ++checks; \
    if (!(cond)) { \
        ++failures; \
        fprintf(stderr, "%s:%d: FAILED: %s\n", __FILE__, __LINE__, #cond); \
    } \
} while (0)

/* Writes text to a scratch file and reads it back as a record. */
static int read_text(const char *text, CareerRecord *r)
{
    FILE *f = tmpfile();
    int ok;
    if (!f) { perror("tmpfile"); exit(2); }
    fputs(text, f);
    rewind(f);
    ok = career_record_read(f, r, CHAPTERS, MISSIONS);
    fclose(f);
    return ok;
}

/* Writes a record and reads it back, as save_campaign and Continue do. */
static int round_trip(const CareerRecord *in, CareerRecord *out, char *text,
                      size_t size)
{
    FILE *f = tmpfile();
    size_t n;
    int ok;
    if (!f) { perror("tmpfile"); exit(2); }
    ok = career_record_write(f, in);
    rewind(f);
    n = fread(text, 1, size - 1, f);
    text[n] = '\0';
    rewind(f);
    ok = ok && career_record_read(f, out, CHAPTERS, MISSIONS);
    fclose(f);
    return ok;
}

static int same_campaign(const CareerRecord *a, const CareerRecord *b)
{
    return a->chapter == b->chapter && a->mission == b->mission &&
           memcmp(a->wingman, b->wingman, sizeof(a->wingman)) == 0 &&
           a->complete == b->complete && a->lives == b->lives &&
           a->victories == b->victories && a->sorties == b->sorties &&
           a->awards == b->awards && a->rank == b->rank &&
           memcmp(a->skill, b->skill, sizeof(a->skill)) == 0 &&
           memcmp(a->conduct, b->conduct, sizeof(a->conduct)) == 0 &&
           a->grounded == b->grounded;
}

static void test_round_trip(void)
{
    CareerRecord in, out;
    char text[512];
    career_record_defaults(&in);
    in.chapter = 4;
    in.mission = 7;
    in.wingman[0] = 10;
    in.wingman[1] = 55;
    in.wingman[2] = 100;
    in.lives = 2;
    in.victories = 14;
    in.sorties = 51;
    in.awards = 0x7u;
    in.rank = 2;
    in.skill[0] = 61; in.skill[1] = 48; in.skill[2] = 77;
    in.conduct[0] = 55; in.conduct[1] = 2; in.conduct[2] = 1;
    in.conduct[3] = 0; in.conduct[4] = 0;
    in.grounded = 1;
    CHECK(round_trip(&in, &out, text, sizeof(text)));
    CHECK(out.version == CAREER_SAVE_VERSION);
    CHECK(same_campaign(&in, &out));
    CHECK(strncmp(text, "CANVAS_SKIES_SAVE 6\n4 7\n10 55 100\n", 34) == 0);
    /* The new field is the last line, so a v5 reader's fields come first. */
    CHECK(strcmp(text + strlen(text) - 3, "\n1\n") == 0);
}

static void test_migration(void)
{
    /* A save exactly as the 0.2.0 RC5 game wrote it, mid-1917 with
     * Thyne lost later in the year still at his place. */
    static const char v5[] =
        "CANVAS_SKIES_SAVE 5\n3 4\n100 100 100\n0\n2\n"
        "9 40 3 1 50 40 60\n62 1 2 0 0\n";
    /* And after his last turn, from a save of 1918. */
    static const char v5_late[] =
        "CANVAS_SKIES_SAVE 5\n5 6\n100 0 100\n0\n3\n"
        "22 70 15 2 80 70 90\n70 0 0 0 0\n";
    static const char v1[] = "CANVAS_SKIES_SAVE 1\n2 3\n100 100 100\n";
    static const char v3[] = "CANVAS_SKIES_SAVE 3\n0 11\n100 100 100\n1\n1\n";
    CareerRecord r, again;
    char text[512];

    CHECK(read_text(v5, &r));
    CHECK(r.version == 5);
    CHECK(r.chapter == 3 && r.mission == 4);
    CHECK(r.wingman[0] == 100 && r.wingman[1] == 100 && r.wingman[2] == 100);
    CHECK(r.lives == 2 && r.victories == 9 && r.sorties == 40);
    CHECK(r.awards == 3u && r.rank == 1);
    CHECK(r.skill[0] == 50 && r.skill[1] == 40 && r.skill[2] == 60);
    CHECK(r.conduct[0] == 62 && r.conduct[1] == 1 && r.conduct[2] == 2);
    CHECK(r.grounded == 0);
    /* Saved again it is version 6 and otherwise the same war. */
    CHECK(round_trip(&r, &again, text, sizeof(text)));
    CHECK(again.version == 6);
    CHECK(same_campaign(&r, &again));
    CHECK(strncmp(text, "CANVAS_SKIES_SAVE 6\n", 20) == 0);

    CHECK(read_text(v5_late, &r));
    CHECK(r.wingman[1] == 0 && r.grounded == 0);
    career_story_wingmen(r.wingman, 1);
    CHECK(r.wingman[0] == 100 && r.wingman[1] == 0 && r.wingman[2] == 100);

    /* The oldest saves take every later field at its peacetime value. */
    CHECK(read_text(v1, &r));
    CHECK(r.version == 1 && r.chapter == 2 && r.mission == 3);
    CHECK(r.complete == 0 && r.lives == 3 && r.victories == 0);
    CHECK(r.conduct[0] == 70 && r.conduct[4] == 0 && r.grounded == 0);
    CHECK(read_text(v3, &r));
    CHECK(r.version == 3 && r.complete == 1 && r.lives == 1);
    CHECK(r.grounded == 0);
}

static void test_refusals(void)
{
    CareerRecord r;
    /* A version from the future, a grounding of two, a v6 cut short
     * before its last line, health over the top, and a stranger's file:
     * each is refused whole, so the game falls back to the .bak. */
    CHECK(!read_text("CANVAS_SKIES_SAVE 7\n0 0\n100 100 100\n0\n3\n"
                     "0 0 0 0 0 0 0\n70 0 0 0 0\n0\n", &r));
    CHECK(!read_text("CANVAS_SKIES_SAVE 6\n0 0\n100 100 100\n0\n3\n"
                     "0 0 0 0 0 0 0\n70 0 0 0 0\n2\n", &r));
    CHECK(!read_text("CANVAS_SKIES_SAVE 6\n0 0\n100 100 100\n0\n3\n"
                     "0 0 0 0 0 0 0\n70 0 0 0 0\n", &r));
    CHECK(!read_text("CANVAS_SKIES_SAVE 6\n0 0\n101 100 100\n0\n3\n"
                     "0 0 0 0 0 0 0\n70 0 0 0 0\n0\n", &r));
    CHECK(!read_text("CANVAS_SKIES_SAVE 6\n8 0\n100 100 100\n0\n3\n"
                     "0 0 0 0 0 0 0\n70 0 0 0 0\n0\n", &r));
    CHECK(!read_text("SOPWITH_SAVE 6\n0 0\n100 100 100\n", &r));
    CHECK(!read_text("", &r));
}

static void test_wingmen(void)
{
    int h, sorties;
    /* Rest is +25 and never above 100. */
    CHECK(career_wingman_after_sortie(100, 0, 0) == 100);
    CHECK(career_wingman_after_sortie(60, 0, 0) == 85);
    CHECK(career_wingman_after_sortie(90, 0, 0) == 100);
    /* Rest, then the day's damage. */
    CHECK(career_wingman_after_sortie(100, 1, 0) == 80);
    CHECK(career_wingman_after_sortie(100, 2, 0) == 60);
    CHECK(career_wingman_after_sortie(40, 2, 0) == 25);
    /* Sent home wounded: the sick bay, at no better than 10. */
    CHECK(career_wingman_after_sortie(100, 3, 1) == 10);
    CHECK(career_wingman_after_sortie(5, 3, 1) == 1);
    /* The war never takes a man to nought; only the story does. */
    CHECK(career_wingman_after_sortie(10, 5, 0) == 1);
    CHECK(career_wingman_after_sortie(0, 0, 0) == 0);
    CHECK(career_wingman_after_sortie(0, 3, 1) == 0);
    /* The threshold. */
    CHECK(!career_wingman_fit(0));
    CHECK(!career_wingman_fit(1));
    CHECK(!career_wingman_fit(34));
    CHECK(career_wingman_fit(35));
    CHECK(career_wingman_fit(100));

    /* Shot out of the fight, he sits out exactly one sortie. */
    h = career_wingman_after_sortie(100, 3, 1);
    CHECK(!career_wingman_fit(h));                 /* tomorrow: on the ground */
    h = career_wingman_after_sortie(h, 0, 0);      /* the sortie he missed */
    CHECK(h == 35 && career_wingman_fit(h));       /* the day after: flying */
    h = career_wingman_after_sortie(h, 0, 0);
    CHECK(h == 60);
    /* And a full recovery from the sick bay takes four sorties. */
    h = 10;
    for (sorties = 0; h < 100; ++sorties)
        h = career_wingman_after_sortie(h, 0, 0);
    CHECK(sorties == 4);
    /* A man back too soon and hit again goes straight back to bed. */
    h = career_wingman_after_sortie(35, 2, 0);
    CHECK(h == 20 && !career_wingman_fit(h));
}

static void test_story(void)
{
    int w[3] = {47, 0, 100};
    /* Before his last turn Thyne is at his place; others keep their wounds. */
    career_story_wingmen(w, 0);
    CHECK(w[0] == 47 && w[1] == 100 && w[2] == 100);
    w[1] = 62;
    career_story_wingmen(w, 0);
    CHECK(w[1] == 62);
    /* After it he is gone, however well he was. */
    career_story_wingmen(w, 1);
    CHECK(w[0] == 47 && w[1] == 0 && w[2] == 100);
    /* A nought elsewhere is only ever a bad save: read as fit again. */
    w[0] = 0;
    career_story_wingmen(w, 1);
    CHECK(w[0] == 100 && w[1] == 0);
}

/* The debrief's save, as save_mission_result makes it: the continuation
 * point moved on (on a success) by career_advance, then written. */
static void debrief_save(CareerRecord *r, int success, CareerRecord *loaded)
{
    char text[512];
    if (success &&
        career_advance(&r->chapter, &r->mission, &r->complete, &r->grounded,
                       CHAPTERS, MISSIONS) == CAREER_NEXT_CHAPTER)
        r->lives = 3;
    CHECK(round_trip(r, loaded, text, sizeof(text)));
}

static void test_grounding(void)
{
    CareerRecord r, loaded;
    int ch, m, done, g;

    /* "Fog on the Somme" (1918, mission 1), a second offence: grounded.
     * The debrief saves, he quits, and Continue must open at "Westward"
     * (mission 3) with "Burn the Stores" struck off and nothing owed. */
    career_record_defaults(&r);
    r.chapter = 5;
    r.mission = 0;
    r.grounded = 1;
    r.conduct[1] = 2;
    debrief_save(&r, 1, &loaded);
    CHECK(loaded.chapter == 5 && loaded.mission == 2);
    CHECK(loaded.grounded == 0);
    /* The evening, seen through, turns the diary to the same page. */
    ch = 5; m = 0; done = 0; g = 1;
    CHECK(career_advance(&ch, &m, &done, &g, CHAPTERS, MISSIONS) ==
          CAREER_NEXT_MISSION);
    CHECK(ch == loaded.chapter && m == loaded.mission && g == loaded.grounded);

    /* Not grounded: simply the next sortie. */
    career_record_defaults(&r);
    r.chapter = 5;
    r.mission = 0;
    debrief_save(&r, 1, &loaded);
    CHECK(loaded.mission == 1 && loaded.grounded == 0);

    /* Grounded on a sortie that failed: Retry is the same sortie, and the
     * grounding is still owed after a quit, struck off the next success. */
    career_record_defaults(&r);
    r.chapter = 2;
    r.mission = 4;
    r.grounded = 1;
    debrief_save(&r, 0, &loaded);
    CHECK(loaded.chapter == 2 && loaded.mission == 4 && loaded.grounded == 1);
    r = loaded;
    debrief_save(&r, 1, &loaded);
    CHECK(loaded.mission == 6 && loaded.grounded == 0);

    /* Grounded before a chapter's last sortie: that one is never struck
     * off; the grounding waits, saved, into the next chapter. */
    career_record_defaults(&r);
    r.chapter = 3;
    r.mission = 9;
    r.grounded = 1;
    debrief_save(&r, 1, &loaded);
    CHECK(loaded.chapter == 3 && loaded.mission == 11 && loaded.grounded == 0);
    career_record_defaults(&r);
    r.chapter = 3;
    r.mission = 10;
    r.grounded = 1;
    r.lives = 1;
    debrief_save(&r, 1, &loaded);
    CHECK(loaded.chapter == 3 && loaded.mission == 11 && loaded.grounded == 1);
    r = loaded;
    debrief_save(&r, 1, &loaded);
    CHECK(loaded.chapter == 4 && loaded.mission == 0 && loaded.grounded == 1);
    CHECK(loaded.lives == 3);
    r = loaded;
    debrief_save(&r, 1, &loaded);
    CHECK(loaded.chapter == 4 && loaded.mission == 2 && loaded.grounded == 0);

    /* The last sortie of the war ends the war, grounded or not. */
    ch = CHAPTERS - 1; m = MISSIONS - 1; done = 0; g = 1;
    CHECK(career_advance(&ch, &m, &done, &g, CHAPTERS, MISSIONS) ==
          CAREER_WAR_OVER);
    CHECK(done == 1 && ch == CHAPTERS - 1 && m == MISSIONS - 1);
}

int main(void)
{
    test_round_trip();
    test_migration();
    test_refusals();
    test_wingmen();
    test_story();
    test_grounding();
    printf("career: %d checks, %d failed\n", checks, failures);
    return failures ? 1 : 0;
}
