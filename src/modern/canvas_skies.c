/*
 * Canvas Skies
 *
 * An original cinematic WWI side-scrolling shooter prototype.  The flight
 * premise owes a debt to Sopwith and Scramble, but all presentation, code and
 * generated artwork in this mode are new.  The renderer deliberately uses an
 * SDL window surface so it exercises the same reliable software-to-OpenGPU
 * presentation path as other AmigaChrome ports.
 */

#include <SDL.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "canvas_campaign.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef CANVAS_TITLE_BMP
#define CANVAS_TITLE_BMP "assets/canvas-skies/title.bmp"
#endif
#ifndef CANVAS_TITLE_BANNER_BMP
#define CANVAS_TITLE_BANNER_BMP "assets/canvas-skies/title-banner.bmp"
#endif
#ifndef CANVAS_BOARD_BMP
#define CANVAS_BOARD_BMP "assets/canvas-skies/mission-board-v2.bmp"
#endif
#ifndef CANVAS_FATAL_MEMORIAL_BMP
#define CANVAS_FATAL_MEMORIAL_BMP "assets/canvas-skies/fatal-memorial-wide.bmp"
#endif
#ifndef CANVAS_FIELD_HOSPITAL_BMP
#define CANVAS_FIELD_HOSPITAL_BMP "assets/canvas-skies/field-hospital.bmp"
#endif
#ifndef CANVAS_JOURNAL_BMP
#define CANVAS_JOURNAL_BMP "assets/canvas-skies/journal.bmp"
#endif
#ifndef CANVAS_SKY_BMP
#define CANVAS_SKY_BMP "assets/canvas-skies/sky.bmp"
#endif
#ifndef CANVAS_BATTLEFIELD_BMP
#define CANVAS_BATTLEFIELD_BMP "assets/canvas-skies/hills.bmp"
#endif
#ifndef CANVAS_FOREGROUND_BMP
#define CANVAS_FOREGROUND_BMP "assets/canvas-skies/foreground-panorama.bmp"
#endif
#ifndef CANVAS_MID_FARM_BMP
#define CANVAS_MID_FARM_BMP "assets/canvas-skies/mid-farm.bmp"
#endif
#ifndef CANVAS_MID_CHURCH_BMP
#define CANVAS_MID_CHURCH_BMP "assets/canvas-skies/mid-church.bmp"
#endif
#ifndef CANVAS_MID_POPLARS_BMP
#define CANVAS_MID_POPLARS_BMP "assets/canvas-skies/mid-poplars.bmp"
#endif
#ifndef CANVAS_MID_SMOKE_BMP
#define CANVAS_MID_SMOKE_BMP "assets/canvas-skies/mid-smoke.bmp"
#endif
#ifndef CANVAS_MID_TELEGRAPH_BMP
#define CANVAS_MID_TELEGRAPH_BMP "assets/canvas-skies/mid-telegraph.bmp"
#endif
#ifndef CANVAS_MID_WALL_BMP
#define CANVAS_MID_WALL_BMP "assets/canvas-skies/mid-wall.bmp"
#endif
#ifndef CANVAS_NEAR_CRATER_BMP
#define CANVAS_NEAR_CRATER_BMP "assets/canvas-skies/near-crater.bmp"
#endif
#ifndef CANVAS_NEAR_TRENCH_BMP
#define CANVAS_NEAR_TRENCH_BMP "assets/canvas-skies/near-trench.bmp"
#endif
#ifndef CANVAS_NEAR_TREE_BMP
#define CANVAS_NEAR_TREE_BMP "assets/canvas-skies/near-tree.bmp"
#endif
#ifndef CANVAS_NEAR_WIRE_BMP
#define CANVAS_NEAR_WIRE_BMP "assets/canvas-skies/near-wire.bmp"
#endif
#ifndef CANVAS_NEAR_ROAD_BMP
#define CANVAS_NEAR_ROAD_BMP "assets/canvas-skies/near-road.bmp"
#endif
#ifndef CANVAS_NEAR_MOUND_BMP
#define CANVAS_NEAR_MOUND_BMP "assets/canvas-skies/near-mound.bmp"
#endif
#ifndef CANVAS_MACHINE_GUN_BMP
#define CANVAS_MACHINE_GUN_BMP "assets/canvas-skies/ground-machine-gun.bmp"
#endif
#ifndef CANVAS_AA_GUN_BMP
#define CANVAS_AA_GUN_BMP "assets/canvas-skies/ground-aa-gun.bmp"
#endif
#ifndef CANVAS_TOPDOWN_BATTLEFIELD_BMP
#define CANVAS_TOPDOWN_BATTLEFIELD_BMP "assets/canvas-skies/topdown-battlefield.bmp"
#endif
#ifndef CANVAS_TOPDOWN_PLAYER_BMP
#define CANVAS_TOPDOWN_PLAYER_BMP "assets/canvas-skies/topdown-player.bmp"
#endif
#ifndef CANVAS_DOGFIGHT_BACKGROUND_BMP
#define CANVAS_DOGFIGHT_BACKGROUND_BMP "assets/canvas-skies/dogfight-panorama-v2.bmp"
#endif
#ifndef CANVAS_DOGFIGHT_ENEMY_BMP
#define CANVAS_DOGFIGHT_ENEMY_BMP "assets/canvas-skies/dogfight-enemy-headon.bmp"
#endif
#ifndef CANVAS_PILOT_COCKPIT_BMP
#define CANVAS_PILOT_COCKPIT_BMP "assets/canvas-skies/pilot-cockpit.bmp"
#endif
#ifndef CANVAS_GUNNER_COCKPIT_BMP
#define CANVAS_GUNNER_COCKPIT_BMP "assets/canvas-skies/gunner-cockpit.bmp"
#endif
#ifndef CANVAS_GUNNER_RING_BMP
#define CANVAS_GUNNER_RING_BMP "assets/canvas-skies/gunner-ring-empty-v2.bmp"
#endif
#ifndef CANVAS_GUNNER_AIRFRAME_BMP
#define CANVAS_GUNNER_AIRFRAME_BMP "assets/canvas-skies/gunner-airframe-tail.bmp"
#endif
#ifndef CANVAS_GUNNER_MOVING_BMP
#define CANVAS_GUNNER_MOVING_BMP "assets/canvas-skies/gunner-single-atlas.bmp"
#endif
#ifndef CANVAS_EXPLOSION_ATLAS_BMP
#define CANVAS_EXPLOSION_ATLAS_BMP "assets/canvas-skies/explosion-atlas.bmp"
#endif
#ifndef CANVAS_TOPDOWN_EXPLOSION_ATLAS_BMP
#define CANVAS_TOPDOWN_EXPLOSION_ATLAS_BMP "assets/canvas-skies/topdown-explosion-atlas.bmp"
#endif
#ifndef CANVAS_DISPLAY_36X42_BMP
#define CANVAS_DISPLAY_36X42_BMP "assets/canvas-skies/font-display-36x42.bmp"
#endif
#ifndef CANVAS_DISPLAY_22X28_BMP
#define CANVAS_DISPLAY_22X28_BMP "assets/canvas-skies/font-display-22x28.bmp"
#endif
#ifndef CANVAS_DISPLAY_16X23_BMP
#define CANVAS_DISPLAY_16X23_BMP "assets/canvas-skies/font-display-16x23.bmp"
#endif
#ifndef CANVAS_DISPLAY_12X18_BMP
#define CANVAS_DISPLAY_12X18_BMP "assets/canvas-skies/font-display-12x18.bmp"
#endif
#ifndef CANVAS_TYPEWRITER_10X15_BMP
#define CANVAS_TYPEWRITER_10X15_BMP "assets/canvas-skies/font-typewriter-10x15.bmp"
#endif
#ifndef CANVAS_TYPEWRITER_7X11_BMP
#define CANVAS_TYPEWRITER_7X11_BMP "assets/canvas-skies/font-typewriter-7x11.bmp"
#endif
#ifndef CANVAS_JOURNAL_8X12_BMP
#define CANVAS_JOURNAL_8X12_BMP "assets/canvas-skies/font-journal-8x12.bmp"
#endif
#ifndef CANVAS_PLAYER_SPRITES_BMP
#define CANVAS_PLAYER_SPRITES_BMP "assets/canvas-skies/aircraft-player.bmp"
#endif
#ifndef CANVAS_ENEMY_SPRITES_BMP
#define CANVAS_ENEMY_SPRITES_BMP "assets/canvas-skies/aircraft-enemy.bmp"
#endif
#ifndef CANVAS_MARCH_WAV
#define CANVAS_MARCH_WAV "assets/canvas-skies/washington-post-gramophone.wav"
#endif
#ifndef CANVAS_MESS_WAV
#define CANVAS_MESS_WAV "assets/canvas-skies/colonel-bogey-gramophone.wav"
#endif
#ifndef CANVAS_PIPER_WAV
#define CANVAS_PIPER_WAV "assets/canvas-skies/amazing-grace-bagpipes.wav"
#endif
#ifndef CANVAS_SAVE_PATH
#define CANVAS_SAVE_PATH "PROGDIR:CanvasSkies.save"
#endif
#ifndef CANVAS_SAVE_NEW_PATH
#define CANVAS_SAVE_NEW_PATH "PROGDIR:CanvasSkies.save.new"
#endif
#ifndef CANVAS_SAVE_BACKUP_PATH
#define CANVAS_SAVE_BACKUP_PATH "PROGDIR:CanvasSkies.save.bak"
#endif

#define SCREEN_W 640
#define SCREEN_H 400
#define ART_H 360
#define GAMEPLAY_Y_OFFSET (SCREEN_H - ART_H)
#define MAX_BULLETS 80
#define MAX_DOGFIGHT_PARTICLES 48
#define MAX_ENEMIES 10
#define MAX_TARGETS 12
#define MAX_EXPLOSIONS 24
#define MAX_BULLET_HOLES 18
#define PLAYER_DURABILITY_MULTIPLIER 8
#define MIDGROUND_ELEMENT_COUNT 6
#define NEAR_ELEMENT_COUNT 6
#define SKY_PARALLAX 0.008f
#define HILLS_PARALLAX 0.055f
#define BUILDINGS_PARALLAX 0.22f
#define FOREGROUND_PARALLAX 1.0f

typedef enum {
    SCENE_TITLE,
    SCENE_MENU,
    SCENE_STORY,
    SCENE_BRIEFING,
    SCENE_SORTIE,
    SCENE_DEBRIEF,
    SCENE_EPILOGUE
} Scene;

typedef enum {
    MUSIC_MARCH,
    MUSIC_RECON,
    MUSIC_TENSION,
    MUSIC_MOURNING,
    MUSIC_ARMISTICE,
    MUSIC_MESS,
    MUSIC_PIPER
} MusicMood;

typedef struct {
    const char *date;
    const char *title;
    const char *story[3];
    const char *orders[3];
    int goal;
    int target_count;
    int enemy_count;
    int armed;
    float distance;
} Chapter;

#define CHAPTER_COUNT 8

static const Chapter chapters[CHAPTER_COUNT] = {
    {"23 AUGUST 1914", "EYES ABOVE THE MARNE",
     {"THE AEROPLANE IS STILL A NOVELTY.", "YOU AND TOM CARRY ONLY A MAP,",
      "A PENCIL, AND THE ARMY'S DOUBT."},
     {"CROSS THE FRONT AND MARK THE COLUMNS.", "KEEP CLEAR OF RIFLE FIRE.",
      "BRING THE OBSERVATION HOME."}, 0, 3, 0, 0, 3200.0f},
    {"1 AUGUST 1915", "THE WAR LEARNS TO FLY",
     {"MACHINE GUNS HAVE ENTERED THE SKY.", "THE FOKKER PATROLS HUNT OUR CORPS,",
      "AND EVERY CROSSING IS CONTESTED."},
     {"ESCORT THE OBSERVATION ROUTE.", "DRIVE OFF THE ENEMY PATROL.",
      "RETURN TO THE WESTERN FIELD."}, 0, 0, 3, 1, 3800.0f},
    {"1 JULY 1916", "OVER THE SOMME",
     {"THE GUNS HAVE ERASED THE HORIZON.", "THE INFANTRY WAITS BELOW THE CLOUDS,",
      "BLIND WITHOUT YOUR REPORT."},
     {"SILENCE TWO FORWARD GUN POSTS.", "PHOTOGRAPH THE SECOND LINE.",
      "SURVIVE THE RETURN CROSSING."}, 2, 3, 4, 1, 4300.0f},
    {"9 APRIL 1917", "BLOODY APRIL",
     {"THE SQUADRON CHAIRS STAND EMPTY.", "NEW PILOTS ARRIVE BEFORE BREAKFAST,",
      "AND VANISH BEFORE SUPPER."},
     {"BREAK THREE ENEMY POSITIONS.", "KEEP THE RECONNAISSANCE LANE OPEN.",
      "DO NOT CHASE THEM EAST."}, 3, 5, 5, 1, 4700.0f},
    {"20 NOVEMBER 1917", "THE IRON ROAD",
     {"TANKS MOVE AT DAWN NEAR CAMBRAI.", "AIR AND GROUND MUST STRIKE TOGETHER,",
      "BEFORE THE RESERVES ARRIVE."},
     {"DESTROY THREE GUN EMPLACEMENTS.", "STRAFE THE ROAD BEYOND THE RIDGE.",
      "COVER THE ADVANCE."}, 3, 7, 6, 1, 5000.0f},
    {"21 MARCH 1918", "THE SPRING STORM",
     {"THE LINE HAS BROKEN IN THE MIST.", "FIELDS WON AT TERRIBLE COST ARE LOST,",
      "AND THE SQUADRON FLIES WEST."},
     {"HOLD FOUR PURSUING BATTERIES.", "PROTECT THE RETREATING COLUMN.",
      "LAND WEST OF THE OLD FIELD."}, 4, 9, 8, 1, 5400.0f},
    {"8 AUGUST 1918", "THE HUNDRED DAYS",
     {"NOW THE ALLIED ARMIES MOVE FORWARD.", "THE SKY BELONGS TO NO ONE, BUT TODAY",
      "IT MAY BELONG TO THE BRAVE."},
     {"BREAK FOUR DEFENSIVE POSITIONS.", "CLEAR THE ROAD TO THE EAST.",
      "PRESS THE ADVANCE HOME."}, 4, 12, 9, 1, 5800.0f},
    {"11 NOVEMBER 1918", "THE ELEVENTH HOUR",
     {"THE MESSAGE SAYS THE GUNS STOP AT ELEVEN.", "UNTIL THEN, YOU FLY ONE LAST PATROL.",
      "NO ONE WANTS TO BE THE FINAL NAME."},
     {"CROSS THE QUIETENING FRONT.", "DO NOT FIRE UNLESS FIRED UPON.",
      "BE HOME WHEN THE BELLS BEGIN."}, 0, 0, 10, 1, 3600.0f}
};

typedef struct {
    float x, y, vx, vy;
    float life, initial_life;
    int enemy;
    int type;
    int active;
} Bullet;

typedef struct {
    float x, y, vx, vy;
    float life, initial_life;
    int kind; /* 0 player tracer, 1 enemy tracer, 2 ejected case */
    int active;
} DogfightParticle;

typedef struct {
    float x, y, age, duration;
    int large;
    int world_axis; /* 0 screen, 1 top-down forward axis, 2 side-on x axis */
    int active;
} Explosion;

/* Compact six-degree-of-freedom aircraft state.  Body axes follow the usual
 * aircraft convention: u points through the propeller, v through the right
 * wing and w down through the wheels.  The quaternion rotates those axes into
 * a north/east/down world; the renderer converts that world to x/up/z. */
typedef struct {
    float u, v, w;
    float p, q, r;
    float qw, qx, qy, qz;
    float aileron, elevator, rudder;
    float throttle;
    float accumulator;
    float alpha, beta, speed;
    int stalled;
} FlightState;

typedef struct {
    float x, y, phase;
    float world_x, world_y, world_z;
    float heading, pitch, bank, speed;
    float vx, vy, vz;
    float roll_rate, pitch_rate, throttle;
    FlightState flight;
    float crash_x, crash_y, crash_vx, crash_vy, crash_angle;
    int crash_size;
    int crashing;
    int active, hp, max_hp;
    uint32_t last_shot;
} Enemy;

typedef struct {
    float world_x;
    int active;
    int type;
    uint32_t last_shot;
    uint32_t next_round;
    int burst_left;
} Target;

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    int x, y;
    float depth;
    int visible;
} Projected3D;

typedef struct {
    float x, y; /* source-airframe coordinates */
    int active;
} BulletHole;

typedef struct {
    SDL_AudioDeviceID device;
    uint32_t sample_clock;
    uint32_t cue_clock;
    uint32_t noise;
    uint8_t *music_data;
    uint32_t music_len;
    uint32_t music_pos;
    uint8_t *mess_data;
    uint32_t mess_len;
    uint32_t mess_pos;
    uint8_t *piper_data;
    uint32_t piper_len;
    uint32_t piper_pos;
    volatile int scene;
    volatile int shot;
    volatile int blast;
    volatile int crash;
    volatile int hit;
    volatile int hit_player;
    volatile float engine_speed;
    volatile float engine_load;
    volatile float other_engine;
    volatile float other_pitch;
    volatile float vehicle_engine;
    volatile int music_mood;
    float engine_phase;
    float engine_sub_phase;
    float other_phase;
    float vehicle_phase;
    float synth_phase[5];
    float gramophone_lp;
    float gramophone_hp;
    float gramophone_previous;
} AudioState;

typedef struct {
    SDL_Window *window;
    SDL_Surface *window_surface;
    SDL_Surface *screen;
    SDL_Surface *title;
    SDL_Surface *title_banner;
    SDL_Surface *title_backdrop;
    SDL_Surface *mission_board;
    SDL_Surface *fatal_memorial;
    SDL_Surface *field_hospital;
    SDL_Surface *journal;
    SDL_Surface *sky;
    SDL_Surface *battlefield;
    SDL_Surface *foreground_panorama;
    SDL_Surface *midground[MIDGROUND_ELEMENT_COUNT];
    SDL_Surface *near_props[NEAR_ELEMENT_COUNT];
    SDL_Surface *machine_gun_sprite;
    SDL_Surface *aa_gun_sprite;
    SDL_Surface *topdown_battlefield;
    SDL_Surface *topdown_player;
    SDL_Surface *dogfight_background;
    SDL_Surface *dogfight_enemy;
    SDL_Surface *pilot_cockpit;
    SDL_Surface *gunner_cockpit;
    SDL_Surface *gunner_ring;
    SDL_Surface *gunner_airframe;
    SDL_Surface *gunner_moving_guns;
    SDL_Surface *explosion_atlas;
    SDL_Surface *topdown_explosion_atlas;
    SDL_Surface *font_display_36x42;
    SDL_Surface *font_display_22x28;
    SDL_Surface *font_display_16x23;
    SDL_Surface *font_display_12x18;
    SDL_Surface *font_typewriter_10x15;
    SDL_Surface *font_typewriter_7x11;
    SDL_Surface *font_journal_8x12;
    SDL_Surface *player_sprites;
    SDL_Surface *enemy_sprites;
    AudioState audio;
    Scene scene;
    int running;
    int fullscreen;
    int redraw_static;
    int qa_hold;
    int success;
    uint8_t key_latched[SDL_NUM_SCANCODES];
    uint32_t last_action_key;
    int menu_selection;
    int confirm_overwrite;
    int chapter;
    int mission;
    int wingman_health[3];
    int has_save;
    int save_loaded_from_backup;
    int campaign_complete;
    int lives;
    int life_lost_this_sortie;
    int mission_save_state;
    int fatal_sortie;
    int saved_campaign_complete;
    int saved_lives;
    int saved_chapter;
    int saved_mission;
    int saved_wingman_health[3];
    uint32_t scene_started;
    float player_x;
    float player_y;
    float player_vy;
    float gunner_view_x;
    float gunner_view_y;
    float view_bank;
    float view_pitch;
    float flight_x;
    float flight_y;
    float flight_z;
    float flight_heading;
    float flight_roll_rate;
    float flight_pitch_rate;
    float flight_vx;
    float flight_vy;
    float flight_vz;
    float flight_throttle;
    FlightState flight;
    float flight_alpha;
    int flight_stalled;
    int flight_doomed;
    uint32_t flight_doomed_started;
    int flight_crashed;
    uint32_t flight_crash_started;
    float airspeed;
    float scroll;
    float fuel;
    int health;
    int tail_integrity;
    int score;
    int kills;
    int objectives;
    int ammunition;
    int bombs_remaining;
    uint32_t last_shot;
    uint32_t last_bomb;
    uint32_t last_damage;
    uint32_t mission_clear_at;
    Bullet bullets[MAX_BULLETS];
    DogfightParticle dogfight_particles[MAX_DOGFIGHT_PARTICLES];
    Explosion explosions[MAX_EXPLOSIONS];
    Enemy enemies[MAX_ENEMIES];
    Target targets[MAX_TARGETS];
    BulletHole bullet_holes[MAX_BULLET_HOLES];
    int next_bullet_hole;
} Game;

static void flight_state_init(FlightState *f, float heading, float pitch,
                              float bank, float speed, float throttle);
static Vec3 flight_rotate_from_ned(const FlightState *f, float north,
                                   float east, float down);

static int is_perspective_mode(MissionMode mode)
{
    return mode == MISSION_DOGFIGHT_3D || mode == MISSION_GUNNER_3D;
}

/* Wingmen follow the written story rather than a damage simulation.
 * Gridpipe Thyne turns east and is lost in "Thyne's Last Turn" (Chapter 5,
 * Mission 9); from the next sortie onward his place stays empty. */
static void apply_story_wingmen(Game *g)
{
    int pilot;
    for (pilot = 0; pilot < 3; ++pilot)
        g->wingman_health[pilot] = 100;
    if (g->chapter > 4 || (g->chapter == 4 && g->mission > 8))
        g->wingman_health[1] = 0;
}

static int load_campaign_save_path(Game *g, const char *path)
{
    FILE *file = fopen(path, "r");
    char magic[24];
    int version;
    int chapter, mission;
    int wingman0, wingman1, wingman2;
    int complete = 0;
    int lives = 3;
    if (!file) return 0;
    if (fscanf(file, "%23s %d\n%d %d\n%d %d %d", magic, &version,
               &chapter, &mission, &wingman0, &wingman1, &wingman2) != 7 ||
        strcmp(magic, "CANVAS_SKIES_SAVE") != 0 ||
        (version != 1 && version != 2 && version != 3) ||
        chapter < 0 || chapter >= CHAPTER_COUNT ||
        mission < 0 || mission >= MISSIONS_PER_CHAPTER ||
        wingman0 < 0 || wingman0 > 100 ||
        wingman1 < 0 || wingman1 > 100 ||
        wingman2 < 0 || wingman2 > 100) {
        fclose(file);
        return 0;
    }
    if (version >= 2 &&
        (fscanf(file, "%d", &complete) != 1 ||
         (complete != 0 && complete != 1))) {
        fclose(file);
        return 0;
    }
    if (version >= 3 &&
        (fscanf(file, "%d", &lives) != 1 || lives < 0 || lives > 3)) {
        fclose(file);
        return 0;
    }
    fclose(file);
    g->saved_chapter = chapter;
    g->saved_mission = mission;
    g->saved_wingman_health[0] = wingman0;
    g->saved_wingman_health[1] = wingman1;
    g->saved_wingman_health[2] = wingman2;
    g->saved_campaign_complete = complete;
    g->saved_lives = lives;
    return 1;
}

static int load_campaign_save(Game *g)
{
    if (load_campaign_save_path(g, CANVAS_SAVE_PATH)) {
        g->save_loaded_from_backup = 0;
        return 1;
    }
    /* A save is first written under a temporary name and the preceding save
     * is retained as .bak.  If power is lost during replacement, continue
     * from the last complete record instead of silently losing the campaign. */
    if (load_campaign_save_path(g, CANVAS_SAVE_BACKUP_PATH)) {
        g->save_loaded_from_backup = 1;
        return 1;
    }
    return 0;
}

static int save_campaign(Game *g)
{
    FILE *file = fopen(CANVAS_SAVE_NEW_PATH, "w");
    int ok;
    int backed_up;
    if (!file) return 0;
    ok = fprintf(file, "CANVAS_SKIES_SAVE 3\n%d %d\n%d %d %d\n%d\n%d\n",
                 g->chapter, g->mission,
                 g->wingman_health[0], g->wingman_health[1],
                 g->wingman_health[2], g->campaign_complete, g->lives) > 0;
    if (fclose(file) != 0) ok = 0;
    if (!ok) {
        remove(CANVAS_SAVE_NEW_PATH);
        return 0;
    }
    if (g->save_loaded_from_backup) {
        /* The primary was already known bad.  Keep the valid backup in place
         * until the replacement has landed instead of rotating corruption
         * over the only recoverable campaign. */
        remove(CANVAS_SAVE_PATH);
        backed_up = 0;
    } else {
        remove(CANVAS_SAVE_BACKUP_PATH);
        backed_up = rename(CANVAS_SAVE_PATH, CANVAS_SAVE_BACKUP_PATH) == 0;
    }
    if (rename(CANVAS_SAVE_NEW_PATH, CANVAS_SAVE_PATH) != 0) {
        if (backed_up)
            rename(CANVAS_SAVE_BACKUP_PATH, CANVAS_SAVE_PATH);
        remove(CANVAS_SAVE_NEW_PATH);
        return 0;
    }
    g->saved_chapter = g->chapter;
    g->saved_mission = g->mission;
    g->saved_campaign_complete = g->campaign_complete;
    g->saved_lives = g->lives;
    memcpy(g->saved_wingman_health, g->wingman_health,
           sizeof(g->wingman_health));
    g->has_save = 1;
    g->save_loaded_from_backup = 0;
    return 1;
}

static int save_mission_result(Game *g)
{
    int active_chapter = g->chapter;
    int active_mission = g->mission;
    int active_complete = g->campaign_complete;
    int ok;
    if (g->fatal_sortie && !g->life_lost_this_sortie) {
        if (g->lives > 0) --g->lives;
        g->life_lost_this_sortie = 1;
    }
    /* The debrief still describes the sortie just flown, but the durable
     * continuation point must already describe what comes next.  A failed
     * sortie deliberately saves the same mission so Retry remains honest. */
    if (g->success) {
        if (g->mission + 1 < MISSIONS_PER_CHAPTER) {
            ++g->mission;
        } else if (g->chapter + 1 < CHAPTER_COUNT) {
            ++g->chapter;
            g->mission = 0;
            /* A fresh chapter brings a fresh start: three lives again, so
             * one bad spell costs a chapter rather than the whole war. */
            g->lives = 3;
        } else {
            g->campaign_complete = 1;
        }
    }
    apply_story_wingmen(g);
    ok = save_campaign(g);
    g->chapter = active_chapter;
    g->mission = active_mission;
    g->campaign_complete = active_complete;
    apply_story_wingmen(g);
    g->mission_save_state = ok ? 1 : -1;
    return ok;
}

static int campaign_menu_count(const Game *g)
{
    return (g->has_save && g->saved_lives > 0) ? 7 : 6;
}

static float gunner_traverse(const Game *g)
{
    float value = (g->gunner_view_x - 320.0f) / 176.0f;
    if (value < -1.0f) value = -1.0f;
    if (value > 1.0f) value = 1.0f;
    return value;
}

static float gunner_elevation(const Game *g)
{
    float value = (235.0f - g->gunner_view_y) / 90.0f;
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;
    return value;
}

static float gunner_aim_traverse(const Game *g)
{
    float value = (g->player_x - 28.0f) / 584.0f;
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;
    return value;
}

static float gunner_aim_elevation(const Game *g)
{
    float value = (300.0f - g->player_y) / 272.0f;
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;
    return value;
}

static void gunner_muzzle_position(const Game *g, float *x, float *y)
{
    /* Measured barrel-bore centres for every painted atlas frame, arranged as
     * [elevation][traverse].  X is not separable from elevation and Y is not
     * separable from traverse: the former one-dimensional approximation made
     * tracers visibly begin beside the Lewis gun at oblique angles. */
    static const int muzzle_x_in_frame[5][5] = {
        { 58, 108, 210, 311, 362 },
        { 60, 106, 209, 312, 359 },
        { 72, 109, 210, 309, 346 },
        { 91, 116, 210, 299, 327 },
        {103, 136, 210, 281, 318 }
    };
    static const int muzzle_y_in_frame[5][5] = {
        {80, 58, 50, 58, 80},
        {64, 54, 50, 54, 65},
        {59, 49, 48, 49, 59},
        {51, 43, 52, 43, 51},
        {35, 36, 36, 36, 35}
    };
    float traverse = gunner_aim_traverse(g);
    float elevation = gunner_aim_elevation(g);
    int column = (int)(traverse * 4.999f);
    int row = (int)(elevation * 4.999f);
    int carriage_x = 320 + (int)((traverse - 0.5f) * 74.0f);
    int carriage_y = 176 + (int)(fabsf(traverse - 0.5f) * 8.0f);
    /* These are the painted barrel openings, below the separate ring sights,
     * not an estimate from the screen centre or the sight itself. */
    *x = (float)(carriage_x - 210 + muzzle_x_in_frame[row][column]);
    *y = (float)(carriage_y + muzzle_y_in_frame[row][column]);
}

static int gunner_tail_blocks_shot(const Game *g, int aim_x, int aim_y)
{
    float look = gunner_traverse(g);
    float elevation = gunner_elevation(g);
    int tail_x = 320 - (int)(look * 132.0f);
    int tail_y = 226 + (int)(elevation * 34.0f);
    int fin_top = 76 + (int)(elevation * 26.0f);
    /* The fin blocks the central vertical cone; the tailplane blocks the
     * lower horizontal cone.  Both move with the transformed airframe. */
    if (abs(aim_x - tail_x) < 27 && aim_y >= fin_top && aim_y <= tail_y + 18)
        return 1;
    if (abs(aim_y - tail_y) < 24 && abs(aim_x - tail_x) < 242)
        return 1;
    return 0;
}

static uint32_t scene_hash(uint32_t value)
{
    value ^= value >> 16;
    value *= 0x7feb352du;
    value ^= value >> 15;
    value *= 0x846ca68bu;
    value ^= value >> 16;
    return value;
}

static void draw_element_layer(Game *g, SDL_Surface **elements, int count,
                               float parallax, int spacing, int ground_y,
                               uint32_t salt)
{
    int camera = (int)(g->scroll * parallax);
    int first_segment = camera / spacing - 1;
    int visible_segments = SCREEN_W / spacing + 3;
    int n;

    for (n = 0; n < visible_segments; ++n) {
        int segment = first_segment + n;
        uint32_t seed = scene_hash((uint32_t)segment * 2654435761u ^
                                   (uint32_t)(g->chapter + 1) * 2246822519u ^
                                   (uint32_t)(g->mission + 1) * 3266489917u ^
                                   salt);
        SDL_Surface *element = elements[seed % (uint32_t)count];
        int jitter = (int)((seed >> 9) % (uint32_t)(spacing / 3));
        int x;
        SDL_Rect dest;

        if (!element)
            continue;
        x = segment * spacing - camera + jitter;
        if (x + element->w < 0 || x >= SCREEN_W)
            continue;
        dest.x = x;
        dest.y = ground_y - element->h;
        dest.w = element->w;
        dest.h = element->h;
        SDL_BlitSurface(element, NULL, g->screen, &dest);
    }
}

/* Direct 32-bit pixel packing.  Every surface the renderer touches per
 * pixel is converted to the screen's 32-bit format at load, so the common
 * case avoids an SDL_GetRGB/SDL_MapRGB call for each pixel. */
static int fast_format(const SDL_PixelFormat *f)
{
    return f->BytesPerPixel == 4 && f->Rloss == 0 && f->Gloss == 0 &&
           f->Bloss == 0 && !f->palette;
}

static void unpack_rgb(const SDL_PixelFormat *f, uint32_t p,
                       uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (fast_format(f)) {
        *r = (uint8_t)((p & f->Rmask) >> f->Rshift);
        *g = (uint8_t)((p & f->Gmask) >> f->Gshift);
        *b = (uint8_t)((p & f->Bmask) >> f->Bshift);
    } else {
        SDL_GetRGB(p, f, r, g, b);
    }
}

static uint32_t pack_rgb(const SDL_PixelFormat *f, uint8_t r, uint8_t g,
                         uint8_t b)
{
    if (fast_format(f))
        return ((uint32_t)r << f->Rshift) | ((uint32_t)g << f->Gshift) |
               ((uint32_t)b << f->Bshift) | f->Amask;
    return SDL_MapRGB(f, r, g, b);
}

static uint32_t rgb(SDL_Surface *s, uint8_t r, uint8_t g, uint8_t b)
{
    return pack_rgb(s->format, r, g, b);
}

static void present_frame(Game *g)
{
    SDL_Surface *dst = g->window_surface;
    SDL_Rect dest;
    uint32_t black;
    if (!dst) {
        dst = SDL_GetWindowSurface(g->window);
        g->window_surface = dst;
    }
    if (!dst) return;
    black = SDL_MapRGB(dst->format, 0, 0, 0);
    SDL_FillRect(dst, NULL, black);
    if (dst->w * SCREEN_H < dst->h * SCREEN_W) {
        dest.w = dst->w;
        dest.h = dst->w * SCREEN_H / SCREEN_W;
    } else {
        dest.h = dst->h;
        dest.w = dst->h * SCREEN_W / SCREEN_H;
    }
    dest.x = (dst->w - dest.w) / 2;
    dest.y = (dst->h - dest.h) / 2;
    SDL_BlitScaled(g->screen, NULL, dst, &dest);
    SDL_UpdateWindowSurface(g->window);
}

static void toggle_fullscreen(Game *g)
{
    uint32_t flags = g->fullscreen ? 0u : SDL_WINDOW_FULLSCREEN;
    if (SDL_SetWindowFullscreen(g->window, flags) == 0) {
        g->fullscreen = !g->fullscreen;
        g->window_surface = SDL_GetWindowSurface(g->window);
        g->redraw_static = 1;
    } else {
        fprintf(stderr, "Canvas Skies fullscreen switch failed: %s\n",
                SDL_GetError());
    }
}

static void rect(SDL_Surface *s, int x, int y, int w, int h, uint32_t c)
{
    SDL_Rect r = {x, y, w, h};
    SDL_FillRect(s, &r, c);
}

static void pixel(SDL_Surface *s, int x, int y, uint32_t c)
{
    if (x < 0 || y < 0 || x >= s->w || y >= s->h)
        return;
    ((uint32_t *)((uint8_t *)s->pixels + y * s->pitch))[x] = c;
}

static void line(SDL_Surface *s, int x0, int y0, int x1, int y1, uint32_t c)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    for (;;) {
        pixel(s, x0, y0, c);
        if (x0 == x1 && y0 == y1)
            break;
        {
            int e2 = err * 2;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }
}

static int edge2d(int ax, int ay, int bx, int by, int px, int py)
{
    return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}

static void fill_triangle(SDL_Surface *s, int x0, int y0, int x1, int y1,
                          int x2, int y2, uint32_t colour)
{
    int min_x = x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2);
    int max_x = x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2);
    int min_y = y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2);
    int max_y = y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2);
    int area = edge2d(x0, y0, x1, y1, x2, y2);
    int x, y;
    if (area == 0) return;
    if (min_x < 0) min_x = 0;
    if (max_x >= s->w) max_x = s->w - 1;
    if (min_y < 0) min_y = 0;
    if (max_y >= s->h) max_y = s->h - 1;
    for (y = min_y; y <= max_y; ++y) {
        for (x = min_x; x <= max_x; ++x) {
            int w0 = edge2d(x1, y1, x2, y2, x, y);
            int w1 = edge2d(x2, y2, x0, y0, x, y);
            int w2 = edge2d(x0, y0, x1, y1, x, y);
            if ((area > 0 && w0 >= 0 && w1 >= 0 && w2 >= 0) ||
                (area < 0 && w0 <= 0 && w1 <= 0 && w2 <= 0))
                pixel(s, x, y, colour);
        }
    }
}

static void circle(SDL_Surface *s, int cx, int cy, int radius, uint32_t c)
{
    int x, y;
    for (y = -radius; y <= radius; ++y)
        for (x = -radius; x <= radius; ++x)
            if (x * x + y * y <= radius * radius)
                pixel(s, cx + x, cy + y, c);
}

static void ellipse(SDL_Surface *s, int cx, int cy, int radius_x,
                    int radius_y, uint32_t c)
{
    int y;
    if (radius_x < 1 || radius_y < 1) return;
    for (y = -radius_y; y <= radius_y; ++y) {
        float n = (float)(y * y) / (float)(radius_y * radius_y);
        int span = (int)((float)radius_x * sqrtf(1.0f - n));
        line(s, cx - span, cy + y, cx + span, cy + y, c);
    }
}

static void blend_rect(SDL_Surface *s, int x, int y, int w, int h,
                       uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
{
    int px, py;
    int keep = 255 - alpha;
    int add_r = r * alpha, add_g = g * alpha, add_b = b * alpha;
    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
    for (py = y; py < y + h && py < s->h; ++py) {
        uint32_t *row;
        if (py < 0) continue;
        row = (uint32_t *)((uint8_t *)s->pixels + py * s->pitch);
        for (px = x; px < x + w && px < s->w; ++px) {
            uint8_t or_, og, ob;
            uint32_t p;
            if (px < 0) continue;
            p = row[px];
            unpack_rgb(s->format, p, &or_, &og, &ob);
            or_ = (uint8_t)((or_ * keep + add_r) / 255);
            og = (uint8_t)((og * keep + add_g) / 255);
            ob = (uint8_t)((ob * keep + add_b) / 255);
            row[px] = pack_rgb(s->format, or_, og, ob);
        }
    }
    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
}

static void blend_circle(SDL_Surface *s, int cx, int cy, int radius,
                         uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
{
    int px, py;
    if (radius <= 0) return;
    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
    for (py = -radius; py <= radius; ++py) {
        int yy = cy + py;
        uint32_t *row;
        if (yy < 0 || yy >= s->h) continue;
        row = (uint32_t *)((uint8_t *)s->pixels + yy * s->pitch);
        for (px = -radius; px <= radius; ++px) {
            int xx = cx + px;
            int aa;
            uint8_t or_, og, ob;
            uint32_t p;
            if (xx < 0 || xx >= s->w || px * px + py * py > radius * radius)
                continue;
            aa = alpha * (radius * radius - px * px - py * py) /
                 (radius * radius);
            p = row[xx];
            unpack_rgb(s->format, p, &or_, &og, &ob);
            or_ = (uint8_t)((or_ * (255 - aa) + r * aa) / 255);
            og = (uint8_t)((og * (255 - aa) + g * aa) / 255);
            ob = (uint8_t)((ob * (255 - aa) + b * aa) / 255);
            row[xx] = pack_rgb(s->format, or_, og, ob);
        }
    }
    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
}

/* Compact 5x7 uppercase font, columns stored least-significant bit first. */
static const uint8_t *glyph(char ch)
{
    static const uint8_t blank[5] = {0,0,0,0,0};
    static const uint8_t table[][5] = {
        {0x3e,0x09,0x09,0x09,0x3e}, /* A */
        {0x3f,0x25,0x25,0x25,0x1a}, /* B */
        {0x1e,0x21,0x21,0x21,0x12}, /* C */
        {0x3f,0x21,0x21,0x12,0x0c}, /* D */
        {0x3f,0x25,0x25,0x25,0x21}, /* E */
        {0x3f,0x05,0x05,0x05,0x01}, /* F */
        {0x1e,0x21,0x29,0x29,0x3a}, /* G */
        {0x3f,0x04,0x04,0x04,0x3f}, /* H */
        {0x21,0x21,0x3f,0x21,0x21}, /* I */
        {0x10,0x20,0x20,0x20,0x1f}, /* J */
        {0x3f,0x04,0x0a,0x11,0x20}, /* K */
        {0x3f,0x20,0x20,0x20,0x20}, /* L */
        {0x3f,0x02,0x04,0x02,0x3f}, /* M */
        {0x3f,0x02,0x04,0x08,0x3f}, /* N */
        {0x1e,0x21,0x21,0x21,0x1e}, /* O */
        {0x3f,0x09,0x09,0x09,0x06}, /* P */
        {0x1e,0x21,0x29,0x11,0x2e}, /* Q */
        {0x3f,0x09,0x19,0x29,0x06}, /* R */
        {0x12,0x25,0x25,0x25,0x18}, /* S */
        {0x01,0x01,0x3f,0x01,0x01}, /* T */
        {0x1f,0x20,0x20,0x20,0x1f}, /* U */
        {0x0f,0x10,0x20,0x10,0x0f}, /* V */
        {0x3f,0x10,0x08,0x10,0x3f}, /* W */
        {0x31,0x0a,0x04,0x0a,0x31}, /* X */
        {0x03,0x04,0x38,0x04,0x03}, /* Y */
        {0x31,0x29,0x25,0x23,0x21}  /* Z */
    };
    static const uint8_t digits[][5] = {
        {0x1e,0x29,0x25,0x23,0x1e}, {0x00,0x22,0x3f,0x20,0x00},
        {0x32,0x29,0x29,0x29,0x26}, {0x12,0x21,0x25,0x25,0x1a},
        {0x0c,0x0a,0x09,0x3f,0x08}, {0x17,0x25,0x25,0x25,0x19},
        {0x1e,0x25,0x25,0x25,0x18}, {0x01,0x01,0x39,0x05,0x03},
        {0x1a,0x25,0x25,0x25,0x1a}, {0x06,0x29,0x29,0x29,0x1e}
    };
    static const uint8_t colon[5] = {0,0x14,0,0x14,0};
    static const uint8_t dash[5] = {0x04,0x04,0x04,0x04,0x04};
    static const uint8_t dot[5] = {0,0x20,0x20,0,0};
    static const uint8_t bang[5] = {0,0,0x2f,0,0};
    if (ch >= 'a' && ch <= 'z') ch = (char)(ch - 'a' + 'A');
    if (ch >= 'A' && ch <= 'Z') return table[ch - 'A'];
    if (ch >= '0' && ch <= '9') return digits[ch - '0'];
    if (ch == ':') return colon;
    if (ch == '-') return dash;
    if (ch == '.') return dot;
    if (ch == '!') return bang;
    return blank;
}

static void text(SDL_Surface *s, int x, int y, const char *str, int scale,
                 uint32_t color)
{
    while (*str) {
        const uint8_t *g;
        int gx, gy;
        if (*str == ' ') {
            x += 4 * scale;
            ++str;
            continue;
        }
        g = glyph(*str++);
        for (gx = 0; gx < 5; ++gx)
            for (gy = 0; gy < 7; ++gy)
                if (g[gx] & (1 << gy))
                    rect(s, x + gx * scale, y + gy * scale,
                         scale, scale, color);
        x += 6 * scale;
    }
}

static uint32_t surface_pixel(SDL_Surface *s, int x, int y)
{
    uint8_t *p = (uint8_t *)s->pixels + y * s->pitch
               + x * s->format->BytesPerPixel;
    switch (s->format->BytesPerPixel) {
    case 1: return *p;
    case 2: return *(uint16_t *)p;
    case 3:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return (uint32_t)((p[0] << 16) | (p[1] << 8) | p[2]);
#else
        return (uint32_t)(p[0] | (p[1] << 8) | (p[2] << 16));
#endif
    default: return *(uint32_t *)p;
    }
}

static void surface_set_pixel(SDL_Surface *s, int x, int y, uint32_t value)
{
    uint8_t *p = (uint8_t *)s->pixels + y * s->pitch
               + x * s->format->BytesPerPixel;
    switch (s->format->BytesPerPixel) {
    case 1: *p = (uint8_t)value; break;
    case 2: *(uint16_t *)p = (uint16_t)value; break;
    case 3:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        p[0] = (uint8_t)(value >> 16);
        p[1] = (uint8_t)(value >> 8);
        p[2] = (uint8_t)value;
#else
        p[0] = (uint8_t)value;
        p[1] = (uint8_t)(value >> 8);
        p[2] = (uint8_t)(value >> 16);
#endif
        break;
    default: *(uint32_t *)p = value; break;
    }
}

static void prepare_magenta_key(SDL_Surface *surface)
{
    uint32_t key;
    int x, y;
    if (!surface) return;
    key = SDL_MapRGB(surface->format, 255, 0, 255);
    if (SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) != 0) {
        SDL_SetColorKey(surface, SDL_TRUE, key);
        return;
    }
    for (y = 0; y < surface->h; ++y) {
        for (x = 0; x < surface->w; ++x) {
            uint8_t r, g, b;
            SDL_GetRGB(surface_pixel(surface, x, y), surface->format,
                       &r, &g, &b);
            /* Authored sprites use pure magenta.  Antialiasing left nearby
             * purple pixels around silhouettes, which an exact SDL colour
             * key cannot remove and which showed as a neon outline in game. */
            if (r > 60 && b > 50 && (int)g + 20 < (int)r &&
                (int)g + 20 < (int)b && abs((int)r - (int)b) < 64)
                surface_set_pixel(surface, x, y, key);
        }
    }
    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
    SDL_SetColorKey(surface, SDL_TRUE, key);
}

/* Pre-rasterised native-size faces: serif display and field typewriter. */
static void period_text(SDL_Surface *dst, SDL_Surface *atlas,
                        int cell_w, int cell_h, int x, int y,
                        const char *str, uint32_t color)
{
    if (!atlas) {
        text(dst, x, y, str, cell_h >= 20 ? 3 : 1, color);
        return;
    }
    if (SDL_MUSTLOCK(atlas)) SDL_LockSurface(atlas);
    while (*str) {
        unsigned int ch = (unsigned char)*str++;
        int index, sx0, sy0, px, py;
        if (ch < 32 || ch > 95) ch = ' ';
        index = (int)ch - 32;
        sx0 = (index & 15) * cell_w;
        sy0 = (index >> 4) * cell_h;
        if (ch != ' ') {
            for (py = 0; py < cell_h; ++py) {
                int sy = sy0 + py;
                for (px = 0; px < cell_w; ++px) {
                    int sx = sx0 + px;
                    uint8_t r, g, b;
                    unpack_rgb(atlas->format, surface_pixel(atlas, sx, sy),
                               &r, &g, &b);
                    if ((int)r + (int)g + (int)b > 150)
                        pixel(dst, x + px, y + py, color);
                }
            }
        }
        x += cell_w;
    }
    if (SDL_MUSTLOCK(atlas)) SDL_UnlockSurface(atlas);
}

static void display_text(Game *g, int x, int y, const char *str,
                         int w, int h, uint32_t color)
{
    SDL_Surface *atlas = NULL;
    if (w == 36 && h == 42) atlas = g->font_display_36x42;
    else if (w == 22 && h == 28) atlas = g->font_display_22x28;
    else if (w == 16 && h == 23) atlas = g->font_display_16x23;
    else if (w == 12 && h == 18) atlas = g->font_display_12x18;
    period_text(g->screen, atlas, w, h, x, y, str, color);
}

static void typewriter_text(Game *g, int x, int y, const char *str,
                            int w, int h, uint32_t color)
{
    SDL_Surface *atlas = NULL;
    if (w == 10 && h == 15) atlas = g->font_typewriter_10x15;
    else if (w == 7 && h == 11) atlas = g->font_typewriter_7x11;
    period_text(g->screen, atlas, w, h, x, y, str, color);
}

static void journal_text(Game *g, int x, int y, const char *str,
                         uint32_t color)
{
    period_text(g->screen, g->font_journal_8x12, 8, 12,
                x, y, str, color);
}

static int journal_line_y(int line)
{
    /* The photographed notebook rules are 17 pixels apart at game size. */
    return 72 + line * 17;
}

static void journal_paragraph(Game *g, int x, int y, const char *str,
                              int columns, uint32_t color)
{
    char line_buf[64];
    int line_len = 0;
    const char *p = str;
    while (*p) {
        const char *word;
        int word_len;
        while (*p == ' ') ++p;
        if (!*p) break;
        word = p;
        while (*p && *p != ' ') ++p;
        word_len = (int)(p - word);
        if (line_len && line_len + 1 + word_len > columns) {
            line_buf[line_len] = '\0';
            journal_text(g, x, y, line_buf, color);
            y += 17;
            line_len = 0;
        }
        if (line_len) line_buf[line_len++] = ' ';
        if (word_len > (int)sizeof(line_buf) - line_len - 1)
            word_len = (int)sizeof(line_buf) - line_len - 1;
        memcpy(line_buf + line_len, word, (size_t)word_len);
        line_len += word_len;
    }
    if (line_len) {
        line_buf[line_len] = '\0';
        journal_text(g, x, y, line_buf, color);
    }
}

/* The 68040 has no hardware sine or power instructions, so the audio
 * callback must not call sinf/powf per sample.  Both come from tables built
 * once at start-up. */
#define SINE_TABLE_SIZE 1024
#define NOTE_TABLE_LOW (-36)
#define NOTE_TABLE_HIGH 48
static float sine_table[SINE_TABLE_SIZE];
static float note_table[NOTE_TABLE_HIGH - NOTE_TABLE_LOW + 1];

static void init_audio_tables(void)
{
    int i;
    for (i = 0; i < SINE_TABLE_SIZE; ++i)
        sine_table[i] = sinf(2.0f * (float)M_PI * (float)i /
                             (float)SINE_TABLE_SIZE);
    for (i = NOTE_TABLE_LOW; i <= NOTE_TABLE_HIGH; ++i)
        note_table[i - NOTE_TABLE_LOW] =
            220.0f * powf(2.0f, (float)i / 12.0f);
}

/* sin(2 * pi * cycles), for any positive or negative cycle count. */
static float fast_sin_cycles(float cycles)
{
    float frac = cycles - (float)(int)cycles;
    if (frac < 0.0f) frac += 1.0f;
    return sine_table[(int)(frac * (float)SINE_TABLE_SIZE) &
                      (SINE_TABLE_SIZE - 1)];
}

static float wrap_unit(float value)
{
    float frac = value - (float)(int)value;
    return frac < 0.0f ? frac + 1.0f : frac;
}

static float note_hz(int semitone)
{
    if (semitone >= NOTE_TABLE_LOW && semitone <= NOTE_TABLE_HIGH)
        return note_table[semitone - NOTE_TABLE_LOW];
    return 220.0f * powf(2.0f, (float)semitone / 12.0f);
}

static float triangle_wave(float p)
{
    float v = wrap_unit(p);
    return 1.0f - 4.0f * fabsf(v - 0.5f);
}

static void audio_callback(void *userdata, uint8_t *stream, int len)
{
    AudioState *a = (AudioState *)userdata;
    int16_t *out = (int16_t *)stream;
    int frames = len / (int)(sizeof(int16_t) * 2);
    static const int mood_chords[4][8][3] = {
        { /* Reconnaissance: measured, curious, unresolved. */
            {0,4,7}, {-2,2,7}, {-4,0,5}, {-2,2,5},
            {0,4,7}, {2,5,9}, {-2,2,7}, {-5,0,4}
        },
        { /* Tension: minor movement without a triumphant cadence. */
            {0,3,7}, {1,4,8}, {-1,2,6}, {-2,1,5},
            {0,3,7}, {-3,0,5}, {-1,2,6}, {-2,1,5}
        },
        { /* Mourning: a restrained descending field-hospital cue. */
            {0,3,7}, {-2,1,5}, {-4,0,3}, {-5,-2,2},
            {-4,0,3}, {-2,1,5}, {-5,-2,2}, {-7,-4,0}
        },
        { /* Armistice: slow open intervals and room for silence. */
            {0,5,7}, {-5,0,4}, {-2,5,7}, {0,4,7},
            {-5,0,4}, {-2,2,7}, {0,5,9}, {0,4,7}
        }
    };
    static const int mess_melody[32] = {
         7,  7,  9, 11, 12, 11,  9,  7,
         4,  4,  7,  9, 11,  9,  7,  4,
         0,  4,  7, 12, 11,  9,  7,  4,
         2,  5,  9,  7,  4,  2,  0,  0
    };
    static const int mess_bass[8] = {0, 7, 5, 7, 0, 4, 5, 7};
    /* Public-domain Amazing Grace melody, phrased for a solo chanter.  This
     * is our own synthesis, not audio or orchestration from a film score. */
    static const int pipe_melody[64] = {
         5,10,10,10,14,10,14,14,12,10,10,10, 7, 5, 5, 5,
         5,10,10,10,14,10,14,14,12,12,12,17,17,17,17,17,
        14,17,17,14,17,17,14,14,10,10,10, 5, 7,10,10,10,
        10, 7, 7, 5, 5, 5,10,10,14,10,14,14,12,10,10,10
    };
    int i;
    for (i = 0; i < frames; ++i) {
        float t = (float)a->sample_clock / 11025.0f;
        float cue_t = (float)a->cue_clock / 11025.0f;
        int mood_index = a->music_mood - MUSIC_RECON;
        float tempo = a->music_mood == MUSIC_TENSION ? 2.05f :
                      a->music_mood == MUSIC_MOURNING ? 0.92f :
                      a->music_mood == MUSIC_ARMISTICE ? 0.68f : 1.38f;
        int beat = (int)(cue_t * tempo);
        int bar = (beat / 4) & 7;
        float pad = 0.0f, bass, melody, drums = 0.0f, sample;
        int v;
        a->noise = a->noise * 1664525u + 1013904223u;
        if (a->scene == SCENE_SORTIE) {
            float speed = (a->engine_speed - 62.0f) / 80.0f;
            float noise_sample = (float)((a->noise >> 16) & 0xffff) /
                                 32768.0f - 1.0f;
            float engine_hz;
            if (speed < 0.0f) speed = 0.0f;
            if (speed > 1.0f) speed = 1.0f;
            engine_hz = 46.0f + speed * 38.0f + a->engine_load * 7.0f;
            a->engine_phase += engine_hz / 11025.0f;
            if (a->engine_phase >= 1.0f) a->engine_phase -= 1.0f;
            a->engine_sub_phase += engine_hz * 0.5f / 11025.0f;
            if (a->engine_sub_phase >= 1.0f) a->engine_sub_phase -= 1.0f;
            sample = triangle_wave(a->engine_phase) * (0.17f + speed * 0.09f);
            /* This must have its own continuous phase. Deriving a half-cycle
             * from engine_phase inverted its slope at every wrap: the tick. */
            sample += fast_sin_cycles(a->engine_sub_phase) * 0.11f;
            sample += noise_sample * (0.012f + speed * 0.035f); /* slipstream */
            if (a->other_engine > 0.001f) {
                float hz = 53.0f + a->other_pitch * 31.0f;
                a->other_phase += hz / 11025.0f;
                if (a->other_phase >= 1.0f) a->other_phase -= 1.0f;
                sample += triangle_wave(a->other_phase) * a->other_engine * 0.16f;
            }
            if (a->vehicle_engine > 0.001f) {
                a->vehicle_phase += 29.0f / 11025.0f;
                if (a->vehicle_phase >= 1.0f) a->vehicle_phase -= 1.0f;
                sample += triangle_wave(a->vehicle_phase) *
                          a->vehicle_engine * 0.13f;
            }
        } else if (a->music_mood == MUSIC_MARCH &&
                   a->music_data && a->music_len >= 2) {
            uint32_t total_frames = a->music_len / 2;
            uint32_t frame = a->music_pos / 2;
            uint32_t fade_frames = total_frames > 8192 ? 2048 : total_frames / 4;
            int16_t *music = (int16_t *)(a->music_data + a->music_pos);
            float music_sample = (float)music[0] / 32768.0f;
            if (fade_frames > 0 && frame >= total_frames - fade_frames) {
                uint32_t intro_frame = frame - (total_frames - fade_frames);
                int16_t *intro = (int16_t *)(a->music_data + intro_frame * 2);
                float intro_sample = (float)intro[0] / 32768.0f;
                float crossfade = (float)intro_frame / (float)fade_frames;
                music_sample = music_sample * (1.0f - crossfade) +
                               intro_sample * crossfade;
            }
            sample = music_sample * 0.48f;
            a->music_pos += 2;
            if (a->music_pos + 2 > a->music_len) a->music_pos = 0;
        } else if (a->music_mood == MUSIC_MESS) {
            if (a->mess_data && a->mess_len >= 2) {
                uint32_t total_frames = a->mess_len / 2;
                uint32_t frame = a->mess_pos / 2;
                uint32_t fade_frames = total_frames > 8192 ?
                                       2048 : total_frames / 4;
                int16_t *music = (int16_t *)(a->mess_data + a->mess_pos);
                float music_sample = (float)music[0] / 32768.0f;
                /* The source finishes close to silence but not at the same
                 * waveform phase as its opening.  Crossfade the final 186 ms
                 * into the beginning so a long debrief cannot expose a click
                 * at the loop boundary. */
                if (fade_frames > 0 && frame >= total_frames - fade_frames) {
                    uint32_t intro_frame =
                        frame - (total_frames - fade_frames);
                    int16_t *intro =
                        (int16_t *)(a->mess_data + intro_frame * 2);
                    float intro_sample = (float)intro[0] / 32768.0f;
                    float crossfade =
                        (float)intro_frame / (float)fade_frames;
                    music_sample = music_sample * (1.0f - crossfade) +
                                   intro_sample * crossfade;
                }
                sample = music_sample * 0.52f;
                a->mess_pos += 2;
                if (a->mess_pos + 2 > a->mess_len) a->mess_pos = 0;
            } else {
                /* Original synth fallback when the supplied recording is not
                 * installed. */
                float position = cue_t * 4.35f;
                int step = (int)position & 31;
                int bass_step = ((int)(position * 0.5f)) & 7;
                float melody_hz = note_hz(mess_melody[step] + 7);
                float bass_hz = note_hz(mess_bass[bass_step] - 17);
                float beat_phase = wrap_unit(position);
                float phrase = 0.72f + 0.28f *
                               fast_sin_cycles(0.5f * beat_phase);
                a->synth_phase[4] += melody_hz / 11025.0f;
                a->synth_phase[3] += bass_hz / 11025.0f;
                if (a->synth_phase[4] >= 1.0f) a->synth_phase[4] -= 1.0f;
                if (a->synth_phase[3] >= 1.0f) a->synth_phase[3] -= 1.0f;
                sample = (fast_sin_cycles(a->synth_phase[4]) * 0.14f +
                          triangle_wave(a->synth_phase[4]) * 0.055f) * phrase;
                sample += triangle_wave(a->synth_phase[3]) * 0.105f;
            }
        } else if (a->music_mood == MUSIC_PIPER) {
            if (a->piper_data && a->piper_pos + 2 <= a->piper_len) {
                int16_t *pipes = (int16_t *)(a->piper_data + a->piper_pos);
                float piper_time = (float)a->piper_pos / 22050.0f;
                float memorial_gain = piper_time < 0.45f ?
                                      piper_time / 0.45f : 1.0f;
                if (piper_time > 15.0f)
                    memorial_gain = (19.0f - piper_time) / 4.0f;
                if (memorial_gain < 0.0f) memorial_gain = 0.0f;
                sample = (float)pipes[0] / 32768.0f;
                sample *= 0.64f * memorial_gain;
                a->piper_pos += 2;
            } else if (a->piper_data) {
                /* One respectful performance, then silence. */
                sample = 0.0f;
            } else {
                float after_silence = cue_t - 0.85f;
                if (after_silence <= 0.0f) {
                    sample = 0.0f;
                } else {
                int pipe_step = (int)(after_silence * 2.15f) & 63;
                float vibrato = fast_sin_cycles(after_silence * 5.15f) * 0.0028f;
                float chanter_hz = note_hz(pipe_melody[pipe_step]) *
                                   (1.0f + vibrato);
                float drone_d = note_hz(-7);
                float drone_a = note_hz(0);
                float attack = after_silence / 1.15f;
                float breath = (float)((a->noise >> 16) & 0xffff) /
                               32768.0f - 1.0f;
                if (attack > 1.0f) attack = 1.0f;
                a->synth_phase[0] += drone_d / 11025.0f;
                a->synth_phase[1] += drone_a / 11025.0f;
                a->synth_phase[4] += chanter_hz / 11025.0f;
                if (a->synth_phase[0] >= 1.0f) a->synth_phase[0] -= 1.0f;
                if (a->synth_phase[1] >= 1.0f) a->synth_phase[1] -= 1.0f;
                if (a->synth_phase[4] >= 1.0f) a->synth_phase[4] -= 1.0f;
                sample = triangle_wave(a->synth_phase[0]) * 0.095f +
                         triangle_wave(a->synth_phase[1]) * 0.055f +
                         triangle_wave(a->synth_phase[4]) * 0.205f +
                         fast_sin_cycles(2.0f * a->synth_phase[4]) *
                         0.055f + breath * 0.006f;
                sample *= attack;
                }
            }
        } else {
            if (mood_index < 0 || mood_index > 3) mood_index = 0;
            for (v = 0; v < 3; ++v) {
                float f = note_hz(mood_chords[mood_index][bar][v]);
                a->synth_phase[v] += f / 11025.0f;
                if (a->synth_phase[v] >= 1.0f) a->synth_phase[v] -= 1.0f;
                pad += triangle_wave(a->synth_phase[v]) * 0.072f;
            }
            {
                float bass_hz = note_hz(mood_chords[mood_index][bar][0] - 12);
                float melody_hz = note_hz(
                    mood_chords[mood_index][bar][(beat / 2) % 3] + 12);
                a->synth_phase[3] += bass_hz / 11025.0f;
                a->synth_phase[4] += melody_hz / 11025.0f;
                if (a->synth_phase[3] >= 1.0f) a->synth_phase[3] -= 1.0f;
                if (a->synth_phase[4] >= 1.0f) a->synth_phase[4] -= 1.0f;
            }
            bass = triangle_wave(a->synth_phase[3]) * 0.13f;
            melody = fast_sin_cycles(a->synth_phase[4]) * 0.07f;
            if (a->music_mood == MUSIC_TENSION && (beat & 1) == 0 &&
                wrap_unit(cue_t * tempo) < 0.09f)
                drums = ((float)((a->noise >> 16) & 0xffff) / 32768.0f - 1.0f) * 0.08f;
            sample = pad * (a->music_mood == MUSIC_ARMISTICE ? 0.48f : 0.72f) +
                     bass * 0.42f + melody * 0.36f + drums;
        }

        /* No synthetic needle ticks or periodic drum impulses here.  The
         * gramophone character is continuous filtering and low-level hiss;
         * rhythmic transients were being heard as an irritating clock tick. */
        if (a->scene != SCENE_SORTIE && a->music_mood != MUSIC_PIPER) {
            /* Narrow-band acoustic horn / shellac response.  This is a
             * continuous filter plus a low bed of surface hiss—not clicks. */
            float filtered;
            float hiss = (float)((a->noise >> 16) & 0xffff) / 32768.0f - 1.0f;
            a->gramophone_lp += 0.22f * (sample - a->gramophone_lp);
            filtered = a->gramophone_lp;
            a->gramophone_hp = 0.985f *
                (a->gramophone_hp + filtered - a->gramophone_previous);
            a->gramophone_previous = filtered;
            sample = a->gramophone_hp * 1.12f + hiss * 0.0045f;
        }
        if (a->shot > 0) {
            int gun_phase = a->shot % 540;
            if (gun_phase < 100) {
                float env = (100.0f - (float)gun_phase) / 100.0f;
                float crack = (float)((a->noise >> 15) & 0x1ffff) / 65536.0f - 1.0f;
                sample += crack * env * 0.42f;
                sample += ((gun_phase & 16) ? 0.22f : -0.18f) * env;
            }
            --a->shot;
        }
        if (a->blast > 0) {
            float env = (float)a->blast / 7000.0f;
            float roar = (float)((a->noise >> 16) & 0xffff) / 32768.0f - 1.0f;
            if (env > 1.0f) env = 1.0f;
            sample += fast_sin_cycles(t * 49.0f) * env * 0.36f;
            sample += roar * env * 0.31f;
            --a->blast;
        }
        if (a->crash > 0) {
            int elapsed = 7200 - a->crash;
            float wreck = (float)((a->noise >> 14) & 0x3ffff) /
                          131072.0f - 1.0f;
            float first_break = elapsed < 230 ?
                                (float)(230 - elapsed) / 230.0f : 0.0f;
            float second_break = elapsed >= 260 && elapsed < 610 ?
                (float)(610 - elapsed) / 350.0f : 0.0f;
            float slide = elapsed < 2400 ?
                          (float)(2400 - elapsed) / 2400.0f : 0.0f;
            /* A wooden-and-fabric airframe folds and scrapes; this is not an
             * explosive blast.  Several irregular structural breaks lead
             * into earth drag and a short metal/rigging shriek. */
            sample += wreck * (first_break * 0.43f +
                               second_break * 0.34f + slide * 0.19f);
            sample += fast_sin_cycles(t * 67.0f) *
                      first_break * 0.17f;
            sample += fast_sin_cycles(t * 347.0f) *
                      slide * slide * 0.12f;
            if ((elapsed < 170) || (elapsed > 330 && elapsed < 470))
                sample += ((elapsed & 20) ? 0.22f : -0.18f) *
                          (first_break + second_break);
            --a->crash;
        }
        if (a->hit > 0) {
            float env = (float)a->hit / (a->hit_player ? 1400.0f : 950.0f);
            float strike = (float)((a->noise >> 15) & 0x1ffff) /
                           65536.0f - 1.0f;
            if (env > 1.0f) env = 1.0f;
            if (a->hit_player) {
                sample += fast_sin_cycles(t * 73.0f) * env * 0.27f;
                sample += strike * env * 0.30f;
            } else {
                sample += fast_sin_cycles(t * 186.0f) * env * 0.18f;
                sample += strike * env * 0.22f;
            }
            --a->hit;
        }
        if (sample > 1.0f) sample = 1.0f;
        if (sample < -1.0f) sample = -1.0f;
        out[i * 2] = (int16_t)(sample * 18000.0f);
        out[i * 2 + 1] = out[i * 2];
        ++a->sample_clock;
        ++a->cue_clock;
    }
}

static int music_mood_for_scene(const Game *g, Scene scene)
{
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    if (scene == SCENE_TITLE || scene == SCENE_MENU) return MUSIC_MARCH;
    if (scene == SCENE_EPILOGUE) return MUSIC_ARMISTICE;
    if (scene == SCENE_DEBRIEF)
        return g->success ? MUSIC_MESS :
               (g->fatal_sortie && g->lives <= 0) ?
               MUSIC_PIPER : MUSIC_MOURNING;
    if (scene == SCENE_SORTIE)
        return MUSIC_TENSION; /* Muted: player-controlled scenes never score. */
    if (g->chapter == CHAPTER_COUNT - 1 && g->mission >= 6)
        return MUSIC_ARMISTICE;
    if (is_perspective_mode(mode) || g->chapter >= 2)
        return MUSIC_TENSION;
    return MUSIC_RECON;
}

static void set_scene(Game *g, Scene scene)
{
    MusicMood previous_mood = (MusicMood)g->audio.music_mood;
    if (scene == SCENE_DEBRIEF && g->health <= 0) g->fatal_sortie = 1;
    if (scene == SCENE_DEBRIEF && g->mission_save_state == 0)
        save_mission_result(g);
    g->scene = scene;
    g->scene_started = SDL_GetTicks();
    g->redraw_static = 1;
    if (g->audio.device) {
        SDL_LockAudioDevice(g->audio.device);
        g->audio.scene = scene;
        g->audio.music_mood = music_mood_for_scene(g, scene);
        g->audio.cue_clock = 0;
        if (previous_mood != (MusicMood)g->audio.music_mood) {
            memset(g->audio.synth_phase, 0, sizeof(g->audio.synth_phase));
            g->audio.gramophone_lp = 0.0f;
            g->audio.gramophone_hp = 0.0f;
            g->audio.gramophone_previous = 0.0f;
        }
        if (scene != SCENE_SORTIE) {
            g->audio.shot = 0;
            g->audio.blast = 0;
            g->audio.engine_speed = 0.0f;
            g->audio.engine_load = 0.0f;
            g->audio.other_engine = 0.0f;
            g->audio.vehicle_engine = 0.0f;
        }
        if (scene == SCENE_TITLE) g->audio.music_pos = 0;
        if (scene == SCENE_DEBRIEF && g->success) g->audio.mess_pos = 0;
        if (scene == SCENE_DEBRIEF && !g->success &&
            g->fatal_sortie && g->lives <= 0)
            g->audio.piper_pos = 0;
        SDL_UnlockAudioDevice(g->audio.device);
    }
}

static void draw_vector_plane(SDL_Surface *s, int x, int y, int enemy)
{
    uint32_t dark = rgb(s, 44, 34, 28);
    uint32_t body = enemy ? rgb(s, 132, 36, 31) : rgb(s, 52, 89, 58);
    uint32_t canvas = enemy ? rgb(s, 202, 170, 138) : rgb(s, 222, 205, 158);
    rect(s, x - 18, y - 2, 34, 5, body);
    rect(s, x - 9, y - 10, 25, 3, canvas);
    rect(s, x - 12, y + 7, 28, 3, canvas);
    line(s, x - 7, y - 7, x - 10, y + 7, dark);
    line(s, x + 9, y - 7, x + 11, y + 7, dark);
    rect(s, x - 22, y - 8, 5, 12, body);
    circle(s, x + 18, y, 4, dark);
    circle(s, x + 18, y, 2, canvas);
}

static void draw_plane(Game *g, int x, int y, int enemy, int pitch_frame)
{
    SDL_Surface *atlas = enemy ? g->enemy_sprites : g->player_sprites;
    SDL_Surface *dst = g->screen;
    int dx, dy;
    const int source_w = 96, source_h = 48;
    const int draw_w = 72, draw_h = 36;
    if (!atlas) {
        draw_vector_plane(dst, x, y, enemy);
        return;
    }
    if (pitch_frame < 0) pitch_frame = 0;
    if (pitch_frame > 4) pitch_frame = 4;
    if (SDL_MUSTLOCK(atlas)) SDL_LockSurface(atlas);
    for (dy = 0; dy < draw_h; ++dy) {
        int sy = dy * source_h / draw_h;
        int ty = y - draw_h / 2 + dy;
        for (dx = 0; dx < draw_w; ++dx) {
            int sx = pitch_frame * source_w + dx * source_w / draw_w;
            int tx = x - draw_w / 2 + dx;
            uint8_t r, green, b;
            uint32_t source = surface_pixel(atlas, sx, sy);
            unpack_rgb(atlas->format, source, &r, &green, &b);
            if (r > 180 && b > 180 && green < 130) continue;
            pixel(dst, tx, ty, atlas->format->format == dst->format->format ?
                  source : rgb(dst, r, green, b));
        }
    }
    if (SDL_MUSTLOCK(atlas)) SDL_UnlockSurface(atlas);
}

static int terrain_y(float world_x)
{
    return 358 + (int)(8.0f * sinf(world_x * 0.007f)
                     + 4.0f * sinf(world_x * 0.019f));
}

static int current_mission_goal(const Game *g)
{
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    if (is_perspective_mode(mode)) {
        int goal = 1 + g->chapter / 2 + g->mission / 6;
        if (goal > chapters[g->chapter].enemy_count)
            goal = chapters[g->chapter].enemy_count;
        if (mode == MISSION_GUNNER_3D && goal > 2) goal = 2;
        if (mode == MISSION_DOGFIGHT_3D && goal > 4) goal = 4;
        return goal;
    }
    return chapters[g->chapter].goal;
}

static int player_machine_gun_damage(const Game *g)
{
    int enemy_hits = 3 + g->chapter / 2;
    int player_hits = enemy_hits * PLAYER_DURABILITY_MULTIPLIER;
    int damage = player_hits > 0 ? 100 / player_hits : 4;
    /* Integer damage must never disappear.  Rounding down gives Neddy at
     * least eight enemy-airframe lifetimes rather than quietly undercutting
     * the promise in later chapters. */
    return damage > 0 ? damage : 1;
}

static void add_dogfight_bullet_hole(Game *g, uint32_t seed)
{
    BulletHole *hole = &g->bullet_holes[g->next_bullet_hole];
    uint32_t scatter = scene_hash(seed ^
        (uint32_t)g->next_bullet_hole * 2654435761u);
    int zone = (int)(scatter % 3u);
    /* Hits accumulate on visible player structure, not in the empty sky:
     * port wing, starboard wing or the cockpit/cowling. */
    if (zone == 0) {
        hole->x = 24.0f + (float)((scatter >> 4) % 154u);
        hole->y = 284.0f + (float)((scatter >> 13) % 72u);
    } else if (zone == 1) {
        hole->x = 462.0f + (float)((scatter >> 4) % 154u);
        hole->y = 284.0f + (float)((scatter >> 13) % 72u);
    } else {
        hole->x = 214.0f + (float)((scatter >> 4) % 212u);
        hole->y = 252.0f + (float)((scatter >> 13) % 112u);
    }
    hole->active = 1;
    g->next_bullet_hole =
        (g->next_bullet_hole + 1) % MAX_BULLET_HOLES;
}

static int perspective_enemy_total(const Game *g)
{
    return current_mission_goal(g);
}

/* Every enemy machine brought down counts towards the goal, whether it fell
 * to our guns, flew into the ground or was lost in a collision. */
static void credit_air_kill(Game *g)
{
    ++g->kills;
    g->score += 250;
}

/* Rear attackers make a single pass, so spare machines give the gunner
 * room to miss one without the sortie becoming unwinnable.  In the unarmed
 * 1914 sorties one enemy observer shares the sky and nobody fires. */
static int perspective_spawn_count(const Game *g)
{
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    int goal = current_mission_goal(g);
    int count;
    if (!is_perspective_mode(mode)) return 0;
    if (!chapters[g->chapter].armed) return 1;
    count = mode == MISSION_GUNNER_3D ? goal + 2 : goal;
    if (count > MAX_ENEMIES) count = MAX_ENEMIES;
    return count;
}

static void reset_sortie(Game *g)
{
    int i;
    int perspective_enemy_count;
    const Chapter *chapter = &chapters[g->chapter];
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    g->mission_save_state = 0;
    g->fatal_sortie = 0;
    g->life_lost_this_sortie = 0;
    apply_story_wingmen(g);
    if (mode == MISSION_TOP_DOWN) {
        g->player_x = 320.0f;
        g->player_y = 310.0f;
    } else if (mode == MISSION_GUNNER_3D) {
        /* Begin clear of the fin so the first burst cannot hit our own tail. */
        g->player_x = 238.0f;
        g->player_y = 126.0f;
    } else if (is_perspective_mode(mode)) {
        g->player_x = 320.0f;
        g->player_y = 190.0f;
    } else {
        g->player_x = 150.0f;
        g->player_y = 265.0f;
    }
    g->player_vy = 0.0f;
    if (mode == MISSION_GUNNER_3D) {
        g->gunner_view_x = 320.0f + (g->player_x - 320.0f) * 0.80f;
        g->gunner_view_y = 235.0f - gunner_aim_elevation(g) * 90.0f;
    } else {
        g->gunner_view_x = g->player_x;
        g->gunner_view_y = g->player_y;
    }
    g->view_bank = 0.0f;
    g->view_pitch = 0.0f;
    g->flight_x = 0.0f;
    g->flight_y = 620.0f;
    g->flight_z = 0.0f;
    g->flight_heading = 0.0f;
    g->flight_roll_rate = 0.0f;
    g->flight_pitch_rate = 0.0f;
    g->flight_vx = 0.0f;
    g->flight_vy = 0.0f;
    g->flight_vz = 43.81f;
    g->flight_throttle = 0.68f;
    flight_state_init(&g->flight, 0.0f, 0.0f, 0.0f, 43.81f,
                      g->flight_throttle);
    g->flight_alpha = 0.0f;
    g->flight_stalled = 0;
    g->flight_doomed = 0;
    g->flight_doomed_started = 0;
    g->flight_crashed = 0;
    g->flight_crash_started = 0;
    g->airspeed = 98.0f;
    g->scroll = 0.0f;
    g->fuel = 100.0f;
    g->health = 100;
    g->tail_integrity = 100;
    g->score = 0;
    g->kills = 0;
    g->objectives = 0;
    g->ammunition = 260 - g->chapter * 13;
    if (g->ammunition < 160) g->ammunition = 160;
    g->ammunition += g->ammunition / 2;
    g->bombs_remaining = 12 - g->chapter / 2;
    if (g->bombs_remaining < 8) g->bombs_remaining = 8;
    g->last_shot = 0;
    g->last_bomb = 0;
    g->last_damage = 0;
    g->mission_clear_at = 0;
    memset(g->bullets, 0, sizeof(g->bullets));
    memset(g->dogfight_particles, 0, sizeof(g->dogfight_particles));
    memset(g->explosions, 0, sizeof(g->explosions));
    memset(g->bullet_holes, 0, sizeof(g->bullet_holes));
    g->next_bullet_hole = 0;
    g->audio.engine_speed = g->airspeed;
    g->audio.engine_load = 0.0f;
    g->audio.other_engine = 0.0f;
    g->audio.other_pitch = 0.0f;
    g->audio.vehicle_engine = 0.0f;
    g->audio.crash = 0;
    g->audio.hit = 0;
    g->audio.hit_player = 0;
    perspective_enemy_count = perspective_spawn_count(g);
    for (i = 0; i < MAX_ENEMIES; ++i) {
        g->enemies[i].x = 760.0f + i * 620.0f;
        g->enemies[i].y = 90.0f + (float)((i * 47) % 120);
        g->enemies[i].phase = i * 0.8f;
        /* Staggered WWI engagements, not a modern fighter formation.  The
         * first machine approaches nearly head-on, the next crosses our
         * nose, and the later aircraft enter from wider offsets. */
        g->enemies[i].world_x = (float)((i & 1) ? 145 + i * 72 : -85 - i * 88);
        g->enemies[i].world_y = 600.0f + (float)((i * 57) % 150);
        g->enemies[i].world_z = 820.0f + (float)i * 470.0f;
        if (i == 0)
            g->enemies[i].heading = 2.92f;
        else if (i == 1)
            g->enemies[i].heading = -2.62f;
        else
            g->enemies[i].heading = (i & 1) ? -2.25f : 2.35f;
        g->enemies[i].pitch = 0.0f;
        g->enemies[i].bank = 0.0f;
        g->enemies[i].speed = 84.0f + (float)((i * 13) % 24);
        g->enemies[i].vx = sinf(g->enemies[i].heading) *
                           g->enemies[i].speed * 0.58f;
        g->enemies[i].vy = 0.0f;
        g->enemies[i].vz = cosf(g->enemies[i].heading) *
                           g->enemies[i].speed * 0.58f;
        g->enemies[i].roll_rate = 0.0f;
        g->enemies[i].pitch_rate = 0.0f;
        g->enemies[i].throttle = 0.67f + (float)(i & 1) * 0.08f;
        flight_state_init(&g->enemies[i].flight, g->enemies[i].heading,
                          g->enemies[i].pitch, g->enemies[i].bank,
                          g->enemies[i].speed * 0.44704f,
                          g->enemies[i].throttle);
        /* Even the earliest aircraft must first show visible battle damage.
         * Later years add durability, but nothing goes straight from pristine
         * to a crash on the first round. */
        g->enemies[i].hp = 3 + g->chapter / 2;
        g->enemies[i].max_hp = g->enemies[i].hp;
        g->enemies[i].crashing = 0;
        g->enemies[i].crash_x = 0.0f;
        g->enemies[i].crash_y = 0.0f;
        g->enemies[i].crash_vx = 0.0f;
        g->enemies[i].crash_vy = 0.0f;
        g->enemies[i].crash_angle = 0.0f;
        g->enemies[i].crash_size = 0;
        g->enemies[i].active = is_perspective_mode(mode) &&
                               i < perspective_enemy_count;
        g->enemies[i].last_shot = SDL_GetTicks() + 650u + (uint32_t)i * 390u;
    }
    for (i = 0; i < MAX_TARGETS; ++i) {
        g->targets[i].world_x = 560.0f + i * 480.0f;
        g->targets[i].active = !is_perspective_mode(mode) &&
                               i < chapter->target_count;
        g->targets[i].type = g->chapter == 0 ? 0 :
                             (i % 3 == 2 ? 2 : i & 1);
        g->targets[i].last_shot = 0;
        g->targets[i].next_round = 0;
        g->targets[i].burst_left = 0;
    }
}

static Bullet *new_bullet(Game *g, float x, float y, float vx, float vy,
                          int enemy, int type, float life)
{
    int i;
    for (i = 0; i < MAX_BULLETS; ++i) {
        Bullet *b = &g->bullets[i];
        if (!b->active) {
            b->x = x; b->y = y; b->vx = vx; b->vy = vy;
            b->enemy = enemy; b->type = type;
            b->life = life; b->initial_life = life;
            b->active = 1;
            return b;
        }
    }
    return NULL;
}

static void new_dogfight_particle(Game *g, float x, float y,
                                  float vx, float vy, float life, int kind)
{
    int i;
    for (i = 0; i < MAX_DOGFIGHT_PARTICLES; ++i) {
        DogfightParticle *p = &g->dogfight_particles[i];
        if (!p->active) {
            p->x = x; p->y = y; p->vx = vx; p->vy = vy;
            p->life = life; p->initial_life = life;
            p->kind = kind; p->active = 1;
            return;
        }
    }
}

static void launch_dogfight_tracer(Game *g, float x, float y,
                                   float target_x, float target_y,
                                   float speed, int kind)
{
    float dx = target_x - x;
    float dy = target_y - y;
    float distance = sqrtf(dx * dx + dy * dy);
    float life;
    if (distance < 1.0f) distance = 1.0f;
    life = distance / speed + 0.055f;
    new_dogfight_particle(g, x, y, dx / distance * speed,
                          dy / distance * speed, life, kind);
}

static void update_dogfight_particles(Game *g, float dt)
{
    int i;
    for (i = 0; i < MAX_DOGFIGHT_PARTICLES; ++i) {
        DogfightParticle *p = &g->dogfight_particles[i];
        if (!p->active) continue;
        p->x += p->vx * dt;
        p->y += p->vy * dt;
        if (p->kind == 2) p->vy += 250.0f * dt;
        p->life -= dt;
        if (p->life <= 0.0f || p->x < -20.0f || p->x > SCREEN_W + 20.0f ||
            p->y < -20.0f || p->y > SCREEN_H + 24.0f)
            p->active = 0;
    }
}

static void new_explosion_at(Game *g, float x, float y, int large, int ground)
{
    int i;
    for (i = 0; i < MAX_EXPLOSIONS; ++i) {
        Explosion *e = &g->explosions[i];
        if (!e->active) {
            MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
            e->world_axis = ground ?
                (mode == MISSION_TOP_DOWN ? 1 : (mode == MISSION_SIDE ? 2 : 0)) : 0;
            e->x = e->world_axis == 2 ? g->scroll + x : x;
            e->y = e->world_axis == 1 ? g->scroll + SCREEN_H - y : y;
            e->age = 0.0f;
            e->duration = large == 3 ? 3.15f :
                          (large == 2 ? 2.70f : (large ? 1.90f : 0.92f));
            e->large = large; e->active = 1;
            return;
        }
    }
}

static void new_explosion(Game *g, float x, float y, int large)
{
    new_explosion_at(g, x, y, large, 0);
}

static void new_ground_explosion(Game *g, float x, float y, int large)
{
    new_explosion_at(g, x, y, large, 1);
}

static void aimed_fire(Game *g, float x, float y, float speed, int type)
{
    float dx = g->player_x - x;
    float dy = g->player_y - y;
    float length = sqrtf(dx * dx + dy * dy);
    if (length < 1.0f) length = 1.0f;
    if (new_bullet(g, x, y, dx / length * speed, dy / length * speed,
                   1, type, type == 2 ? 2.8f : 3.5f)) {
        if (type == 2) g->audio.blast = 5200;
        else g->audio.shot = 1700;
    }
}

static void artillery_fire(Game *g, Target *target, float x, float y,
                           uint32_t now)
{
    const float gravity = 92.0f;
    const float flight_time = 1.65f;
    uint32_t seed = scene_hash((uint32_t)target->world_x ^ now ^
                               (uint32_t)(g->chapter + 1) * 2654435761u);
    float error_x = ((float)(seed & 255u) - 127.0f) * 0.34f;
    float error_y = ((float)((seed >> 8) & 255u) - 127.0f) * 0.18f;
    float aim_x = g->player_x + error_x;
    float aim_y = g->player_y + error_y;
    float vx = (aim_x - x) / flight_time;
    float vy = (aim_y - y - 0.5f * gravity * flight_time * flight_time) /
               flight_time;
    if (new_bullet(g, x, y, vx, vy, 1, 2, flight_time + 0.08f))
        g->audio.blast = 2600;
}

static void fire(Game *g)
{
    uint32_t now = SDL_GetTicks();
    if (g->ammunition <= 0) return;
    if (now - g->last_shot < 130) return;
    if (new_bullet(g, g->player_x + 34.0f, g->player_y - 1.0f,
                   390.0f, g->player_vy * 0.12f, 0, 0, 2.0f)) {
        g->last_shot = now;
        --g->ammunition;
        g->audio.shot = 1700;
    }
}

static void drop_bomb(Game *g)
{
    uint32_t now = SDL_GetTicks();
    if (g->bombs_remaining <= 0) return;
    if (now - g->last_bomb < 550) return;
    if (new_bullet(g, g->player_x, g->player_y + 16.0f,
                   42.0f, 55.0f, 0, 1, 4.0f)) {
        g->last_bomb = now;
        --g->bombs_remaining;
    }
}

static int top_down_target_x(int index)
{
    return 82 + (int)(scene_hash((uint32_t)index * 2654435761u) % 476u);
}

static void top_down_target_position(const Game *g, int index,
                                     float *sx, float *sy)
{
    const Target *target = &g->targets[index];
    *sx = (float)top_down_target_x(index);
    *sy = SCREEN_H - (target->world_x - g->scroll);
    if (target->type == 2) {
        float phase = g->scroll * 0.0042f + index * 1.73f;
        *sx = 320.0f + sinf(phase) * 226.0f;
        *sy += cosf(phase * 0.73f) * 14.0f;
    }
}

static float wrap_angle(float angle)
{
    while (angle > (float)M_PI) angle -= (float)(M_PI * 2.0);
    while (angle < (float)-M_PI) angle += (float)(M_PI * 2.0);
    return angle;
}

static Projected3D project_world_point(const Game *g, Vec3 point)
{
    Projected3D out;
    float dx = point.x - g->flight_x;
    float dy = point.y - g->flight_y;
    float dz = point.z - g->flight_z;
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    const float focal = 330.0f;
    if (mode == MISSION_DOGFIGHT_3D) {
        /* The flight quaternion is the camera attitude.  Transform the world
         * displacement straight back into body axes so the terrain, horizon,
         * clouds and aircraft all share exactly one frame of reference. */
        Vec3 body = flight_rotate_from_ned(&g->flight, dz, dx, -dy);
        out.depth = body.x;
        out.visible = body.x > 5.0f;
        if (out.visible) {
            out.x = 320 + (int)(body.y * focal / body.x);
            out.y = 190 + (int)(body.z * focal / body.x);
        } else {
            out.x = out.y = -10000;
        }
        return out;
    }
    {
    float ch = cosf(g->flight_heading);
    float sh = sinf(g->flight_heading);
    float cp = cosf(g->view_pitch);
    float sp = sinf(g->view_pitch);
    float cr = cosf(g->view_bank);
    float sr = sinf(g->view_bank);
    float camera_x = ch * dx - sh * dz;
    float forward = sh * dx + ch * dz;
    float camera_y = cp * dy - sp * forward;
    float camera_z = sp * dy + cp * forward;
    float rolled_x = cr * camera_x + sr * camera_y;
    float rolled_y = -sr * camera_x + cr * camera_y;
    out.depth = camera_z;
    out.visible = camera_z > 5.0f;
    if (out.visible) {
        out.x = 320 + (int)(rolled_x * focal / camera_z);
        out.y = 190 - (int)(rolled_y * focal / camera_z);
    } else {
        out.x = out.y = -10000;
    }
    }
    return out;
}

static Vec3 enemy_model_point(const Enemy *enemy, float x, float y, float z)
{
    Vec3 out;
    float cr = cosf(enemy->bank), sr = sinf(enemy->bank);
    float cp = cosf(enemy->pitch), sp = sinf(enemy->pitch);
    float ch = cosf(enemy->heading), sh = sinf(enemy->heading);
    float rolled_x = cr * x - sr * y;
    float rolled_y = sr * x + cr * y;
    float pitched_y = cp * rolled_y - sp * z;
    float pitched_z = sp * rolled_y + cp * z;
    out.x = enemy->world_x + ch * rolled_x + sh * pitched_z;
    out.y = enemy->world_y + pitched_y;
    out.z = enemy->world_z - sh * rolled_x + ch * pitched_z;
    return out;
}

static float flight_clamp(float value, float low, float high)
{
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

static float flight_approach(float value, float target, float amount)
{
    if (value < target) {
        value += amount;
        if (value > target) value = target;
    } else if (value > target) {
        value -= amount;
        if (value < target) value = target;
    }
    return value;
}

static void flight_quaternion_from_euler(FlightState *f, float bank,
                                         float pitch, float heading)
{
    float cr = cosf(bank * 0.5f), sr = sinf(bank * 0.5f);
    float cp = cosf(pitch * 0.5f), sp = sinf(pitch * 0.5f);
    float cy = cosf(heading * 0.5f), sy = sinf(heading * 0.5f);
    f->qw = cr * cp * cy + sr * sp * sy;
    f->qx = sr * cp * cy - cr * sp * sy;
    f->qy = cr * sp * cy + sr * cp * sy;
    f->qz = cr * cp * sy - sr * sp * cy;
}

static Vec3 flight_rotate_to_ned(const FlightState *f, float x, float y,
                                 float z)
{
    Vec3 out;
    float xx = f->qx * f->qx, yy = f->qy * f->qy, zz = f->qz * f->qz;
    float xy = f->qx * f->qy, xz = f->qx * f->qz, yz = f->qy * f->qz;
    float wx = f->qw * f->qx, wy = f->qw * f->qy, wz = f->qw * f->qz;
    out.x = (1.0f - 2.0f * (yy + zz)) * x +
            2.0f * (xy - wz) * y + 2.0f * (xz + wy) * z;
    out.y = 2.0f * (xy + wz) * x +
            (1.0f - 2.0f * (xx + zz)) * y +
            2.0f * (yz - wx) * z;
    out.z = 2.0f * (xz - wy) * x + 2.0f * (yz + wx) * y +
            (1.0f - 2.0f * (xx + yy)) * z;
    return out;
}

static Vec3 flight_rotate_from_ned(const FlightState *f, float north,
                                   float east, float down)
{
    Vec3 out;
    float xx = f->qx * f->qx, yy = f->qy * f->qy, zz = f->qz * f->qz;
    float xy = f->qx * f->qy, xz = f->qx * f->qz, yz = f->qy * f->qz;
    float wx = f->qw * f->qx, wy = f->qw * f->qy, wz = f->qw * f->qz;
    out.x = (1.0f - 2.0f * (yy + zz)) * north +
            2.0f * (xy + wz) * east + 2.0f * (xz - wy) * down;
    out.y = 2.0f * (xy - wz) * north +
            (1.0f - 2.0f * (xx + zz)) * east +
            2.0f * (yz + wx) * down;
    out.z = 2.0f * (xz + wy) * north + 2.0f * (yz - wx) * east +
            (1.0f - 2.0f * (xx + yy)) * down;
    return out;
}

static void flight_euler(const FlightState *f, float *bank, float *pitch,
                         float *heading)
{
    float sin_pitch = 2.0f * (f->qw * f->qy - f->qz * f->qx);
    *bank = atan2f(2.0f * (f->qw * f->qx + f->qy * f->qz),
                   1.0f - 2.0f * (f->qx * f->qx + f->qy * f->qy));
    *pitch = asinf(flight_clamp(sin_pitch, -1.0f, 1.0f));
    *heading = atan2f(2.0f * (f->qw * f->qz + f->qx * f->qy),
                     1.0f - 2.0f * (f->qy * f->qy + f->qz * f->qz));
}

static void flight_state_init(FlightState *f, float heading, float pitch,
                              float bank, float speed, float throttle)
{
    memset(f, 0, sizeof(*f));
    f->u = speed;
    f->throttle = throttle;
    f->speed = speed;
    flight_quaternion_from_euler(f, bank, pitch, heading);
}

/* One 50 Hz step of the compact six-degree-of-freedom model.  This is the
 * JSBSim force/moment/state pipeline reduced to the terms which matter to a
 * fabric-covered WWI aeroplane.  Nothing writes an angle or a direction of
 * travel directly: control surfaces make moments, moments make body rates,
 * and the tilted lift vector makes the turn. */
static void ww1_flight_step(FlightState *f, Vec3 *position, float dt,
                            float aileron_command, float elevator_command)
{
    const float mass = 430.0f;
    const float ixx = 1050.0f, iyy = 1350.0f, izz = 1900.0f;
    const float wing_area = 21.46f, span = 8.53f, chord = 2.52f;
    const float rho = 1.10f, gravity = 9.80665f;
    const float stall_alpha = 0.285f;
    float speed, alpha, beta, dynamic_pressure, phat, qhat, rhat;
    float cl, cd, cy, roll_c, pitch_c, yaw_c, effectiveness = 1.0f;
    float lift, drag, side, fx, fy, fz, roll_m, pitch_m, yaw_m;
    float pdot, qdot, rdot, udot, vdot, wdot;
    float dw, dx, dy, dz, norm;
    Vec3 gravity_body, velocity_ned;

    /* The Camel's controls are direct cable runs.  Keyboard input therefore
     * reaches the requested surface position quickly (about 0.13 s), while
     * the aeroplane's angular momentum remains in p/q/r after release. */
    f->aileron = flight_approach(f->aileron, aileron_command, 7.5f * dt);
    f->elevator = flight_approach(f->elevator, elevator_command, 7.5f * dt);

    speed = sqrtf(f->u * f->u + f->v * f->v + f->w * f->w);
    if (speed < 1.0f) speed = 1.0f;
    alpha = atan2f(f->w, f->u);
    beta = asinf(flight_clamp(f->v / speed, -1.0f, 1.0f));
    /* A small coordinated-rudder assist replaces the pedals which do not fit
     * the four-key control scheme.  It counters adverse yaw; it never turns
     * the aeroplane by changing heading. */
    f->rudder = flight_approach(f->rudder,
        flight_clamp(0.38f * f->aileron - 1.8f * beta - 0.16f * f->r,
                     -1.0f, 1.0f), 1.65f * dt);

    dynamic_pressure = 0.5f * rho * speed * speed;
    phat = f->p * span / (2.0f * speed);
    qhat = f->q * chord / (2.0f * speed);
    rhat = f->r * span / (2.0f * speed);
    if (fabsf(alpha) > stall_alpha) {
        float excess = fabsf(alpha) - stall_alpha;
        effectiveness = flight_clamp(1.0f - excess * 2.1f, 0.16f, 1.0f);
    }

    cl = 0.18f + 4.35f * alpha + 0.18f * f->elevator * effectiveness;
    if (fabsf(alpha) > stall_alpha) {
        float sign = alpha < 0.0f ? -1.0f : 1.0f;
        float peak = 0.18f + 4.35f * stall_alpha * sign +
                     0.18f * f->elevator * effectiveness;
        float decay = flight_clamp(1.0f -
                      (fabsf(alpha) - stall_alpha) * 1.45f, 0.18f, 1.0f);
        cl = peak * decay;
    }
    cd = 0.052f + 0.078f * cl * cl + 0.22f * beta * beta +
         0.018f * (fabsf(f->aileron) + fabsf(f->elevator));
    if (fabsf(alpha) > stall_alpha)
        cd += 0.95f * (fabsf(alpha) - stall_alpha);
    cy = -0.72f * beta + 0.16f * f->rudder;

    roll_c = -0.075f * beta - 0.19f * phat +
             0.009f * f->aileron * effectiveness + 0.012f * f->rudder;
    pitch_c = -0.58f * alpha - 5.8f * qhat +
              0.095f * f->elevator * effectiveness;
    yaw_c = 0.115f * beta - 0.24f * rhat +
            0.065f * f->rudder - 0.018f * f->aileron * effectiveness;

    lift = dynamic_pressure * wing_area * cl;
    drag = dynamic_pressure * wing_area * cd;
    side = dynamic_pressure * wing_area * cy;
    fx = f->throttle * 2050.0f - drag * cosf(alpha) + lift * sinf(alpha);
    fy = side;
    fz = -drag * sinf(alpha) - lift * cosf(alpha);
    roll_m = dynamic_pressure * wing_area * span * roll_c;
    pitch_m = dynamic_pressure * wing_area * chord * pitch_c;
    yaw_m = dynamic_pressure * wing_area * span * yaw_c;

    pdot = (roll_m + (iyy - izz) * f->q * f->r) / ixx;
    qdot = (pitch_m + (izz - ixx) * f->p * f->r) / iyy;
    rdot = (yaw_m + (ixx - iyy) * f->p * f->q) / izz;
    f->p += pdot * dt;
    f->q += qdot * dt;
    f->r += rdot * dt;

    gravity_body = flight_rotate_from_ned(f, 0.0f, 0.0f, gravity);
    udot = fx / mass + gravity_body.x - (f->q * f->w - f->r * f->v);
    vdot = fy / mass + gravity_body.y - (f->r * f->u - f->p * f->w);
    wdot = fz / mass + gravity_body.z - (f->p * f->v - f->q * f->u);
    f->u += udot * dt;
    f->v += vdot * dt;
    f->w += wdot * dt;

    dw = -0.5f * (f->qx * f->p + f->qy * f->q + f->qz * f->r);
    dx =  0.5f * (f->qw * f->p + f->qy * f->r - f->qz * f->q);
    dy =  0.5f * (f->qw * f->q + f->qz * f->p - f->qx * f->r);
    dz =  0.5f * (f->qw * f->r + f->qx * f->q - f->qy * f->p);
    f->qw += dw * dt;
    f->qx += dx * dt;
    f->qy += dy * dt;
    f->qz += dz * dt;
    norm = sqrtf(f->qw * f->qw + f->qx * f->qx +
                 f->qy * f->qy + f->qz * f->qz);
    if (norm > 0.0001f) {
        f->qw /= norm; f->qx /= norm; f->qy /= norm; f->qz /= norm;
    }

    velocity_ned = flight_rotate_to_ned(f, f->u, f->v, f->w);
    position->x += velocity_ned.y * dt;
    position->y -= velocity_ned.z * dt;
    position->z += velocity_ned.x * dt;
    f->speed = sqrtf(f->u * f->u + f->v * f->v + f->w * f->w);
    f->alpha = atan2f(f->w, f->u);
    f->beta = asinf(flight_clamp(f->v / (f->speed > 1.0f ? f->speed : 1.0f),
                                 -1.0f, 1.0f));
    f->stalled = f->speed < 20.0f || fabsf(f->alpha) > stall_alpha;
}

static void ww1_flight_advance(FlightState *f, Vec3 *position, float frame_dt,
                               float aileron_command,
                               float elevator_command)
{
    const float step = 0.02f;
    int iterations = 0;
    if (frame_dt > 0.10f) frame_dt = 0.10f;
    f->accumulator += frame_dt;
    while (f->accumulator >= step && iterations < 5) {
        ww1_flight_step(f, position, step, aileron_command, elevator_command);
        f->accumulator -= step;
        ++iterations;
    }
}

static void flight_outputs(const FlightState *f, float *heading, float *pitch,
                           float *bank, float *vx, float *vy, float *vz,
                           float *speed_mph)
{
    Vec3 velocity_ned = flight_rotate_to_ned(f, f->u, f->v, f->w);
    flight_euler(f, bank, pitch, heading);
    *vx = velocity_ned.y;
    *vy = -velocity_ned.z;
    *vz = velocity_ned.x;
    *speed_mph = f->speed * 2.23694f;
}

static float enemy_depth_3d(const Game *g, const Enemy *enemy, int index)
{
    static const float closing_rate[4] = {1.05f, 1.48f, 0.84f, 1.22f};
    int behaviour = index & 3;
    float depth = enemy->x - g->scroll * closing_rate[behaviour];
    if (campaign_missions[g->chapter][g->mission].mode == MISSION_GUNNER_3D) {
        /* Rear attackers do not sit in a formation.  Each works in and out of
         * range independently, briefly pulling away before closing again. */
        depth += sinf(g->scroll * (0.0043f + index * 0.0007f) +
                      enemy->phase) * (260.0f + index * 95.0f);
    }
    return depth;
}

static int project_enemy_3d(const Game *g, const Enemy *enemy, int index,
                            int *sx, int *sy, int *size)
{
    float depth = enemy_depth_3d(g, enemy, index);
    float approach, close, past;
    float side = (index & 1) ? 1.0f : -1.0f;
    int path = index & 3;
    if (depth < -520.0f || depth > 3300.0f) return 0;
    if (depth >= 90.0f) {
        int flight_x;
        int flight_y;
        approach = (3300.0f - depth) / 3210.0f;
        if (approach < 0.0f) approach = 0.0f;
        if (approach > 1.0f) approach = 1.0f;
        if (approach < 0.035f) return 0;
        close = depth < 760.0f ? (760.0f - depth) / 670.0f : 0.0f;
        *size = (int)(10.0f + powf(approach, 1.22f) * 98.0f);
        flight_x = 320 + (int)(sinf(enemy->phase + g->scroll * 0.0025f) *
                               (38.0f + approach * 145.0f));
        flight_y = 176 + (int)((enemy->y - 150.0f) * 0.30f +
                               cosf(enemy->phase * 1.7f +
                                    g->scroll * 0.0018f) *
                               (11.0f + approach * 35.0f));
        if (campaign_missions[g->chapter][g->mission].mode ==
            MISSION_GUNNER_3D && approach < 0.30f) {
            float entry = (approach - 0.035f) / 0.265f;
            int entry_x = (index & 1) ? SCREEN_W + 42 : -42;
            int entry_y = (index & 1) ? 275 : 18;
            if (entry < 0.0f) entry = 0.0f;
            if (entry > 1.0f) entry = 1.0f;
            /* Smooth turn-in: dive from high port or catch from low
             * starboard, rather than appearing at a combat position. */
            entry = entry * entry * (3.0f - 2.0f * entry);
            *sx = entry_x + (int)((flight_x - entry_x) * entry);
            *sy = entry_y + (int)((flight_y - entry_y) * entry);
        } else {
            *sx = flight_x;
            *sy = flight_y;
        }
        if (path == 0) {
            *sx += (int)(side * close * close * 112.0f);
            *sy += (int)(close * close * 42.0f);
        } else if (path == 1) {
            *sx -= (int)(side * close * 178.0f);
            *sy += (int)(close * close * 168.0f - (1.0f - close) * 46.0f);
        } else if (path == 2) {
            *sx += (int)(side * close * close * 330.0f);
            *sy -= (int)(close * close * 112.0f);
        } else {
            *sx += (int)(side * (55.0f + (1.0f - approach) * 135.0f));
            *sy -= (int)((1.0f - approach) * 64.0f);
        }
    } else {
        past = (90.0f - depth) / 610.0f;
        if (past > 1.0f) past = 1.0f;
        *size = 108 + (int)(past * 52.0f);
        if (path == 1) {
            *sx = 320 - (int)(side * (220.0f + past * 560.0f));
            *sy = 212 + (int)(past * 135.0f);
        } else {
            *sx = 320 + (int)(side * (205.0f + past * 610.0f));
            *sy = 142 - (int)(past * 290.0f);
        }
    }
    *sx -= (int)(g->view_bank * 185.0f);
    *sy += (int)(g->view_pitch * 150.0f);
    if (campaign_missions[g->chapter][g->mission].mode == MISSION_GUNNER_3D) {
        /* The observer swivels through the aft 90-degree arc.  Moving the
         * view must move the world, rather than sliding a sight over a fixed
         * postcard.  Elevation likewise pitches the view with the Lewis gun. */
        *sx -= (int)(gunner_traverse(g) * 306.0f);
        *sy += (int)(gunner_elevation(g) * 118.0f);
    }
    return 1;
}

static void finish_alternate_sortie(Game *g)
{
    if (is_perspective_mode(campaign_missions[g->chapter][g->mission].mode))
        g->success = g->kills >= current_mission_goal(g) &&
                     g->health > 0 && g->fuel > 0;
    else
        g->success = g->objectives >= current_mission_goal(g) &&
                     g->health > 0 && g->fuel > 0;
    if (g->health <= 0 || g->tail_integrity <= 0) g->fatal_sortie = 1;
    set_scene(g, SCENE_DEBRIEF);
}

static void age_explosions(Game *g, float dt)
{
    int i;
    for (i = 0; i < MAX_EXPLOSIONS; ++i) if (g->explosions[i].active) {
        g->explosions[i].age += dt;
        if (g->explosions[i].age >= g->explosions[i].duration)
            g->explosions[i].active = 0;
    }
}

static void update_top_down_sortie(Game *g, float dt, const uint8_t *keys)
{
    int i, j;
    uint32_t now = SDL_GetTicks();
    if (keys[SDL_SCANCODE_X]) g->airspeed += 42.0f * dt;
    if (keys[SDL_SCANCODE_Z]) g->airspeed -= 42.0f * dt;
    if (g->airspeed < 72.0f) g->airspeed = 72.0f;
    if (g->airspeed > 152.0f) g->airspeed = 152.0f;
    if (keys[SDL_SCANCODE_LEFT]) g->player_x -= 175.0f * dt;
    if (keys[SDL_SCANCODE_RIGHT]) g->player_x += 175.0f * dt;
    if (keys[SDL_SCANCODE_UP]) g->player_y -= 150.0f * dt;
    if (keys[SDL_SCANCODE_DOWN]) g->player_y += 150.0f * dt;
    if (g->player_x < 34.0f) g->player_x = 34.0f;
    if (g->player_x > SCREEN_W - 34.0f) g->player_x = SCREEN_W - 34.0f;
    if (g->player_y < 86.0f) g->player_y = 86.0f;
    if (g->player_y > SCREEN_H - 38.0f) g->player_y = SCREEN_H - 38.0f;
    g->scroll += g->airspeed * dt;
    g->fuel -= dt * 1.08f;
    g->audio.engine_speed = g->airspeed;
    g->audio.engine_load = keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_RIGHT];
    if (chapters[g->chapter].armed && g->ammunition > 0 &&
        keys[SDL_SCANCODE_SPACE] &&
        now - g->last_shot >= 115u) {
        if (new_bullet(g, g->player_x, g->player_y - 28.0f,
                       0.0f, -430.0f, 0, 0, 1.35f)) {
            g->last_shot = now;
            --g->ammunition;
            g->audio.shot = 1500;
        }
    }
    if (chapters[g->chapter].armed && g->bombs_remaining > 0 &&
        keys[SDL_SCANCODE_B] &&
        now - g->last_bomb >= 650u) {
        if (new_bullet(g, g->player_x, g->player_y - 4.0f,
                       0.0f, 42.0f, 0, 1, 1.12f)) {
            g->last_bomb = now;
            --g->bombs_remaining;
        }
    }
    age_explosions(g, dt);
    for (i = 0; i < MAX_BULLETS; ++i) if (g->bullets[i].active) {
        Bullet *b = &g->bullets[i];
        b->x += b->vx * dt;
        b->y += b->vy * dt;
        b->life -= dt;
        if (!b->enemy && b->type == 1 && b->life <= 0.0f) {
            new_ground_explosion(g, b->x, b->y, 2);
            g->audio.blast = 7000;
            b->active = 0;
        } else if (b->life <= 0.0f || b->y < -30.0f || b->y > SCREEN_H + 30.0f) {
            b->active = 0;
        }
        if (b->active && b->enemy &&
            fabsf(b->x - g->player_x) < 24.0f &&
            fabsf(b->y - g->player_y) < 24.0f &&
            now - g->last_damage > 240u) {
            g->health -= 4;
            g->last_damage = now;
            b->active = 0;
            new_explosion(g, g->player_x, g->player_y, 0);
        }
    }
    for (i = 0; i < MAX_TARGETS; ++i) if (g->targets[i].active) {
        Target *target = &g->targets[i];
        float sx, sy;
        top_down_target_position(g, i, &sx, &sy);
        if (sy > 18.0f && sy < SCREEN_H - 12.0f) {
            if (target->type != 2 &&
                now - target->last_shot > 1600u + (uint32_t)(i * 97 % 620)) {
                aimed_fire(g, sx, sy, target->type ? 185.0f : 235.0f, 0);
                target->last_shot = now;
            }
            for (j = 0; j < MAX_BULLETS; ++j) {
                Bullet *b = &g->bullets[j];
                if (!b->active || b->enemy) continue;
                if (fabsf(b->x - sx) < (b->type == 1 ? 38.0f : 18.0f) &&
                    fabsf(b->y - sy) < (b->type == 1 ? 38.0f : 20.0f)) {
                    if (b->type == 1 && b->life > 0.30f) continue;
                    target->active = 0;
                    b->active = 0;
                    ++g->objectives;
                    g->score += 500;
                    new_ground_explosion(g, sx, sy,
                                         b->type == 1 ? 3 : 1);
                    g->audio.blast = 7000;
                    break;
                }
            }
        }
    }
    if (g->scroll > chapters[g->chapter].distance ||
        g->fuel <= 0.0f || g->health <= 0)
        finish_alternate_sortie(g);
}

static void update_perspective_sortie(Game *g, float dt,
                                      const uint8_t *keys, int gunner)
{
    int i;
    int active_enemies = 0;
    int crashing_enemies = 0;
    int tail_strike = 0;
    uint32_t now = SDL_GetTicks();
    int aim_x;
    int aim_y;
    int firing = keys[SDL_SCANCODE_SPACE] && chapters[g->chapter].armed &&
                 g->ammunition > 0 &&
                 now - g->last_shot >= (gunner ? 110u : 130u);
    if (gunner) {
        float aim_elevation;
        float follow;
        float target_view_x;
        float target_view_y;
        /* Tom flies a gentle evasive course.  The player swivels Neddy's
         * crosshair through the rear arc.  The observer's viewpoint follows
         * with damped motion while the single Lewis gun stays on the sight. */
        g->airspeed = 112.0f + sinf(g->scroll * 0.006f) * 8.0f;
        if (keys[SDL_SCANCODE_LEFT]) g->player_x -= 225.0f * dt;
        if (keys[SDL_SCANCODE_RIGHT]) g->player_x += 225.0f * dt;
        if (keys[SDL_SCANCODE_UP]) g->player_y -= 195.0f * dt;
        if (keys[SDL_SCANCODE_DOWN]) g->player_y += 195.0f * dt;
        if (g->player_x < 28.0f) g->player_x = 28.0f;
        if (g->player_x > 612.0f) g->player_x = 612.0f;
        if (g->player_y < 28.0f) g->player_y = 28.0f;
        if (g->player_y > 300.0f) g->player_y = 300.0f;
        aim_elevation = gunner_aim_elevation(g);
        target_view_x = 320.0f + (g->player_x - 320.0f) * 0.80f;
        target_view_y = 235.0f - aim_elevation * 90.0f;
        follow = 1.0f - expf(-4.2f * dt);
        g->gunner_view_x += (target_view_x - g->gunner_view_x) * follow;
        g->gunner_view_y += (target_view_y - g->gunner_view_y) * follow;
        {
            float tail_control = (float)g->tail_integrity / 100.0f;
            float threat = 0.0f;
            for (i = 0; i < MAX_ENEMIES; ++i) {
                float depth;
                float proximity;
                if (!g->enemies[i].active || g->enemies[i].crashing)
                    continue;
                depth = enemy_depth_3d(g, &g->enemies[i], i);
                if (depth <= 40.0f || depth >= 1700.0f) continue;
                proximity = 1.0f - depth / 1700.0f;
                if (now >= g->enemies[i].last_shot &&
                    now - g->enemies[i].last_shot < 650u)
                    proximity += 0.24f;
                if (proximity > threat) threat = proximity;
            }
            if (threat > 1.0f) threat = 1.0f;
            if (tail_control < 0.0f) tail_control = 0.0f;
            /* The pilot flies the evasive aeroplane.  Manoeuvres become
             * forceful when an attacker closes or has just fired, so the
             * horizon, tail and target all move under the gunner. */
            g->view_bank = (sinf(g->scroll * 0.0027f) *
                            (0.12f + threat * 0.25f) +
                            sinf(g->scroll * 0.0097f + 1.4f) *
                            (0.05f + threat * 0.11f)) * tail_control;
            g->view_pitch = (sinf(g->scroll * 0.0019f) *
                             (0.07f + threat * 0.13f) +
                             sinf(g->scroll * 0.0079f + 0.8f) *
                             (0.03f + threat * 0.07f)) * tail_control;
        }
    } else {
        /* In the scout the controls fly the aeroplane, not a mouse cursor.
         * The fixed Vickers sight remains on the centre line. */
        if (keys[SDL_SCANCODE_X]) g->airspeed += 48.0f * dt;
        if (keys[SDL_SCANCODE_Z]) g->airspeed -= 48.0f * dt;
        if (g->airspeed < 78.0f) g->airspeed = 78.0f;
        if (g->airspeed > 165.0f) g->airspeed = 165.0f;
        if (keys[SDL_SCANCODE_LEFT]) g->view_bank -= 1.05f * dt;
        else if (keys[SDL_SCANCODE_RIGHT]) g->view_bank += 1.05f * dt;
        else g->view_bank *= powf(0.12f, dt);
        if (keys[SDL_SCANCODE_UP]) g->view_pitch -= 0.82f * dt;
        else if (keys[SDL_SCANCODE_DOWN]) g->view_pitch += 0.82f * dt;
        else g->view_pitch *= powf(0.18f, dt);
        if (g->view_bank < -0.62f) g->view_bank = -0.62f;
        if (g->view_bank > 0.62f) g->view_bank = 0.62f;
        if (g->view_pitch < -0.44f) g->view_pitch = -0.44f;
        if (g->view_pitch > 0.44f) g->view_pitch = 0.44f;
        g->player_x = 320.0f;
        g->player_y = 190.0f;
    }
    /* In the gunner view the open crosshair is the authoritative hit point.
     * The camera follows it with lag; it is never replaced by the gun sight. */
    aim_x = (int)g->player_x;
    aim_y = (int)g->player_y;
    tail_strike = gunner && gunner_tail_blocks_shot(g, aim_x, aim_y);
    if (firing) {
        float muzzle_x = 320.0f;
        float muzzle_y = 248.0f;
        float dx;
        float dy;
        float length;
        g->last_shot = now;
        --g->ammunition;
        g->audio.shot = gunner ? 1750 : 1500;
        if (gunner) gunner_muzzle_position(g, &muzzle_x, &muzzle_y);
        dx = (float)aim_x - muzzle_x;
        dy = (float)aim_y - muzzle_y;
        length = sqrtf(dx * dx + dy * dy);
        if (length < 1.0f) length = 1.0f;
        /* Every round is visible, including an unsafe round stopped by our
         * own tail.  The tracer starts at the painted muzzle and passes
         * through the authoritative crosshair. */
        new_bullet(g, muzzle_x, muzzle_y,
                   dx / length * 900.0f, dy / length * 900.0f,
                   0, 0, 0.65f);
        if (tail_strike) {
            g->tail_integrity -= 9;
            if (g->tail_integrity < 0) g->tail_integrity = 0;
            g->health -= 2;
            if (g->health < 0) g->health = 0;
            new_explosion(g, (float)aim_x, (float)aim_y, 0);
            g->audio.blast = 1700;
            /* The round stops in our airframe; it cannot also hit an enemy. */
            firing = 0;
        }
    }
    g->scroll += g->airspeed * dt * 1.16f;
    g->fuel -= dt * 1.2f;
    g->audio.engine_speed = g->airspeed;
    g->audio.engine_load = gunner ? 0.35f :
        (fabsf(g->view_bank) + fabsf(g->view_pitch));
    g->audio.other_engine = 0.0f;
    g->audio.other_pitch = 0.0f;
    age_explosions(g, dt);
    for (i = 0; i < MAX_BULLETS; ++i) if (g->bullets[i].active) {
        Bullet *b = &g->bullets[i];
        b->x += b->vx * dt;
        b->y += b->vy * dt;
        b->life -= dt;
        if (b->life <= 0.0f) b->active = 0;
        if (b->active && b->enemy &&
            fabsf(b->x - 320.0f) < 28.0f &&
            fabsf(b->y - 190.0f) < 28.0f &&
            now - g->last_damage > 520u) {
            if (gunner) {
                BulletHole *hole = &g->bullet_holes[g->next_bullet_hole];
                uint32_t scatter = scene_hash(now ^
                    (uint32_t)g->next_bullet_hole * 2654435761u);
                hole->x = 190.0f + (float)(scatter % 261u);
                hole->y = 205.0f + (float)((scatter >> 9) % 126u);
                hole->active = 1;
                g->next_bullet_hole =
                    (g->next_bullet_hole + 1) % MAX_BULLET_HOLES;
            }
            if (!g->qa_hold)
                g->health -= player_machine_gun_damage(g);
            g->last_damage = now;
            b->active = 0;
            /* A bullet strike damages the airframe; it is not a mid-air
             * explosion.  The normal damage-smoke renderer shows the result. */
        }
    }
    for (i = 0; i < MAX_ENEMIES; ++i) if (g->enemies[i].active) {
        Enemy *enemy = &g->enemies[i];
        float depth = enemy_depth_3d(g, enemy, i);
        int sx, sy, size;
        if (enemy->crashing) {
            ++crashing_enemies;
            enemy->crash_angle += dt * (2.8f + (float)(i & 3) * 0.32f);
            enemy->crash_vy += 34.0f * dt;
            enemy->crash_x += (enemy->crash_vx +
                cosf(enemy->crash_angle) * 72.0f) * dt;
            enemy->crash_y += (enemy->crash_vy +
                sinf(enemy->crash_angle) * 22.0f) * dt;
            if (enemy->crash_y > SCREEN_H + 70.0f ||
                enemy->crash_x < -150.0f || enemy->crash_x > SCREEN_W + 150.0f) {
                enemy->active = 0;
                --crashing_enemies;
            }
            continue;
        }
        if (depth < -520.0f || ((i & 3) == 3 && depth > 3300.0f)) {
            enemy->active = 0;
            continue;
        }
        ++active_enemies;
        if (!project_enemy_3d(g, enemy, i, &sx, &sy, &size)) continue;
        if (firing) {
            if (abs(aim_x - sx) < size / 2 + 10 &&
                abs(aim_y - sy) < size / 3 + 9) {
                --enemy->hp;
                if (enemy->hp <= 0) {
                    enemy->crashing = 1;
                    enemy->crash_x = (float)sx;
                    enemy->crash_y = (float)sy;
                    enemy->crash_vx = (i & 1) ? -42.0f : 42.0f;
                    enemy->crash_vy = 24.0f;
                    enemy->crash_angle = enemy->phase;
                    enemy->crash_size = size;
                    --active_enemies;
                    ++crashing_enemies;
                    credit_air_kill(g);
                }
            }
        }
        if (enemy->crashing) continue;
        if (chapters[g->chapter].armed &&
            depth < 1250.0f && depth > 140.0f &&
            now >= enemy->last_shot &&
            now - enemy->last_shot >
            (uint32_t)(1500 - g->chapter * 95 + i * 57 % 330)) {
            float dx = 320.0f - sx;
            float dy = 190.0f - sy;
            float length = sqrtf(dx * dx + dy * dy);
            if (length < 1.0f) length = 1.0f;
            new_bullet(g, (float)sx, (float)sy,
                       dx / length * 250.0f, dy / length * 250.0f,
                       1, 0, 2.2f);
            enemy->last_shot = now;
        }
        {
            float presence = 1.0f - depth / 1800.0f;
            if (presence > g->audio.other_engine) {
                if (presence < 0.0f) presence = 0.0f;
                g->audio.other_engine = presence;
                g->audio.other_pitch = 0.55f + presence * 0.45f;
            }
        }
    }
    if (gunner && g->tail_integrity <= 0) {
        finish_alternate_sortie(g);
    } else if (g->qa_hold && active_enemies == 0 && crashing_enemies == 0 &&
               g->scroll > 520.0f) {
        float view_x = g->player_x;
        float view_y = g->player_y;
        float camera_x = g->gunner_view_x;
        float camera_y = g->gunner_view_y;
        int hold = g->qa_hold;
        reset_sortie(g);
        g->player_x = view_x;
        g->player_y = view_y;
        g->gunner_view_x = camera_x;
        g->gunner_view_y = camera_y;
        g->qa_hold = hold;
    } else if (!g->qa_hold && ((active_enemies == 0 && crashing_enemies == 0 &&
        g->scroll > 520.0f) ||
        (current_mission_goal(g) > 0 && g->kills >= current_mission_goal(g) &&
         crashing_enemies == 0) ||
        g->scroll > chapters[g->chapter].distance ||
        g->fuel <= 0.0f || g->health <= 0))
        finish_alternate_sortie(g);
}

static void update_dogfight_3d_sortie(Game *g, float dt,
                                      const uint8_t *keys)
{
    uint32_t now = SDL_GetTicks();
    int i;
    int active = 0;
    float aileron_command = 0.0f;
    float elevator_command = 0.0f;
    Vec3 position;
    age_explosions(g, dt);
    update_dogfight_particles(g, dt);
    if (g->flight_crashed) {
        g->audio.engine_speed = 0.0f;
        g->audio.engine_load = 0.0f;
        g->audio.other_engine = 0.0f;
        if (now - g->flight_crash_started > 3900u)
            finish_alternate_sortie(g);
        return;
    }
    /* Conventional stick: left/right roll the matching wing down; pulling
     * back (Down) raises the nose and pushing forward (Up) lowers it. */
    if (g->flight_doomed) {
        aileron_command = 0.72f;
        elevator_command = -0.32f;
        g->flight_throttle = flight_approach(g->flight_throttle,
                                             0.12f, 0.34f * dt);
        /* A collision tears lift away.  The normal six-axis integrator still
         * supplies forward motion and rotation; this growing sink guarantees
         * that the damaged player aircraft reaches the same physical ground
         * as its opponent. */
    } else {
        if (keys[SDL_SCANCODE_LEFT]) aileron_command = -1.0f;
        if (keys[SDL_SCANCODE_RIGHT]) aileron_command = 1.0f;
        if (keys[SDL_SCANCODE_DOWN]) elevator_command = 1.0f;
        if (keys[SDL_SCANCODE_UP]) elevator_command = -1.0f;
        if (keys[SDL_SCANCODE_X]) g->flight_throttle += 0.42f * dt;
        if (keys[SDL_SCANCODE_Z]) g->flight_throttle -= 0.42f * dt;
    }
    if (g->flight_throttle < 0.12f) g->flight_throttle = 0.12f;
    if (g->flight_throttle > 1.0f) g->flight_throttle = 1.0f;
    g->flight.throttle = g->flight_throttle;
    position.x = g->flight_x;
    position.y = g->flight_y;
    position.z = g->flight_z;
    ww1_flight_advance(&g->flight, &position, dt,
                       aileron_command, elevator_command);
    if (g->flight_doomed) {
        float doomed_age = (float)(now - g->flight_doomed_started) / 1000.0f;
        float sink = 35.0f + doomed_age * 20.0f;
        if (sink > 140.0f) sink = 140.0f;
        position.y -= sink * dt;
    }
    g->flight_x = position.x;
    g->flight_y = position.y;
    g->flight_z = position.z;
    flight_outputs(&g->flight, &g->flight_heading, &g->view_pitch,
                   &g->view_bank, &g->flight_vx, &g->flight_vy,
                   &g->flight_vz, &g->airspeed);
    g->flight_roll_rate = g->flight.p;
    g->flight_pitch_rate = g->flight.q;
    g->flight_alpha = g->flight.alpha;
    g->flight_stalled = g->flight.stalled;
    /* No invisible altitude rails in the simulator.  The ground is a real
     * consequence and the sky remains open, so a long climb can genuinely
     * run out of energy and stall. */
    if (g->flight_y <= 0.0f) {
        g->flight_y = 0.0f;
        g->health = 0;
        g->fatal_sortie = 1;
        g->flight_crashed = 1;
        g->flight_crash_started = now;
        g->audio.blast = 0;
        g->audio.crash = 7200;
    }
    g->scroll += g->airspeed * dt;
    g->fuel -= dt * (0.36f + g->flight_throttle * 1.05f);
    if (g->fuel < 0.0f) g->fuel = 0.0f;
    g->audio.engine_speed = g->airspeed;
    g->audio.engine_load = g->flight_throttle * 0.55f +
        fabsf(g->flight_alpha) + (g->flight_stalled ? 0.45f : 0.0f);
    g->audio.other_engine = 0.0f;

    if (!g->flight_doomed && keys[SDL_SCANCODE_SPACE] &&
        chapters[g->chapter].armed &&
        g->ammunition > 0 && now - g->last_shot >= 160u) {
        g->last_shot = now;
        --g->ammunition;
        g->audio.shot = 1500;
        launch_dogfight_tracer(g, 292.0f, 231.0f,
                               320.0f, 210.0f, 300.0f, 0);
        launch_dogfight_tracer(g, 350.0f, 231.0f,
                               320.0f, 210.0f, 300.0f, 0);
        new_dogfight_particle(g, 298.0f, 245.0f,
                              -54.0f, -68.0f, 0.42f, 2);
        new_dogfight_particle(g, 344.0f, 245.0f,
                               58.0f, -62.0f, 0.42f, 2);
        for (i = 0; i < MAX_ENEMIES; ++i) if (g->enemies[i].active &&
                                                  !g->enemies[i].crashing) {
            Projected3D p = project_world_point(g, (Vec3){
                g->enemies[i].world_x, g->enemies[i].world_y,
                g->enemies[i].world_z});
            int radius;
            if (!p.visible) continue;
            radius = (int)(1300.0f / p.depth);
            if (radius < 7) radius = 7;
            if (radius > 48) radius = 48;
            if (abs(p.x - 320) <= radius && abs(p.y - 218) <= radius) {
                --g->enemies[i].hp;
                g->audio.hit_player = 0;
                g->audio.hit = 950;
                if (g->enemies[i].hp <= 0) {
                    g->enemies[i].crashing = 1;
                    credit_air_kill(g);
                }
                break;
            }
        }
    }

    for (i = 0; i < MAX_ENEMIES; ++i) if (g->enemies[i].active) {
        Enemy *enemy = &g->enemies[i];
        float dx, dy, dz, horizontal, distance;
        float desired_heading, heading_error, desired_bank, desired_pitch;
        if (enemy->crashing) {
            /* Preserve forward momentum, but force a real downward velocity.
             * The previous pitch decrement had the model's nose-up sign and
             * made a descending centre look as though the aircraft fell
             * upwards. */
            if (enemy->vy > -12.0f) enemy->vy = -12.0f;
            enemy->vy -= (31.0f + i * 1.7f) * dt;
            enemy->bank += dt * (1.7f + 0.16f * i);
            enemy->pitch = flight_approach(enemy->pitch, 0.82f, 0.64f * dt);
            enemy->heading += sinf(enemy->bank) * 0.24f * dt;
            enemy->world_y += enemy->vy * dt;
            enemy->world_x += sinf(enemy->heading) * enemy->speed * 0.42f * dt;
            enemy->world_z += cosf(enemy->heading) * enemy->speed * 0.42f * dt;
            if (enemy->world_y < 15.0f) {
                Projected3D impact = project_world_point(g, (Vec3){
                    enemy->world_x, 0.0f, enemy->world_z});
                enemy->world_y = 0.0f;
                enemy->active = 0;
                if (impact.visible && impact.x > -90 &&
                    impact.x < SCREEN_W + 90 && impact.y > -90 &&
                    impact.y < SCREEN_H + 90)
                    new_ground_explosion(g, (float)impact.x,
                                         (float)impact.y, 3);
                g->audio.blast = 4300;
                g->audio.crash = 7200;
            } else ++active;
            continue;
        }
        dx = g->flight_x - enemy->world_x;
        dy = g->flight_y - enemy->world_y;
        dz = g->flight_z - enemy->world_z;
        horizontal = sqrtf(dx * dx + dz * dz);
        distance = sqrtf(horizontal * horizontal + dy * dy);
        if (distance < 34.0f) {
            /* A failed break-away damages both machines.  The enemy enters
             * the same physical spiral used for gun damage; the player loses
             * control and descends through the flight model until ground
             * contact, rather than exploding or disappearing in mid-air. */
            enemy->crashing = 1;
            enemy->hp = 0;
            credit_air_kill(g);
            if (enemy->vy > -16.0f) enemy->vy = -16.0f;
            if (!g->flight_doomed) {
                g->flight_doomed = 1;
                g->flight_doomed_started = now;
                g->health = 0;
                g->fatal_sortie = 1;
                g->audio.hit_player = 1;
                g->audio.hit = 1400;
                g->audio.blast = 2400;
            }
            ++active;
            continue;
        }
        desired_heading = atan2f(dx, dz) +
            sinf(g->scroll * 0.0021f + enemy->phase) * 0.12f;
        if (distance < 360.0f) {
            float urgency = (360.0f - distance) / 360.0f;
            float break_side = (i & 1) ? 1.0f : -1.0f;
            /* Stop pointing at the player's centre and cross clear of the
             * collision cone.  Alternating sides prevent several attackers
             * choosing the same escape path. */
            desired_heading += break_side * (0.48f + urgency * 0.72f);
        }
        heading_error = wrap_angle(desired_heading - enemy->heading);
        desired_bank = heading_error * 1.18f;
        if (desired_bank < -0.70f) desired_bank = -0.70f;
        if (desired_bank > 0.70f) desired_bank = 0.70f;
        desired_pitch = atan2f(dy, horizontal > 1.0f ? horizontal : 1.0f);
        if (desired_pitch < -0.30f) desired_pitch = -0.30f;
        if (desired_pitch > 0.30f) desired_pitch = 0.30f;
        {
            Vec3 enemy_position = {enemy->world_x, enemy->world_y,
                                   enemy->world_z};
            float enemy_aileron = flight_clamp(
                (desired_bank - enemy->bank) * 2.2f - enemy->flight.p * 0.32f,
                -1.0f, 1.0f);
            float enemy_elevator = flight_clamp(
                (desired_pitch - enemy->pitch) * 2.4f - enemy->flight.q * 0.28f,
                -1.0f, 1.0f);
            enemy->flight.throttle = enemy->throttle;
            ww1_flight_advance(&enemy->flight, &enemy_position, dt,
                               enemy_aileron, enemy_elevator);
            enemy->world_x = enemy_position.x;
            enemy->world_y = enemy_position.y;
            enemy->world_z = enemy_position.z;
            flight_outputs(&enemy->flight, &enemy->heading, &enemy->pitch,
                           &enemy->bank, &enemy->vx, &enemy->vy, &enemy->vz,
                           &enemy->speed);
            enemy->roll_rate = enemy->flight.p;
            enemy->pitch_rate = enemy->flight.q;
        }
        if (enemy->world_y <= 0.0f) {
            enemy->world_y = 0.0f;
            enemy->crashing = 1;
            credit_air_kill(g);
            enemy->crash_x = 320.0f;
            enemy->crash_y = 300.0f;
            enemy->crash_size = 18;
            continue;
        }
        {
            Projected3D attack_view = project_world_point(g, (Vec3){
                enemy->world_x, enemy->world_y, enemy->world_z});
            /* An attacker can only hit while it is actually in the player's
             * forward view and has settled its nose.  This prevents several
             * unseen aircraft draining the airframe during their entrance. */
            if (chapters[g->chapter].armed &&
                now - g->scene_started > 5200u && distance < 900.0f &&
                attack_view.visible && attack_view.x > 24 &&
                attack_view.x < SCREEN_W - 24 && attack_view.y > 18 &&
                attack_view.y < SCREEN_H - 80 &&
                fabsf(heading_error) < 0.095f &&
                fabsf(desired_pitch - enemy->pitch) < 0.085f &&
                now >= enemy->last_shot && now - enemy->last_shot >
                (uint32_t)(1550 - g->chapter * 45 + (i * 173) % 520)) {
                enemy->last_shot = now;
                launch_dogfight_tracer(g, (float)attack_view.x,
                                       (float)attack_view.y,
                                       320.0f, 218.0f, 390.0f, 1);
                if (!g->qa_hold || g->health > 30) {
                    g->health -= player_machine_gun_damage(g);
                    add_dogfight_bullet_hole(g, now);
                }
                g->audio.hit_player = 1;
                g->audio.hit = 1400;
                g->last_damage = now;
            }
        }
        {
            float presence = 1.0f - distance / 1600.0f;
            if (presence > g->audio.other_engine) {
                if (presence < 0.0f) presence = 0.0f;
                g->audio.other_engine = presence;
                g->audio.other_pitch = 0.55f + presence * 0.5f;
            }
        }
        ++active;
    }
    if (active == 0 && !g->flight_doomed) {
        /* Hold the last impact for one beat so its explosion and crash sound
         * are seen and heard, then end both campaign and direct-test sorties. */
        if (!g->mission_clear_at) g->mission_clear_at = now;
        if (now - g->mission_clear_at >= 900u)
            finish_alternate_sortie(g);
    } else if (active > 0) {
        g->mission_clear_at = 0;
    }
    if (!g->qa_hold && active > 0 && g->health <= 0 &&
        !g->flight_doomed && !g->flight_crashed) {
        /* Fatal gun damage first becomes a loss-of-control sequence.  Keep
         * the pilot in the cockpit through the spiral and physical impact;
         * only the completed wreck sequence may open the failed debrief. */
        g->flight_doomed = 1;
        g->flight_doomed_started = now;
        g->fatal_sortie = 1;
        g->audio.hit_player = 1;
        g->audio.hit = 1400;
        g->audio.blast = 1800;
    }
    if (!g->qa_hold && active > 0 && g->fuel <= 0.0f) {
        finish_alternate_sortie(g);
    } else if (!g->qa_hold && !chapters[g->chapter].armed && active > 0 &&
               !g->flight_doomed && !g->flight_crashed &&
               g->scroll > chapters[g->chapter].distance) {
        /* 1914: nobody is armed.  Shadowing the other machine for the
         * length of the patrol is the whole task. */
        finish_alternate_sortie(g);
    }
}

static void update_gunner_3d_sortie(Game *g, float dt,
                                    const uint8_t *keys)
{
    update_perspective_sortie(g, dt, keys, 1);
}

static void update_sortie(Game *g, float dt, const uint8_t *keys)
{
    int i, j;
    uint32_t now = SDL_GetTicks();
    int ground;
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    if (mode == MISSION_TOP_DOWN) {
        update_top_down_sortie(g, dt, keys);
        return;
    }
    if (mode == MISSION_DOGFIGHT_3D) {
        update_dogfight_3d_sortie(g, dt, keys);
        return;
    }
    if (mode == MISSION_GUNNER_3D) {
        update_gunner_3d_sortie(g, dt, keys);
        return;
    }
    if (keys[SDL_SCANCODE_X]) g->airspeed += 42.0f * dt;
    if (keys[SDL_SCANCODE_Z]) g->airspeed -= 42.0f * dt;
    if (g->airspeed < 62.0f) g->airspeed = 62.0f;
    if (g->airspeed > 142.0f) g->airspeed = 142.0f;
    if (keys[SDL_SCANCODE_LEFT]) g->player_x -= 155.0f * dt;
    if (keys[SDL_SCANCODE_RIGHT]) g->player_x += 155.0f * dt;
    if (g->player_x < 44.0f) g->player_x = 44.0f;
    if (g->player_x > 590.0f) g->player_x = 590.0f;
    if (keys[SDL_SCANCODE_UP])
        g->player_vy -= 245.0f * dt;
    if (keys[SDL_SCANCODE_DOWN])
        g->player_vy += 245.0f * dt;
    g->player_vy *= powf(0.10f, dt);
    if (g->player_vy < -120.0f) g->player_vy = -120.0f;
    if (g->player_vy > 120.0f) g->player_vy = 120.0f;
    g->player_y += g->player_vy * dt;
    if (g->player_y < 46.0f) { g->player_y = 46.0f; g->player_vy = 0.0f; }
    ground = terrain_y(g->scroll + g->player_x) - 19;
    if (g->player_y > ground) {
        g->player_y = (float)ground;
        if (fabsf(g->player_vy) > 45.0f) g->health -= 3;
        g->player_vy = -8.0f;
    }
    g->scroll += g->airspeed * dt;
    g->fuel -= dt * 1.15f;
    g->audio.engine_speed = g->airspeed;
    g->audio.engine_load = fabsf(g->player_vy) / 120.0f;
    g->audio.other_engine = 0.0f;
    g->audio.other_pitch = 0.0f;
    if (chapters[g->chapter].armed || mode == MISSION_SIDE) {
        if (keys[SDL_SCANCODE_SPACE]) fire(g);
        if (keys[SDL_SCANCODE_B]) drop_bomb(g);
    }

    for (i = 0; i < MAX_EXPLOSIONS; ++i) if (g->explosions[i].active) {
        g->explosions[i].age += dt;
        if (g->explosions[i].age >= g->explosions[i].duration)
            g->explosions[i].active = 0;
    }

    for (i = 0; i < MAX_BULLETS; ++i) if (g->bullets[i].active) {
        Bullet *b = &g->bullets[i];
        if (b->type == 1) b->vy += 105.0f * dt;
        if (b->type == 2) b->vy += 92.0f * dt;
        b->x += b->vx * dt;
        b->y += b->vy * dt;
        b->life -= dt;
        if (b->active && b->enemy && b->type == 2) {
            float dx = b->x - g->player_x;
            float dy = b->y - g->player_y;
            float distance = sqrtf(dx * dx + dy * dy);
            if (distance < 42.0f || b->life < 0.08f) {
                new_explosion(g, b->x, b->y, 1);
                if (distance < 82.0f && now - g->last_damage > 260u) {
                    int damage = distance < 42.0f ? 16 : 9;
                    g->health -= damage;
                    g->last_damage = now;
                    new_explosion(g, g->player_x - 4.0f, g->player_y, 0);
                }
                g->audio.blast = 6200;
                b->active = 0;
            }
        }
        if (b->life <= 0.0f || b->x < -24.0f || b->x > SCREEN_W + 24.0f ||
            b->y < 24.0f || b->y > SCREEN_H + 24.0f)
            b->active = 0;
        if (b->active && b->enemy && b->type != 2 &&
            fabsf(b->x - g->player_x) < 25.0f &&
            fabsf(b->y - g->player_y) < 14.0f && now - g->last_damage > 260u) {
            g->health -= b->type == 2 ? 8 : 3;
            g->last_damage = now;
            b->active = 0;
            new_explosion(g, g->player_x - 2.0f, g->player_y, 0);
            if (b->type == 2) g->audio.blast = 5200;
            else g->audio.shot = 800;
        }
    }

    for (i = 0; i < MAX_ENEMIES; ++i) if (g->enemies[i].active) {
        int difficulty = g->chapter * MISSIONS_PER_CHAPTER + g->mission;
        uint32_t fire_delay = (uint32_t)(1550 - difficulty * 8);
        float shot_speed = 210.0f + difficulty * 1.35f;
        float sx = g->enemies[i].x - g->scroll;
        float sy = g->enemies[i].y + sinf(g->scroll * 0.01f + g->enemies[i].phase) * 30.0f;
        {
            float dx = sx - g->player_x;
            float dy = sy - g->player_y;
            float distance = sqrtf(dx * dx + dy * dy);
            float presence = 1.0f - distance / 520.0f;
            if (presence > g->audio.other_engine) {
                if (presence < 0.0f) presence = 0.0f;
                g->audio.other_engine = presence;
                g->audio.other_pitch = 0.45f + 0.5f * (1.0f - distance / 520.0f);
            }
        }
        if (sx < -80) g->enemies[i].active = 0;
        if (sx > g->player_x + 65.0f && sx < SCREEN_W + 20.0f &&
            now - g->enemies[i].last_shot > fire_delay + (uint32_t)(i * 47 % 260)) {
            aimed_fire(g, sx - 30.0f, sy, shot_speed, 0);
            g->enemies[i].last_shot = now;
        }
        for (j = 0; j < MAX_BULLETS; ++j) if (g->bullets[j].active) {
            if (!g->bullets[j].enemy && g->bullets[j].type == 0 &&
                fabsf(g->bullets[j].x - sx) < 34.0f &&
                fabsf(g->bullets[j].y - sy) < 18.0f) {
                g->bullets[j].active = 0;
                if (--g->enemies[i].hp <= 0) {
                    g->enemies[i].active = 0;
                    g->score += 250;
                    new_explosion(g, sx, sy, 1);
                    g->audio.blast = 6500;
                }
            }
        }
        if (fabsf(sx - g->player_x) < 36.0f && fabsf(sy - g->player_y) < 18.0f) {
            g->health -= 25;
            g->enemies[i].active = 0;
            new_explosion(g, g->player_x, g->player_y, 1);
            g->audio.blast = 6200;
        }
    }
    for (i = 0; i < MAX_TARGETS; ++i) if (g->targets[i].active) {
        float sx = g->targets[i].world_x - g->scroll;
        int ty = terrain_y(g->targets[i].world_x) - 12;
        Target *target = &g->targets[i];
        if (sx > 20.0f && sx < SCREEN_W - 10.0f) {
            if (target->type) {
                if (now - target->last_shot > 2850u + (uint32_t)(i * 83 % 520)) {
                    artillery_fire(g, target, sx, (float)ty - 38.0f, now);
                    target->last_shot = now;
                }
            } else {
                /* A ground Vickers fires six-round bursts at about 11 rps.
                 * Its mounting cannot track an aircraft above this elevation. */
                if (target->burst_left > 0 && now >= target->next_round) {
                    aimed_fire(g, sx + 34.0f, (float)ty - 18.0f, 270.0f, 0);
                    --target->burst_left;
                    target->next_round = now + 90u;
                    if (!target->burst_left) target->last_shot = now;
                } else if (!target->burst_left && g->player_y > 145.0f &&
                           now - target->last_shot >
                           2050u + (uint32_t)(i * 61 % 420)) {
                    target->burst_left = 6;
                    target->next_round = now;
                }
            }
        }
        for (j = 0; j < MAX_BULLETS; ++j) if (g->bullets[j].active) {
            Bullet *b = &g->bullets[j];
            if (!b->enemy &&
                ((b->type == 0 && fabsf(b->x - sx) < 20.0f && fabsf(b->y - ty) < 16.0f) ||
                 (b->type == 1 && fabsf(b->x - sx) < 27.0f && b->y > ty - 16.0f))) {
                g->targets[i].active = 0;
                b->active = 0;
                g->score += 500;
                ++g->objectives;
                new_ground_explosion(g, sx, (float)ty,
                                     b->type == 1 ? 3 : 1);
                g->audio.blast = 7000;
            }
        }
    }
    /* A missed bomb still strikes the physical ground and detonates.  Target
     * collision is resolved first above so a direct hit gets the same larger
     * blast without being replaced by a generic terrain impact. */
    for (i = 0; i < MAX_BULLETS; ++i) {
        Bullet *b = &g->bullets[i];
        if (b->active && !b->enemy && b->type == 1) {
            int impact_y = terrain_y(g->scroll + b->x) - 2;
            if (b->y >= impact_y) {
                b->y = (float)impact_y;
                b->active = 0;
                new_ground_explosion(g, b->x, b->y, 2);
                g->audio.blast = 7000;
            }
        }
    }
    if (g->scroll > chapters[g->chapter].distance ||
        g->fuel <= 0.0f || g->health <= 0) {
        if (g->health <= 0) g->fatal_sortie = 1;
        if (is_perspective_mode(campaign_missions[g->chapter][g->mission].mode))
            g->success = g->kills >= current_mission_goal(g) &&
                         g->health > 0 && g->fuel > 0;
        else
            g->success = g->objectives >= current_mission_goal(g) &&
                         g->health > 0 && g->fuel > 0;
        set_scene(g, SCENE_DEBRIEF);
    }
}

static void draw_victorian_plaque(Game *g, int x, int y, int w,
                                  const char *label, int selected)
{
    SDL_Surface *s = g->screen;
    uint32_t shadow = rgb(s, 20, 17, 14);
    uint32_t brass = rgb(s, selected ? 219 : 166,
                         selected ? 174 : 123,
                         selected ? 79 : 52);
    uint32_t enamel = rgb(s, selected ? 66 : 36,
                          selected ? 50 : 34,
                          selected ? 34 : 30);
    uint32_t ink = rgb(s, selected ? 255 : 225,
                       selected ? 230 : 199,
                       selected ? 163 : 137);
    int text_x = x + (w - (int)strlen(label) * 7) / 2;
    rect(s, x + 3, y + 4, w, 30, shadow);
    rect(s, x, y, w, 30, brass);
    rect(s, x + 2, y + 2, w - 4, 26, enamel);
    rect(s, x + 5, y + 5, w - 10, 20, rgb(s, 55, 41, 31));
    circle(s, x + 8, y + 8, 2, brass);
    circle(s, x + w - 9, y + 8, 2, brass);
    circle(s, x + 8, y + 21, 2, brass);
    circle(s, x + w - 9, y + 21, 2, brass);
    typewriter_text(g, text_x, y + 9, label, 7, 11, ink);
}

static void draw_title(Game *g)
{
    SDL_Surface *s = g->screen;
    const char *title = "CANVAS SKIES";
    uint32_t now = SDL_GetTicks();
    int i;
    /* The painting and its two shade bands never change: compose them once
     * and copy the result each frame, so only the banner is redrawn. */
    if (!g->title_backdrop) {
        SDL_Surface *b = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_W, SCREEN_H,
            32, s->format->format);
        if (b) {
            rect(b, 0, 0, SCREEN_W, SCREEN_H, rgb(b, 20, 31, 48));
            if (g->title) SDL_BlitSurface(g->title, NULL, b, NULL);
            blend_rect(b, 0, 0, 640, 112, 5, 12, 24, 112);
            blend_rect(b, 0, 300, SCREEN_W, SCREEN_H - 300, 5, 10, 18, 150);
            g->title_backdrop = b;
        }
    }
    if (g->title_backdrop) {
        SDL_BlitSurface(g->title_backdrop, NULL, s, NULL);
    } else {
        rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 20, 31, 48));
        if (g->title) SDL_BlitSurface(g->title, NULL, s, NULL);
        blend_rect(s, 0, 0, 640, 112, 5, 12, 24, 112);
        blend_rect(s, 0, 300, SCREEN_W, SCREEN_H - 300, 5, 10, 18, 150);
    }
    /* The lettering is embroidered into the cloth asset, so the complete
     * banner—thread, seams and fabric—moves as one wind-distorted object. */
    if (g->title_banner) {
        int banner_x = (SCREEN_W - g->title_banner->w) / 2;
        for (i = 0; i < g->title_banner->w; ++i) {
            float phase = (float)i * 0.031f + (float)now * 0.0042f;
            int wave = (int)(4.0f * sinf(phase) +
                             2.0f * sinf(phase * 0.43f));
            SDL_Rect source = {i, 0, 1, g->title_banner->h};
            SDL_Rect dest = {banner_x + i, 14 + wave, 1, g->title_banner->h};
            SDL_BlitSurface(g->title_banner, &source, s, &dest);
        }
    } else {
        for (i = 0; i < 506; ++i) {
            float phase = (float)i * 0.031f + (float)now * 0.0042f;
            int wave = (int)(4.0f * sinf(phase) +
                             2.0f * sinf(phase * 0.43f));
            rect(s, 67 + i, 17 + wave, 1, 63, rgb(s, 202, 181, 132));
        }
        display_text(g, 52, 27, title, 36, 42, rgb(s, 46, 34, 24));
    }
    draw_victorian_plaque(g, (SCREEN_W - 255) / 2, 344, 255,
                          "ENTER  OFFICERS' MENU", 1);
}

static void draw_campaign_menu(Game *g)
{
    static const char *new_campaign_items[6] = {
        "BEGIN FULL CAMPAIGN - AUGUST 1914",
        "JOIN THE CONFLICT - 1914",
        "JOIN THE CONFLICT - 1915",
        "JOIN THE CONFLICT - 1916",
        "JOIN THE CONFLICT - 1917",
        "JOIN THE CONFLICT - 1918"
    };
    SDL_Surface *s = g->screen;
    int count = campaign_menu_count(g);
    int has_continue = g->has_save && g->saved_lives > 0;
    int i;
    draw_title(g);
    blend_rect(s, 0, 0, SCREEN_W, SCREEN_H, 5, 7, 10, 150);
    rect(s, 93, 62, 454, 286, rgb(s, 24, 18, 14));
    rect(s, 96, 59, 448, 286, rgb(s, 183, 139, 63));
    rect(s, 100, 63, 440, 278, rgb(s, 52, 37, 27));
    rect(s, 106, 69, 428, 266, rgb(s, 74, 51, 31));
    circle(s, 112, 75, 4, rgb(s, 205, 161, 76));
    circle(s, 528, 75, 4, rgb(s, 205, 161, 76));
    circle(s, 112, 328, 4, rgb(s, 205, 161, 76));
    circle(s, 528, 328, 4, rgb(s, 205, 161, 76));
    display_text(g, 190, 77, "SELECT COMMISSION", 12, 18,
                 rgb(s, 239, 215, 155));
    typewriter_text(g, 205, 101, "UP / DOWN  CHOOSE   ENTER  ACCEPT", 7, 11,
                    rgb(s, 191, 170, 125));
    for (i = 0; i < count; ++i) {
        const char *label;
        char continued[64];
        if (has_continue && i == 0) {
            if (g->saved_campaign_complete)
                snprintf(continued, sizeof(continued),
                         "WAR RECORD COMPLETE - VIEW ARMISTICE");
            else
                snprintf(continued, sizeof(continued),
                         "CONTINUE CAMPAIGN - CHAPTER %d / MISSION %d",
                         g->saved_chapter + 1, g->saved_mission + 1);
            label = continued;
        } else {
            int new_index = i - (has_continue ? 1 : 0);
            label = new_campaign_items[new_index];
        }
        draw_victorian_plaque(g, 128, 112 + i * 31, 384, label,
                              i == g->menu_selection);
    }
    if (g->confirm_overwrite) {
        blend_rect(s, 106, 69, 428, 266, 5, 7, 10, 170);
        rect(s, 140, 160, 360, 84, rgb(s, 183, 139, 63));
        rect(s, 144, 164, 352, 76, rgb(s, 52, 37, 27));
        display_text(g, 158, 176, "OVERWRITE SAVED CAMPAIGN?", 12, 18,
                     rgb(s, 239, 215, 155));
        typewriter_text(g, 186, 210, "ENTER  YES      ESCAPE  NO", 7, 11,
                        rgb(s, 225, 199, 137));
    }
}

static void gradient(SDL_Surface *s, uint8_t r0, uint8_t g0, uint8_t b0,
                     uint8_t r1, uint8_t g1, uint8_t b1)
{
    int y;
    for (y = 0; y < s->h; ++y) {
        float t = (float)y / (float)(s->h - 1);
        rect(s, 0, y, s->w, 1, rgb(s,
             (uint8_t)(r0 + (r1 - r0) * t),
             (uint8_t)(g0 + (g1 - g0) * t),
             (uint8_t)(b0 + (b1 - b0) * t)));
    }
}

static void draw_story(Game *g)
{
    SDL_Surface *s = g->screen;
    const Chapter *chapter = &chapters[g->chapter];
    const CampaignMission *mission = &campaign_missions[g->chapter][g->mission];
    char buf[48];
    uint32_t ink = rgb(s, 47, 37, 27);
    uint32_t red = rgb(s, 125, 39, 31);
    rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 34, 24, 17));
    if (g->journal) SDL_BlitSurface(g->journal, NULL, s, NULL);
    journal_text(g, 112, journal_line_y(0), "FLIGHT LOG", red);
    journal_text(g, 112, journal_line_y(2), "NEDDY SEAGOON", ink);
    journal_text(g, 112, journal_line_y(3), "ROYAL FLYING CORPS", ink);
    journal_text(g, 112, journal_line_y(5), chapter->date, red);
    snprintf(buf, sizeof(buf), "CHAPTER %d / MISSION %d",
             g->chapter + 1, g->mission + 1);
    journal_text(g, 112, journal_line_y(7), buf, ink);
    journal_paragraph(g, 112, journal_line_y(9), chapter->title, 22, red);
    journal_text(g, 348, journal_line_y(0), mission->title, red);
    journal_text(g, 348, journal_line_y(2), mission_mode_name(mission->mode), ink);
    journal_paragraph(g, 348, journal_line_y(4), mission->log, 25, ink);
    if (mission->mode == MISSION_DOGFIGHT_3D) {
        journal_text(g, 348, journal_line_y(11), "FLYING WITH:", red);
        snprintf(buf, sizeof(buf), "%s / %s", allied_pilot_names[0],
                 allied_pilot_names[2]);
        journal_text(g, 348, journal_line_y(12), buf, ink);
        journal_text(g, 348, journal_line_y(13), allied_pilot_names[1], ink);
    } else if (mission->mode == MISSION_GUNNER_3D) {
        journal_text(g, 348, journal_line_y(11), "REAR COCKPIT:", red);
        journal_text(g, 348, journal_line_y(12), "TOM AT THE CONTROLS", ink);
        journal_text(g, 348, journal_line_y(13), "LEWIS GUN READY", ink);
    }
    journal_text(g, 348, journal_line_y(14), "ENTER - READ ORDERS", red);
}

static void draw_briefing(Game *g)
{
    SDL_Surface *s = g->screen;
    const Chapter *chapter = &chapters[g->chapter];
    const CampaignMission *mission = &campaign_missions[g->chapter][g->mission];
    const char *order0;
    const char *order1;
    const char *order2;
    char mission_line[48];
    uint32_t ink = rgb(s, 47, 37, 27);
    uint32_t red = rgb(s, 125, 39, 31);
    rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 34, 24, 17));
    if (g->mission_board) SDL_BlitSurface(g->mission_board, NULL, s, NULL);
    if (mission->mode == MISSION_TOP_DOWN) {
        order0 = "FOLLOW THE MARKED MAP GRID.";
        order1 = "OBSERVE OR BOMB THE ASSIGNED SECTOR.";
        order2 = "RETURN ALONE WITH YOUR REPORT.";
    } else if (mission->mode == MISSION_DOGFIGHT_3D) {
        order0 = "RENDEZVOUS WITH THE ALLIED FLIGHT.";
        order1 = "ENGAGE THE ENEMY SQUADRON.";
        order2 = "BRING YOUR WINGMEN HOME.";
    } else if (mission->mode == MISSION_GUNNER_3D) {
        order0 = "TOM WILL HOLD THE RECONNAISSANCE COURSE.";
        order1 = "DEFEND THE REAR QUARTER WITH THE LEWIS GUN.";
        order2 = "KEEP THE TWO-SEATER IN THE AIR.";
    } else {
        order0 = chapter->orders[0];
        order1 = chapter->orders[1];
        order2 = chapter->orders[2];
    }
    display_text(g, 69, 36, "GENERAL HEADQUARTERS", 12, 18, ink);
    typewriter_text(g, 72, 62, "BY ORDER OF THE COMMANDER-IN-CHIEF", 7, 11, red);
    snprintf(mission_line, sizeof(mission_line), "%s  %d/12",
             mission_mode_name(mission->mode), g->mission + 1);
    typewriter_text(g, 72, 84, mission_line, 7, 11, ink);
    typewriter_text(g, 72, 105, mission->title, 7, 11, red);
    typewriter_text(g, 72, 127, order0, 7, 11, ink);
    typewriter_text(g, 72, 143, order1, 7, 11, ink);
    typewriter_text(g, 72, 159, order2, 7, 11, ink);
    display_text(g, 72, 177, "FLIGHT ORDERS", 16, 23, red);
    typewriter_text(g, 73, 205,
                    mission->mode == MISSION_GUNNER_3D ?
                    "ARROW KEYS  TRAVERSE GUN RING" :
                    "ARROW KEYS  MANOEUVRE", 7, 11, ink);
    typewriter_text(g, 73, 221,
                    mission->mode == MISSION_GUNNER_3D ?
                    "PILOT        HOLDS THE COURSE" :
                    "X / Z       THROTTLE", 7, 11, ink);
    if (chapter->armed) {
        typewriter_text(g, 73, 237,
                        mission->mode == MISSION_GUNNER_3D ?
                        "SPACE       LEWIS GUN" :
                        "SPACE       MACHINE GUN", 7, 11, ink);
        typewriter_text(g, 73, 253,
                        is_perspective_mode(mission->mode) ?
                        "              RETURN ALIVE" :
                        "B           DROP BOMB", 7, 11, ink);
    } else {
        typewriter_text(g, 73, 237, "NO ARMAMENT FITTED", 7, 11, red);
        typewriter_text(g, 73, 253, "OBSERVE AND RETURN", 7, 11, ink);
    }
    rect(s, 68, 344, 194, 25, rgb(s, 184, 139, 63));
    rect(s, 70, 346, 190, 21, rgb(s, 57, 39, 27));
    circle(s, 76, 351, 2, rgb(s, 184, 139, 63));
    circle(s, 253, 351, 2, rgb(s, 184, 139, 63));
    typewriter_text(g, 83, 351, "ENTER  FLY THE MISSION", 7, 11,
                    rgb(s, 239, 215, 155));
}

static void draw_ground_gun(Game *g, int sx, int ty, Target *target)
{
    SDL_Surface *s = g->screen;
    SDL_Surface *sprite = target->type ? g->aa_gun_sprite :
                                        g->machine_gun_sprite;
    uint32_t iron = rgb(s, 45, 46, 39);
    uint32_t olive = rgb(s, 72, 75, 57);
    uint32_t sand = rgb(s, 119, 96, 66);
    int flash = SDL_GetTicks() - target->last_shot < 180;
    if (sprite) {
        SDL_Rect dst = {sx - sprite->w / 2, ty + 12 - sprite->h, 0, 0};
        SDL_BlitSurface(sprite, NULL, s, &dst);
        if (flash) {
            if (target->type)
                circle(s, sx + 20, ty - 57, 7, rgb(s, 255, 177, 54));
            else
                circle(s, sx + 49, ty - 24, 6, rgb(s, 255, 210, 78));
        }
        return;
    }
    if (target->type == 0) {
        /* Vickers-style machine-gun nest. */
        circle(s, sx - 8, ty + 4, 7, sand);
        circle(s, sx + 6, ty + 4, 7, sand);
        line(s, sx - 1, ty, sx - 15, ty - 8, iron);
        line(s, sx, ty, sx - 5, ty + 10, iron);
        line(s, sx, ty, sx + 7, ty + 10, iron);
        rect(s, sx - 5, ty - 4, 10, 6, olive);
        if (flash) circle(s, sx - 17, ty - 9, 3, rgb(s, 255, 196, 72));
    } else {
        /* Wheeled anti-aircraft cannon with a high-angle barrel. */
        circle(s, sx - 8, ty + 7, 6, iron);
        circle(s, sx + 9, ty + 7, 6, iron);
        rect(s, sx - 13, ty - 2, 27, 8, olive);
        circle(s, sx, ty - 5, 7, iron);
        line(s, sx, ty - 7, sx + 12, ty - 25, iron);
        line(s, sx + 1, ty - 7, sx + 13, ty - 25, iron);
        if (flash) circle(s, sx + 14, ty - 27, 4, rgb(s, 255, 177, 54));
    }
}

static void draw_scenery(Game *g)
{
    SDL_Surface *s = g->screen;
    int i;
    int base = ((int)g->scroll / 150) * 150;
    for (i = -1; i < 7; ++i) {
        int wx = base + i * 150;
        int sx = wx - (int)g->scroll;
        int gy = terrain_y((float)wx);
        int kind = abs(wx / 150) % 6;
        if (kind == 0) {
            /* A shelled farmhouse. */
            rect(s, sx - 15, gy - 22, 30, 22, rgb(s, 91, 68, 49));
            line(s, sx - 19, gy - 25, sx - 1, gy - 35, rgb(s, 55, 45, 38));
            line(s, sx - 1, gy - 35, sx + 18, gy - 24, rgb(s, 55, 45, 38));
            rect(s, sx + 3, gy - 22, 9, 10, rgb(s, 30, 29, 27));
            rect(s, sx - 4, gy - 13, 8, 13, rgb(s, 40, 35, 31));
        } else if (kind == 1) {
            /* A blasted tree rather than a tidy green lollipop. */
            rect(s, sx - 2, gy - 19, 4, 19, rgb(s, 57, 43, 31));
            line(s, sx, gy - 18, sx - 10, gy - 31, rgb(s, 57, 43, 31));
            line(s, sx - 5, gy - 25, sx - 15, gy - 24, rgb(s, 57, 43, 31));
            line(s, sx, gy - 17, sx + 11, gy - 29, rgb(s, 57, 43, 31));
        } else if (kind == 2) {
            /* Shell crater. */
            circle(s, sx, gy + 2, 15, rgb(s, 49, 40, 32));
            circle(s, sx, gy - 1, 10, rgb(s, 73, 55, 39));
            rect(s, sx - 14, gy, 28, 8, rgb(s, 42, 36, 31));
        } else if (kind == 3) {
            /* Sandbags and a short trench cut. */
            int n;
            rect(s, sx - 28, gy - 4, 56, 8, rgb(s, 48, 39, 31));
            for (n = -3; n <= 3; ++n)
                circle(s, sx + n * 8, gy - 6 - (n & 1) * 3, 5,
                       rgb(s, 119, 96, 66));
        } else if (kind == 4) {
            /* A lone surviving poplar. */
            rect(s, sx - 2, gy - 25, 4, 25, rgb(s, 57, 43, 31));
            circle(s, sx, gy - 38, 9, rgb(s, 42, 62, 39));
            circle(s, sx - 3, gy - 31, 8, rgb(s, 49, 70, 42));
        } else {
            /* Smouldering impact column. */
            int drift = (int)(sinf((SDL_GetTicks() + wx) * 0.003f) * 4.0f);
            circle(s, sx, gy - 8, 5, rgb(s, 67, 60, 52));
            circle(s, sx + drift, gy - 19, 7, rgb(s, 91, 84, 74));
            circle(s, sx + drift * 2, gy - 32, 9, rgb(s, 111, 105, 96));
        }
    }
}

static void draw_cloud_cluster(SDL_Surface *s, int x, int y, int width,
                               uint8_t alpha, uint32_t seed, int shadow)
{
    int count = 5 + (int)(seed % 4u);
    int i;
    for (i = 0; i < count; ++i) {
        uint32_t lobe = scene_hash(seed + (uint32_t)i * 2246822519u);
        int px = x + (int)(lobe % (uint32_t)width);
        int py = y + (int)((lobe >> 9) % 19u) - 9;
        int radius = width / 10 +
                     (int)((lobe >> 17) % (uint32_t)(width / 12 + 2));
        if (shadow)
            blend_circle(s, px + 13, py + 19, radius + 2,
                         35, 42, 43, (uint8_t)(alpha / 3));
        blend_circle(s, px, py, radius, 222, 224, 217, alpha);
        blend_circle(s, px - radius / 4, py - radius / 3,
                     radius * 2 / 3, 246, 245, 234,
                     (uint8_t)(alpha * 2 / 3));
    }
    blend_rect(s, x + width / 9, y + 2, width * 7 / 9, width / 9,
               202, 205, 200, (uint8_t)(alpha / 2));
}

static void draw_sky_detail(Game *g)
{
    SDL_Surface *s = g->screen;
    int camera = (int)(g->scroll * 0.075f);
    int first = camera / 190 - 2;
    int i;
    for (i = 0; i < 7; ++i) {
        int segment = first + i;
        uint32_t seed = scene_hash((uint32_t)segment * 2654435761u ^
                                   (uint32_t)(g->chapter + 3) * 3266489917u);
        int x = segment * 190 - camera + (int)(seed % 105u) - 46;
        int y = 34 + (int)((seed >> 8) % 112u);
        int width = 64 + (int)((seed >> 17) % 88u);
        uint8_t alpha = (uint8_t)(54 + ((seed >> 25) & 35u));
        draw_cloud_cluster(s, x, y, width, alpha, seed, 0);
    }
}

static void draw_top_down_clouds(Game *g)
{
    int i;
    for (i = 0; i < 8; ++i) {
        uint32_t seed = scene_hash((uint32_t)(i + 1) * 2654435761u ^
                                   (uint32_t)(g->chapter + 1) * 2246822519u ^
                                   (uint32_t)(g->mission + 7) * 3266489917u);
        float travel = (float)(seed % 620u) + g->scroll *
                       (0.105f + (float)((seed >> 10) & 31u) * 0.0018f);
        int y = (int)fmodf(travel, 570.0f) - 92;
        int x = (int)((seed >> 12) % 690u) - 72 +
                (int)(sinf(g->scroll * 0.0012f + i * 1.37f) * 19.0f);
        int width = 72 + (int)((seed >> 21) % 105u);
        uint8_t alpha = (uint8_t)(39 + ((seed >> 27) & 23u));
        draw_cloud_cluster(g->screen, x, y, width, alpha, seed, 1);
    }
}

static void draw_distant_villages(Game *g)
{
    SDL_Surface *s = g->screen;
    int i;
    int drift = (int)(g->scroll * 0.32f);
    for (i = -1; i < 6; ++i) {
        int x = i * 150 - (drift % 150);
        int ground = 278 + (int)(12.0f * sinf((x + drift) * 0.012f));
        uint32_t silhouette = rgb(s, 61, 72, 68);
        rect(s, x + 18, ground - 18, 36, 18, silhouette);
        line(s, x + 14, ground - 18, x + 36, ground - 31, silhouette);
        line(s, x + 36, ground - 31, x + 58, ground - 18, silhouette);
        if ((i & 1) == 0) {
            rect(s, x + 70, ground - 38, 14, 38, silhouette);
            line(s, x + 67, ground - 38, x + 77, ground - 54, silhouette);
            line(s, x + 77, ground - 54, x + 87, ground - 38, silhouette);
        }
    }
}

static void draw_explosion_frame(Game *g, SDL_Surface *atlas, int frame,
                                 int center_x, int anchor_y, int draw_size,
                                 int ground_profile, uint8_t alpha)
{
    SDL_Surface *dst = g->screen;
    const int cell = 96;
    int source_x = (frame & 3) * cell;
    int source_y = (frame >> 2) * cell;
    int left = center_x - draw_size / 2;
    int top = ground_profile ? anchor_y - draw_size + draw_size / 10
                             : anchor_y - draw_size / 2;
    int dx, dy;
    if (!atlas || atlas->w < 384 || atlas->h < 384) return;
    if (SDL_MUSTLOCK(atlas)) SDL_LockSurface(atlas);
    if (SDL_MUSTLOCK(dst)) SDL_LockSurface(dst);
    for (dy = 0; dy < draw_size; ++dy) {
        int ty = top + dy;
        int sy = source_y + dy * cell / draw_size;
        if (ty < 0 || ty >= dst->h) continue;
        for (dx = 0; dx < draw_size; ++dx) {
            int tx = left + dx;
            int sx = source_x + dx * cell / draw_size;
            uint8_t r, green, b;
            uint32_t source;
            if (tx < 0 || tx >= dst->w) continue;
            source = surface_pixel(atlas, sx, sy);
            SDL_GetRGB(source, atlas->format, &r, &green, &b);
            if ((r > 168 && b > 115 && green + 42 < r && green + 42 < b) ||
                (r > 70 && b > 45 && green + 28 < r && green + 10 < b))
                continue;
            if (alpha >= 250) {
                pixel(dst, tx, ty, rgb(dst, r, green, b));
            } else {
                uint8_t dr, dg, db;
                uint32_t bg = ((uint32_t *)((uint8_t *)dst->pixels +
                                              ty * dst->pitch))[tx];
                SDL_GetRGB(bg, dst->format, &dr, &dg, &db);
                dr = (uint8_t)((dr * (255 - alpha) + r * alpha) / 255);
                dg = (uint8_t)((dg * (255 - alpha) + green * alpha) / 255);
                db = (uint8_t)((db * (255 - alpha) + b * alpha) / 255);
                pixel(dst, tx, ty, rgb(dst, dr, dg, db));
            }
        }
    }
    if (SDL_MUSTLOCK(dst)) SDL_UnlockSurface(dst);
    if (SDL_MUSTLOCK(atlas)) SDL_UnlockSurface(atlas);
}

static void draw_explosions(Game *g)
{
    SDL_Surface *s = g->screen;
    int top_down = campaign_missions[g->chapter][g->mission].mode ==
                   MISSION_TOP_DOWN;
    int i;
    for (i = 0; i < MAX_EXPLOSIONS; ++i) if (g->explosions[i].active) {
        Explosion *e = &g->explosions[i];
        float p = e->age / e->duration;
        int radius = (int)((e->large == 2 ? 43.0f :
                            (e->large ? 29.0f : 15.0f)) * p) + 3;
        int x = e->world_axis == 2 ? (int)(e->x - g->scroll) : (int)e->x;
        int y = e->world_axis == 1 ?
                SCREEN_H - (int)(e->y - g->scroll) : (int)e->y;
        uint32_t seed = scene_hash((uint32_t)i * 2246822519u +
                                   (uint32_t)(e->x * 13.0f));
        SDL_Surface *atlas = top_down && g->topdown_explosion_atlas ?
                             g->topdown_explosion_atlas : g->explosion_atlas;
        if (atlas) {
            int ground = e->large >= 2;
            float violent = ground ? 1.12f : (e->large ? 0.76f : 0.38f);
            int frame_base = top_down ? (e->large == 3 ? 8 : 0) :
                             (ground ? 8 : 0);
            int size = e->large == 3 ? (top_down ? 214 : 205) :
                       (e->large == 2 ? (top_down ? 148 : 164) :
                       (e->large ? 108 : 58));
            if (e->age <= violent) {
                int frame = (int)(e->age / violent * 8.0f);
                if (frame > 7) frame = 7;
                draw_explosion_frame(g, atlas, frame_base + frame, x, y,
                                     size, ground && !top_down, 255);
            } else {
                float smoke = (e->age - violent) / (e->duration - violent);
                uint8_t alpha = (uint8_t)((1.0f - smoke) * 225.0f);
                int rise = (int)(smoke * (ground ? 74.0f : 48.0f));
                int growth = (int)(smoke * (ground ? 34.0f : 18.0f));
                draw_explosion_frame(g, atlas, frame_base + 7, x, y - rise,
                                     size + growth, ground && !top_down,
                                     alpha);
            }
            continue;
        }
        if (e->large >= 2) {
            int lobe;
            if (p < 0.10f) {
                int flash = 32 - (int)(p * 190.0f);
                circle(s, x, y, flash, rgb(s, 255, 238, 184));
                circle(s, x, y, flash / 2, rgb(s, 255, 255, 226));
            }
            if (p < 0.48f) {
                int fire_r = radius < 8 ? 8 : radius;
                circle(s, x, y, fire_r, rgb(s, 204, 63, 23));
                circle(s, x - fire_r / 3, y - fire_r / 4,
                       fire_r * 2 / 3, rgb(s, 245, 126, 31));
                circle(s, x + fire_r / 4, y - fire_r / 3,
                       fire_r / 2, rgb(s, 255, 211, 82));
                for (lobe = 0; lobe < 8; ++lobe) {
                    float angle = (float)lobe * (float)M_PI * 0.25f;
                    int reach = 10 + (int)(p * (52 + (seed >> (lobe & 7)) % 22));
                    int dx = (int)(cosf(angle) * reach);
                    int dy = (int)(sinf(angle) * reach * 0.66f);
                    line(s, x, y, x + dx, y + dy,
                         rgb(s, 226, 104, 31));
                    circle(s, x + dx, y + dy, 2 + lobe % 3,
                           rgb(s, 255, 177, 50));
                }
            }
            /* The fireball rolls upward into several overlapping, expanding
             * smoke lobes instead of collapsing into one grey disc. */
            if (p > 0.22f) {
                float smoke_p = (p - 0.22f) / 0.78f;
                for (lobe = 0; lobe < 7; ++lobe) {
                    int drift = (int)((((seed >> (lobe * 3)) & 7u) - 3) *
                                      (3.0f + smoke_p * 2.0f));
                    int py = y - 7 - (int)(smoke_p * (20 + lobe * 5));
                    int pr = 7 + lobe / 2 + (int)(smoke_p * 11.0f);
                    uint8_t grey = (uint8_t)(58 + lobe * 7 +
                                             (int)(smoke_p * 24.0f));
                    circle(s, x + drift, py, pr,
                           rgb(s, grey, grey - 3, grey - 8));
                    circle(s, x + drift - pr / 3, py - pr / 4, pr * 2 / 3,
                           rgb(s, grey + 14, grey + 11, grey + 6));
                }
            }
        } else if (p < 0.48f) {
            circle(s, x, y, radius, rgb(s, 239, 116, 35));
            circle(s, x, y, radius / 2, rgb(s, 255, 221, 108));
        } else {
            circle(s, x - radius / 3, y, radius / 2, rgb(s, 69, 64, 58));
            circle(s, x + radius / 3, y - radius / 4, radius / 2, rgb(s, 88, 82, 73));
            circle(s, x, y - radius / 2, radius / 2, rgb(s, 106, 99, 88));
        }
    }
}

static void draw_aircraft_smoke(Game *g, int x, int y, int health,
                                int trail_direction, uint32_t seed)
{
    SDL_Surface *s = g->screen;
    int i;
    uint32_t now = SDL_GetTicks();
    int count = health < 42 ? 12 : 8;
    for (i = 0; i < count; ++i) {
        float phase = fmodf((float)now * 0.00072f +
                            (float)i / (float)count, 1.0f);
        int distance = 28 + (int)(phase * (health < 42 ? 118.0f : 82.0f));
        int wobble = (int)(sinf(phase * 13.0f + i * 1.9f +
                                 (seed & 15u)) * (3.0f + phase * 5.0f));
        int px = x + trail_direction * distance;
        int py = y - 2 - (int)(phase * 20.0f) + wobble;
        int radius = 5 + (int)(phase * (health < 42 ? 14.0f : 10.0f));
        uint8_t grey = (uint8_t)(54 + (int)(phase * 66.0f));
        circle(s, px, py, radius, rgb(s, grey, grey, grey - 5));
        circle(s, px - trail_direction * radius / 3, py - radius / 3,
               radius * 2 / 3, rgb(s, grey + 19, grey + 18, grey + 14));
        circle(s, px + trail_direction * radius / 3, py + radius / 5,
               radius / 2, rgb(s, grey - 9, grey - 10, grey - 13));
    }
}

static void draw_player_damage(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t now = SDL_GetTicks();
    if (g->health >= 80) return;
    draw_aircraft_smoke(g, (int)g->player_x, (int)g->player_y,
                        g->health, -1, 0x4e454444u);
    if (g->health < 42) {
        int flicker = (now / 45) & 3;
        circle(s, (int)g->player_x - 28, (int)g->player_y + 2,
               7 + flicker, rgb(s, 237, 89, 24));
        circle(s, (int)g->player_x - 26, (int)g->player_y,
               4 + (flicker & 1), rgb(s, 255, 215, 75));
    }
}

static void draw_field_sign(Game *g, int x, int y, int w, const char *label)
{
    SDL_Surface *s = g->screen;
    int text_width = (int)strlen(label) * 7;
    int text_x = x + (w - text_width) / 2;
    uint32_t wood = rgb(s, 54, 38, 27);
    uint32_t brass = rgb(s, 180, 137, 63);
    rect(s, x + 2, y + 3, w, 25, rgb(s, 25, 25, 24));
    rect(s, x, y, w, 25, brass);
    rect(s, x + 2, y + 2, w - 4, 21, wood);
    circle(s, x + 6, y + 6, 2, brass);
    circle(s, x + w - 7, y + 6, 2, brass);
    if (text_x < x + 4) text_x = x + 4;
    typewriter_text(g, text_x, y + 6, label, 7, 11,
                    rgb(s, 239, 215, 155));
}

static void draw_top_plane(SDL_Surface *s, int x, int y, int enemy)
{
    uint32_t dark = rgb(s, 39, 31, 25);
    uint32_t body = enemy ? rgb(s, 126, 36, 31) : rgb(s, 54, 92, 57);
    uint32_t cloth = enemy ? rgb(s, 201, 167, 132) : rgb(s, 224, 208, 160);
    rect(s, x - 3, y - 24, 7, 49, body);
    rect(s, x - 25, y - 8, 51, 6, cloth);
    rect(s, x - 22, y + 4, 45, 5, cloth);
    line(s, x - 18, y - 7, x - 16, y + 8, dark);
    line(s, x + 18, y - 7, x + 16, y + 8, dark);
    rect(s, x - 10, y + 18, 21, 5, body);
    circle(s, x, y - 24, 4, dark);
    line(s, x - 7, y - 27, x + 7, y - 21, dark);
    line(s, x + 7, y - 27, x - 7, y - 21, dark);
}

static void draw_top_target(Game *g, int x, int y, Target *target)
{
    SDL_Surface *s = g->screen;
    uint32_t earth = rgb(s, 74, 58, 39);
    uint32_t iron = rgb(s, 47, 48, 42);
    uint32_t sand = rgb(s, 139, 112, 73);
    if (target->type == 2) {
        /* Moving road convoy, seen from directly above. */
        rect(s, x - 19, y - 8, 38, 16, rgb(s, 38, 36, 29));
        rect(s, x - 15, y - 7, 21, 14, rgb(s, 76, 83, 55));
        rect(s, x + 6, y - 6, 11, 12, rgb(s, 92, 94, 62));
        rect(s, x + 10, y - 4, 5, 8, rgb(s, 42, 48, 41));
        circle(s, x - 11, y - 9, 3, iron);
        circle(s, x - 11, y + 9, 3, iron);
        circle(s, x + 11, y - 8, 3, iron);
        circle(s, x + 11, y + 8, 3, iron);
        line(s, x - 14, y - 2, x + 5, y - 2, sand);
        return;
    }
    circle(s, x, y, target->type ? 16 : 13, earth);
    circle(s, x, y, target->type ? 12 : 10, sand);
    rect(s, x - 6, y - 5, 12, 10, iron);
    if (target->type) {
        line(s, x, y - 2, x + 3, y - 23, iron);
        line(s, x + 2, y - 2, x + 6, y - 23, iron);
    } else {
        line(s, x, y - 2, x, y - 19, iron);
    }
}

static void draw_top_down_sortie(Game *g)
{
    SDL_Surface *s = g->screen;
    int x, y, i;
    char buf[64];
    uint32_t field_a = rgb(s, 91, 91, 56);
    uint32_t field_b = rgb(s, 112, 96, 58);
    uint32_t trench = rgb(s, 48, 39, 31);
    int offset = (int)g->scroll % 96;
    if (g->topdown_battlefield) {
        SDL_Rect source, dest;
        int texture_h = g->topdown_battlefield->h;
        int texture_x = (g->topdown_battlefield->w - SCREEN_W) / 2;
        int position = (texture_h - ((int)g->scroll % texture_h)) % texture_h;
        int first = texture_h - position;
        if (first > SCREEN_H) first = SCREEN_H;
        source.x = texture_x;
        source.y = position;
        source.w = SCREEN_W;
        source.h = first;
        dest.x = 0;
        dest.y = 0;
        SDL_BlitSurface(g->topdown_battlefield, &source, s, &dest);
        if (first < SCREEN_H) {
            source.y = 0;
            source.h = SCREEN_H - first;
            dest.y = first;
            SDL_BlitSurface(g->topdown_battlefield, &source, s, &dest);
        }
    } else {
        rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 105, 101, 65));
        for (y = -96; y < SCREEN_H + 96; y += 96) {
            int row = (y + offset) / 96;
            int sy = y + offset;
            for (x = 0; x < SCREEN_W; x += 128)
                rect(s, x + 2, sy + 2, 124, 92,
                     ((x / 128 + row) & 1) ? field_a : field_b);
            line(s, 0, sy, SCREEN_W, sy, rgb(s, 64, 60, 43));
        }
        rect(s, 286, 0, 68, SCREEN_H, rgb(s, 127, 108, 72));
        line(s, 294, 0, 294, SCREEN_H, rgb(s, 84, 72, 52));
        line(s, 346, 0, 346, SCREEN_H, rgb(s, 84, 72, 52));
        for (y = -120; y < SCREEN_H + 120; y += 170) {
            int sy = y + ((int)g->scroll % 170);
            line(s, 0, sy, SCREEN_W, sy + 18, trench);
            line(s, 0, sy + 6, SCREEN_W, sy + 24, trench);
        }
    }
    for (i = 0; i < MAX_TARGETS; ++i) if (g->targets[i].active) {
        float fx, fy;
        int tx, ty;
        top_down_target_position(g, i, &fx, &fy);
        tx = (int)fx;
        ty = (int)fy;
        if (ty > -30 && ty < SCREEN_H + 30)
            draw_top_target(g, tx, ty, &g->targets[i]);
    }
    draw_top_down_clouds(g);
    for (i = 0; i < MAX_BULLETS; ++i) if (g->bullets[i].active) {
        Bullet *b = &g->bullets[i];
        if (b->type == 1) {
            float fall = 1.0f - b->life / b->initial_life;
            int shadow_y = (int)b->y + 28 - (int)(fall * 22.0f);
            int body = 3 + (int)(fall * 4.0f);
            blend_circle(s, (int)b->x + 8, shadow_y, 6 + body,
                         28, 30, 27, 65);
            circle(s, (int)b->x, (int)b->y, body, rgb(s, 48, 43, 35));
            circle(s, (int)b->x - 1, (int)b->y - body / 2,
                   body / 2, rgb(s, 178, 145, 79));
        } else {
            uint32_t c = b->enemy ? rgb(s, 255, 107, 53) : rgb(s, 255, 230, 143);
            rect(s, (int)b->x - 1, (int)b->y - 7, 3, 12, c);
        }
    }
    draw_explosions(g);
    if (g->health < 80)
        draw_aircraft_smoke(g, (int)g->player_x, (int)g->player_y,
                            g->health, 1, 0x544f5044u);
    if (g->topdown_player) {
        SDL_Rect dst = {(int)g->player_x - g->topdown_player->w / 2,
                        (int)g->player_y - g->topdown_player->h / 2, 0, 0};
        SDL_BlitSurface(g->topdown_player, NULL, s, &dst);
    } else {
        draw_top_plane(s, (int)g->player_x, (int)g->player_y, 0);
    }
    snprintf(buf, sizeof(buf), "SCORE %05d", g->score);
    draw_field_sign(g, 8, 8, 112, buf);
    snprintf(buf, sizeof(buf), "TARGETS %d/%d", g->objectives,
             current_mission_goal(g));
    draw_field_sign(g, 8, 39, 112, buf);
    snprintf(buf, sizeof(buf), "AMMO %03d", g->ammunition);
    draw_field_sign(g, 264, 8, 108, buf);
    snprintf(buf, sizeof(buf), "BOMBS %02d", g->bombs_remaining);
    draw_field_sign(g, 264, 39, 108, buf);
    snprintf(buf, sizeof(buf), "FUEL %02d", (int)g->fuel);
    draw_field_sign(g, 526, 8, 106, buf);
    snprintf(buf, sizeof(buf), "AIRFRAME %03d", g->health > 0 ? g->health : 0);
    draw_field_sign(g, 502, 39, 130, buf);
}

static void draw_perspective_plane(Game *g, int x, int y, int size,
                                   int enemy, float bank)
{
    SDL_Surface *s = g->screen;
    uint32_t body = enemy ? rgb(s, 132, 38, 32) : rgb(s, 63, 91, 58);
    uint32_t cloth = enemy ? rgb(s, 202, 170, 137) : rgb(s, 221, 205, 158);
    uint32_t dark = rgb(s, 38, 31, 27);
    uint32_t marking = enemy ? rgb(s, 45, 39, 34) : rgb(s, 180, 45, 42);
    int half, wing, strut;
    if (size < 8) size = 8;
    if (size > 58) size = 58;
    /* At long range, downsampling the detailed painted sprite leaves isolated
     * propeller and undercarriage pixels which read as screen corruption.
     * Use the clean vector silhouette until the aircraft is large enough for
     * its painted detail to survive. */
    if (enemy && g->dogfight_enemy && size >= 56) {
        SDL_Surface *sprite = g->dogfight_enemy;
        int draw_w = size * 2;
        int draw_h = size;
        int dx, dy;
        float cosine = cosf(bank);
        float sine = sinf(bank);
        if (SDL_MUSTLOCK(sprite)) SDL_LockSurface(sprite);
        for (dy = -draw_w / 2; dy <= draw_w / 2; ++dy) {
            for (dx = -draw_w / 2; dx <= draw_w / 2; ++dx) {
                float ux = cosine * (float)dx + sine * (float)dy;
                float uy = -sine * (float)dx + cosine * (float)dy;
                int sx, sy;
                int tx = x + dx;
                int ty = y + dy;
                uint8_t r, green, b;
                if (ux < -(float)draw_w * 0.5f ||
                    ux >= (float)draw_w * 0.5f ||
                    uy < -(float)draw_h * 0.5f ||
                    uy >= (float)draw_h * 0.5f)
                    continue;
                sx = (int)((ux + draw_w * 0.5f) * sprite->w / draw_w);
                sy = (int)((uy + draw_h * 0.5f) * sprite->h / draw_h);
                SDL_GetRGB(surface_pixel(sprite, sx, sy), sprite->format,
                           &r, &green, &b);
                /* SDL2.module's scaled blit does not preserve colour keys on
                 * every backend.  Keep this memory-to-memory scale explicit. */
                if (r > 170 && b > 120 && green + 35 < r &&
                    green + 35 < b) continue;
                pixel(s, tx, ty, rgb(s, r, green, b));
            }
        }
        if (SDL_MUSTLOCK(sprite)) SDL_UnlockSurface(sprite);
        return;
    }
    half = size / 2;
    wing = size * 4 / 5;
    strut = size / 3;
    if (enemy) {
        int radius = size / 5;
        int left_y = y - size / 4 - (int)(bank * wing);
        int right_y = y - size / 4 + (int)(bank * wing);
        int lower_left_y = y + size / 8 - (int)(bank * wing * 0.8f);
        int lower_right_y = y + size / 8 + (int)(bank * wing * 0.8f);
        int thickness;
        if (radius < 2) radius = 2;
        /* Head-on attacker: the cowling and propeller face the rear gunner.
         * This deliberately differs from the rear-quarter allied silhouette. */
        for (thickness = -2; thickness <= 2; ++thickness) {
            line(s, x - wing, left_y + thickness,
                 x + wing, right_y + thickness,
                 thickness == -2 || thickness == 2 ? dark : cloth);
            line(s, x - wing + size / 8, lower_left_y + thickness,
                 x + wing - size / 8, lower_right_y + thickness,
                 thickness == -2 || thickness == 2 ? dark : cloth);
        }
        line(s, x - strut, y - size / 5, x - strut + 2,
             y + size / 6, dark);
        line(s, x + strut, y - size / 5, x + strut - 2,
             y + size / 6, dark);
        circle(s, x, y, radius + 2, dark);
        circle(s, x, y, radius, body);
        line(s, x - half, y, x + half, y, rgb(s, 92, 82, 68));
        line(s, x, y - half, x, y + half, rgb(s, 92, 82, 68));
        circle(s, x, y, size > 20 ? 3 : 2, rgb(s, 196, 167, 116));
        if (size > 25) {
            rect(s, x - wing * 2 / 3, y - size / 4 + 2,
                 size / 4, 3, marking);
            rect(s, x + wing * 2 / 3 - size / 4,
                 y - size / 4 + 2, size / 4, 3, marking);
        }
        return;
    }
    /* Rear-quarter silhouette: two fabric wings, wire bracing, narrow
     * fuselage, tailplane, wheels and period national markings. */
    rect(s, x - wing, y - size / 4, wing * 2, size / 6 + 2, dark);
    rect(s, x - wing + 2, y - size / 4 + 1,
         wing * 2 - 4, size / 6, cloth);
    rect(s, x - wing + size / 9, y + size / 10,
         (wing - size / 9) * 2, size / 7 + 2, dark);
    rect(s, x - wing + size / 9 + 2, y + size / 10 + 1,
         (wing - size / 9) * 2 - 4, size / 7, cloth);
    rect(s, x - size / 9, y - half, size / 5 + 2, size, body);
    rect(s, x - size / 3, y + half - 5, size * 2 / 3, 5, cloth);
    line(s, x - strut, y - size / 5, x - strut + 2, y + size / 7, dark);
    line(s, x + strut, y - size / 5, x + strut - 2, y + size / 7, dark);
    line(s, x - wing + 3, y - size / 5, x - size / 5, y + size / 7, dark);
    line(s, x + wing - 3, y - size / 5, x + size / 5, y + size / 7, dark);
    if (size > 24) {
        rect(s, x - wing * 2 / 3, y - size / 4 + 2, size / 4, 3, marking);
        rect(s, x + wing * 2 / 3 - size / 4, y - size / 4 + 2,
             size / 4, 3, marking);
        circle(s, x - size / 7, y + half - 1, 2, dark);
        circle(s, x + size / 7, y + half - 1, 2, dark);
    }
}

static void draw_spiralling_enemy(Game *g, const Enemy *enemy, int index)
{
    int size = enemy->crash_size;
    if (size < 10) size = 10;
    if (size > 62) size = 62;
    draw_aircraft_smoke(g, (int)enemy->crash_x, (int)enemy->crash_y,
                        0, 1, 0x43524153u + (uint32_t)index * 97u);
    /* Keep the aeroplane visually intact while its trajectory spirals.  A
     * rotating resample at this scale broke the wings and propeller into
     * disconnected fragments which looked like screen corruption. */
    draw_perspective_plane(g, (int)enemy->crash_x,
                           (int)enemy->crash_y, size, 1,
                           sinf(enemy->crash_angle) * 0.95f);
}

static void draw_flight_background(Game *g)
{
    SDL_Surface *s = g->screen;
    SDL_Surface *bg = g->dogfight_background;
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    int y;
    int bpp;
    int base_x;
    int base_y;
    int locked = 0;
    if (!bg || bg->w < SCREEN_W || bg->h < SCREEN_H) return;
    bpp = s->format->BytesPerPixel;
    if (bg->format->BytesPerPixel != bpp) return;
    base_x = (bg->w - SCREEN_W) / 2;
    base_y = (bg->h - SCREEN_H) / 2 - (int)(g->view_pitch * 58.0f);
    if (mode == MISSION_DOGFIGHT_3D) {
        /* Couple the painted world to aircraft heading.  Previously the
         * physics changed heading but the panorama stayed fixed, so a real
         * turn looked like nothing more than a banked camera. */
        base_x += (int)(g->flight_heading * (float)bg->w /
                        (2.0f * (float)M_PI));
        base_y = (bg->h - SCREEN_H) / 2 -
                 (int)(g->view_pitch * 180.0f);
    }
    if (mode == MISSION_GUNNER_3D) {
        /* The generated panorama is twice the viewport width.  Spend all of
         * that width on the observer's 90-degree rear arc, and use the spare
         * vertical field for the gun's 0..75-degree elevation. */
        base_x += (int)(gunner_traverse(g) *
                        (float)(bg->w - SCREEN_W) * 0.5f);
        base_y -= (int)(gunner_elevation(g) *
                        (float)(bg->h - SCREEN_H));
    }
    if (base_y < 0) base_y = 0;
    if (base_y + SCREEN_H > bg->h) base_y = bg->h - SCREEN_H;
    if (SDL_MUSTLOCK(bg)) {
        if (SDL_LockSurface(bg) != 0) return;
        locked = 1;
    }
    if (mode == MISSION_DOGFIGHT_3D) {
        /* Inverse-map the panorama through the aircraft attitude.  Sampling
         * one source pixel per 2x2 block keeps the 68040 cost bounded while
         * making the horizon genuinely bank, climb and dive with the view. */
        float c = cosf(g->view_bank);
        float bank_sine = sinf(g->view_bank);
        float source_cx = (float)(base_x + SCREEN_W / 2);
        float source_cy = (float)(base_y + SCREEN_H / 2);
        int x;
        for (y = 0; y < SCREEN_H; y += 2) {
            for (x = 0; x < SCREEN_W; x += 2) {
                float dx = (float)(x - SCREEN_W / 2);
                float dy = (float)(y - SCREEN_H / 2);
                int sx = (int)(source_cx + c * dx + bank_sine * dy);
                int sy = (int)(source_cy - bank_sine * dx + c * dy);
                const uint8_t *src;
                uint8_t *dst0;
                uint8_t *dst1;
                sx %= bg->w;
                if (sx < 0) sx += bg->w;
                if (sy < 0) sy = 0;
                if (sy >= bg->h) sy = bg->h - 1;
                src = (const uint8_t *)bg->pixels + sy * bg->pitch + sx * bpp;
                dst0 = (uint8_t *)s->pixels + y * s->pitch + x * bpp;
                memcpy(dst0, src, (size_t)bpp);
                if (x + 1 < SCREEN_W) memcpy(dst0 + bpp, src, (size_t)bpp);
                if (y + 1 < SCREEN_H) {
                    dst1 = dst0 + s->pitch;
                    memcpy(dst1, src, (size_t)bpp);
                    if (x + 1 < SCREEN_W)
                        memcpy(dst1 + bpp, src, (size_t)bpp);
                }
            }
        }
    } else for (y = 0; y < SCREEN_H; ++y) {
        /* The rear gunner pans around the cockpit rather than banking the
         * aeroplane, so its cheaper scanline path remains appropriate. */
        int sx = base_x + (int)(g->view_bank * 0.46f * (y - SCREEN_H / 2));
        int first_pixels;
        const uint8_t *src;
        uint8_t *dst;
        sx %= bg->w;
        if (sx < 0) sx += bg->w;
        src = (const uint8_t *)bg->pixels + (base_y + y) * bg->pitch + sx * bpp;
        dst = (uint8_t *)s->pixels + y * s->pitch;
        first_pixels = bg->w - sx;
        if (first_pixels > SCREEN_W) first_pixels = SCREEN_W;
        memcpy(dst, src, (size_t)first_pixels * (size_t)bpp);
        if (first_pixels < SCREEN_W) {
            src = (const uint8_t *)bg->pixels + (base_y + y) * bg->pitch;
            memcpy(dst + first_pixels * bpp, src,
                   (size_t)(SCREEN_W - first_pixels) * (size_t)bpp);
        }
    }
    if (locked) SDL_UnlockSurface(bg);
}

static void draw_gunner_airframe(Game *g)
{
    SDL_Surface *source = g->gunner_airframe;
    SDL_Surface *dest = g->screen;
    float look = gunner_traverse(g);
    float elevation = gunner_elevation(g);
    float scale = 1.0f - fabsf(look) * 0.14f;
    float centre_shift = -look * 104.0f;
    float vertical_shift = elevation * 34.0f + g->view_pitch * 22.0f;
    const int foreground_drop = 40;
    uint32_t key;
    int x, y;
    int source_locked = 0;
    int dest_locked = 0;
    if (!source || source->w != SCREEN_W || source->h != SCREEN_H) return;
    key = SDL_MapRGB(source->format, 255, 0, 255);
    if (SDL_MUSTLOCK(source)) {
        if (SDL_LockSurface(source) != 0) return;
        source_locked = 1;
    }
    if (SDL_MUSTLOCK(dest)) {
        if (SDL_LockSurface(dest) != 0) {
            if (source_locked) SDL_UnlockSurface(source);
            return;
        }
        dest_locked = 1;
    }
    /* Row-wise inverse mapping keeps the CPU surface authoritative and gives
     * the tail a convincing parallax/shear as the observer turns on the
     * swivelling stool.  Pilot bank rolls the structure independently. */
    for (y = 72; y < SCREEN_H; ++y) {
        for (x = 0; x < SCREEN_W; ++x) {
            float roll = g->view_bank * (float)(x - SCREEN_W / 2) * 0.22f;
            float shear = look * (float)(y - 215) * 0.20f;
            int sy = (int)((float)y - vertical_shift - roll -
                           (float)foreground_drop);
            int sx = (int)(SCREEN_W * 0.5f +
                     ((float)x - SCREEN_W * 0.5f - centre_shift - shear) /
                     scale);
            uint32_t colour;
            if (sx < 0 || sx >= source->w || sy < 0 || sy >= source->h)
                continue;
            colour = surface_pixel(source, sx, sy);
            if (colour != key) pixel(dest, x, y, colour);
        }
    }
    if (dest_locked) SDL_UnlockSurface(dest);
    if (source_locked) SDL_UnlockSurface(source);
}

static void draw_gunner_bullet_holes(Game *g)
{
    SDL_Surface *s = g->screen;
    float look = gunner_traverse(g);
    float elevation = gunner_elevation(g);
    float scale = 1.0f - fabsf(look) * 0.14f;
    float centre_shift = -look * 104.0f;
    float vertical_shift = elevation * 34.0f + g->view_pitch * 22.0f;
    int i;
    for (i = 0; i < MAX_BULLET_HOLES; ++i) if (g->bullet_holes[i].active) {
        float source_x = g->bullet_holes[i].x;
        float source_y = g->bullet_holes[i].y;
        float shear = look * (source_y - 215.0f) * 0.20f;
        int x = (int)(SCREEN_W * 0.5f +
                (source_x - SCREEN_W * 0.5f) * scale + centre_shift + shear);
        float roll = g->view_bank * (float)(x - SCREEN_W / 2) * 0.22f;
        int y = (int)(source_y + vertical_shift + roll + 40.0f);
        circle(s, x, y, 3, rgb(s, 187, 150, 95));
        circle(s, x, y, 2, rgb(s, 35, 27, 22));
        pixel(s, x - 1, y - 1, rgb(s, 232, 198, 135));
    }
}

static void draw_simulator_terrain(Game *g)
{
    SDL_Surface *s = g->screen;
    int horizon_left, horizon_right;
    float horizon_dx, horizon_dy, horizon_length;
    int ground_positive = 1;
    int tx, tz, pass;
    {
        const float far = 12000.0f;
        const float wide = 14000.0f;
        float sh = sinf(g->flight_heading), ch = cosf(g->flight_heading);
        Vec3 centre = {g->flight_x + sh * far, 0.0f,
                       g->flight_z + ch * far};
        Projected3D left = project_world_point(g, (Vec3){
            centre.x - ch * wide, centre.y, centre.z + sh * wide});
        Projected3D right = project_world_point(g, (Vec3){
            centre.x + ch * wide, centre.y, centre.z - sh * wide});
        horizon_left = left.visible ? left.y : SCREEN_H / 2;
        horizon_right = right.visible ? right.y : SCREEN_H / 2;
        horizon_left = (int)flight_clamp((float)horizon_left,
                                         -SCREEN_H * 3.0f,
                                          SCREEN_H * 4.0f);
        horizon_right = (int)flight_clamp((float)horizon_right,
                                          -SCREEN_H * 3.0f,
                                           SCREEN_H * 4.0f);
    }
    horizon_dx = (float)(SCREEN_W - 1);
    horizon_dy = (float)(horizon_right - horizon_left);
    horizon_length = sqrtf(horizon_dx * horizon_dx +
                           horizon_dy * horizon_dy);
    if (horizon_length < 1.0f) horizon_length = 1.0f;
    {
        float sh = sinf(g->flight_heading), ch = cosf(g->flight_heading);
        Projected3D ground_probe = project_world_point(g, (Vec3){
            g->flight_x + sh * 2400.0f, 0.0f,
            g->flight_z + ch * 2400.0f});
        if (ground_probe.visible) {
            float side = horizon_dx *
                         ((float)ground_probe.y - horizon_left) -
                         horizon_dy * (float)ground_probe.x;
            ground_positive = side >= 0.0f;
        }
    }
    {
        uint32_t sky_colour[261];
        uint32_t ground = rgb(s, 70, 74, 45);
        int distance, x, y;
        for (distance = 0; distance <= 260; ++distance) {
            float t = 1.0f - (float)distance / 260.0f;
            uint8_t red = (uint8_t)(23.0f + 169.0f * t);
            uint8_t green = (uint8_t)(48.0f + 113.0f * t);
            uint8_t blue = (uint8_t)(86.0f + 38.0f * t);
            sky_colour[distance] = rgb(s, red, green, blue);
        }
        /* Signed perpendicular distance gives a solid sky and ground at any
         * bank angle.  Parallel raster lines left the regular pinholes seen
         * in the steep-bank screenshots. */
        /* The signed distance is linear along a row, so it is stepped in
         * 16.16 fixed point: one float setup per row, integer work per
         * pixel, no per-pixel divide. */
        {
            int32_t step = (int32_t)(-horizon_dy / horizon_length * 65536.0f);
            for (y = 0; y < SCREEN_H; ++y) {
                uint32_t *row = (uint32_t *)((uint8_t *)s->pixels +
                                             y * s->pitch);
                float start = horizon_dx * ((float)y - horizon_left) /
                              horizon_length;
                int32_t side;
                if (start > 30000.0f) start = 30000.0f;
                if (start < -30000.0f) start = -30000.0f;
                side = (int32_t)(start * 65536.0f);
                for (x = 0; x < SCREEN_W; ++x, side += step) {
                    int is_ground = ground_positive ? side >= 0 : side <= 0;
                    if (is_ground) {
                        row[x] = ground;
                    } else {
                        int32_t haze_distance = (side < 0 ? -side : side) >> 16;
                        if (haze_distance > 260) haze_distance = 260;
                        row[x] = sky_colour[haze_distance];
                    }
                }
            }
        }
    }

    /* Two deterministic ridge rings bridge atmosphere and ground.  Their
     * bearings are fixed in the world, so they roll and pan with the same
     * quaternion projection as the fields rather than sliding on the screen. */
    for (pass = 0; pass < 2; ++pass) {
        const int segments = 64;
        float distance = pass == 0 ? 14200.0f : 9400.0f;
        int segment;
        for (segment = 0; segment < segments; ++segment) {
            float a0 = (float)segment * (2.0f * (float)M_PI / segments);
            float a1 = (float)(segment + 1) *
                       (2.0f * (float)M_PI / segments);
            uint32_t seed0 = scene_hash((uint32_t)segment * 92821u +
                                        (uint32_t)pass * 0x48494c4cu);
            uint32_t seed1 = scene_hash((uint32_t)((segment + 1) % segments) *
                                        92821u +
                                        (uint32_t)pass * 0x48494c4cu);
            float height0 = (pass == 0 ? 430.0f : 260.0f) +
                            (float)(seed0 % (pass == 0 ? 690u : 430u));
            float height1 = (pass == 0 ? 430.0f : 260.0f) +
                            (float)(seed1 % (pass == 0 ? 690u : 430u));
            Vec3 w0 = {g->flight_x + sinf(a0) * distance, 0.0f,
                       g->flight_z + cosf(a0) * distance};
            Vec3 w1 = {g->flight_x + sinf(a1) * distance, 0.0f,
                       g->flight_z + cosf(a1) * distance};
            Projected3D b0 = project_world_point(g, w0);
            Projected3D b1 = project_world_point(g, w1);
            Projected3D t0, t1;
            w0.y = height0;
            w1.y = height1;
            t0 = project_world_point(g, w0);
            t1 = project_world_point(g, w1);
            if (b0.visible && b1.visible && t0.visible && t1.visible &&
                abs(b0.x) < 1500 && abs(b1.x) < 1500) {
                uint32_t hill = pass == 0 ?
                    rgb(s, 91, 99, 83) : rgb(s, 76, 83, 58);
                fill_triangle(s, b0.x, b0.y, b1.x, b1.y,
                              t1.x, t1.y, hill);
                fill_triangle(s, b0.x, b0.y, t1.x, t1.y,
                              t0.x, t0.y, hill);
                if (pass == 1)
                    line(s, t0.x, t0.y, t1.x, t1.y, rgb(s, 103, 105, 76));
            }
        }
    }

    /* A coarse 1.7 km grid carries fields almost to the ridge; the 520 m
     * pass then restores hedges and texture around the aircraft.  Previously
     * the detailed ground simply stopped after 2.6 km, exposing a flat wedge
     * which was visibly disconnected from the horizon. */
    for (pass = 0; pass < 2; ++pass) {
        const float tile_size = pass == 0 ? 1700.0f : 520.0f;
        const int reach = 5;
        int base_tile_x = (int)floorf(g->flight_x / tile_size);
        int base_tile_z = (int)floorf(g->flight_z / tile_size);
        for (tz = base_tile_z - reach; tz <= base_tile_z + reach; ++tz) {
        for (tx = base_tile_x - reach; tx <= base_tile_x + reach; ++tx) {
            float x0 = (float)tx * tile_size;
            float z0 = (float)tz * tile_size;
            Projected3D p0 = project_world_point(g, (Vec3){x0, 0.0f, z0});
            Projected3D p1 = project_world_point(g,
                (Vec3){x0 + tile_size, 0.0f, z0});
            Projected3D p2 = project_world_point(g,
                (Vec3){x0 + tile_size, 0.0f, z0 + tile_size});
            Projected3D p3 = project_world_point(g,
                (Vec3){x0, 0.0f, z0 + tile_size});
            uint32_t seed = scene_hash((uint32_t)(tx * 92821) ^
                                       (uint32_t)(tz * 68917));
            uint32_t field;
            if (pass == 0)
                field = (seed & 1u) ? rgb(s, 79, 82, 57) :
                                      rgb(s, 70, 78, 54);
            else if ((tx % 6) == 0 || (tz % 7) == 0)
                field = rgb(s, 108, 91, 58);
            else if (seed & 1u)
                field = rgb(s, 91, 86, 50);
            else
                field = rgb(s, 66, 88, 49);
            if (p0.visible && p1.visible && p2.visible && p3.visible &&
                abs(p0.x) < 1800 && abs(p1.x) < 1800 &&
                abs(p2.x) < 1800 && abs(p3.x) < 1800 &&
                abs(p0.y) < 1200 && abs(p1.y) < 1200 &&
                abs(p2.y) < 1200 && abs(p3.y) < 1200) {
                fill_triangle(s, p0.x, p0.y, p1.x, p1.y,
                              p2.x, p2.y, field);
                fill_triangle(s, p0.x, p0.y, p2.x, p2.y,
                              p3.x, p3.y, field);
                if (pass == 1) {
                    line(s, p0.x, p0.y, p1.x, p1.y, rgb(s, 48, 61, 36));
                    line(s, p0.x, p0.y, p3.x, p3.y, rgb(s, 56, 65, 39));
                }
            }
        }
        }
    }
}

static void draw_simulator_clouds(Game *g)
{
    SDL_Surface *s = g->screen;
    const float cell_size = 1750.0f;
    int base_x = (int)floorf(g->flight_x / cell_size);
    int base_z = (int)floorf(g->flight_z / cell_size);
    int cx, cz;
    for (cz = base_z - 4; cz <= base_z + 4; ++cz) {
    for (cx = base_x - 4; cx <= base_x + 4; ++cx) {
        uint32_t seed = scene_hash((uint32_t)(cx * 73471) ^
                                   (uint32_t)(cz * 91229) ^ 0x434c4f55u);
        Vec3 centre;
        Projected3D p;
        int radius, puffs, puff, cloud_type;
        if ((seed & 7u) > 2u) continue;
        cloud_type = (int)((seed >> 27) % 3u);
        centre.x = ((float)cx + 0.18f + (float)(seed & 255u) / 410.0f) *
                   cell_size;
        centre.z = ((float)cz + 0.15f +
                   (float)((seed >> 8) & 255u) / 390.0f) * cell_size;
        centre.y = 650.0f + (float)((seed >> 16) % 660u);
        if (cloud_type == 1) centre.y += 180.0f;
        if (cloud_type == 2) centre.y += 360.0f;
        p = project_world_point(g, centre);
        if (!p.visible || p.x < -180 || p.x > SCREEN_W + 180 ||
            p.y < -100 || p.y > SCREEN_H + 100) continue;
        radius = (int)((41000.0f + (float)(seed & 12287u)) / p.depth);
        if (radius < 5) continue;
        if (radius > 44) radius = 44;
        puffs = cloud_type == 2 ? 8 + (int)((seed >> 5) & 3u) :
                5 + (int)((seed >> 5) & 3u);
        /* Three deterministic shapes break up the old row of equally spaced
         * bubbles: low cumulus, wind-smeared stratus and tall storm cells.
         * They stay in world space, so variety does not make them swim across
         * the view as the aircraft turns. */
        if (cloud_type == 1) {
            ellipse(s, p.x, p.y + radius / 6, radius * 8 / 5,
                    radius / 4 + 1, rgb(s, 116, 126, 134));
            ellipse(s, p.x - radius / 3, p.y - radius / 10,
                    radius * 7 / 5, radius / 4 + 1,
                    rgb(s, 174, 180, 180));
        } else {
            ellipse(s, p.x, p.y + radius / 4,
                    cloud_type == 2 ? radius : radius * 6 / 5,
                    radius / 3 + 1, rgb(s, 111, 122, 132));
            ellipse(s, p.x - radius / 8, p.y,
                    cloud_type == 2 ? radius * 3 / 4 : radius,
                    radius / 3 + 1, rgb(s, 160, 167, 168));
        }
        for (puff = 0; puff < puffs; ++puff) {
            uint32_t ps = scene_hash(seed + (uint32_t)puff * 2654435761u);
            int span_x = cloud_type == 1 ? radius * 3 / 2 :
                         cloud_type == 2 ? radius * 3 / 5 : radius;
            int span_y = cloud_type == 1 ? radius / 5 :
                         cloud_type == 2 ? radius : radius / 2;
            int px = p.x + (int)(ps % (uint32_t)(span_x * 2 + 1)) - span_x;
            int py = p.y - (int)((ps >> 9) % (uint32_t)(span_y + 1));
            int pr = radius / 3 + (int)((ps >> 17) %
                     (uint32_t)(radius / 4 + 1));
            int pry;
            uint8_t shade = (uint8_t)(184 + ((ps >> 25) & 23u));
            if (puff == 0) {
                px = p.x - radius / 4;
                py = p.y - (cloud_type == 2 ? radius * 2 / 3 : radius / 3);
            } else if (puff == 1) {
                px = p.x + radius / 4;
                py = p.y - (cloud_type == 2 ? radius / 3 : radius / 4);
            }
            if (pr < 2) pr = 2;
            pry = cloud_type == 1 ? pr / 3 :
                  cloud_type == 2 ? pr * 4 / 5 :
                  pr * (3 + (int)((ps >> 22) & 1u)) / 5;
            if (pry < 2) pry = 2;
            ellipse(s, px, py + pry / 3, pr, pry,
                    rgb(s, 123, 132, 138));
            ellipse(s, px - pr / 8, py - pry / 5,
                    pr * 4 / 5, pry * 4 / 5,
                    rgb(s, shade, shade, shade - 5));
        }
    }
    }
}

static void draw_enemy_aircraft_3d(Game *g, const Enemy *enemy)
{
    SDL_Surface *s = g->screen;
    Projected3D centre = project_world_point(g, (Vec3){enemy->world_x,
        enemy->world_y, enemy->world_z});
    Projected3D upper[4], lower[4], tail[4], rear, fin;
    Projected3D prop_l, prop_r, prop_u, prop_d;
    Projected3D wheel_l, wheel_r, mark_l, mark_r;
    Projected3D cockpit, pilot, cowling, gun_l, gun_r;
    Projected3D nose_l, nose_r, nose_t, nose_b;
    Projected3D mid_l, mid_r, mid_t, mid_b;
    Projected3D rear_l, rear_r, rear_t, rear_b;
    uint32_t outline = rgb(s, 35, 27, 24);
    uint32_t canvas = rgb(s, 214, 187, 142);
    uint32_t underside = rgb(s, 172, 148, 111);
    uint32_t rib = rgb(s, 126, 101, 70);
    uint32_t body = rgb(s, 143, 39, 31);
    uint32_t body_light = rgb(s, 194, 66, 45);
    uint32_t metal = rgb(s, 91, 91, 84);
    int i;
    static const float wing[4][3] = {
        {-17.4f, 2.0f, -3.2f}, {17.4f, 2.0f, -3.2f},
        {17.4f, 2.0f, 3.6f}, {-17.4f, 2.0f, 3.6f}
    };
    static const float low_wing[4][3] = {
        {-15.7f, -2.0f, -2.9f}, {15.7f, -2.0f, -2.9f},
        {15.7f, -2.0f, 3.1f}, {-15.7f, -2.0f, 3.1f}
    };
    static const float tail_wing[4][3] = {
        {-7.0f, 0.0f, -15.2f}, {7.0f, 0.0f, -15.2f},
        {6.0f, 0.0f, -12.5f}, {-6.0f, 0.0f, -12.5f}
    };
    if (!centre.visible || centre.x < -180 || centre.x > SCREEN_W + 180 ||
        centre.y < -180 || centre.y > SCREEN_H + 180) return;
    if (centre.depth > 1650.0f) {
        Projected3D far_l = project_world_point(g,
            enemy_model_point(enemy, -17.4f, 2.0f, 0.0f));
        Projected3D far_r = project_world_point(g,
            enemy_model_point(enemy, 17.4f, 2.0f, 0.0f));
        Projected3D far_n = project_world_point(g,
            enemy_model_point(enemy, 0.0f, 0.0f, 18.5f));
        Projected3D far_t = project_world_point(g,
            enemy_model_point(enemy, 0.0f, 0.0f, -16.2f));
        Projected3D far_tl = project_world_point(g,
            enemy_model_point(enemy, -7.0f, 0.0f, -14.0f));
        Projected3D far_tr = project_world_point(g,
            enemy_model_point(enemy, 7.0f, 0.0f, -14.0f));
        /* Even the far LOD uses the projected wing, fuselage and tail axes.
         * A black dot gave no clue which way an attacker was flying. */
        if (far_l.visible && far_r.visible) {
            line(s, far_l.x, far_l.y, far_r.x, far_r.y, outline);
            if (centre.depth < 3000.0f)
                line(s, far_l.x, far_l.y - 1,
                     far_r.x, far_r.y - 1, canvas);
        }
        if (far_n.visible && far_t.visible)
            line(s, far_n.x, far_n.y, far_t.x, far_t.y, body);
        if (far_tl.visible && far_tr.visible)
            line(s, far_tl.x, far_tl.y, far_tr.x, far_tr.y, outline);
        if (far_l.x == far_r.x && far_l.y == far_r.y)
            circle(s, centre.x, centre.y, 1, outline);
        return;
    }
    for (i = 0; i < 4; ++i) {
        upper[i] = project_world_point(g, enemy_model_point(enemy,
            wing[i][0], wing[i][1], wing[i][2]));
        lower[i] = project_world_point(g, enemy_model_point(enemy,
            low_wing[i][0], low_wing[i][1], low_wing[i][2]));
        tail[i] = project_world_point(g, enemy_model_point(enemy,
            tail_wing[i][0], tail_wing[i][1], tail_wing[i][2]));
    }
    rear = project_world_point(g, enemy_model_point(enemy, 0.0f, 0.0f, -16.2f));
    fin = project_world_point(g, enemy_model_point(enemy, 0.0f, 6.0f, -14.8f));
    for (i = 0; i < 4; ++i)
        if (!upper[i].visible || !lower[i].visible || !tail[i].visible) return;
    fill_triangle(s, upper[0].x, upper[0].y, upper[1].x, upper[1].y,
                  upper[2].x, upper[2].y, canvas);
    fill_triangle(s, upper[0].x, upper[0].y, upper[2].x, upper[2].y,
                  upper[3].x, upper[3].y, canvas);
    fill_triangle(s, lower[0].x, lower[0].y, lower[1].x, lower[1].y,
                  lower[2].x, lower[2].y, underside);
    fill_triangle(s, lower[0].x, lower[0].y, lower[2].x, lower[2].y,
                  lower[3].x, lower[3].y, underside);
    fill_triangle(s, tail[0].x, tail[0].y, tail[1].x, tail[1].y,
                  tail[2].x, tail[2].y, canvas);
    fill_triangle(s, tail[0].x, tail[0].y, tail[2].x, tail[2].y,
                  tail[3].x, tail[3].y, canvas);
    for (i = 0; i < 4; ++i) {
        int n = (i + 1) & 3;
        line(s, upper[i].x, upper[i].y, upper[n].x, upper[n].y, outline);
        line(s, lower[i].x, lower[i].y, lower[n].x, lower[n].y, outline);
    }
    /* The ribs and contrasting leading edges keep the two wings readable
     * when the aeroplane banks.  These disappear naturally with distance,
     * instead of turning into a noisy sprite-like patch. */
    if (centre.depth < 1250.0f) {
        int rib_index;
        for (rib_index = -3; rib_index <= 3; ++rib_index) {
            float rx = (float)rib_index * 4.25f;
            Projected3D rib_a = project_world_point(g,
                enemy_model_point(enemy, rx, 2.04f, -3.0f));
            Projected3D rib_b = project_world_point(g,
                enemy_model_point(enemy, rx, 2.04f, 3.4f));
            if (rib_a.visible && rib_b.visible)
                line(s, rib_a.x, rib_a.y, rib_b.x, rib_b.y, rib);
        }
        line(s, upper[3].x, upper[3].y, upper[2].x, upper[2].y,
             rgb(s, 239, 211, 163));
        line(s, lower[3].x, lower[3].y, lower[2].x, lower[2].y, rib);
    }
    /* A projected tapered fuselage replaces the old seven screen-space
     * parallel lines.  Those lines stayed upright while the wings banked and
     * made the aircraft look like disconnected symbols rather than a solid
     * aeroplane. */
    nose_l = project_world_point(g, enemy_model_point(enemy, -3.0f, 0.0f, 13.4f));
    nose_r = project_world_point(g, enemy_model_point(enemy,  3.0f, 0.0f, 13.4f));
    nose_t = project_world_point(g, enemy_model_point(enemy,  0.0f, 2.7f, 13.4f));
    nose_b = project_world_point(g, enemy_model_point(enemy,  0.0f,-2.4f, 13.4f));
    mid_l = project_world_point(g, enemy_model_point(enemy, -2.3f, 0.0f, -3.8f));
    mid_r = project_world_point(g, enemy_model_point(enemy,  2.3f, 0.0f, -3.8f));
    mid_t = project_world_point(g, enemy_model_point(enemy,  0.0f, 2.2f, -3.8f));
    mid_b = project_world_point(g, enemy_model_point(enemy,  0.0f,-1.9f, -3.8f));
    rear_l = project_world_point(g, enemy_model_point(enemy, -0.8f, 0.0f,-14.7f));
    rear_r = project_world_point(g, enemy_model_point(enemy,  0.8f, 0.0f,-14.7f));
    rear_t = project_world_point(g, enemy_model_point(enemy,  0.0f, 0.9f,-14.7f));
    rear_b = project_world_point(g, enemy_model_point(enemy,  0.0f,-0.7f,-14.7f));
    if (nose_l.visible && nose_r.visible && nose_t.visible && nose_b.visible &&
        mid_l.visible && mid_r.visible && mid_t.visible && mid_b.visible &&
        rear_l.visible && rear_r.visible && rear_t.visible && rear_b.visible) {
        fill_triangle(s, nose_l.x, nose_l.y, nose_t.x, nose_t.y,
                      mid_t.x, mid_t.y, body);
        fill_triangle(s, nose_l.x, nose_l.y, mid_t.x, mid_t.y,
                      mid_l.x, mid_l.y, body);
        fill_triangle(s, nose_t.x, nose_t.y, nose_r.x, nose_r.y,
                      mid_r.x, mid_r.y, body_light);
        fill_triangle(s, nose_t.x, nose_t.y, mid_r.x, mid_r.y,
                      mid_t.x, mid_t.y, body_light);
        fill_triangle(s, mid_l.x, mid_l.y, mid_t.x, mid_t.y,
                      rear_t.x, rear_t.y, body);
        fill_triangle(s, mid_l.x, mid_l.y, rear_t.x, rear_t.y,
                      rear_l.x, rear_l.y, body);
        fill_triangle(s, mid_t.x, mid_t.y, mid_r.x, mid_r.y,
                      rear_r.x, rear_r.y, body_light);
        fill_triangle(s, mid_t.x, mid_t.y, rear_r.x, rear_r.y,
                      rear_t.x, rear_t.y, body_light);
        fill_triangle(s, nose_l.x, nose_l.y, mid_l.x, mid_l.y,
                      mid_b.x, mid_b.y, rgb(s, 91, 39, 31));
        fill_triangle(s, nose_l.x, nose_l.y, mid_b.x, mid_b.y,
                      nose_b.x, nose_b.y, rgb(s, 91, 39, 31));
        line(s, nose_l.x, nose_l.y, rear_l.x, rear_l.y, outline);
        line(s, nose_r.x, nose_r.y, rear_r.x, rear_r.y, outline);
        line(s, nose_t.x, nose_t.y, rear_t.x, rear_t.y,
             rgb(s, 222, 91, 57));
        line(s, nose_b.x, nose_b.y, rear_b.x, rear_b.y, outline);
    }
    if (rear.visible && fin.visible) {
        Projected3D fin_front = project_world_point(g,
            enemy_model_point(enemy, 0.0f, 0.2f, -10.2f));
        Projected3D fin_rear = project_world_point(g,
            enemy_model_point(enemy, 0.0f, 0.4f, -16.0f));
        Projected3D fin_top_front = project_world_point(g,
            enemy_model_point(enemy, 0.0f, 5.8f, -11.5f));
        if (fin_front.visible && fin_rear.visible && fin_top_front.visible) {
            fill_triangle(s, fin_front.x, fin_front.y,
                          fin_rear.x, fin_rear.y, fin.x, fin.y, body);
            fill_triangle(s, fin_front.x, fin_front.y,
                          fin.x, fin.y, fin_top_front.x, fin_top_front.y,
                          body_light);
            line(s, fin_front.x, fin_front.y,
                 fin_top_front.x, fin_top_front.y, outline);
            line(s, fin_top_front.x, fin_top_front.y, fin.x, fin.y, outline);
            line(s, fin.x, fin.y, fin_rear.x, fin_rear.y, outline);
        }
    }
    line(s, upper[0].x, upper[0].y, lower[0].x, lower[0].y, outline);
    line(s, upper[1].x, upper[1].y, lower[1].x, lower[1].y, outline);
    {
        Projected3D strut_ul = project_world_point(g,
            enemy_model_point(enemy, -7.1f, 2.0f, 0.0f));
        Projected3D strut_ll = project_world_point(g,
            enemy_model_point(enemy, -7.1f, -2.0f, 0.0f));
        Projected3D strut_ur = project_world_point(g,
            enemy_model_point(enemy, 7.1f, 2.0f, 0.0f));
        Projected3D strut_lr = project_world_point(g,
            enemy_model_point(enemy, 7.1f, -2.0f, 0.0f));
        if (strut_ul.visible && strut_ll.visible)
            line(s, strut_ul.x, strut_ul.y, strut_ll.x, strut_ll.y, outline);
        if (strut_ur.visible && strut_lr.visible)
            line(s, strut_ur.x, strut_ur.y, strut_lr.x, strut_lr.y, outline);
        if (strut_ul.visible && strut_lr.visible)
            line(s, strut_ul.x, strut_ul.y, strut_lr.x, strut_lr.y,
                 rgb(s, 91, 80, 66));
        if (strut_ur.visible && strut_ll.visible)
            line(s, strut_ur.x, strut_ur.y, strut_ll.x, strut_ll.y,
                 rgb(s, 91, 80, 66));
        if (centre.depth < 1100.0f) {
            Projected3D cabane_ul = project_world_point(g,
                enemy_model_point(enemy, -2.8f, 2.0f, 1.2f));
            Projected3D cabane_ll = project_world_point(g,
                enemy_model_point(enemy, -1.8f, -0.8f, 1.2f));
            Projected3D cabane_ur = project_world_point(g,
                enemy_model_point(enemy, 2.8f, 2.0f, 1.2f));
            Projected3D cabane_lr = project_world_point(g,
                enemy_model_point(enemy, 1.8f, -0.8f, 1.2f));
            Projected3D wire_l = project_world_point(g,
                enemy_model_point(enemy, -14.0f, 2.0f, 0.0f));
            Projected3D wire_r = project_world_point(g,
                enemy_model_point(enemy, 14.0f, 2.0f, 0.0f));
            if (cabane_ul.visible && cabane_ll.visible)
                line(s, cabane_ul.x, cabane_ul.y,
                     cabane_ll.x, cabane_ll.y, outline);
            if (cabane_ur.visible && cabane_lr.visible)
                line(s, cabane_ur.x, cabane_ur.y,
                     cabane_lr.x, cabane_lr.y, outline);
            if (wire_l.visible && cabane_lr.visible)
                line(s, wire_l.x, wire_l.y,
                     cabane_lr.x, cabane_lr.y, rgb(s, 80, 72, 62));
            if (wire_r.visible && cabane_ll.visible)
                line(s, wire_r.x, wire_r.y,
                     cabane_ll.x, cabane_ll.y, rgb(s, 80, 72, 62));
        }
    }
    /* Engine, open cockpit and pilot are strong recognition cues at combat
     * range, much more useful than a flat coloured fuselage. */
    cowling = project_world_point(g, enemy_model_point(enemy, 0.0f, 0.0f, 13.6f));
    cockpit = project_world_point(g, enemy_model_point(enemy, 0.0f, 1.0f, -3.7f));
    pilot = project_world_point(g, enemy_model_point(enemy, 0.0f, 3.2f, -4.2f));
    if (cowling.visible) {
        int engine_radius = centre.depth < 520.0f ? 4 : 3;
        circle(s, cowling.x, cowling.y, engine_radius, outline);
        circle(s, cowling.x, cowling.y, engine_radius - 1, metal);
    }
    if (cockpit.visible) circle(s, cockpit.x, cockpit.y,
                                centre.depth < 650.0f ? 4 : 2, outline);
    if (pilot.visible) {
        circle(s, pilot.x, pilot.y, centre.depth < 650.0f ? 3 : 2,
               rgb(s, 91, 67, 45));
        if (centre.depth < 700.0f)
            line(s, pilot.x - 2, pilot.y - 1, pilot.x + 2, pilot.y - 1,
                 rgb(s, 32, 29, 25));
    }
    gun_l = project_world_point(g, enemy_model_point(enemy, -1.5f, 2.0f, 10.0f));
    gun_r = project_world_point(g, enemy_model_point(enemy, 1.5f, 2.0f, 10.0f));
    if (centre.depth < 850.0f && cowling.visible) {
        if (gun_l.visible) line(s, cockpit.x - 1, cockpit.y, gun_l.x, gun_l.y, outline);
        if (gun_r.visible) line(s, cockpit.x + 1, cockpit.y, gun_r.x, gun_r.y, outline);
    }
    prop_l = project_world_point(g, enemy_model_point(enemy, -6.7f, 0.0f, 19.1f));
    prop_r = project_world_point(g, enemy_model_point(enemy, 6.7f, 0.0f, 19.1f));
    prop_u = project_world_point(g, enemy_model_point(enemy, 0.0f, 6.7f, 19.1f));
    prop_d = project_world_point(g, enemy_model_point(enemy, 0.0f, -6.7f, 19.1f));
    if (prop_l.visible && prop_r.visible)
        line(s, prop_l.x, prop_l.y, prop_r.x, prop_r.y, rgb(s, 119, 79, 44));
    if (prop_u.visible && prop_d.visible)
        line(s, prop_u.x, prop_u.y, prop_d.x, prop_d.y, rgb(s, 119, 79, 44));
    wheel_l = project_world_point(g, enemy_model_point(enemy, -4.1f, -4.2f, 3.0f));
    wheel_r = project_world_point(g, enemy_model_point(enemy, 4.1f, -4.2f, 3.0f));
    if (wheel_l.visible && cowling.visible) {
        line(s, cowling.x, cowling.y, wheel_l.x, wheel_l.y, outline);
        circle(s, wheel_l.x, wheel_l.y, centre.depth < 700 ? 4 : 2, outline);
    }
    if (wheel_r.visible && cowling.visible) {
        line(s, cowling.x, cowling.y, wheel_r.x, wheel_r.y, outline);
        circle(s, wheel_r.x, wheel_r.y, centre.depth < 700 ? 4 : 2, outline);
    }
    if (wheel_l.visible && wheel_r.visible)
        line(s, wheel_l.x, wheel_l.y, wheel_r.x, wheel_r.y,
             rgb(s, 61, 52, 45));
    mark_l = project_world_point(g, enemy_model_point(enemy, -11.0f, 2.08f, 0.0f));
    mark_r = project_world_point(g, enemy_model_point(enemy, 11.0f, 2.08f, 0.0f));
    if (centre.depth < 1050.0f) {
        Projected3D mark_l_v0 = project_world_point(g,
            enemy_model_point(enemy, -11.0f, 2.10f, -2.1f));
        Projected3D mark_l_v1 = project_world_point(g,
            enemy_model_point(enemy, -11.0f, 2.10f, 2.1f));
        Projected3D mark_l_h0 = project_world_point(g,
            enemy_model_point(enemy, -13.1f, 2.10f, 0.0f));
        Projected3D mark_l_h1 = project_world_point(g,
            enemy_model_point(enemy, -8.9f, 2.10f, 0.0f));
        Projected3D mark_r_v0 = project_world_point(g,
            enemy_model_point(enemy, 11.0f, 2.10f, -2.1f));
        Projected3D mark_r_v1 = project_world_point(g,
            enemy_model_point(enemy, 11.0f, 2.10f, 2.1f));
        Projected3D mark_r_h0 = project_world_point(g,
            enemy_model_point(enemy, 8.9f, 2.10f, 0.0f));
        Projected3D mark_r_h1 = project_world_point(g,
            enemy_model_point(enemy, 13.1f, 2.10f, 0.0f));
        if (mark_l.visible && mark_l_v0.visible && mark_l_v1.visible &&
            mark_l_h0.visible && mark_l_h1.visible) {
            int thick;
            for (thick = -1; thick <= 1; ++thick) {
                line(s, mark_l_v0.x + thick, mark_l_v0.y,
                     mark_l_v1.x + thick, mark_l_v1.y, outline);
                line(s, mark_l_h0.x, mark_l_h0.y + thick,
                     mark_l_h1.x, mark_l_h1.y + thick, outline);
            }
        }
        if (mark_r.visible && mark_r_v0.visible && mark_r_v1.visible &&
            mark_r_h0.visible && mark_r_h1.visible) {
            int thick;
            for (thick = -1; thick <= 1; ++thick) {
                line(s, mark_r_v0.x + thick, mark_r_v0.y,
                     mark_r_v1.x + thick, mark_r_v1.y, outline);
                line(s, mark_r_h0.x, mark_r_h0.y + thick,
                     mark_r_h1.x, mark_r_h1.y + thick, outline);
            }
        }
    } else {
        if (mark_l.visible) circle(s, mark_l.x, mark_l.y, 1, outline);
        if (mark_r.visible) circle(s, mark_r.x, mark_r.y, 1, outline);
    }
    if (enemy->hp < enemy->max_hp && rear.visible) {
        int smoke = (enemy->max_hp - enemy->hp) * 2 + 2;
        for (i = 0; i < smoke; ++i)
            circle(s, rear.x - i * 3, rear.y + (i & 1), 2 + i / 3,
                   rgb(s, 69 + i * 5, 66 + i * 5, 61 + i * 5));
    }
}

static void draw_dogfight_bullet_holes(Game *g)
{
    SDL_Surface *s = g->screen;
    int i;
    for (i = 0; i < MAX_BULLET_HOLES; ++i) {
        BulletHole *hole = &g->bullet_holes[i];
        int x, y;
        if (!hole->active) continue;
        x = (int)hole->x;
        y = (int)hole->y;
        circle(s, x, y, 4, rgb(s, 151, 112, 66));
        circle(s, x, y, 2, rgb(s, 24, 22, 20));
        pixel(s, x - 1, y - 1, rgb(s, 220, 190, 127));
        line(s, x - 7, y + 2, x - 3, y, rgb(s, 76, 58, 43));
        line(s, x + 3, y - 1, x + 8, y - 4, rgb(s, 76, 58, 43));
        if ((i & 2) == 0)
            line(s, x + 1, y + 3, x + 3, y + 8, rgb(s, 76, 58, 43));
    }
}

static void draw_player_crash_overlay(Game *g)
{
    SDL_Surface *s = g->screen;
    float age;
    float collapse;
    int i;
    if (!g->flight_crashed) return;
    age = (float)(SDL_GetTicks() - g->flight_crash_started) / 1000.0f;
    collapse = age / 0.72f;
    if (collapse > 1.0f) collapse = 1.0f;

    /* Earth and chalk are driven out along the ground.  No fireball: the
     * aircraft hits, folds, sheds fabric and slides to a broken stop. */
    for (i = 0; i < 13; ++i) {
        uint32_t seed = scene_hash(0x44555354u + (uint32_t)i * 113u);
        float dust_age = age + (float)(i & 3) * 0.04f;
        int spread = 20 + (int)(dust_age * (34 + (seed & 15u)));
        int x = 320 + ((i & 1) ? spread : -spread) +
                (int)((seed >> 8) % 31u) - 15;
        int y = 352 - (int)(dust_age * (10 + (i % 5) * 3));
        int radius = 11 + (i % 4) * 3 + (int)(dust_age * 2.0f);
        uint8_t earth = (uint8_t)(76 + (i % 5) * 7);
        if (dust_age < 2.6f)
            circle(s, x, y, radius, rgb(s, earth, earth - 8, earth - 18));
    }

    /* Canvas wings buckle inward and spars snap as the cockpit crumples. */
    fill_triangle(s, 0, 340, 184, 333 + (int)(collapse * 38.0f),
                  131, 400, rgb(s, 128, 106, 72));
    fill_triangle(s, 640, 340, 455, 333 + (int)(collapse * 38.0f),
                  514, 400, rgb(s, 123, 101, 69));
    fill_triangle(s, 112, 353, 242, 340 + (int)(collapse * 51.0f),
                  198, 400, rgb(s, 67, 54, 43));
    fill_triangle(s, 528, 353, 399, 340 + (int)(collapse * 51.0f),
                  444, 400, rgb(s, 64, 52, 42));
    line(s, 18, 347, 205, 389, rgb(s, 66, 47, 30));
    line(s, 622, 347, 434, 389, rgb(s, 66, 47, 30));
    line(s, 126, 359, 235, 398, rgb(s, 45, 37, 31));
    line(s, 514, 359, 406, 398, rgb(s, 45, 37, 31));

    /* Short-range fragments tumble out of the collapsing structure rather
     * than radiating from an explosion. */
    for (i = 0; i < 18; ++i) {
        uint32_t seed = scene_hash(0x44454252u + (uint32_t)i * 131u);
        float life = age < 1.1f ? age : 1.1f;
        int vx = (int)(seed % 69u) - 34;
        int vy = -18 - (int)((seed >> 8) % 44u);
        int x = 320 + (int)(vx * life);
        int y = 344 + (int)(vy * life + 46.0f * life * life);
        uint32_t colour = (i % 3 == 0) ? rgb(s, 61, 49, 39) :
                          (i % 3 == 1) ? rgb(s, 137, 91, 48) :
                                         rgb(s, 111, 104, 89);
        line(s, x - 3 - (i & 3), y - 2, x + 4 + (i & 1), y + 3, colour);
        if ((i & 3) == 0) circle(s, x, y, 3, colour);
    }
    /* The last view remains in the cockpit: cracked glass and torn rigging
     * close around the pilot before the picture finally darkens. */
    line(s, 319, 201, 276, 159, rgb(s, 35, 31, 28));
    line(s, 319, 201, 356, 147, rgb(s, 35, 31, 28));
    line(s, 319, 201, 401, 184, rgb(s, 35, 31, 28));
    line(s, 319, 201, 247, 229, rgb(s, 35, 31, 28));
    line(s, 276, 159, 261, 126, rgb(s, 64, 50, 39));
    line(s, 356, 147, 378, 119, rgb(s, 64, 50, 39));
    /* Jagged black gaps make the foreground read as a broken airframe, not
     * an intact cockpit hidden behind a generic explosion. */
    fill_triangle(s, 55, 421, 205, 380, 144, 458, rgb(s, 27, 24, 21));
    fill_triangle(s, 585, 421, 438, 382, 503, 458, rgb(s, 27, 24, 21));
    if (age > 1.35f) {
        int darkness = (int)((age - 1.35f) * 82.0f);
        if (darkness > 178) darkness = 178;
        blend_rect(s, 0, 0, SCREEN_W, SCREEN_H, 16, 13, 12,
                   (uint8_t)darkness);
    }
    if (age > 2.45f)
        draw_field_sign(g, 213, 270, 214,
                        age < 3.25f ? "AIRCRAFT WRECKED" : "NO RESPONSE");
}

static void draw_dogfight_particles(Game *g, int foreground)
{
    SDL_Surface *s = g->screen;
    int i;
    for (i = 0; i < MAX_DOGFIGHT_PARTICLES; ++i) {
        DogfightParticle *p = &g->dogfight_particles[i];
        if (!p->active) continue;
        if (foreground != (p->kind == 2)) continue;
        if (p->kind == 2) {
            uint32_t brass = rgb(s, 191, 139, 61);
            line(s, (int)p->x, (int)p->y,
                 (int)p->x + (p->vx < 0.0f ? -2 : 2),
                 (int)p->y + 1, brass);
        } else {
            float speed = sqrtf(p->vx * p->vx + p->vy * p->vy);
            int length = p->kind == 0 ? 6 : 5;
            int tx, ty;
            uint32_t edge = p->kind == 0 ? rgb(s, 185, 116, 48) :
                                            rgb(s, 176, 61, 37);
            uint32_t core = p->kind == 0 ? rgb(s, 255, 225, 143) :
                                            rgb(s, 255, 153, 78);
            if (speed < 1.0f) speed = 1.0f;
            tx = (int)(p->x - p->vx / speed * length);
            ty = (int)(p->y - p->vy / speed * length);
            line(s, tx, ty, (int)p->x, (int)p->y, edge);
            line(s, tx + 1, ty, (int)p->x + 1, (int)p->y, core);
            pixel(s, (int)p->x, (int)p->y, rgb(s, 255, 244, 199));
        }
    }
}

static void draw_dogfight_simulator(Game *g)
{
    SDL_Surface *s = g->screen;
    int order[MAX_ENEMIES];
    float depth[MAX_ENEMIES];
    int count = 0;
    int i, j;
    char buf[64];
    uint32_t now = SDL_GetTicks();
    draw_simulator_terrain(g);
    draw_simulator_clouds(g);
    for (i = 0; i < MAX_ENEMIES; ++i) if (g->enemies[i].active) {
        Projected3D p = project_world_point(g, (Vec3){g->enemies[i].world_x,
            g->enemies[i].world_y, g->enemies[i].world_z});
        if (p.visible) { order[count] = i; depth[count++] = p.depth; }
    }
    for (i = 0; i < count; ++i) for (j = i + 1; j < count; ++j) {
        if (depth[i] < depth[j]) {
            float td = depth[i]; int ti = order[i];
            depth[i] = depth[j]; order[i] = order[j];
            depth[j] = td; order[j] = ti;
        }
    }
    for (i = 0; i < count; ++i) {
        Enemy *enemy = &g->enemies[order[i]];
        Projected3D p = project_world_point(g, (Vec3){enemy->world_x,
            enemy->world_y, enemy->world_z});
        draw_enemy_aircraft_3d(g, enemy);
        if (now >= enemy->last_shot && now - enemy->last_shot < 42u &&
            p.visible && p.x >= 0 && p.x < SCREEN_W &&
            p.y >= 0 && p.y < SCREEN_H) {
            circle(s, p.x, p.y, 3, rgb(s, 255, 124, 53));
            pixel(s, p.x, p.y, rgb(s, 255, 236, 164));
        }
    }
    draw_explosions(g);
    draw_dogfight_particles(g, 0);
    if (g->pilot_cockpit)
        SDL_BlitSurface(g->pilot_cockpit, NULL, s, NULL);
    draw_dogfight_bullet_holes(g);
    draw_dogfight_particles(g, 1);
    if (now >= g->last_shot && now - g->last_shot < 45u) {
        fill_triangle(s, 288, 232, 292, 222, 297, 232,
                      rgb(s, 255, 140, 45));
        fill_triangle(s, 345, 232, 350, 222, 354, 232,
                      rgb(s, 255, 140, 45));
        circle(s, 292, 228, 2, rgb(s, 255, 238, 157));
        circle(s, 350, 228, 2, rgb(s, 255, 238, 157));
    }
    if (perspective_enemy_total(g) > 0)
        snprintf(buf, sizeof(buf), "DOWNED %d/%d",
                 g->kills, perspective_enemy_total(g));
    else
        snprintf(buf, sizeof(buf), "UNARMED PATROL");
    draw_field_sign(g, 8, 342, 130, buf);
    snprintf(buf, sizeof(buf), "AMMO %03d", g->ammunition);
    draw_field_sign(g, 8, 371, 130, buf);
    snprintf(buf, sizeof(buf), "IAS %03d", (int)g->airspeed);
    draw_field_sign(g, 266, 342, 108, buf);
    snprintf(buf, sizeof(buf), "ALT %04d", (int)g->flight_y);
    draw_field_sign(g, 266, 371, 108, buf);
    snprintf(buf, sizeof(buf), "FUEL %02d", (int)g->fuel);
    draw_field_sign(g, 526, 342, 106, buf);
    snprintf(buf, sizeof(buf), "AIRFRAME %03d", g->health > 0 ? g->health : 0);
    draw_field_sign(g, 502, 371, 130, buf);
    if (g->flight_stalled && ((now / 220u) & 1u))
        draw_field_sign(g, 240, 304, 160, "STALL - NOSE DOWN");
    draw_player_crash_overlay(g);
}

static void draw_dogfight_3d_sortie(Game *g)
{
    SDL_Surface *s = g->screen;
    int x, y, i;
    char buf[64];
    int horizon = 213;
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    int gunner = mode == MISSION_GUNNER_3D;
    if (!gunner) {
        draw_dogfight_simulator(g);
        return;
    }
    if (g->dogfight_background) {
        draw_flight_background(g);
    } else {
        gradient(s, 45, 76, 111, 190, 167, 119);
        rect(s, 0, horizon, SCREEN_W, SCREEN_H - horizon, rgb(s, 83, 78, 50));
        for (y = horizon + 8; y < SCREEN_H; y += 13 + (y - horizon) / 8)
            line(s, 0, y, SCREEN_W, y, rgb(s, 111, 96, 59));
        for (x = -320; x <= 960; x += 80)
            line(s, 320, horizon, x, SCREEN_H, rgb(s, 57, 52, 39));
        for (x = 0; x < SCREEN_W; x += 34) {
            int height = 5 + (int)(scene_hash((uint32_t)x +
                         (uint32_t)g->chapter) % 14u);
            rect(s, x, horizon - height, 30, height, rgb(s, 54, 66, 51));
        }
        draw_sky_detail(g);
    }
    for (i = 0; i < MAX_ENEMIES; ++i) if (g->enemies[i].active) {
        int sx, sy, size;
        if (g->enemies[i].crashing) {
            draw_spiralling_enemy(g, &g->enemies[i], i);
            continue;
        }
        if (project_enemy_3d(g, &g->enemies[i], i, &sx, &sy, &size)) {
            if (g->enemies[i].hp < g->enemies[i].max_hp) {
                int health = g->enemies[i].hp * 100 / g->enemies[i].max_hp;
                draw_aircraft_smoke(g, sx, sy, health, 1,
                                    0x3344454eu + (uint32_t)i * 79u);
            }
            {
                float phase = g->enemies[i].phase +
                              g->scroll * (0.0025f + i * 0.00035f);
                float bank = cosf(phase) * (0.20f +
                             (size > 48 ? 0.30f : 0.14f));
                if (i & 1) bank = -bank;
                draw_perspective_plane(g, sx, sy, size, 1, bank);
            }
        }
    }
    /* Eccles, Gridpipe Thyne and Bluebottle hold a loose formation around us. */
    /* Tom flies the two-seater alone; the scouts only escort dogfights. */
    for (i = 0; i < 3; ++i) if (!gunner && g->wingman_health[i] > 0) {
        int wx = i == 0 ? 175 : (i == 1 ? 455 : 110);
        int wy = i == 2 ? 252 : 278;
        draw_perspective_plane(g, wx, wy, i == 2 ? 20 : 27, 0, 0.0f);
        typewriter_text(g, wx - 25, wy + 20, allied_pilot_names[i], 7, 11,
                        rgb(s, 228, 214, 173));
    }
    for (i = 0; i < MAX_BULLETS; ++i) if (g->bullets[i].active) {
        Bullet *b = &g->bullets[i];
        uint32_t tracer = b->enemy ? rgb(s, 255, 113, 57) :
                                     rgb(s, 255, 239, 157);
        /* A tracer is a short bright round, not a continuous beam back to its
         * weapon.  Projectile velocity and collision remain unchanged; only
         * the exposed luminous length is shortened. */
        int tail_x = (int)b->x - (int)(b->vx * (b->enemy ? 0.028f : 0.024f));
        int tail_y = (int)b->y - (int)(b->vy * (b->enemy ? 0.028f : 0.024f));
        line(s, (int)b->x, (int)b->y, tail_x, tail_y, tracer);
        if (!b->enemy)
            line(s, (int)b->x, (int)b->y + 1, tail_x, tail_y + 1, tracer);
    }
    draw_explosions(g);
    if (gunner) {
        draw_gunner_airframe(g);
        draw_gunner_bullet_holes(g);
    }
    if (gunner && g->gunner_moving_guns) {
        SDL_Rect gun_source;
        SDL_Rect gun_position;
        float traverse = gunner_aim_traverse(g);
        float elevation = gunner_aim_elevation(g);
        int column;
        int row;
        int carriage_x;
        int carriage_y;
        if (traverse < 0.0f) traverse = 0.0f;
        if (traverse > 1.0f) traverse = 1.0f;
        if (elevation < 0.0f) elevation = 0.0f;
        if (elevation > 1.0f) elevation = 1.0f;
        column = (int)(traverse * 4.999f);
        row = (int)(elevation * 4.999f);
        /* The single Lewis gun remains anchored to the Scarff rail.  Most of
         * the 90-degree traverse is now expressed by the moving viewpoint;
         * this smaller carriage shift shows the physical mount following it. */
        carriage_x = 320 + (int)((traverse - 0.5f) * 74.0f);
        carriage_y = 176 + (int)(fabsf(traverse - 0.5f) * 8.0f);
        gun_source.x = column * 420;
        gun_source.y = row * 220;
        gun_source.w = 420;
        gun_source.h = 220;
        gun_position.x = carriage_x - 210;
        gun_position.y = carriage_y;
        gun_position.w = gun_position.h = 0;
        SDL_BlitSurface(g->gunner_moving_guns, &gun_source, s, &gun_position);
        /* The rail is already painted into the airframe foreground.  The old
         * full-screen ring overlay belonged to the abandoned twin-gun mockup
         * and would duplicate the mount around the single Lewis. */
    } else if (gunner && g->gunner_cockpit)
        SDL_BlitSurface(g->gunner_cockpit, NULL, s, NULL);
    else if (!gunner && g->pilot_cockpit)
        SDL_BlitSurface(g->pilot_cockpit, NULL, s, NULL);
    else {
        circle(s, 320, 421, 116, rgb(s, 50, 36, 27));
        circle(s, 320, 421, 96, rgb(s, 90, 61, 37));
        rect(s, 312, 318, 16, 82, rgb(s, 45, 44, 39));
    }
    if (gunner) {
        int ax = (int)g->player_x;
        int ay = (int)g->player_y;
        int blocked = gunner_tail_blocks_shot(g, ax, ay);
        uint32_t sight = blocked ? rgb(s, 242, 72, 45) :
                                   rgb(s, 239, 221, 164);
        /* The open crosshair is separate from the gun.  It is the real hit
         * point; red warns that the line of fire intersects our own tail. */
        line(s, ax - 24, ay, ax - 8, ay, sight);
        line(s, ax + 8, ay, ax + 24, ay, sight);
        line(s, ax, ay - 24, ax, ay - 8, sight);
        line(s, ax, ay + 8, ax, ay + 24, sight);
        line(s, ax - 12, ay - 12, ax - 6, ay - 16, sight);
        line(s, ax + 12, ay - 12, ax + 6, ay - 16, sight);
        line(s, ax - 12, ay + 12, ax - 6, ay + 16, sight);
        line(s, ax + 12, ay + 12, ax + 6, ay + 16, sight);
    }
    if (perspective_enemy_total(g) > 0)
        snprintf(buf, sizeof(buf), "DOWNED %d/%d",
                 g->kills, perspective_enemy_total(g));
    else
        snprintf(buf, sizeof(buf), "UNARMED PATROL");
    draw_field_sign(g, 8, 342, 130, buf);
    snprintf(buf, sizeof(buf), "AMMO %03d", g->ammunition);
    draw_field_sign(g, 8, 371, 130, buf);
    if (gunner)
        snprintf(buf, sizeof(buf), "TAIL %03d", g->tail_integrity);
    else
        snprintf(buf, sizeof(buf), "SPEED %03d", (int)g->airspeed);
    draw_field_sign(g, 266, 371, 108, buf);
    snprintf(buf, sizeof(buf), "FUEL %02d", (int)g->fuel);
    draw_field_sign(g, 526, 342, 106, buf);
    snprintf(buf, sizeof(buf), "AIRFRAME %03d", g->health > 0 ? g->health : 0);
    draw_field_sign(g, 502, 371, 130, buf);
}

static void draw_sortie(Game *g)
{
    SDL_Surface *s = g->screen;
    int x, i;
    uint32_t far, near, earth, wire;
    char buf[64];
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    if (mode == MISSION_TOP_DOWN) {
        draw_top_down_sortie(g);
        return;
    }
    if (is_perspective_mode(mode)) {
        draw_dogfight_3d_sortie(g);
        return;
    }
    /* The authored panoramas remain at their native 1280x360.  The taller
     * 640x400 playfield reveals more vertical world space without scaling
     * any artwork: sky stays at y=0, world layers are lowered by 40 pixels. */
    gradient(s, 46, 80, 117, 190, 168, 122);
    if (g->sky) {
        SDL_Rect source;
        SDL_Rect dest = {0, 0, 0, 0};
        int limit = g->sky->w - SCREEN_W;
        source.x = (int)(g->scroll * SKY_PARALLAX);
        if (source.x < 0) source.x = 0;
        if (source.x > limit) source.x = limit;
        source.y = 0;
        source.w = SCREEN_W;
        source.h = g->sky->h < ART_H ? g->sky->h : ART_H;
        SDL_BlitSurface(g->sky, &source, s, &dest);
    } else {
        draw_sky_detail(g);
    }
    if (g->sky) draw_sky_detail(g);
    if (g->battlefield) {
        SDL_Rect source;
        SDL_Rect dest = {0, GAMEPLAY_Y_OFFSET, 0, 0};
        int limit = g->battlefield->w - SCREEN_W;
        source.x = (int)(g->scroll * HILLS_PARALLAX);
        if (source.x < 0) source.x = 0;
        if (source.x > limit) source.x = limit;
        source.y = 0;
        source.w = SCREEN_W;
        source.h = g->battlefield->h < ART_H ? g->battlefield->h : ART_H;
        SDL_BlitSurface(g->battlefield, &source, s, &dest);
    }
    far = rgb(s, 87, 105, 99);
    near = rgb(s, 72, 59, 46);
    earth = rgb(s, 43, 37, 31);
    wire = rgb(s, 43, 39, 34);
    draw_element_layer(g, g->midground, MIDGROUND_ELEMENT_COUNT,
                       BUILDINGS_PARALLAX, 285, 325, 0x4d494431u);
    if (!g->battlefield) {
        for (x = 0; x < SCREEN_W; ++x) {
            int fy = 268 + (int)(18 * sinf((x + g->scroll * 0.18f) * 0.012f));
            rect(s, x, fy, 1, SCREEN_H - fy, far);
        }
        draw_distant_villages(g);
        for (x = 0; x < SCREEN_W; ++x) {
            float wx = g->scroll + x;
            int gy = terrain_y(wx);
            rect(s, x, gy, 1, SCREEN_H - gy, near);
            rect(s, x, gy + 9, 1, SCREEN_H - gy - 9, earth);
        }
        for (x = 15; x < SCREEN_W; x += 58) {
            int gy = terrain_y(g->scroll + x);
            line(s, x, gy + 9, x + 42, gy + 15, rgb(s, 103, 77, 48));
        }
        for (x = 40; x < SCREEN_W; x += 90) {
            int gy0 = terrain_y(g->scroll + x);
            int gy1 = terrain_y(g->scroll + x + 90);
            line(s, x, gy0 - 5, x, gy0 + 25, wire);
            line(s, x + 2, gy0 - 5, x + 2, gy0 + 25, wire);
            line(s, x, gy0 + 2, x + 90, gy1 + 6, wire);
            line(s, x, gy0 + 7, x + 90, gy1 + 1, wire);
        }
        draw_scenery(g);
    }
    if (g->foreground_panorama) {
        SDL_Rect source, dest;
        int position = (int)(g->scroll * FOREGROUND_PARALLAX) %
                       g->foreground_panorama->w;
        int first = g->foreground_panorama->w - position;
        if (first > SCREEN_W) first = SCREEN_W;
        /* Cool atmospheric separation behind the darker near-ground layer. */
        blend_rect(s, 0, 275, SCREEN_W, 125, 70, 83, 91, 34);
        source.x = position;
        source.y = 0;
        source.w = first;
        source.h = g->foreground_panorama->h < ART_H ?
                   g->foreground_panorama->h : ART_H;
        dest.x = 0;
        dest.y = GAMEPLAY_Y_OFFSET;
        SDL_BlitSurface(g->foreground_panorama, &source, s, &dest);
        if (first < SCREEN_W) {
            source.x = 0;
            source.w = SCREEN_W - first;
            dest.x = first;
            SDL_BlitSurface(g->foreground_panorama, &source, s, &dest);
        }
    }
    draw_element_layer(g, g->near_props, NEAR_ELEMENT_COUNT,
                       FOREGROUND_PARALLAX, 235, 390, 0x4e454152u);
    for (i = 0; i < MAX_TARGETS; ++i) if (g->targets[i].active) {
        int sx = (int)(g->targets[i].world_x - g->scroll);
        if (sx > -20 && sx < SCREEN_W + 20) {
            int ty = terrain_y(g->targets[i].world_x) - 12;
            draw_ground_gun(g, sx, ty, &g->targets[i]);
        }
    }
    for (i = 0; i < MAX_ENEMIES; ++i) if (g->enemies[i].active) {
        int sx = (int)(g->enemies[i].x - g->scroll);
        int sy = (int)(g->enemies[i].y + sinf(g->scroll * 0.01f + g->enemies[i].phase) * 30.0f);
        if (sx > -50 && sx < SCREEN_W + 50) {
            if (g->enemies[i].hp < g->enemies[i].max_hp) {
                int health = g->enemies[i].hp * 100 / g->enemies[i].max_hp;
                draw_aircraft_smoke(g, sx, sy, health, 1,
                                    0x454e454du + (uint32_t)i * 97u);
            }
            draw_plane(g, sx, sy, 1, 2 + (int)(sinf(g->scroll * 0.02f + i) * 1.2f));
        }
    }
    for (i = 0; i < MAX_BULLETS; ++i) if (g->bullets[i].active) {
        Bullet *b = &g->bullets[i];
        if (b->type == 1) {
            circle(s, (int)b->x, (int)b->y, 3, rgb(s, 45, 37, 31));
            line(s, (int)b->x - 5, (int)b->y - 5, (int)b->x, (int)b->y,
                 rgb(s, 184, 153, 104));
        } else if (b->type == 2) {
            int tail_x = (int)(b->x - b->vx * 0.035f);
            int tail_y = (int)(b->y - b->vy * 0.035f);
            line(s, tail_x, tail_y, (int)b->x, (int)b->y,
                 rgb(s, 234, 151, 65));
            circle(s, (int)b->x, (int)b->y, 3, rgb(s, 48, 42, 36));
            pixel(s, (int)b->x, (int)b->y, rgb(s, 244, 202, 111));
        } else {
            uint32_t tracer = b->enemy ? rgb(s, 255, 113, 57) : rgb(s, 255, 228, 144);
            float speed = sqrtf(b->vx * b->vx + b->vy * b->vy);
            float length = b->enemy ? 13.0f : 10.0f;
            int tail_x, tail_y;
            if (speed < 1.0f) speed = 1.0f;
            tail_x = (int)(b->x - b->vx / speed * length);
            tail_y = (int)(b->y - b->vy / speed * length);
            line(s, tail_x, tail_y, (int)b->x, (int)b->y, tracer);
            line(s, tail_x, tail_y + 1, (int)b->x, (int)b->y + 1, tracer);
            if (b->enemy)
                circle(s, (int)b->x, (int)b->y, 2,
                       rgb(s, 255, 229, 132));
        }
    }
    draw_explosions(g);
    draw_player_damage(g);
    {
        int pitch = 2 + (int)(g->player_vy / 45.0f);
        draw_plane(g, (int)g->player_x, (int)g->player_y, 0, pitch);
    }
    snprintf(buf, sizeof(buf), "SCORE %05d", g->score);
    draw_field_sign(g, 8, 8, 112, buf);
    if (is_perspective_mode(campaign_missions[g->chapter][g->mission].mode))
        snprintf(buf, sizeof(buf), "AIR KILLS %d/%d", g->kills,
                 current_mission_goal(g));
    else if (current_mission_goal(g) > 0)
        snprintf(buf, sizeof(buf), "TARGETS %d/%d", g->objectives,
                 current_mission_goal(g));
    else
        snprintf(buf, sizeof(buf), "DUTY  PATROL");
    draw_field_sign(g, 8, 39, 112, buf);
    snprintf(buf, sizeof(buf), "AMMO %03d", g->ammunition);
    draw_field_sign(g, 264, 8, 108, buf);
    snprintf(buf, sizeof(buf), "BOMBS %02d", g->bombs_remaining);
    draw_field_sign(g, 264, 39, 108, buf);
    snprintf(buf, sizeof(buf), "FUEL %02d", (int)g->fuel);
    draw_field_sign(g, 526, 8, 106, buf);
    snprintf(buf, sizeof(buf), "AIRFRAME %03d", g->health > 0 ? g->health : 0);
    draw_field_sign(g, 502, 39, 130, buf);
}

static void draw_board_lighting(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t now = SDL_GetTicks();
    uint32_t step = now / 83u;
    float blend = (float)(now % 83u) / 83.0f;
    float a = (float)(scene_hash(step * 2654435761u) & 255u) / 255.0f;
    float b = (float)(scene_hash((step + 1u) * 2654435761u) & 255u) / 255.0f;
    float noise = a + (b - a) * blend;
    int glow = 25 + (int)(noise * 22.0f) +
               (int)((sinf(now * 0.0067f) + 1.0f) * 5.0f);
    int flame_x = 628 + (int)(sinf(now * 0.011f) * 2.0f);
    int flame_y = 56 + (int)(noise * 3.0f);
    /* Warm lantern spill at the right edge. */
    blend_circle(s, 628, 66, 42, 255, 168, 65, (uint8_t)(glow / 2));
    blend_circle(s, 628, 63, 25, 255, 196, 84, (uint8_t)glow);
    blend_circle(s, flame_x, flame_y, 7, 255, 222, 129,
                 (uint8_t)(glow + 30));
    blend_circle(s, flame_x, flame_y - 3, 3, 255, 247, 204,
                 (uint8_t)(glow + 70));
    /* The hanging goggles on the opposite side catch the same changing
     * light, delayed and much weaker across their two glass lenses. */
    blend_circle(s, 43, 139, 12, 211, 138, 55,
                 (uint8_t)(42 + glow / 2));
    blend_circle(s, 46, 176, 12, 211, 138, 55,
                 (uint8_t)(42 + glow / 2));
    blend_circle(s, 47, 135, 5, 255, 211, 121,
                 (uint8_t)(92 + glow));
    blend_circle(s, 50, 172, 5, 255, 211, 121,
                 (uint8_t)(92 + glow));
    line(s, 43, 133, 50, 137,
         rgb(s, 255, 224 - (int)(noise * 28.0f), 144));
    line(s, 46, 170, 53, 174,
         rgb(s, 255, 224 - (int)(noise * 28.0f), 144));
}

static int draw_field_hospital(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t elapsed = SDL_GetTicks() - g->scene_started;
    uint32_t ink = rgb(s, 55, 39, 27);
    int fade;
    char buf[48];
    if (!g->fatal_sortie || g->lives <= 0) return 0;
    if (g->field_hospital)
        SDL_BlitSurface(g->field_hospital, NULL, s, NULL);
    else
        rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 83, 64, 44));
    /* An official paper memorandum, not another wooden game plaque. */
    rect(s, 25, 22, 292, 157, rgb(s, 51, 40, 31));
    rect(s, 20, 17, 292, 157, rgb(s, 225, 213, 180));
    rect(s, 24, 21, 284, 149, rgb(s, 236, 226, 198));
    line(s, 31, 63, 300, 63, rgb(s, 95, 75, 53));
    display_text(g, 54, 29, "ROYAL FLYING CORPS", 12, 18,
                 rgb(s, 72, 54, 38));
    typewriter_text(g, 49, 51, "HEAD OFFICE - MEDICAL MEMORANDUM",
                    7, 11, ink);
    typewriter_text(g, 31, 73, "LT. N. SEAGOON WAS EXCEEDINGLY LUCKY.",
                    7, 11, ink);
    typewriter_text(g, 39, 90, "THE MACHINE IS FINISHED. HE IS NOT.",
                    7, 11, ink);
    snprintf(buf, sizeof(buf), "LIVES REMAINING: %d", g->lives);
    typewriter_text(g, 39, 110, buf, 7, 11, ink);
    rect(s, 158, 130, 137, 25, rgb(s, 128, 45, 37));
    rect(s, 161, 133, 131, 19, rgb(s, 236, 226, 198));
    typewriter_text(g, 168, 137, "RETURN TO DUTY", 7, 11,
                    rgb(s, 128, 45, 37));
    typewriter_text(g, 39, 151, "ENTER  ACKNOWLEDGE", 7, 11, ink);
    fade = elapsed < 1600u ? 255 - (int)(elapsed * 255u / 1600u) : 0;
    if (fade > 0)
        blend_rect(s, 0, 0, SCREEN_W, SCREEN_H, 0, 0, 0,
                   (uint8_t)fade);
    return 1;
}

static int draw_fatal_memorial(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t elapsed = SDL_GetTicks() - g->scene_started;
    uint32_t inscription = rgb(s, 55, 47, 39);
    int fade = 0;
    if (!g->fatal_sortie || g->lives > 0 || elapsed >= 20000u) return 0;
    if (g->fatal_memorial)
        SDL_BlitSurface(g->fatal_memorial, NULL, s, NULL);
    else
        rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 35, 32, 29));

    /* Lettering is rendered by the game's native small typewriter face so
     * the photographic asset can remain clean and localisation-safe. */
    typewriter_text(g, 299, 217, "2ND LT", 7, 11, inscription);
    typewriter_text(g, 303, 241, "NEDDY", 7, 11, inscription);
    typewriter_text(g, 296, 254, "SEAGOON", 7, 11, inscription);
    typewriter_text(g, 310, 276, "RFC", 7, 11, inscription);
    typewriter_text(g, 293, 300, "LEST WE", 7, 11, inscription);
    typewriter_text(g, 296, 313, "FORGET", 7, 11, inscription);

    if (elapsed < 2000u)
        fade = 255;
    else if (elapsed < 4500u)
        fade = 255 - (int)((elapsed - 2000u) * 255u / 2500u);
    else if (elapsed > 15000u)
        fade = (int)((elapsed - 15000u) * 255u / 4000u);
    if (fade > 255) fade = 255;
    if (fade > 0)
        blend_rect(s, 0, 0, SCREEN_W, SCREEN_H, 0, 0, 0,
                   (uint8_t)fade);
    return 1;
}

static void draw_debrief(Game *g)
{
    SDL_Surface *s = g->screen;
    char buf[64];
    uint32_t elapsed = SDL_GetTicks() - g->scene_started;
    uint32_t ink = rgb(s, 47, 37, 27);
    uint32_t red = rgb(s, 125, 39, 31);
    if (draw_field_hospital(g)) return;
    if (draw_fatal_memorial(g)) return;
    rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 34, 24, 17));
    if (g->mission_board) SDL_BlitSurface(g->mission_board, NULL, s, NULL);
    draw_board_lighting(g);
    display_text(g, 69, 48,
         g->success ? "MISSION ACCOMPLISHED" : "MISSION INCOMPLETE",
         12, 18, red);
    if (is_perspective_mode(campaign_missions[g->chapter][g->mission].mode))
        snprintf(buf, sizeof(buf), "ENEMY AIRCRAFT              %d/%d",
                 g->kills, current_mission_goal(g));
    else
        snprintf(buf, sizeof(buf), "MISSION OBJECTIVES          %d/%d",
                 g->objectives, current_mission_goal(g));
    typewriter_text(g, 74, 115, buf, 7, 11, ink);
    snprintf(buf, sizeof(buf), "AIRCRAFT SCORE              %d", g->score);
    typewriter_text(g, 74, 136, buf, 7, 11, ink);
    snprintf(buf, sizeof(buf), "FUEL REMAINING              %d", (int)g->fuel);
    typewriter_text(g, 74, 157, buf, 7, 11, ink);
    snprintf(buf, sizeof(buf), "AIRFRAME REMAINING          %d", g->health);
    typewriter_text(g, 74, 178, buf, 7, 11, ink);
    display_text(g, 73, 226,
         g->success ? "THE MESS WILL HEAR." :
         g->fatal_sortie ? "A LONE PIPER PLAYS." :
                           "WE FLY AGAIN AT DAWN.",
         12, 18, red);
    if (g->mission_save_state > 0) {
        if (g->saved_campaign_complete)
            snprintf(buf, sizeof(buf), "WAR RECORD SAVED");
        else
            snprintf(buf, sizeof(buf), "POSITION SAVED - CHAPTER %d / MISSION %d",
                     g->saved_chapter + 1, g->saved_mission + 1);
    } else {
        snprintf(buf, sizeof(buf), "SAVE FAILED - ENTER WILL RETRY");
    }
    draw_field_sign(g, 164, 303, 312, buf);
    draw_field_sign(g, 176, 341, 288,
                    g->success ? "ENTER  CONTINUE THE CAMPAIGN" :
                    (g->fatal_sortie && g->lives <= 0) ?
                                 "ENTER  RETURN TO OFFICERS' MESS" :
                                 "ENTER  FLY THE MISSION AGAIN");
    if (g->fatal_sortie && elapsed < 21000u) {
        int fade = 255 - (int)((elapsed - 20000u) * 255u / 1000u);
        if (fade < 0) fade = 0;
        blend_rect(s, 0, 0, SCREEN_W, SCREEN_H, 0, 0, 0,
                   (uint8_t)fade);
    }
}

static void draw_epilogue(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t ink = rgb(s, 47, 37, 27);
    uint32_t red = rgb(s, 125, 39, 31);
    rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 34, 24, 17));
    if (g->mission_board) SDL_BlitSurface(g->mission_board, NULL, s, NULL);
    draw_board_lighting(g);
    display_text(g, 72, 48, "11:00 HOURS", 16, 23, red);
    display_text(g, 72, 82, "ARMISTICE", 22, 28, ink);
    typewriter_text(g, 73, 137, "THE GUNS FALL SILENT.", 7, 11, ink);
    typewriter_text(g, 73, 158, "FOUR YEARS OF SKY AND MUD ARE OVER.", 7, 11, ink);
    typewriter_text(g, 73, 179, "THE EMPTY CHAIRS REMAIN.", 7, 11, ink);
    typewriter_text(g, 73, 221, "TOM RINGS THE FIELD BELL.", 7, 11, red);
    typewriter_text(g, 73, 242, "YOU TURN FOR HOME.", 7, 11, red);
    typewriter_text(g, 73, 348, "ENTER  RETURN TO TITLE", 7, 11, ink);
}

static int init_audio(Game *g)
{
    SDL_AudioSpec want, wav_spec;
    uint8_t *wav_data = NULL;
    uint32_t wav_len = 0;
    SDL_AudioCVT cvt;
    /* Recordings are kept as mono 11 kHz: the mixer sums them to one
     * channel anyway, and mono halves their memory. */
    init_audio_tables();
    if (SDL_LoadWAV(CANVAS_MARCH_WAV, &wav_spec, &wav_data, &wav_len)) {
        int cvt_result = SDL_BuildAudioCVT(&cvt, wav_spec.format,
                                           wav_spec.channels, wav_spec.freq,
                                           AUDIO_S16SYS, 1, 11025);
        if (cvt_result >= 0 && cvt.needed) {
            cvt.len = (int)wav_len;
            cvt.buf = (uint8_t *)SDL_malloc((size_t)wav_len * cvt.len_mult);
            if (cvt.buf) {
                memcpy(cvt.buf, wav_data, wav_len);
                SDL_FreeWAV(wav_data);
                wav_data = NULL;
                if (SDL_ConvertAudio(&cvt) == 0) {
                    g->audio.music_data = cvt.buf;
                    g->audio.music_len = (uint32_t)cvt.len_cvt;
                } else {
                    SDL_free(cvt.buf);
                }
            }
        } else if (cvt_result >= 0) {
            g->audio.music_data = wav_data;
            g->audio.music_len = wav_len;
            wav_data = NULL;
        }
        if (wav_data) SDL_FreeWAV(wav_data);
    } else {
        fprintf(stderr, "Gramophone march unavailable; using synth fallback: %s\n",
                SDL_GetError());
    }
    wav_data = NULL;
    wav_len = 0;
    if (SDL_LoadWAV(CANVAS_MESS_WAV, &wav_spec, &wav_data, &wav_len)) {
        int cvt_result = SDL_BuildAudioCVT(&cvt, wav_spec.format,
                                           wav_spec.channels, wav_spec.freq,
                                           AUDIO_S16SYS, 1, 11025);
        if (cvt_result >= 0 && cvt.needed) {
            cvt.len = (int)wav_len;
            cvt.buf = (uint8_t *)SDL_malloc((size_t)wav_len * cvt.len_mult);
            if (cvt.buf) {
                memcpy(cvt.buf, wav_data, wav_len);
                SDL_FreeWAV(wav_data);
                wav_data = NULL;
                if (SDL_ConvertAudio(&cvt) == 0) {
                    g->audio.mess_data = cvt.buf;
                    g->audio.mess_len = (uint32_t)cvt.len_cvt;
                } else {
                    SDL_free(cvt.buf);
                }
            }
        } else if (cvt_result >= 0) {
            g->audio.mess_data = wav_data;
            g->audio.mess_len = wav_len;
            wav_data = NULL;
        }
        if (wav_data) SDL_FreeWAV(wav_data);
    } else {
        fprintf(stderr, "Colonel Bogey recording unavailable; using synth fallback: %s\n",
                SDL_GetError());
    }
    wav_data = NULL;
    wav_len = 0;
    if (SDL_LoadWAV(CANVAS_PIPER_WAV, &wav_spec, &wav_data, &wav_len)) {
        int cvt_result = SDL_BuildAudioCVT(&cvt, wav_spec.format,
                                           wav_spec.channels, wav_spec.freq,
                                           AUDIO_S16SYS, 1, 11025);
        if (cvt_result >= 0 && cvt.needed) {
            cvt.len = (int)wav_len;
            cvt.buf = (uint8_t *)SDL_malloc((size_t)wav_len * cvt.len_mult);
            if (cvt.buf) {
                memcpy(cvt.buf, wav_data, wav_len);
                SDL_FreeWAV(wav_data);
                wav_data = NULL;
                if (SDL_ConvertAudio(&cvt) == 0) {
                    g->audio.piper_data = cvt.buf;
                    g->audio.piper_len = (uint32_t)cvt.len_cvt;
                } else {
                    SDL_free(cvt.buf);
                }
            }
        } else if (cvt_result >= 0) {
            g->audio.piper_data = wav_data;
            g->audio.piper_len = wav_len;
            wav_data = NULL;
        }
        if (wav_data) SDL_FreeWAV(wav_data);
    } else {
        fprintf(stderr, "Amazing Grace pipes unavailable; using synth fallback: %s\n",
                SDL_GetError());
    }
    memset(&want, 0, sizeof(want));
    want.freq = 11025;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 512;
    want.callback = audio_callback;
    want.userdata = &g->audio;
    g->audio.noise = 0x19171917u;
    g->audio.device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
    if (!g->audio.device) {
        fprintf(stderr, "Canvas Skies audio disabled: %s\n", SDL_GetError());
        return 0;
    }
    SDL_PauseAudioDevice(g->audio.device, 0);
    return 1;
}

static int game_init(Game *g)
{
    memset(g, 0, sizeof(*g));
    g->running = 1;
    g->lives = 3;
    g->has_save = load_campaign_save(g);
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return 0;
    }
    g->window = SDL_CreateWindow("Canvas Skies",
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);
    if (!g->window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        return 0;
    }
    g->window_surface = SDL_GetWindowSurface(g->window);
    g->screen = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_W, SCREEN_H, 32,
                                               SDL_PIXELFORMAT_ARGB8888);
    if (!g->window_surface || !g->screen ||
        g->screen->format->BytesPerPixel != 4) {
        fprintf(stderr, "Canvas Skies could not create its 32-bit framebuffer\n");
        return 0;
    }
    g->title = SDL_LoadBMP(CANVAS_TITLE_BMP);
    if (!g->title)
        fprintf(stderr, "Title art unavailable (%s): %s\n", CANVAS_TITLE_BMP, SDL_GetError());
    g->title_banner = SDL_LoadBMP(CANVAS_TITLE_BANNER_BMP);
    if (g->title_banner)
        prepare_magenta_key(g->title_banner);
    else
        fprintf(stderr, "Stitched title banner unavailable (%s): %s\n",
                CANVAS_TITLE_BANNER_BMP, SDL_GetError());
    g->mission_board = SDL_LoadBMP(CANVAS_BOARD_BMP);
    if (!g->mission_board)
        fprintf(stderr, "Mission board unavailable (%s): %s\n",
                CANVAS_BOARD_BMP, SDL_GetError());
    g->fatal_memorial = SDL_LoadBMP(CANVAS_FATAL_MEMORIAL_BMP);
    if (g->fatal_memorial) {
        SDL_Surface *converted = SDL_ConvertSurface(g->fatal_memorial,
                                                    g->screen->format, 0);
        if (converted) {
            SDL_FreeSurface(g->fatal_memorial);
            g->fatal_memorial = converted;
        }
    } else {
        fprintf(stderr, "Fatal memorial unavailable (%s): %s\n",
                CANVAS_FATAL_MEMORIAL_BMP, SDL_GetError());
    }
    g->field_hospital = SDL_LoadBMP(CANVAS_FIELD_HOSPITAL_BMP);
    if (g->field_hospital) {
        SDL_Surface *converted = SDL_ConvertSurface(g->field_hospital,
                                                    g->screen->format, 0);
        if (converted) {
            SDL_FreeSurface(g->field_hospital);
            g->field_hospital = converted;
        }
    } else {
        fprintf(stderr, "Field hospital unavailable (%s): %s\n",
                CANVAS_FIELD_HOSPITAL_BMP, SDL_GetError());
    }
    g->journal = SDL_LoadBMP(CANVAS_JOURNAL_BMP);
    if (!g->journal)
        fprintf(stderr, "Journal art unavailable (%s): %s\n",
                CANVAS_JOURNAL_BMP, SDL_GetError());
    g->sky = SDL_LoadBMP(CANVAS_SKY_BMP);
    if (g->sky) {
        SDL_Surface *converted = SDL_ConvertSurface(g->sky, g->screen->format, 0);
        if (converted) {
            SDL_FreeSurface(g->sky);
            g->sky = converted;
        }
    } else {
        fprintf(stderr, "Sky panorama unavailable (%s): %s\n",
                CANVAS_SKY_BMP, SDL_GetError());
    }
    g->battlefield = SDL_LoadBMP(CANVAS_BATTLEFIELD_BMP);
    if (g->battlefield) {
        SDL_Surface *converted = SDL_ConvertSurface(g->battlefield,
                                                    g->screen->format, 0);
        if (converted) {
            SDL_FreeSurface(g->battlefield);
            g->battlefield = converted;
        }
        prepare_magenta_key(g->battlefield);
    } else {
        fprintf(stderr, "Battlefield panorama unavailable (%s): %s\n",
                CANVAS_BATTLEFIELD_BMP, SDL_GetError());
    }
    g->foreground_panorama = SDL_LoadBMP(CANVAS_FOREGROUND_BMP);
    if (g->foreground_panorama)
        prepare_magenta_key(g->foreground_panorama);
    else
        fprintf(stderr, "Foreground panorama unavailable (%s): %s\n",
                CANVAS_FOREGROUND_BMP, SDL_GetError());
    {
        const char *mid_paths[MIDGROUND_ELEMENT_COUNT] = {
            CANVAS_MID_FARM_BMP, CANVAS_MID_CHURCH_BMP,
            CANVAS_MID_POPLARS_BMP, CANVAS_MID_SMOKE_BMP,
            CANVAS_MID_TELEGRAPH_BMP, CANVAS_MID_WALL_BMP
        };
        const char *near_paths[NEAR_ELEMENT_COUNT] = {
            CANVAS_NEAR_CRATER_BMP, CANVAS_NEAR_TRENCH_BMP,
            CANVAS_NEAR_TREE_BMP, CANVAS_NEAR_WIRE_BMP,
            CANVAS_NEAR_ROAD_BMP, CANVAS_NEAR_MOUND_BMP
        };
        int i;
        for (i = 0; i < MIDGROUND_ELEMENT_COUNT; ++i) {
            g->midground[i] = SDL_LoadBMP(mid_paths[i]);
            if (g->midground[i])
                prepare_magenta_key(g->midground[i]);
            else
                fprintf(stderr, "Midground element unavailable (%s): %s\n",
                        mid_paths[i], SDL_GetError());
        }
        for (i = 0; i < NEAR_ELEMENT_COUNT; ++i) {
            g->near_props[i] = SDL_LoadBMP(near_paths[i]);
            if (g->near_props[i])
                prepare_magenta_key(g->near_props[i]);
            else
                fprintf(stderr, "Near-ground element unavailable (%s): %s\n",
                        near_paths[i], SDL_GetError());
        }
    }
    g->machine_gun_sprite = SDL_LoadBMP(CANVAS_MACHINE_GUN_BMP);
    g->aa_gun_sprite = SDL_LoadBMP(CANVAS_AA_GUN_BMP);
    {
        SDL_Surface *sprites[] = {g->machine_gun_sprite, g->aa_gun_sprite};
        int i;
        for (i = 0; i < 2; ++i) if (sprites[i])
            prepare_magenta_key(sprites[i]);
    }
    g->topdown_battlefield = SDL_LoadBMP(CANVAS_TOPDOWN_BATTLEFIELD_BMP);
    g->topdown_player = SDL_LoadBMP(CANVAS_TOPDOWN_PLAYER_BMP);
    g->dogfight_background = SDL_LoadBMP(CANVAS_DOGFIGHT_BACKGROUND_BMP);
    g->dogfight_enemy = SDL_LoadBMP(CANVAS_DOGFIGHT_ENEMY_BMP);
    g->pilot_cockpit = SDL_LoadBMP(CANVAS_PILOT_COCKPIT_BMP);
    g->gunner_cockpit = SDL_LoadBMP(CANVAS_GUNNER_COCKPIT_BMP);
    g->gunner_ring = SDL_LoadBMP(CANVAS_GUNNER_RING_BMP);
    g->gunner_airframe = SDL_LoadBMP(CANVAS_GUNNER_AIRFRAME_BMP);
    g->gunner_moving_guns = SDL_LoadBMP(CANVAS_GUNNER_MOVING_BMP);
    g->explosion_atlas = SDL_LoadBMP(CANVAS_EXPLOSION_ATLAS_BMP);
    g->topdown_explosion_atlas = SDL_LoadBMP(CANVAS_TOPDOWN_EXPLOSION_ATLAS_BMP);
    {
        SDL_Surface **convert[] = {
            &g->topdown_battlefield, &g->topdown_player,
            &g->dogfight_background, &g->dogfight_enemy,
            &g->pilot_cockpit, &g->gunner_cockpit,
            &g->gunner_ring, &g->gunner_airframe,
            &g->gunner_moving_guns,
            &g->explosion_atlas, &g->topdown_explosion_atlas
        };
        int i;
        for (i = 0; i < 11; ++i) if (*convert[i]) {
            SDL_Surface *converted = SDL_ConvertSurface(*convert[i],
                                                        g->screen->format, 0);
            if (converted) {
                SDL_FreeSurface(*convert[i]);
                *convert[i] = converted;
            }
        }
    }
    {
        SDL_Surface *keyed[] = {g->topdown_player, g->dogfight_enemy,
                                g->pilot_cockpit, g->gunner_cockpit,
                                g->gunner_ring, g->gunner_airframe,
                                g->gunner_moving_guns,
                                g->explosion_atlas,
                                g->topdown_explosion_atlas};
        int i;
        for (i = 0; i < 9; ++i) if (keyed[i])
            prepare_magenta_key(keyed[i]);
    }
    if (!g->topdown_battlefield || !g->topdown_player ||
        !g->dogfight_background || !g->dogfight_enemy ||
        !g->pilot_cockpit || !g->gunner_cockpit || !g->gunner_ring ||
        !g->gunner_airframe || !g->gunner_moving_guns || !g->explosion_atlas)
        fprintf(stderr, "One or more alternate-view art assets are unavailable\n");
    g->font_display_36x42 = SDL_LoadBMP(CANVAS_DISPLAY_36X42_BMP);
    g->font_display_22x28 = SDL_LoadBMP(CANVAS_DISPLAY_22X28_BMP);
    g->font_display_16x23 = SDL_LoadBMP(CANVAS_DISPLAY_16X23_BMP);
    g->font_display_12x18 = SDL_LoadBMP(CANVAS_DISPLAY_12X18_BMP);
    g->font_typewriter_10x15 = SDL_LoadBMP(CANVAS_TYPEWRITER_10X15_BMP);
    g->font_typewriter_7x11 = SDL_LoadBMP(CANVAS_TYPEWRITER_7X11_BMP);
    g->font_journal_8x12 = SDL_LoadBMP(CANVAS_JOURNAL_8X12_BMP);
    if (!g->font_display_36x42 || !g->font_display_22x28 ||
        !g->font_display_16x23 || !g->font_display_12x18 ||
        !g->font_typewriter_10x15 || !g->font_typewriter_7x11 ||
        !g->font_journal_8x12)
        fprintf(stderr, "One or more native-size font atlases are unavailable\n");
    g->player_sprites = SDL_LoadBMP(CANVAS_PLAYER_SPRITES_BMP);
    g->enemy_sprites = SDL_LoadBMP(CANVAS_ENEMY_SPRITES_BMP);
    if (!g->player_sprites || !g->enemy_sprites)
        fprintf(stderr, "Aircraft sprite atlas unavailable; using vector fallback\n");
    {
        /* Fonts and aircraft atlases are read pixel by pixel every frame.
         * Holding them in the screen's 32-bit format lets the renderer read
         * them directly instead of decoding 24-bit BMP pixels through SDL. */
        SDL_Surface **atlases[] = {
            &g->font_display_36x42, &g->font_display_22x28,
            &g->font_display_16x23, &g->font_display_12x18,
            &g->font_typewriter_10x15, &g->font_typewriter_7x11,
            &g->font_journal_8x12, &g->player_sprites, &g->enemy_sprites
        };
        int i;
        for (i = 0; i < (int)(sizeof(atlases) / sizeof(atlases[0])); ++i)
            if (*atlases[i]) {
                SDL_Surface *converted = SDL_ConvertSurface(*atlases[i],
                                                            g->screen->format, 0);
                if (converted) {
                    SDL_FreeSurface(*atlases[i]);
                    *atlases[i] = converted;
                }
            }
    }
    init_audio(g);
    set_scene(g, SCENE_TITLE);
    return 1;
}

static void handle_event(Game *g, SDL_Event *ev)
{
    SDL_Scancode scancode;
    if (ev->type == SDL_QUIT) g->running = 0;
    if (ev->type == SDL_WINDOWEVENT &&
        (ev->window.event == SDL_WINDOWEVENT_EXPOSED ||
         ev->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
        g->window_surface = SDL_GetWindowSurface(g->window);
        g->redraw_static = 1;
    }
    if (ev->type == SDL_KEYUP) {
        scancode = ev->key.keysym.scancode;
        if (scancode >= 0 && scancode < SDL_NUM_SCANCODES)
            g->key_latched[scancode] = 0;
        return;
    }
    if (ev->type != SDL_KEYDOWN) return;
    scancode = ev->key.keysym.scancode;
    /* SDL2.module may report host-generated repeats with repeat == 0.  Latch
     * menu/action keys until their physical key-up so one Enter cannot skip
     * the commission board, journal and briefing in a single press. */
    if (scancode >= 0 && scancode < SDL_NUM_SCANCODES) {
        if (g->key_latched[scancode]) return;
        g->key_latched[scancode] = 1;
    }
    if ((ev->key.keysym.sym == SDLK_RETURN ||
         ev->key.keysym.sym == SDLK_KP_ENTER) &&
        g->last_action_key && SDL_GetTicks() - g->last_action_key < 450u)
        return;
    switch (ev->key.keysym.sym) {
    case SDLK_F9:
        toggle_fullscreen(g);
        break;
    case SDLK_ESCAPE:
        if (g->scene == SCENE_MENU && g->confirm_overwrite) {
            g->confirm_overwrite = 0;
            g->redraw_static = 1;
        } else if (g->scene == SCENE_TITLE) g->running = 0;
        else set_scene(g, SCENE_TITLE);
        break;
    case SDLK_UP:
        if (g->scene == SCENE_MENU) {
            int count = campaign_menu_count(g);
            g->confirm_overwrite = 0;
            g->menu_selection = (g->menu_selection + count - 1) % count;
            g->redraw_static = 1;
        }
        break;
    case SDLK_DOWN:
        if (g->scene == SCENE_MENU) {
            g->confirm_overwrite = 0;
            g->menu_selection = (g->menu_selection + 1) %
                                campaign_menu_count(g);
            g->redraw_static = 1;
        }
        break;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        g->last_action_key = SDL_GetTicks();
        if (g->scene == SCENE_TITLE) {
            g->menu_selection = 0;
            g->confirm_overwrite = 0;
            set_scene(g, SCENE_MENU);
        }
        else if (g->scene == SCENE_MENU) {
            static const int start_chapter[6] = {0, 0, 1, 2, 3, 5};
            int pilot;
            int has_continue = g->has_save && g->saved_lives > 0;
            if (has_continue && g->menu_selection == 0) {
                g->chapter = g->saved_chapter;
                g->mission = g->saved_mission;
                g->campaign_complete = g->saved_campaign_complete;
                g->lives = g->saved_lives;
                memcpy(g->wingman_health, g->saved_wingman_health,
                       sizeof(g->wingman_health));
                if (g->campaign_complete) {
                    set_scene(g, SCENE_EPILOGUE);
                    break;
                }
            } else {
                int new_index = g->menu_selection - (has_continue ? 1 : 0);
                if (has_continue && !g->confirm_overwrite) {
                    /* A new commission replaces the saved campaign, so ask
                     * once before the old record is overwritten. */
                    g->confirm_overwrite = 1;
                    g->redraw_static = 1;
                    break;
                }
                g->confirm_overwrite = 0;
                g->chapter = start_chapter[new_index];
                g->mission = 0;
                g->campaign_complete = 0;
                g->lives = 3;
                for (pilot = 0; pilot < 3; ++pilot)
                    g->wingman_health[pilot] = 100;
                apply_story_wingmen(g);
                save_campaign(g);
            }
            set_scene(g, SCENE_STORY);
        }
        else if (g->scene == SCENE_STORY) set_scene(g, SCENE_BRIEFING);
        else if (g->scene == SCENE_BRIEFING) {
            reset_sortie(g);
            set_scene(g, SCENE_SORTIE);
        } else if (g->scene == SCENE_DEBRIEF) {
            if (g->mission_save_state < 0) {
                g->mission_save_state = 0;
                if (!save_mission_result(g)) break;
            }
            if (!g->success && g->fatal_sortie && g->lives <= 0) {
                g->menu_selection = 0;
                set_scene(g, SCENE_TITLE);
            }
            else if (!g->success) set_scene(g, SCENE_BRIEFING);
            else if (g->mission + 1 < MISSIONS_PER_CHAPTER) {
                ++g->mission;
                set_scene(g, SCENE_STORY);
            } else if (g->chapter + 1 >= CHAPTER_COUNT) {
                g->campaign_complete = 1;
                set_scene(g, SCENE_EPILOGUE);
            } else {
                ++g->chapter;
                g->mission = 0;
                set_scene(g, SCENE_STORY);
            }
        } else if (g->scene == SCENE_EPILOGUE) set_scene(g, SCENE_TITLE);
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{
    Game g;
    uint32_t previous;
    if (!game_init(&g)) return 1;
    /* QA entry points keep the normal title/campaign flow untouched while
     * allowing each renderer to be launched directly on real Amiga hardware. */
    if (argc > 1 && strcmp(argv[1], "SIDE") == 0) {
        g.chapter = 2;
        g.mission = 4;
        reset_sortie(&g);
        set_scene(&g, SCENE_SORTIE);
    } else if (argc > 1 && strcmp(argv[1], "TOPDOWN") == 0) {
        g.chapter = 2;
        g.mission = 1;
        reset_sortie(&g);
        set_scene(&g, SCENE_SORTIE);
    } else if (argc > 1 && strcmp(argv[1], "DOGFIGHT") == 0) {
        /* Use a durable late-war squadron for visual QA so the direct test
         * remains playable instead of completing before the window appears. */
        g.chapter = 6;
        g.mission = 5;
        reset_sortie(&g);
        g.qa_hold = 1;
        set_scene(&g, SCENE_SORTIE);
    } else if (argc > 1 && strcmp(argv[1], "DEATH") == 0) {
        int damage_mark;
        /* Deterministic first-person fatal sequence: loss of control, ground
         * impact, wreck hold, failed debrief and solo-piper cue.  The staged
         * holes make accumulated battle damage visible during this QA path. */
        g.chapter = 6;
        g.mission = 5;
        g.lives = 1;
        reset_sortie(&g);
        for (damage_mark = 0; damage_mark < 12; ++damage_mark)
            add_dogfight_bullet_hole(&g,
                0x19180000u + (uint32_t)damage_mark * 977u);
        g.flight_y = 420.0f;
        g.health = 0;
        g.fatal_sortie = 1;
        g.flight_doomed = 1;
        g.flight_doomed_started = SDL_GetTicks();
        set_scene(&g, SCENE_SORTIE);
    } else if (argc > 1 && strcmp(argv[1], "HOSPITAL") == 0) {
        /* The same fatal crash with lives in reserve must recover into the
         * field hospital rather than the final memorial. */
        g.chapter = 3;
        g.mission = 4;
        g.lives = 3;
        reset_sortie(&g);
        g.flight_y = 260.0f;
        g.health = 0;
        g.fatal_sortie = 1;
        g.flight_doomed = 1;
        g.flight_doomed_started = SDL_GetTicks();
        set_scene(&g, SCENE_SORTIE);
    } else if (argc > 1 && strcmp(argv[1], "GUNNER") == 0) {
        /* Late-war QA keeps more durable attackers in view long enough to
         * inspect the full rail traverse and approach paths. */
        g.chapter = 6;
        g.mission = 2;
        reset_sortie(&g);
        g.qa_hold = 1;
        set_scene(&g, SCENE_SORTIE);
    } else if (argc > 1 && strcmp(argv[1], "DEBRIEF") == 0) {
        g.chapter = 2;
        g.mission = 2;
        reset_sortie(&g);
        g.success = 0;
        g.health = 25;
        set_scene(&g, SCENE_DEBRIEF);
    } else if (argc > 1 && strcmp(argv[1], "DEBRIEF_SUCCESS") == 0) {
        /* Direct listening/visual test for the mission-complete board and its
         * de-clicked officers' mess cue. */
        g.chapter = 2;
        g.mission = 2;
        reset_sortie(&g);
        g.success = 1;
        g.kills = current_mission_goal(&g);
        g.score = g.kills * 250;
        set_scene(&g, SCENE_DEBRIEF);
    } else if (argc > 1 && strcmp(argv[1], "FINAL_SUCCESS") == 0) {
        /* Direct campaign-completion test.  Enter must save a completed war
         * record and open the Armistice rather than replaying mission 96. */
        g.chapter = CHAPTER_COUNT - 1;
        g.mission = MISSIONS_PER_CHAPTER - 1;
        g.wingman_health[0] = 100;
        g.wingman_health[1] = 100;
        g.wingman_health[2] = 100;
        reset_sortie(&g);
        g.success = 1;
        g.kills = current_mission_goal(&g);
        g.score = g.kills * 250;
        set_scene(&g, SCENE_DEBRIEF);
    } else if (argc > 1 && strcmp(argv[1], "CHAPTER_SUCCESS") == 0) {
        /* Direct chapter-boundary test.  Enter must advance mission 12 to
         * mission 1 of the following chapter and persist that exact point. */
        g.chapter = 0;
        g.mission = MISSIONS_PER_CHAPTER - 1;
        g.wingman_health[0] = 100;
        g.wingman_health[1] = 100;
        g.wingman_health[2] = 100;
        reset_sortie(&g);
        g.success = 1;
        g.kills = current_mission_goal(&g);
        g.score = g.kills * 250;
        set_scene(&g, SCENE_DEBRIEF);
    }
    previous = SDL_GetTicks();
    while (g.running) {
        SDL_Event ev;
        uint32_t now;
        float dt;
        const uint8_t *keys;
        while (SDL_PollEvent(&ev)) handle_event(&g, &ev);
        now = SDL_GetTicks();
        dt = (float)(now - previous) / 1000.0f;
        previous = now;
        if (dt > 0.05f) dt = 0.05f;
        keys = SDL_GetKeyboardState(NULL);
        if (g.scene == SCENE_SORTIE) update_sortie(&g, dt, keys);
        /* The officers' menu is static: it is redrawn only when the selection
         * or prompt changes (redraw_static), not every frame. */
        if (g.scene == SCENE_TITLE ||
            g.scene == SCENE_SORTIE || g.scene == SCENE_DEBRIEF ||
            g.redraw_static) {
            if (SDL_MUSTLOCK(g.screen)) SDL_LockSurface(g.screen);
            switch (g.scene) {
            case SCENE_TITLE: draw_title(&g); break;
            case SCENE_MENU: draw_campaign_menu(&g); break;
            case SCENE_STORY: draw_story(&g); break;
            case SCENE_BRIEFING: draw_briefing(&g); break;
            case SCENE_SORTIE: draw_sortie(&g); break;
            case SCENE_DEBRIEF: draw_debrief(&g); break;
            case SCENE_EPILOGUE: draw_epilogue(&g); break;
            }
            if (SDL_MUSTLOCK(g.screen)) SDL_UnlockSurface(g.screen);
            present_frame(&g);
            g.redraw_static = 0;
        }
        SDL_Delay(8);
    }
    if (g.audio.device) SDL_CloseAudioDevice(g.audio.device);
    if (g.title) SDL_FreeSurface(g.title);
    if (g.title_banner) SDL_FreeSurface(g.title_banner);
    if (g.title_backdrop) SDL_FreeSurface(g.title_backdrop);
    if (g.mission_board) SDL_FreeSurface(g.mission_board);
    if (g.fatal_memorial) SDL_FreeSurface(g.fatal_memorial);
    if (g.field_hospital) SDL_FreeSurface(g.field_hospital);
    if (g.journal) SDL_FreeSurface(g.journal);
    if (g.sky) SDL_FreeSurface(g.sky);
    if (g.battlefield) SDL_FreeSurface(g.battlefield);
    if (g.foreground_panorama) SDL_FreeSurface(g.foreground_panorama);
    {
        int i;
        for (i = 0; i < MIDGROUND_ELEMENT_COUNT; ++i)
            if (g.midground[i]) SDL_FreeSurface(g.midground[i]);
        for (i = 0; i < NEAR_ELEMENT_COUNT; ++i)
            if (g.near_props[i]) SDL_FreeSurface(g.near_props[i]);
    }
    if (g.machine_gun_sprite) SDL_FreeSurface(g.machine_gun_sprite);
    if (g.aa_gun_sprite) SDL_FreeSurface(g.aa_gun_sprite);
    if (g.topdown_battlefield) SDL_FreeSurface(g.topdown_battlefield);
    if (g.topdown_player) SDL_FreeSurface(g.topdown_player);
    if (g.dogfight_background) SDL_FreeSurface(g.dogfight_background);
    if (g.dogfight_enemy) SDL_FreeSurface(g.dogfight_enemy);
    if (g.pilot_cockpit) SDL_FreeSurface(g.pilot_cockpit);
    if (g.gunner_cockpit) SDL_FreeSurface(g.gunner_cockpit);
    if (g.gunner_ring) SDL_FreeSurface(g.gunner_ring);
    if (g.gunner_airframe) SDL_FreeSurface(g.gunner_airframe);
    if (g.gunner_moving_guns) SDL_FreeSurface(g.gunner_moving_guns);
    if (g.explosion_atlas) SDL_FreeSurface(g.explosion_atlas);
    if (g.topdown_explosion_atlas) SDL_FreeSurface(g.topdown_explosion_atlas);
    if (g.font_display_36x42) SDL_FreeSurface(g.font_display_36x42);
    if (g.font_display_22x28) SDL_FreeSurface(g.font_display_22x28);
    if (g.font_display_16x23) SDL_FreeSurface(g.font_display_16x23);
    if (g.font_display_12x18) SDL_FreeSurface(g.font_display_12x18);
    if (g.font_typewriter_10x15) SDL_FreeSurface(g.font_typewriter_10x15);
    if (g.font_typewriter_7x11) SDL_FreeSurface(g.font_typewriter_7x11);
    if (g.font_journal_8x12) SDL_FreeSurface(g.font_journal_8x12);
    if (g.player_sprites) SDL_FreeSurface(g.player_sprites);
    if (g.enemy_sprites) SDL_FreeSurface(g.enemy_sprites);
    if (g.audio.music_data) SDL_free(g.audio.music_data);
    if (g.audio.mess_data) SDL_free(g.audio.mess_data);
    if (g.audio.piper_data) SDL_free(g.audio.piper_data);
    if (g.screen) SDL_FreeSurface(g.screen);
    SDL_DestroyWindow(g.window);
    SDL_Quit();
    return 0;
}
