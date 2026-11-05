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
#include "canvas_news.h"

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
#ifndef CANVAS_PILOT_COCKPIT_UNARMED_BMP
#define CANVAS_PILOT_COCKPIT_UNARMED_BMP "assets/canvas-skies/pilot-cockpit-unarmed.bmp"
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
/* Period gramophone records for the story, diary and briefing of each year. */
#ifndef CANVAS_TIPPERARY_WAV
#define CANVAS_TIPPERARY_WAV "assets/canvas-skies/tipperary-gramophone.wav"
#endif
#ifndef CANVAS_HOME_FIRES_WAV
#define CANVAS_HOME_FIRES_WAV "assets/canvas-skies/home-fires-gramophone.wav"
#endif
#ifndef CANVAS_KIT_BAG_WAV
#define CANVAS_KIT_BAG_WAV "assets/canvas-skies/kit-bag-gramophone.wav"
#endif
#ifndef CANVAS_HOLYROOD_WAV
#define CANVAS_HOLYROOD_WAV "assets/canvas-skies/holyrood-gramophone.wav"
#endif
#ifndef CANVAS_VOICE_OF_THE_GUNS_WAV
#define CANVAS_VOICE_OF_THE_GUNS_WAV "assets/canvas-skies/voice-of-the-guns-gramophone.wav"
#endif
#ifndef CANVAS_HOLST_WAV
#define CANVAS_HOLST_WAV "assets/canvas-skies/holst-second-suite-gramophone.wav"
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

/* The game draws into a hi-res 16-bit (RGB565) buffer by default.  It halves
 * the memory traffic of a 32-bit buffer, which matters most on a 68060 or a
 * PiStorm, and matches a 16-bit RTG screen without conversion.  Build with
 * -DCANVAS_SCREEN_BITS=32 for the previous ARGB8888 buffer. */
#ifndef CANVAS_SCREEN_BITS
#define CANVAS_SCREEN_BITS 16
#endif
#if CANVAS_SCREEN_BITS == 16
typedef uint16_t pix_t;
#define CANVAS_SCREEN_FORMAT SDL_PIXELFORMAT_RGB565
#define PIX_PACK(r, g, b) ((pix_t)((((unsigned)(r) & 0xf8u) << 8) | \
                                   (((unsigned)(g) & 0xfcu) << 3) | \
                                   ((unsigned)(b) >> 3)))
#else
typedef uint32_t pix_t;
#define CANVAS_SCREEN_FORMAT SDL_PIXELFORMAT_ARGB8888
#define PIX_PACK(r, g, b) ((pix_t)(0xff000000u | ((uint32_t)(r) << 16) | \
                                   ((uint32_t)(g) << 8) | (uint32_t)(b)))
#endif
#define PIX_ROW(s, y) ((pix_t *)((uint8_t *)(s)->pixels + (y) * (s)->pitch))

/* Mix two screen pixels; t runs 0 (all a) to 32 (all b).  The channels are
 * spread apart in one register so a single multiply blends all three. */
static inline pix_t pix_mix(pix_t a, pix_t b, int t)
{
#if CANVAS_SCREEN_BITS == 16
    uint32_t x = ((uint32_t)a | ((uint32_t)a << 16)) & 0x07e0f81fu;
    uint32_t y = ((uint32_t)b | ((uint32_t)b << 16)) & 0x07e0f81fu;
    uint32_t r = ((x * (uint32_t)(32 - t) + y * (uint32_t)t) >> 5) &
                 0x07e0f81fu;
    return (pix_t)(r | (r >> 16));
#else
    uint32_t rb = (((a & 0xff00ffu) * (uint32_t)(32 - t) +
                    (b & 0xff00ffu) * (uint32_t)t) >> 5) & 0xff00ffu;
    uint32_t gg = (((a & 0x00ff00u) * (uint32_t)(32 - t) +
                    (b & 0x00ff00u) * (uint32_t)t) >> 5) & 0x00ff00u;
    return (pix_t)(0xff000000u | rb | gg);
#endif
}
#define ART_H 360
#define GAMEPLAY_Y_OFFSET (SCREEN_H - ART_H)
#define MAX_BULLETS 80
#define MAX_DOGFIGHT_PARTICLES 48
#define MAX_ENEMIES 10
#define MAX_TARGETS 12
#define MAX_EXPLOSIONS 24
#define MAX_BULLET_HOLES 18
#define MAX_ROUNDS 96
#define MAX_SPARKS 160
#define WORLD_SPAN 16
#define SKY_SPAN 32
#define GROUND_METRES_PER_TEXEL 2.0f
#define GROUND_FOG_NEAR 450.0f
#define GROUND_FOG_FAR 9500.0f
#define GROUND_MAX_RANGE 14000.0f
#define SKY_AZIMUTH_OFFSET 1.572f
#define SKY_ROWS_PER_RADIAN 330.0f
#define SUN_AZIMUTH 0.44f
#define SUN_ELEVATION 0.07f
#define WORLD_FOCAL 330.0f
#define AI_ATTACK 0
#define AI_BREAK 1
#define AI_EVADE 2
#define AI_FLEE 3
#define AI_FORMATION 4
#define AI_HOMEWARD 5
#define PLAYER_DURABILITY_MULTIPLIER 8
#define MIDGROUND_ELEMENT_COUNT 6
#define GROUND_MIPS 6
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
    SCENE_EPILOGUE,
    SCENE_AWARD,
    SCENE_SCRAPBOOK
} Scene;

typedef enum {
    MUSIC_MARCH,
    MUSIC_RECON,
    MUSIC_TENSION,
    MUSIC_MOURNING,
    MUSIC_ARMISTICE,
    MUSIC_MESS,
    MUSIC_PIPER,
    MUSIC_RECORD
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
    {"3 SEPTEMBER 1914", "EYES ABOVE THE MARNE",
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
    int ai_state, target, fled;
    float ai_timer, ai_side, smoke_timer;
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
    Vec3 p, v;
    float life, hit_time;
    int owner;   /* 0 player, 1 enemy, 2 wingman */
    int target;  /* enemy rounds: -1 player, 0..2 wingman */
    int will_hit;
    int active;
} WorldRound;

typedef struct {
    Vec3 p, v;
    float life, initial;
    int kind;    /* 0 spark, 1 fabric, 2 smoke */
    int active;
} WorldSpark;

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
    /* One period record at a time: the chapter's, loaded when it is
     * first needed, so a 68040 never holds every record at once. */
    uint8_t *record_data;
    uint32_t record_len;
    uint32_t record_pos;
    int record_track;
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
    SDL_Surface *pilot_cockpit_unarmed; /* 1914: no guns, no ring sight */
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
    SDL_Surface *cockpit_composite;
    SDL_Surface *cockpit_composite_unarmed;
    pix_t *ground_mip[GROUND_MIPS];
    int ground_mips;
    pix_t ground_haze_tint;
    pix_t sky_top;
    pix_t sky_haze;
    pix_t world_haze;
    int half_res_world;
    int realistic_flight;
    char notice[48];
    uint32_t notice_until;
    float shake;
    float gun_recoil;
    SDL_Joystick *joystick;
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
    /* Pilot's record: victories, decorations, rank and growing skills. */
    int victories, sorties, rank;
    unsigned awards;
    int saved_victories, saved_sorties, saved_rank;
    unsigned saved_awards;
    float saved_skill[3];
    int pending_award, pending_rank;
    int career_counted;
    int story_stage;
    int scrap_index;
    int special;           /* MissionSpecial of the sortie being flown */
    int pudding_delivered; /* Christmas 1914 */
    uint32_t reveal_started;
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
    Enemy wingmen[3];
    int wingman_hit_today[3];
    WorldRound rounds[MAX_ROUNDS];
    WorldSpark sparks[MAX_SPARKS];
    uint32_t combat_seed;
    float home_heading;
    int guns_jammed;
    uint32_t jam_clear_until;
    uint32_t heading_home_at;
    float skill_flying;
    float skill_gunnery;
    float skill_engine;
    Target targets[MAX_TARGETS];
    BulletHole bullet_holes[MAX_BULLET_HOLES];
    int next_bullet_hole;
} Game;

static void flight_state_init(FlightState *f, float heading, float pitch,
                              float bank, float speed, float throttle);
static void reset_wingmen(Game *g);
static void show_notice(Game *g, const char *text);
static float wrap_angle(float angle);
static Vec3 flight_rotate_from_ned(const FlightState *f, float north,
                                   float east, float down);

static int is_perspective_mode(MissionMode mode)
{
    return mode == MISSION_DOGFIGHT_3D || mode == MISSION_GUNNER_3D;
}

/* Whether this sortie carries a gun.  1914 is unarmed, and on the
 * Christmas truce nobody fires: the only thing aboard is the pudding.
 * Every firing path, the cockpit art and the gun signs ask this. */
static int mission_armed(const Game *g)
{
    return chapters[g->chapter].armed &&
           campaign_missions[g->chapter][g->mission].special != SPECIAL_TRUCE;
}

/* Wingmen follow the written story rather than a damage simulation.
 * Gridpipe Thyne turns east and is lost in "Thyne's Last Turn" (Chapter 5,
 * Mission 9); from the next sortie onward his place stays empty. */
/* QA entry points (DOGFIGHT, DEATH, CHAPTER_SUCCESS...) write their own
 * save file so testing on a player's install never replaces the campaign. */
static const char *save_path = CANVAS_SAVE_PATH;
static const char *save_new_path = CANVAS_SAVE_NEW_PATH;
static const char *save_backup_path = CANVAS_SAVE_BACKUP_PATH;

static void use_qa_save_paths(void)
{
    save_path = "PROGDIR:CanvasSkies-QA.save";
    save_new_path = "PROGDIR:CanvasSkies-QA.save.new";
    save_backup_path = "PROGDIR:CanvasSkies-QA.save.bak";
}

static void apply_story_wingmen(Game *g)
{
    int pilot;
    for (pilot = 0; pilot < 3; ++pilot)
        g->wingman_health[pilot] = 100;
    if (g->chapter > 4 || (g->chapter == 4 && g->mission > 8))
        g->wingman_health[1] = 0;
}

/* The Royal Air Force was formed on 1 April 1918; before that the pilot
 * belongs to the Royal Flying Corps. */
static int is_raf(const Game *g)
{
    return g->chapter > 5 || (g->chapter == 5 && g->mission >= 2);
}

static const char *corps_name(const Game *g)
{
    return is_raf(g) ? "ROYAL AIR FORCE" : "ROYAL FLYING CORPS";
}

static const char *corps_short(const Game *g)
{
    return is_raf(g) ? "R.A.F." : "R.F.C.";
}

/* The summer influenza of 1918 keeps Bluebottle in bed for a while. */
static int wingman_sick(const Game *g, int pilot)
{
    return pilot == 2 && g->chapter == 5 && g->mission >= 8;
}

/* The day of this sortie: the diary date carried by the latest newspaper
 * of the chapter, or the chapter's own date before the first one. */
static const char *mission_date(const Game *g)
{
    const char *date = chapters[g->chapter].date;
    int m;
    for (m = 0; m <= g->mission; ++m) {
        int n = news_index_for(g->chapter, m);
        if (n < 0) continue;
        if (m == g->mission && news_items[n].diary_date)
            date = news_items[n].diary_date;
        else
            date = news_items[n].date;
    }
    return date;
}

/* Cuttings are pasted into the book once the pilot has reached that day. */
static int news_unlocked(const Game *g, int index)
{
    const NewsItem *n = &news_items[index];
    if (!g->has_save) return 0;
    if (g->saved_campaign_complete) return 1;
    return n->chapter < g->saved_chapter ||
           (n->chapter == g->saved_chapter && n->mission <= g->saved_mission);
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
    int victories = 0, sorties = 0, rank = 0;
    unsigned awards = 0;
    int skill[3] = {0, 0, 0};
    if (!file) return 0;
    if (fscanf(file, "%23s %d\n%d %d\n%d %d %d", magic, &version,
               &chapter, &mission, &wingman0, &wingman1, &wingman2) != 7 ||
        strcmp(magic, "CANVAS_SKIES_SAVE") != 0 ||
        (version < 1 || version > 4) ||
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
    /* Version 4 adds the pilot's record.  Older saves start it at zero. */
    if (version >= 4 &&
        (fscanf(file, "%d %d %u %d %d %d %d", &victories, &sorties, &awards,
                &rank, &skill[0], &skill[1], &skill[2]) != 7 ||
         victories < 0 || sorties < 0 || rank < 0 || rank > 3 ||
         skill[0] < 0 || skill[0] > 100 || skill[1] < 0 || skill[1] > 100 ||
         skill[2] < 0 || skill[2] > 100)) {
        fclose(file);
        return 0;
    }
    fclose(file);
    g->saved_victories = victories;
    g->saved_sorties = sorties;
    g->saved_awards = awards;
    g->saved_rank = rank;
    g->saved_skill[0] = (float)skill[0] / 100.0f;
    g->saved_skill[1] = (float)skill[1] / 100.0f;
    g->saved_skill[2] = (float)skill[2] / 100.0f;
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
    if (load_campaign_save_path(g, save_path)) {
        g->save_loaded_from_backup = 0;
        return 1;
    }
    /* A save is first written under a temporary name and the preceding save
     * is retained as .bak.  If power is lost during replacement, continue
     * from the last complete record instead of silently losing the campaign. */
    if (load_campaign_save_path(g, save_backup_path)) {
        g->save_loaded_from_backup = 1;
        return 1;
    }
    return 0;
}

static int save_campaign(Game *g)
{
    FILE *file = fopen(save_new_path, "w");
    int ok;
    int backed_up;
    if (!file) return 0;
    ok = fprintf(file, "CANVAS_SKIES_SAVE 4\n%d %d\n%d %d %d\n%d\n%d\n"
                 "%d %d %u %d %d %d %d\n",
                 g->chapter, g->mission,
                 g->wingman_health[0], g->wingman_health[1],
                 g->wingman_health[2], g->campaign_complete, g->lives,
                 g->victories, g->sorties, g->awards, g->rank,
                 (int)(g->skill_flying * 100.0f + 0.5f),
                 (int)(g->skill_gunnery * 100.0f + 0.5f),
                 (int)(g->skill_engine * 100.0f + 0.5f)) > 0;
    if (fclose(file) != 0) ok = 0;
    if (!ok) {
        remove(save_new_path);
        return 0;
    }
    if (g->save_loaded_from_backup) {
        /* The primary was already known bad.  Keep the valid backup in place
         * until the replacement has landed instead of rotating corruption
         * over the only recoverable campaign. */
        remove(save_path);
        backed_up = 0;
    } else {
        remove(save_backup_path);
        backed_up = rename(save_path, save_backup_path) == 0;
    }
    if (rename(save_new_path, save_path) != 0) {
        if (backed_up)
            rename(save_backup_path, save_path);
        remove(save_new_path);
        return 0;
    }
    g->saved_chapter = g->chapter;
    g->saved_mission = g->mission;
    g->saved_campaign_complete = g->campaign_complete;
    g->saved_lives = g->lives;
    memcpy(g->saved_wingman_health, g->wingman_health,
           sizeof(g->wingman_health));
    g->saved_victories = g->victories;
    g->saved_sorties = g->sorties;
    g->saved_awards = g->awards;
    g->saved_rank = g->rank;
    g->saved_skill[0] = g->skill_flying;
    g->saved_skill[1] = g->skill_gunnery;
    g->saved_skill[2] = g->skill_engine;
    g->has_save = 1;
    g->save_loaded_from_backup = 0;
    return 1;
}

/* ---- Pilot's record ------------------------------------------------------
 *
 * Every sortie flown makes the pilot a little better: flying sharpens the
 * turn, gunnery tightens the spread and makes jams rarer, and time with the
 * fitters clears jams faster.  Victories bring decorations and promotion. */
typedef struct {
    const char *name;
    const char *citation;
    int victories;
    uint8_t ribbon[3][3];
    int shape; /* 0 leaf, 1 cross, 2 star cross */
} Decoration;

static const Decoration decorations[5] = {
    {"MENTIONED IN DESPATCHES",
     "FOR GALLANT SERVICE IN THE FIELD. HIS NAME WAS READ OUT IN THE MESS "
     "AND HE WAS MADE TO BUY THE FIRST ROUND.", 1,
     {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, 0},
    {"MILITARY CROSS",
     "FOR CONSPICUOUS GALLANTRY IN THE AIR. HE ENGAGED A SUPERIOR ENEMY "
     "FORMATION, BROKE IT UP AND DROVE IT BACK OVER ITS OWN LINES.", 5,
     {{236, 232, 220}, {104, 48, 112}, {236, 232, 220}}, 1},
    {"DISTINGUISHED FLYING CROSS",
     "FOR SKILL AND COURAGE OVER THE LINES. TEN TIMES HE HAS COME HOME WITH "
     "AN ENEMY MACHINE TO HIS NAME AND A HOLED AEROPLANE TO SHOW FOR IT.", 10,
     {{104, 48, 112}, {236, 232, 220}, {104, 48, 112}}, 2},
    {"DISTINGUISHED SERVICE ORDER",
     "FOR LEADERSHIP OF THE HIGHEST ORDER. UNDER HIS COMMAND THE FLIGHT HELD "
     "THE SKY OVER THE ADVANCE AND BROUGHT ITS PILOTS HOME.", 20,
     {{170, 36, 36}, {40, 54, 120}, {170, 36, 36}}, 2},
    {"BAR TO THE MILITARY CROSS",
     "FOR FURTHER ACTS OF GALLANTRY. THIRTY ENEMY MACHINES HAVE FALLEN TO HIS "
     "GUNS. HE SPEAKS OF NONE OF THEM.", 30,
     {{236, 232, 220}, {104, 48, 112}, {236, 232, 220}}, 1}
};

static const char *rank_names[4] = {
    "SECOND LIEUTENANT", "LIEUTENANT", "CAPTAIN", "MAJOR"
};

static const char *rank_short[4] = {"2ND LT.", "LT.", "CAPT.", "MAJOR"};

static int earned_rank(const Game *g)
{
    if (g->victories >= 25 && g->chapter >= 5) return 3;
    if (g->victories >= 12 && g->chapter >= 3) return 2;
    if (g->victories >= 3 || g->sorties >= 10) return 1;
    return 0;
}

static void reset_career(Game *g)
{
    g->victories = g->sorties = g->rank = 0;
    g->awards = 0;
    g->skill_flying = g->skill_gunnery = g->skill_engine = 0.0f;
    g->pending_award = 0;
    g->pending_rank = 0;
}

static void restore_saved_career(Game *g)
{
    g->victories = g->saved_victories;
    g->sorties = g->saved_sorties;
    g->awards = g->saved_awards;
    g->rank = g->saved_rank;
    g->skill_flying = g->saved_skill[0];
    g->skill_gunnery = g->saved_skill[1];
    g->skill_engine = g->saved_skill[2];
    g->pending_award = 0;
    g->pending_rank = 0;
}

static float grow_skill(float skill, float amount)
{
    skill += amount * (1.0f - skill * 0.5f);
    return skill > 1.0f ? 1.0f : skill;
}

static void count_sortie_in_record(Game *g)
{
    int i, rank;
    if (g->career_counted) return;
    g->career_counted = 1;
    ++g->sorties;
    if (is_perspective_mode(campaign_missions[g->chapter][g->mission].mode))
        g->victories += g->kills;
    g->skill_flying = grow_skill(g->skill_flying, g->success ? 0.03f : 0.012f);
    g->skill_gunnery = grow_skill(g->skill_gunnery, 0.006f + 0.018f * g->kills);
    g->skill_engine = grow_skill(g->skill_engine, 0.015f);
    g->pending_award = 0;
    for (i = 0; i < 5; ++i)
        if (!(g->awards & (1u << i)) && g->victories >= decorations[i].victories) {
            g->awards |= 1u << i;
            g->pending_award |= 1 << i;
        }
    rank = earned_rank(g);
    g->pending_rank = rank > g->rank;
    if (rank > g->rank) g->rank = rank;
    if (g->fatal_sortie) {
        /* A pilot who goes down shows no decoration card that evening. */
        g->pending_award = 0;
        g->pending_rank = 0;
    }
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
    count_sortie_in_record(g);
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

/* The new-campaign commissions, any saved campaign, and the cuttings book
 * which is always the last plaque. */
static int campaign_menu_count(const Game *g)
{
    return ((g->has_save && g->saved_lives > 0) ? 7 : 6) + 1;
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

/* Direct pixel packing.  Every surface the renderer touches per pixel is
 * converted to the screen's format at load, so the common case avoids an
 * SDL_GetRGB/SDL_MapRGB call for each pixel. */
static int fast_format(const SDL_PixelFormat *f)
{
    return f->format == CANVAS_SCREEN_FORMAT;
}

static void unpack_rgb(const SDL_PixelFormat *f, uint32_t p,
                       uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (fast_format(f)) {
#if CANVAS_SCREEN_BITS == 16
        unsigned r5 = (p >> 11) & 31u, g6 = (p >> 5) & 63u, b5 = p & 31u;
        *r = (uint8_t)((r5 << 3) | (r5 >> 2));
        *g = (uint8_t)((g6 << 2) | (g6 >> 4));
        *b = (uint8_t)((b5 << 3) | (b5 >> 2));
#else
        *r = (uint8_t)(p >> 16);
        *g = (uint8_t)(p >> 8);
        *b = (uint8_t)p;
#endif
    } else {
        SDL_GetRGB(p, f, r, g, b);
    }
}

static uint32_t pack_rgb(const SDL_PixelFormat *f, uint8_t r, uint8_t g,
                         uint8_t b)
{
    if (fast_format(f))
        return PIX_PACK(r, g, b);
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
    PIX_ROW(s, y)[x] = (pix_t)c;
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

/* Scanline triangle fill: sort the corners by height, walk the long edge
 * against the two short ones and fill whole spans.  The old bounding-box
 * test evaluated three edge functions for every pixel in the box. */
static void fill_triangle(SDL_Surface *s, int x0, int y0, int x1, int y1,
                          int x2, int y2, uint32_t colour)
{
    pix_t c = (pix_t)colour;
    float long_step, top_step, bottom_step;
    int y, y_start, y_end, t;
    if (y1 < y0) { t = y0; y0 = y1; y1 = t; t = x0; x0 = x1; x1 = t; }
    if (y2 < y0) { t = y0; y0 = y2; y2 = t; t = x0; x0 = x2; x2 = t; }
    if (y2 < y1) { t = y1; y1 = y2; y2 = t; t = x1; x1 = x2; x2 = t; }
    if (y2 == y0 || y2 < 0 || y0 >= s->h) return;
    if ((x0 < 0 && x1 < 0 && x2 < 0) ||
        (x0 >= s->w && x1 >= s->w && x2 >= s->w)) return;
    long_step = (float)(x2 - x0) / (float)(y2 - y0);
    top_step = y1 != y0 ? (float)(x1 - x0) / (float)(y1 - y0) : 0.0f;
    bottom_step = y2 != y1 ? (float)(x2 - x1) / (float)(y2 - y1) : 0.0f;
    y_start = y0 < 0 ? 0 : y0;
    y_end = y2 >= s->h ? s->h - 1 : y2;
    for (y = y_start; y <= y_end; ++y) {
        float xa = (float)x0 + long_step * (float)(y - y0);
        float xb = y < y1 ? (float)x0 + top_step * (float)(y - y0)
                          : (float)x1 + bottom_step * (float)(y - y1);
        int left, right;
        pix_t *row;
        if (xa > xb) { float tmp = xa; xa = xb; xb = tmp; }
        if (xb < 0.0f || xa >= (float)s->w) continue;
        left = xa < 0.0f ? 0 : (int)(xa + 0.5f);
        right = xb >= (float)(s->w - 1) ? s->w - 1 : (int)(xb + 0.5f);
        row = PIX_ROW(s, y);
        while (left <= right) row[left++] = c;
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
        pix_t *row;
        if (py < 0) continue;
        row = PIX_ROW(s, py);
        for (px = x; px < x + w && px < s->w; ++px) {
            uint8_t or_, og, ob;
            uint32_t p;
            if (px < 0) continue;
            p = row[px];
            unpack_rgb(s->format, p, &or_, &og, &ob);
            or_ = (uint8_t)((or_ * keep + add_r) / 255);
            og = (uint8_t)((og * keep + add_g) / 255);
            ob = (uint8_t)((ob * keep + add_b) / 255);
            row[px] = (pix_t)pack_rgb(s->format, or_, og, ob);
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
        pix_t *row;
        if (yy < 0 || yy >= s->h) continue;
        row = PIX_ROW(s, yy);
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
            row[xx] = (pix_t)pack_rgb(s->format, or_, og, ob);
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
        } else if (a->music_mood == MUSIC_RECORD &&
                   a->record_data && a->record_len >= 2) {
            uint32_t total_frames = a->record_len / 2;
            uint32_t frame = a->record_pos / 2;
            uint32_t fade_frames = total_frames > 8192 ? 2048 : total_frames / 4;
            int16_t *music = (int16_t *)(a->record_data + a->record_pos);
            float music_sample = (float)music[0] / 32768.0f;
            if (fade_frames > 0 && frame >= total_frames - fade_frames) {
                uint32_t intro_frame = frame - (total_frames - fade_frames);
                int16_t *intro = (int16_t *)(a->record_data + intro_frame * 2);
                float crossfade = (float)intro_frame / (float)fade_frames;
                music_sample = music_sample * (1.0f - crossfade) +
                               (float)intro[0] / 32768.0f * crossfade;
            }
            sample = music_sample * 0.50f;
            a->record_pos += 2;
            if (a->record_pos + 2 > a->record_len) a->record_pos = 0;
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

enum {
    RECORD_NONE = -1,
    RECORD_TIPPERARY,
    RECORD_HOME_FIRES,
    RECORD_KIT_BAG,
    RECORD_HOLYROOD,
    RECORD_VOICE_OF_THE_GUNS,
    RECORD_HOLST,
    RECORD_COUNT
};

static const char *const record_paths[RECORD_COUNT] = {
    CANVAS_TIPPERARY_WAV, CANVAS_HOME_FIRES_WAV, CANVAS_KIT_BAG_WAV,
    CANVAS_HOLYROOD_WAV, CANVAS_VOICE_OF_THE_GUNS_WAV, CANVAS_HOLST_WAV
};

/* 1914 Tipperary, 1915 Keep the Home Fires Burning, 1916 Pack Up Your
 * Troubles, then Holyrood and The Voice of the Guns through 1917 and 1918;
 * the Armistice chapter has Holst's Second Suite. */
static const int chapter_records[CHAPTER_COUNT] = {
    RECORD_TIPPERARY, RECORD_HOME_FIRES, RECORD_KIT_BAG, RECORD_HOLYROOD,
    RECORD_VOICE_OF_THE_GUNS, RECORD_HOLYROOD, RECORD_VOICE_OF_THE_GUNS,
    RECORD_HOLST
};

static int record_for_scene(const Game *g, Scene scene)
{
    if (scene == SCENE_EPILOGUE) return RECORD_HOLST;
    if (scene == SCENE_STORY || scene == SCENE_BRIEFING)
        return chapter_records[g->chapter];
    return RECORD_NONE;
}

static int music_mood_for_scene(const Game *g, Scene scene)
{
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    int record = record_for_scene(g, scene);
    if (record != RECORD_NONE && g->audio.record_data &&
        g->audio.record_track == record)
        return MUSIC_RECORD;
    if (scene == SCENE_TITLE || scene == SCENE_MENU ||
        scene == SCENE_SCRAPBOOK) return MUSIC_MARCH;
    if (scene == SCENE_EPILOGUE) return MUSIC_ARMISTICE;
    if (scene == SCENE_AWARD) return MUSIC_MESS;
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

/* Gridpipe Thyne is lost late in 1917; the evening after, his chair is
 * empty in the mess. */
static int thyne_lost_tonight(const Game *g)
{
    return g->chapter == 4 && g->mission == 9;
}

static uint8_t *load_mono_wav(const char *path, uint32_t *len_out);

/* Loads the scene's record if another is in memory.  The file is read
 * while the previous music keeps playing, then the buffers are swapped
 * under the audio lock and the old record freed, so a scene change never
 * leaves a gap in the sound. */
static void prepare_record(Game *g, Scene scene)
{
    int record = record_for_scene(g, scene);
    uint8_t *data, *old;
    uint32_t len = 0;
    if (record == RECORD_NONE || !g->audio.device ||
        record == g->audio.record_track)
        return;
    data = load_mono_wav(record_paths[record], &len);
    if (!data) {
        fprintf(stderr, "Gramophone record unavailable; using synth: %s\n",
                SDL_GetError());
        return;
    }
    SDL_LockAudioDevice(g->audio.device);
    old = g->audio.record_data;
    g->audio.record_data = data;
    g->audio.record_len = len;
    g->audio.record_pos = 0;
    g->audio.record_track = record;
    SDL_UnlockAudioDevice(g->audio.device);
    if (old) SDL_free(old);
}

static void set_scene(Game *g, Scene scene)
{
    MusicMood previous_mood = (MusicMood)g->audio.music_mood;
    prepare_record(g, scene);
    if (scene == SCENE_STORY)
        g->story_stage = g->mission == 0 ? 0 : thyne_lost_tonight(g) ? 1 :
                         news_index_for(g->chapter, g->mission) >= 0 ? 3 : 2;
    if (scene == SCENE_STORY || scene == SCENE_AWARD)
        g->reveal_started = SDL_GetTicks();
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
    int special = campaign_missions[g->chapter][g->mission].special;
    if (special == SPECIAL_AIRSHIP) return 1;
    if (special == SPECIAL_GOTHA) return 2;
    if (special == SPECIAL_TRUCE) return 0;
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
    if (!mission_armed(g)) return 1;
    if (campaign_missions[g->chapter][g->mission].special == SPECIAL_AIRSHIP)
        return 1;
    if (campaign_missions[g->chapter][g->mission].special == SPECIAL_GOTHA)
        return 3;
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
    g->career_counted = 0;
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
    if (!mission_armed(g)) {
        g->ammunition = 0;
        g->bombs_remaining = 0;
    }
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
        /* They appear as specks one to two kilometres out and close. */
        g->enemies[i].world_x = (float)((i & 1) ? 1 : -1) *
                                (120.0f + (float)i * 95.0f);
        g->enemies[i].world_y = 620.0f + (float)((i * 57) % 180);
        g->enemies[i].world_z = 1150.0f + (float)i * 260.0f;
        g->enemies[i].heading = wrap_angle(atan2f(-g->enemies[i].world_x,
                                                  -g->enemies[i].world_z) +
                                           ((i & 1) ? 0.25f : -0.25f));
        g->enemies[i].ai_state = AI_ATTACK;
        g->enemies[i].ai_timer = 0.0f;
        g->enemies[i].ai_side = (i & 1) ? 1.0f : -1.0f;
        g->enemies[i].target = -1;
        g->enemies[i].fled = 0;
        g->enemies[i].smoke_timer = 0.0f;
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
    memset(g->rounds, 0, sizeof(g->rounds));
    memset(g->sparks, 0, sizeof(g->sparks));
    g->combat_seed ^= SDL_GetTicks() * 2654435761u + 12345u;
    g->home_heading = (float)M_PI;
    g->guns_jammed = 0;
    g->jam_clear_until = 0;
    g->heading_home_at = 0;
    g->notice_until = 0;
    g->shake = 0.0f;
    g->gun_recoil = 0.0f;
    reset_wingmen(g);
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
    g->special = campaign_missions[g->chapter][g->mission].special;
    g->pudding_delivered = 0;
    if (g->special == SPECIAL_TRUCE) {
        /* Two gatherings in no man's land, then their aerodrome. */
        for (i = 0; i < MAX_TARGETS; ++i)
            g->targets[i].type = i < 2 ? 3 : 4;
        g->bombs_remaining = 1;
        g->ammunition = 0;
    } else if (g->special == SPECIAL_AIRSHIP ||
               g->special == SPECIAL_GOTHA) {
        int airship = g->special == SPECIAL_AIRSHIP;
        for (i = 0; i < MAX_ENEMIES; ++i) {
            Enemy *e = &g->enemies[i];
            if (!e->active) continue;
            /* They hold their course for the city and do not dogfight. */
            e->world_x = airship ? -700.0f : -520.0f + (float)i * 70.0f;
            e->world_y = airship ? 820.0f : 700.0f + (float)i * 12.0f;
            e->world_z = airship ? 1300.0f : 900.0f + (float)(i & 1) * 60.0f;
            e->heading = (float)M_PI * 0.5f;
            e->speed = airship ? 40.0f : 70.0f;
            e->throttle = 0.6f;
            e->hp = airship ? 14 : 7;
            e->max_hp = e->hp;
            e->ai_state = AI_FORMATION;
            flight_state_init(&e->flight, e->heading, 0.0f, 0.0f,
                              e->speed * 0.44704f, e->throttle);
        }
        snprintf(g->notice, sizeof(g->notice), "%s",
                 airship ? "NIGHT OVER ESSEX" : "OVER THE THAMES");
        g->notice_until = SDL_GetTicks() + 4000u;
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

/* ---- Arcade flight ------------------------------------------------------
 *
 * The default handling flies like an arcade biplane: the stick rolls and
 * pitches the aircraft, turns are coordinated for you, speed comes from the
 * throttle and is traded with height in climbs and dives, and a slow
 * aircraft drops its nose.  F8 switches to the full six-axis model. */
static void arcade_flight_step(FlightState *f, Vec3 *position, float dt,
                               float aileron, float elevator, float agility)
{
    float bank, pitch, heading, speed = f->u, turn;
    float target_speed, accel;
    Vec3 velocity;
    float dw, dx, dy, dz, norm;
    flight_euler(f, &bank, &pitch, &heading);
    /* Roll: proportional, with a gentle self-levelling when centred. */
    if (fabsf(aileron) > 0.05f)
        f->p = flight_approach(f->p, aileron * 1.45f * agility, 6.0f * dt);
    else
        f->p = flight_approach(f->p,
                               fabsf(bank) < 1.2f ? -bank * 0.7f : 0.0f,
                               6.0f * dt);
    /* Pitch authority fades as the aircraft slows. */
    f->q = flight_approach(f->q, elevator * 1.05f * agility *
                           flight_clamp(speed / 44.0f, 0.3f, 1.15f),
                           5.0f * dt);
    /* Coordinated turn: bank turns the nose round the horizon. */
    turn = 9.81f * sinf(bank) / (speed > 15.0f ? speed : 15.0f);
    f->r = turn * cosf(bank) * cosf(pitch);
    {
        float q = f->q + turn * sinf(bank) * cosf(pitch);
        float stall_drop = speed < 27.0f ? (27.0f - speed) * 0.06f : 0.0f;
        /* A slow aircraft's nose falls toward the ground, whichever way up. */
        q -= stall_drop * cosf(bank);
        f->r += stall_drop * sinf(bank);
        dw = -0.5f * (f->qx * f->p + f->qy * q + f->qz * f->r);
        dx =  0.5f * (f->qw * f->p + f->qy * f->r - f->qz * q);
        dy =  0.5f * (f->qw * q + f->qz * f->p - f->qx * f->r);
        dz =  0.5f * (f->qw * f->r + f->qx * q - f->qy * f->p);
    }
    f->qw += dw * dt; f->qx += dx * dt; f->qy += dy * dt; f->qz += dz * dt;
    norm = sqrtf(f->qw * f->qw + f->qx * f->qx + f->qy * f->qy + f->qz * f->qz);
    if (norm > 0.0001f) {
        f->qw /= norm; f->qx /= norm; f->qy /= norm; f->qz /= norm;
    }
    /* Speed: throttle sets the level speed, gravity trades it with height. */
    target_speed = 30.0f + f->throttle * 30.0f;
    accel = (target_speed - speed) * 0.32f - 9.81f * sinf(pitch);
    speed += accel * dt;
    if (speed < 14.0f) speed = 14.0f;
    if (speed > 82.0f) speed = 82.0f;
    f->u = speed; f->v = 0.0f; f->w = 0.0f;
    velocity = flight_rotate_to_ned(f, speed, 0.0f, 0.0f);
    position->x += velocity.y * dt;
    position->y -= velocity.z * dt;
    position->z += velocity.x * dt;
    f->speed = speed;
    f->alpha = 0.0f;
    f->beta = 0.0f;
    f->stalled = speed < 24.0f;
}

static void arcade_flight_advance(FlightState *f, Vec3 *position,
                                  float frame_dt, float aileron,
                                  float elevator, float agility)
{
    const float step = 0.02f;
    int iterations = 0;
    if (frame_dt > 0.10f) frame_dt = 0.10f;
    f->accumulator += frame_dt;
    while (f->accumulator >= step && iterations < 5) {
        arcade_flight_step(f, position, step, aileron, elevator, agility);
        f->accumulator -= step;
        ++iterations;
    }
}

typedef struct {
    Vec3 pos, right, up, forward;
} AircraftPose;

/* Pose from a flight quaternion: body x forward, y right, z down in NED,
 * converted to the renderer's x east, y up, z north. */
static AircraftPose pose_from_flight(const FlightState *f, Vec3 pos)
{
    AircraftPose p;
    Vec3 fwd = flight_rotate_to_ned(f, 1.0f, 0.0f, 0.0f);
    Vec3 rt = flight_rotate_to_ned(f, 0.0f, 1.0f, 0.0f);
    Vec3 dn = flight_rotate_to_ned(f, 0.0f, 0.0f, 1.0f);
    p.pos = pos;
    p.forward = (Vec3){fwd.y, -fwd.z, fwd.x};
    p.right = (Vec3){rt.y, -rt.z, rt.x};
    p.up = (Vec3){-dn.y, dn.z, -dn.x};
    return p;
}

static Vec3 pose_point(const AircraftPose *p, float x, float y, float z)
{
    Vec3 out;
    out.x = p->pos.x + p->right.x * x + p->up.x * y + p->forward.x * z;
    out.y = p->pos.y + p->right.y * x + p->up.y * y + p->forward.y * z;
    out.z = p->pos.z + p->right.z * x + p->up.z * y + p->forward.z * z;
    return out;
}

/* ---- Joystick -----------------------------------------------------------
 *
 * The first joystick or pad flies the aircraft: the stick maps onto the
 * arrow keys and the first button onto the guns (and Enter in menus). */
static void open_joystick(Game *g)
{
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0) return;
    if (SDL_NumJoysticks() > 0) g->joystick = SDL_JoystickOpen(0);
}

static void merge_joystick_keys(Game *g, uint8_t *keys)
{
    int x, y;
    if (!g->joystick) return;
    x = SDL_JoystickGetAxis(g->joystick, 0);
    y = SDL_JoystickGetAxis(g->joystick, 1);
    if (x < -9000) keys[SDL_SCANCODE_LEFT] = 1;
    if (x > 9000) keys[SDL_SCANCODE_RIGHT] = 1;
    if (y < -9000) keys[SDL_SCANCODE_UP] = 1;
    if (y > 9000) keys[SDL_SCANCODE_DOWN] = 1;
    if (SDL_JoystickNumHats(g->joystick) > 0) {
        uint8_t hat = SDL_JoystickGetHat(g->joystick, 0);
        if (hat & SDL_HAT_LEFT) keys[SDL_SCANCODE_LEFT] = 1;
        if (hat & SDL_HAT_RIGHT) keys[SDL_SCANCODE_RIGHT] = 1;
        if (hat & SDL_HAT_UP) keys[SDL_SCANCODE_UP] = 1;
        if (hat & SDL_HAT_DOWN) keys[SDL_SCANCODE_DOWN] = 1;
    }
    if (SDL_JoystickGetButton(g->joystick, 0)) keys[SDL_SCANCODE_SPACE] = 1;
    if (SDL_JoystickNumButtons(g->joystick) > 1 &&
        SDL_JoystickGetButton(g->joystick, 1)) keys[SDL_SCANCODE_X] = 1;
    if (SDL_JoystickNumButtons(g->joystick) > 2 &&
        SDL_JoystickGetButton(g->joystick, 2)) keys[SDL_SCANCODE_Z] = 1;
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
    if (mission_armed(g) && g->ammunition > 0 &&
        keys[SDL_SCANCODE_SPACE] &&
        now - g->last_shot >= 115u) {
        if (new_bullet(g, g->player_x, g->player_y - 28.0f,
                       0.0f, -430.0f, 0, 0, 1.35f)) {
            g->last_shot = now;
            --g->ammunition;
            g->audio.shot = 1500;
        }
    }
    if ((mission_armed(g) || g->special == SPECIAL_TRUCE) &&
        g->bombs_remaining > 0 && keys[SDL_SCANCODE_B] &&
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
            if (g->special != SPECIAL_TRUCE) {
                new_ground_explosion(g, b->x, b->y, 2);
                g->audio.blast = 7000;
            }
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
            if (target->type < 2 && g->special != SPECIAL_TRUCE &&
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
                    if (target->type == 3) continue;
                    if (target->type == 4) {
                        /* Christmas pudding, compliments of the mess. */
                        if (b->type == 1 && !g->pudding_delivered) {
                            g->pudding_delivered = 1;
                            g->score += 1000;
                            b->active = 0;
                        }
                        continue;
                    }
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
    int firing = keys[SDL_SCANCODE_SPACE] && mission_armed(g) &&
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
        if (mission_armed(g) &&
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

/* ---- Air combat ---------------------------------------------------------
 *
 * Rounds are real objects in the world: they leave the guns at muzzle
 * speed plus the aircraft's own, converge on the sight line and hit what
 * they pass through.  Enemy pilots attack, break away, defend when we are
 * on their tail and run for home when badly hit.  The flight joins the
 * fight once the enemy is close. */
#define ROUND_SPEED 380.0f
#define ENEMY_HIT_RADIUS 6.5f

static float random_unit(Game *g)
{
    g->combat_seed = g->combat_seed * 1664525u + 1013904223u;
    return (float)(g->combat_seed >> 8) / 16777216.0f;
}

static Vec3 vec3_sub(Vec3 a, Vec3 b)
{
    return (Vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static float vec3_length(Vec3 a)
{
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

static Vec3 aircraft_velocity(const FlightState *f)
{
    Vec3 v = flight_rotate_to_ned(f, f->u, f->v, f->w);
    return (Vec3){v.y, -v.z, v.x};
}

static WorldRound *new_round(Game *g, Vec3 from, Vec3 velocity, int owner,
                             float life)
{
    int i;
    for (i = 0; i < MAX_ROUNDS; ++i) if (!g->rounds[i].active) {
        WorldRound *r = &g->rounds[i];
        memset(r, 0, sizeof(*r));
        r->p = from;
        r->v = velocity;
        r->life = life;
        r->owner = owner;
        r->target = -1;
        r->active = 1;
        return r;
    }
    return NULL;
}

static void new_spark(Game *g, Vec3 at, Vec3 velocity, int kind, float life)
{
    int i, oldest = 0;
    for (i = 0; i < MAX_SPARKS; ++i) {
        if (!g->sparks[i].active) { oldest = i; break; }
        if (g->sparks[i].life < g->sparks[oldest].life) oldest = i;
    }
    g->sparks[oldest].p = at;
    g->sparks[oldest].v = velocity;
    g->sparks[oldest].kind = kind;
    g->sparks[oldest].life = life;
    g->sparks[oldest].initial = life;
    g->sparks[oldest].active = 1;
}

static void burst_of_debris(Game *g, Vec3 at, Vec3 carry, int fabric)
{
    int k;
    for (k = 0; k < 5; ++k) {
        Vec3 v = {carry.x * 0.6f + (random_unit(g) - 0.5f) * 30.0f,
                  carry.y * 0.6f + (random_unit(g) - 0.3f) * 24.0f,
                  carry.z * 0.6f + (random_unit(g) - 0.5f) * 30.0f};
        new_spark(g, at, v, k < 3 || !fabric ? 0 : 1,
                  k < 3 ? 0.35f : 1.6f);
    }
}

/* Two synchronised guns either side of the sight, converging on the ring
 * sight line 150 m out. */
static void fire_player_guns(Game *g)
{
    Vec3 me = {g->flight_x, g->flight_y, g->flight_z};
    AircraftPose pose = pose_from_flight(&g->flight, me);
    Vec3 carry = aircraft_velocity(&g->flight);
    Vec3 aim = pose_point(&pose, 0.0f, -150.0f * 20.0f / WORLD_FOCAL, 150.0f);
    float spread = 0.012f - g->skill_gunnery * 0.008f;
    int gun;
    for (gun = 0; gun < 2; ++gun) {
        Vec3 from = pose_point(&pose, gun ? 0.42f : -0.42f, -0.35f, 2.2f);
        Vec3 dir = vec3_sub(aim, from);
        float len = vec3_length(dir);
        WorldRound *r;
        dir.x /= len; dir.y /= len; dir.z /= len;
        dir.x += (random_unit(g) - 0.5f) * spread;
        dir.y += (random_unit(g) - 0.5f) * spread;
        dir.z += (random_unit(g) - 0.5f) * spread;
        r = new_round(g, from, (Vec3){carry.x + dir.x * ROUND_SPEED,
                                      carry.y + dir.y * ROUND_SPEED,
                                      carry.z + dir.z * ROUND_SPEED}, 0, 1.6f);
        (void)r;
    }
}

static float segment_sphere_distance(Vec3 a, Vec3 b, Vec3 c)
{
    Vec3 ab = vec3_sub(b, a), ac = vec3_sub(c, a);
    float ab2 = ab.x * ab.x + ab.y * ab.y + ab.z * ab.z;
    float t = ab2 > 1e-6f ? (ac.x * ab.x + ac.y * ab.y + ac.z * ab.z) / ab2
                          : 0.0f;
    Vec3 closest;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    closest = (Vec3){a.x + ab.x * t, a.y + ab.y * t, a.z + ab.z * t};
    return vec3_length(vec3_sub(c, closest));
}

static void damage_enemy(Game *g, Enemy *enemy, Vec3 at, int by_player)
{
    Vec3 carry = aircraft_velocity(&enemy->flight);
    if (!by_player && enemy->hp <= 1) return;
    --enemy->hp;
    burst_of_debris(g, at, carry, 1);
    if (by_player) {
        g->audio.hit_player = 0;
        g->audio.hit = 950;
    }
    if (enemy->hp <= 0) {
        enemy->crashing = 1;
        if (by_player) credit_air_kill(g);
    }
}

/* The aircraft we are hit by: player (-1) or a wingman (0..2). */
static void enemy_round_arrives(Game *g, WorldRound *r)
{
    if (r->target < 0) {
        if (g->flight_doomed || g->flight_crashed) return;
        if (!g->qa_hold || g->health > 30) {
            g->health -= player_machine_gun_damage(g);
            add_dogfight_bullet_hole(g, SDL_GetTicks() ^ (uint32_t)(r - g->rounds));
        }
        g->audio.hit_player = 1;
        g->audio.hit = 1400;
        g->last_damage = SDL_GetTicks();
        g->shake = 1.0f;
    } else if (r->target < 3 && g->wingmen[r->target].active) {
        Enemy *w = &g->wingmen[r->target];
        burst_of_debris(g, r->p, aircraft_velocity(&w->flight), 1);
        if (--w->hp <= 0 && w->ai_state != AI_HOMEWARD) {
            /* Wounded, not killed: he turns for home trailing smoke. */
            w->ai_state = AI_HOMEWARD;
            w->ai_timer = 22.0f;
            g->wingman_hit_today[r->target] = 1;
            snprintf(g->notice, sizeof(g->notice), "%s IS HIT",
                     allied_pilot_names[r->target]);
            g->notice_until = SDL_GetTicks() + 2200u;
        }
    }
}

static void update_rounds(Game *g, float dt)
{
    int i, e;
    for (i = 0; i < MAX_ROUNDS; ++i) {
        WorldRound *r = &g->rounds[i];
        Vec3 next;
        if (!r->active) continue;
        r->life -= dt;
        next = (Vec3){r->p.x + r->v.x * dt, r->p.y + r->v.y * dt,
                      r->p.z + r->v.z * dt};
        next.y -= 4.9f * dt * dt;
        r->v.y -= 9.81f * dt;
        if (r->owner == 0 || r->owner == 2) {
            for (e = 0; e < MAX_ENEMIES; ++e) {
                Enemy *enemy = &g->enemies[e];
                Vec3 c;
                if (!enemy->active || enemy->crashing) continue;
                c = (Vec3){enemy->world_x, enemy->world_y, enemy->world_z};
                if (g->special == SPECIAL_AIRSHIP) {
                    /* A long hull: test three points along it. */
                    float hx = sinf(enemy->heading) * 24.0f;
                    float hz = cosf(enemy->heading) * 24.0f;
                    int k, struck = 0;
                    for (k = -1; k <= 1 && !struck; ++k) {
                        Vec3 at = {c.x + hx * (float)k, c.y, c.z + hz * (float)k};
                        if (segment_sphere_distance(r->p, next, at) < 11.0f)
                            struck = 1;
                    }
                    if (struck) {
                        damage_enemy(g, enemy, c, r->owner == 0);
                        r->active = 0;
                        break;
                    }
                    continue;
                }
                if (segment_sphere_distance(r->p, next, c) <
                    (g->special == SPECIAL_GOTHA ? ENEMY_HIT_RADIUS * 1.6f :
                                                   ENEMY_HIT_RADIUS)) {
                    damage_enemy(g, enemy, c, r->owner == 0);
                    r->active = 0;
                    break;
                }
            }
        } else if (r->will_hit) {
            r->hit_time -= dt;
            if (r->hit_time <= 0.0f) {
                enemy_round_arrives(g, r);
                r->active = 0;
            }
        }
        r->p = next;
        if (r->life <= 0.0f || r->p.y < 0.0f) r->active = 0;
    }
    for (i = 0; i < MAX_SPARKS; ++i) {
        WorldSpark *sp = &g->sparks[i];
        if (!sp->active) continue;
        sp->life -= dt;
        if (sp->life <= 0.0f) { sp->active = 0; continue; }
        sp->p.x += sp->v.x * dt;
        sp->p.y += sp->v.y * dt;
        sp->p.z += sp->v.z * dt;
        if (sp->kind == 2) {
            sp->v.x *= 1.0f - dt * 0.8f;
            sp->v.z *= 1.0f - dt * 0.8f;
            sp->v.y += 1.6f * dt;
        } else {
            sp->v.y -= (sp->kind == 1 ? 3.0f : 9.81f) * dt;
        }
    }
}

/* Enemy guns: a burst has a chance to strike that depends on range, the
 * pilot's skill and how hard we are manoeuvring.  The tracers fly at the
 * target (or wide of it) so what you see matches what happened. */
static void enemy_burst(Game *g, Enemy *enemy, Vec3 target, Vec3 target_v,
                        int target_index, float hit_chance)
{
    Vec3 from = {enemy->world_x, enemy->world_y, enemy->world_z};
    AircraftPose pose = pose_from_flight(&enemy->flight, from);
    Vec3 carry = aircraft_velocity(&enemy->flight);
    float distance = vec3_length(vec3_sub(target, from));
    float flight_time = distance / ROUND_SPEED;
    int hit = random_unit(g) < hit_chance;
    int k;
    Vec3 aim = {target.x + target_v.x * flight_time,
                target.y + target_v.y * flight_time,
                target.z + target_v.z * flight_time};
    for (k = 0; k < 3; ++k) {
        Vec3 aim_k = aim, gun = pose_point(&pose, k & 1 ? 0.4f : -0.4f,
                                           0.7f, 4.6f);
        Vec3 dir;
        float len;
        WorldRound *r;
        if (!hit || k > 0) {
            float miss = hit ? 2.0f : 7.0f + random_unit(g) * 9.0f;
            aim_k.x += (random_unit(g) - 0.5f) * 2.0f * miss;
            aim_k.y += (random_unit(g) - 0.5f) * 2.0f * miss;
            aim_k.z += (random_unit(g) - 0.5f) * 2.0f * miss;
        }
        dir = vec3_sub(aim_k, gun);
        len = vec3_length(dir);
        if (len < 1.0f) len = 1.0f;
        r = new_round(g, gun, (Vec3){carry.x * 0.2f + dir.x / len * ROUND_SPEED,
                                     carry.y * 0.2f + dir.y / len * ROUND_SPEED,
                                     carry.z * 0.2f + dir.z / len * ROUND_SPEED},
                      1, len / ROUND_SPEED + 0.6f);
        if (r && hit && k == 0) {
            r->will_hit = 1;
            r->hit_time = len / ROUND_SPEED;
            r->target = target_index;
        }
    }
    enemy->last_shot = SDL_GetTicks();
}

/* Shared autopilot: steer toward a heading and pitch with a bank limit. */
static void ai_fly(Enemy *a, float dt, float desired_heading,
                   float desired_pitch, float max_bank)
{
    Vec3 position = {a->world_x, a->world_y, a->world_z};
    float heading_error = wrap_angle(desired_heading - a->heading);
    float desired_bank = flight_clamp(heading_error * 1.3f, -max_bank,
                                      max_bank);
    float aileron = flight_clamp((desired_bank - a->bank) * 2.2f -
                                 a->flight.p * 0.32f, -1.0f, 1.0f);
    float elevator = flight_clamp((desired_pitch - a->pitch) * 2.4f -
                                  a->flight.q * 0.28f +
                                  fabsf(a->bank) * 0.55f, -1.0f, 1.0f);
    a->flight.throttle = a->throttle;
    ww1_flight_advance(&a->flight, &position, dt, aileron, elevator);
    a->world_x = position.x;
    a->world_y = position.y;
    a->world_z = position.z;
    flight_outputs(&a->flight, &a->heading, &a->pitch, &a->bank,
                   &a->vx, &a->vy, &a->vz, &a->speed);
    a->roll_rate = a->flight.p;
    a->pitch_rate = a->flight.q;
}

static void lead_pursuit(const Enemy *a, Vec3 target, Vec3 target_v,
                         float *heading, float *pitch, float *distance)
{
    Vec3 me = {a->world_x, a->world_y, a->world_z};
    float d = vec3_length(vec3_sub(target, me));
    float t = d / ROUND_SPEED;
    Vec3 lead = {target.x + target_v.x * t - me.x,
                 target.y + target_v.y * t - me.y,
                 target.z + target_v.z * t - me.z};
    float horizontal = sqrtf(lead.x * lead.x + lead.z * lead.z);
    *heading = atan2f(lead.x, lead.z);
    *pitch = flight_clamp(atan2f(lead.y, horizontal > 1.0f ? horizontal : 1.0f),
                          -0.45f, 0.45f);
    *distance = d;
}

/* Angle between a's nose and the direction to point p. */
static float off_nose(const Enemy *a, Vec3 p)
{
    Vec3 me = {a->world_x, a->world_y, a->world_z};
    Vec3 fwd = aircraft_velocity(&a->flight);
    Vec3 to = vec3_sub(p, me);
    float lf = vec3_length(fwd), lt = vec3_length(to);
    float c;
    if (lf < 1e-3f || lt < 1e-3f) return 0.0f;
    c = (fwd.x * to.x + fwd.y * to.y + fwd.z * to.z) / (lf * lt);
    return acosf(flight_clamp(c, -1.0f, 1.0f));
}

static int active_wingman_count(const Game *g)
{
    int i, n = 0;
    for (i = 0; i < 3; ++i)
        if (g->wingmen[i].active && g->wingmen[i].ai_state != AI_HOMEWARD) ++n;
    return n;
}

static void reset_wingmen(Game *g)
{
    static const float offset[3][3] = {
        {-34.0f, 2.0f, 30.0f}, {40.0f, -3.0f, 45.0f}, {-72.0f, 6.0f, 60.0f}
    };
    Vec3 me = {g->flight_x, g->flight_y, g->flight_z};
    AircraftPose pose = pose_from_flight(&g->flight, me);
    int i;
    memset(g->wingmen, 0, sizeof(g->wingmen));
    memset(g->wingman_hit_today, 0, sizeof(g->wingman_hit_today));
    if (!mission_armed(g) ||
        campaign_missions[g->chapter][g->mission].mode != MISSION_DOGFIGHT_3D)
        return;
    for (i = 0; i < 3; ++i) {
        Enemy *w = &g->wingmen[i];
        Vec3 at;
        if (g->wingman_health[i] <= 0 || wingman_sick(g, i)) continue;
        at = pose_point(&pose, offset[i][0], offset[i][1], offset[i][2]);
        w->world_x = at.x; w->world_y = at.y; w->world_z = at.z;
        w->heading = g->flight_heading;
        w->speed = 98.0f;
        w->throttle = 0.7f;
        flight_state_init(&w->flight, w->heading, 0.0f, 0.0f, 43.8f, 0.7f);
        w->hp = 3;
        w->max_hp = 3;
        w->active = 1;
        w->ai_state = AI_FORMATION;
        w->ai_timer = 7.0f + (float)i * 1.5f;
        w->target = -1;
        w->last_shot = SDL_GetTicks() + 2000u;
    }
}

static void update_wingmen(Game *g, float dt)
{
    static const float slot[3][2] = {{-34.0f, 30.0f}, {40.0f, 45.0f},
                                     {-72.0f, 60.0f}};
    uint32_t now = SDL_GetTicks();
    Vec3 me = {g->flight_x, g->flight_y, g->flight_z};
    AircraftPose my_pose = pose_from_flight(&g->flight, me);
    int i, e;
    for (i = 0; i < 3; ++i) {
        Enemy *w = &g->wingmen[i];
        Vec3 pos;
        float heading = w->heading, pitch = 0.0f, distance = 0.0f;
        float max_bank = 0.85f;
        if (!w->active) continue;
        pos = (Vec3){w->world_x, w->world_y, w->world_z};
        w->ai_timer -= dt;
        if (w->ai_state == AI_HOMEWARD) {
            heading = g->home_heading;
            pitch = 0.05f;
            w->throttle = 0.55f;
            if (w->ai_timer <= 0.0f) w->active = 0;
            if (((now / 90u) + (uint32_t)i) % 3u == 0u)
                new_spark(g, pos, (Vec3){0.0f, 2.0f, 0.0f}, 2, 2.6f);
        } else if (w->ai_state == AI_FORMATION) {
            Vec3 want = pose_point(&my_pose, slot[i][0], 0.0f, slot[i][1]);
            Vec3 to = vec3_sub(want, pos);
            float along = to.x * my_pose.forward.x + to.z * my_pose.forward.z;
            heading = atan2f(to.x + my_pose.forward.x * 60.0f,
                             to.z + my_pose.forward.z * 60.0f);
            pitch = flight_clamp(to.y * 0.01f, -0.2f, 0.2f);
            w->throttle = flight_clamp(g->flight_throttle + along * 0.01f,
                                       0.2f, 1.0f);
            /* Break formation once the enemy is close or time is up. */
            for (e = 0; e < MAX_ENEMIES; ++e) {
                Enemy *enemy = &g->enemies[e];
                if (!enemy->active || enemy->crashing) continue;
                if (vec3_length(vec3_sub((Vec3){enemy->world_x,
                    enemy->world_y, enemy->world_z}, pos)) < 900.0f)
                    w->ai_timer = 0.0f;
            }
            if (w->ai_timer <= 0.0f) w->ai_state = AI_ATTACK;
        } else {
            Enemy *target = NULL;
            float best = 1e9f;
            if (w->target >= 0 && w->target < MAX_ENEMIES &&
                g->enemies[w->target].active &&
                !g->enemies[w->target].crashing)
                target = &g->enemies[w->target];
            else {
                w->target = -1;
                for (e = 0; e < MAX_ENEMIES; ++e) {
                    Enemy *enemy = &g->enemies[e];
                    float d;
                    if (!enemy->active || enemy->crashing) continue;
                    d = vec3_length(vec3_sub((Vec3){enemy->world_x,
                        enemy->world_y, enemy->world_z}, pos)) +
                        (float)((e + i) % 3) * 150.0f;
                    if (d < best) { best = d; w->target = e; }
                }
                if (w->target >= 0) target = &g->enemies[w->target];
            }
            if (target) {
                Vec3 tp = {target->world_x, target->world_y, target->world_z};
                lead_pursuit(w, tp, aircraft_velocity(&target->flight),
                             &heading, &pitch, &distance);
                w->throttle = 0.85f;
                if (distance < 120.0f) heading += (i & 1) ? 0.9f : -0.9f;
                if (distance < 420.0f && off_nose(w, tp) < 0.12f &&
                    now - w->last_shot > 900u) {
                    enemy_burst(g, w, tp, aircraft_velocity(&target->flight),
                                -2, 0.0f);
                    /* Wingmen harry the enemy but leave the kill to you. */
                    if (random_unit(g) < 0.22f)
                        damage_enemy(g, target, tp, 0);
                }
            } else {
                /* Sky clear: rejoin. */
                w->ai_state = AI_FORMATION;
                w->ai_timer = 999.0f;
            }
        }
        if (w->world_y < 120.0f) pitch = 0.3f;
        ai_fly(w, dt, heading, pitch, max_bank);
        if (w->world_y < 1.0f) w->active = 0;
    }
}

static void choose_enemy_target(Game *g, Enemy *enemy)
{
    int i, pick = -1, n = active_wingman_count(g);
    enemy->target = -1;
    if (n > 0 && random_unit(g) < 0.3f) {
        int skip = (int)(random_unit(g) * (float)n);
        for (i = 0; i < 3; ++i)
            if (g->wingmen[i].active && g->wingmen[i].ai_state != AI_HOMEWARD &&
                skip-- == 0) { pick = i; break; }
        enemy->target = pick;
    }
}

/* One enemy pilot's decision for this frame. */
static void enemy_think(Game *g, Enemy *enemy, int index, float dt,
                        float *heading, float *pitch, float *max_bank,
                        float *distance_to_player)
{
    Vec3 me = {enemy->world_x, enemy->world_y, enemy->world_z};
    Vec3 player = {g->flight_x, g->flight_y, g->flight_z};
    Vec3 target = player, target_v = aircraft_velocity(&g->flight);
    int ace = index == 0 && g->chapter >= 3;
    float skill = flight_clamp(0.25f + (float)g->chapter * 0.1f +
                               (ace ? 0.3f : 0.0f), 0.0f, 1.0f);
    float player_distance = vec3_length(vec3_sub(player, me));
    float distance;
    *max_bank = flight_clamp(0.7f + skill * 0.5f, 0.7f, 1.25f);
    *distance_to_player = player_distance;
    enemy->ai_timer -= dt;
    if (g->special == SPECIAL_AIRSHIP || g->special == SPECIAL_GOTHA) {
        /* Raiders hold their course for London whatever happens. */
        *heading = (float)M_PI * 0.5f +
                   (g->special == SPECIAL_GOTHA ?
                    sinf(g->scroll * 0.001f + enemy->phase) * 0.06f : 0.0f);
        *pitch = 0.0f;
        *max_bank = 0.3f;
        return;
    }
    if (enemy->target >= 0 && (enemy->target > 2 ||
        !g->wingmen[enemy->target].active ||
        g->wingmen[enemy->target].ai_state == AI_HOMEWARD))
        enemy->target = -1;
    if (enemy->target >= 0) {
        Enemy *w = &g->wingmen[enemy->target];
        target = (Vec3){w->world_x, w->world_y, w->world_z};
        target_v = aircraft_velocity(&w->flight);
    }
    /* Badly hit: run, once. */
    if (enemy->hp == 1 && enemy->max_hp > 1 && !enemy->fled &&
        enemy->ai_state != AI_FLEE) {
        enemy->ai_state = AI_FLEE;
        enemy->ai_timer = 5.0f;
        enemy->fled = 1;
    }
    /* We are on its tail: defend. */
    if (enemy->ai_state == AI_ATTACK && player_distance < 450.0f &&
        off_nose(enemy, player) > 2.3f && random_unit(g) < skill * dt * 3.0f) {
        enemy->ai_state = AI_EVADE;
        enemy->ai_timer = 2.5f + skill * 1.5f;
        enemy->ai_side = random_unit(g) < 0.5f ? -1.0f : 1.0f;
    }
    switch (enemy->ai_state) {
    case AI_BREAK:
        *heading = enemy->heading + enemy->ai_side * 1.3f;
        *pitch = (index & 1) ? 0.28f : -0.18f;
        if (enemy->ai_timer <= 0.0f) {
            enemy->ai_state = AI_ATTACK;
            choose_enemy_target(g, enemy);
        }
        break;
    case AI_EVADE:
        if (((int)(enemy->ai_timer / 1.4f)) & 1) enemy->ai_side = -enemy->ai_side;
        *heading = enemy->heading + enemy->ai_side * 1.5f;
        *pitch = (enemy->ai_timer > 1.5f) ? -0.2f : 0.25f;
        if (enemy->ai_timer <= 0.0f) enemy->ai_state = AI_ATTACK;
        break;
    case AI_FLEE:
        *heading = atan2f(me.x - player.x, me.z - player.z);
        *pitch = -0.22f;
        enemy->throttle = 1.0f;
        if (enemy->ai_timer <= 0.0f) {
            enemy->ai_state = AI_ATTACK;
            enemy->throttle = 0.75f;
        }
        break;
    default:
        lead_pursuit(enemy, target, target_v, heading, pitch, &distance);
        *heading += sinf(g->scroll * 0.0021f + enemy->phase) * 0.08f *
                    (1.0f - skill);
        if (distance < 190.0f + (1.0f - skill) * 110.0f) {
            enemy->ai_state = AI_BREAK;
            enemy->ai_timer = 2.0f + random_unit(g) * 1.5f;
            enemy->ai_side = (index & 1) ? 1.0f : -1.0f;
        }
        break;
    }
    if (enemy->world_y < 150.0f) *pitch = 0.3f;
}

/* Fires when the attacker's nose is on its target. */
static void enemy_maybe_fire(Game *g, Enemy *enemy, int index)
{
    uint32_t now = SDL_GetTicks();
    Vec3 me = {enemy->world_x, enemy->world_y, enemy->world_z};
    Vec3 target = {g->flight_x, g->flight_y, g->flight_z};
    Vec3 target_v = aircraft_velocity(&g->flight);
    int ace = index == 0 && g->chapter >= 3;
    float distance, chance, cooldown;
    if (g->special == SPECIAL_AIRSHIP) return;
    if (g->special == SPECIAL_GOTHA) {
        /* The rear gunner covers the tail of the bomber. */
        distance = vec3_length(vec3_sub(target, me));
        if (now - g->scene_started < 5200u || now < enemy->last_shot ||
            now - enemy->last_shot < 1400u + (uint32_t)(index * 211 % 500) ||
            distance > 450.0f || off_nose(enemy, target) < 1.9f) return;
        enemy_burst(g, enemy, target, target_v, -1,
                    0.28f * (1.0f - distance / 520.0f));
        return;
    }
    if (!mission_armed(g) || enemy->ai_state != AI_ATTACK ||
        now - g->scene_started < 5200u || now < enemy->last_shot) return;
    if (enemy->target >= 0) {
        Enemy *w = &g->wingmen[enemy->target];
        target = (Vec3){w->world_x, w->world_y, w->world_z};
        target_v = aircraft_velocity(&w->flight);
    }
    distance = vec3_length(vec3_sub(target, me));
    cooldown = 1150.0f - (float)g->chapter * 45.0f + (float)((index * 173) % 400);
    if (distance > 600.0f || now - enemy->last_shot < (uint32_t)cooldown ||
        off_nose(enemy, target) > 0.1f) return;
    chance = (0.3f + (float)g->chapter * 0.035f + (ace ? 0.15f : 0.0f)) *
             (1.0f - distance / 700.0f);
    if (enemy->target < 0 && fabsf(g->view_bank) > 0.6f) chance *= 0.6f;
    if (g->qa_hold && enemy->target < 0 && g->health <= 30) chance = 0.0f;
    enemy_burst(g, enemy, target, target_v, enemy->target, chance);
}

static void draw_combat_effects(Game *g, int smoke_pass)
{
    SDL_Surface *s = g->screen;
    int i;
    if (smoke_pass) {
        for (i = 0; i < MAX_SPARKS; ++i) {
            WorldSpark *sp = &g->sparks[i];
            Projected3D p;
            int r, alpha;
            if (!sp->active || sp->kind != 2) continue;
            p = project_world_point(g, sp->p);
            if (!p.visible || p.x < -40 || p.x > SCREEN_W + 40 ||
                p.y < -40 || p.y > SCREEN_H + 40) continue;
            r = (int)((2.0f + (sp->initial - sp->life) * 3.0f) * 330.0f /
                      p.depth);
            if (r < 1) r = 1;
            if (r > 40) r = 40;
            alpha = (int)(170.0f * sp->life / sp->initial);
            blend_circle(s, p.x, p.y, r, 46, 40, 38, (uint8_t)alpha);
        }
        return;
    }
    for (i = 0; i < MAX_SPARKS; ++i) {
        WorldSpark *sp = &g->sparks[i];
        Projected3D p;
        if (!sp->active || sp->kind == 2) continue;
        p = project_world_point(g, sp->p);
        if (!p.visible) continue;
        if (sp->kind == 0) {
            pixel(s, p.x, p.y, PIX_PACK(255, 226, 140));
            pixel(s, p.x + 1, p.y, PIX_PACK(255, 170, 70));
        } else {
            int size = (int)(60.0f / p.depth) + 1;
            if (size > 4) size = 4;
            rect(s, p.x, p.y, size, size > 1 ? size - 1 : 1,
                 PIX_PACK(196, 176, 136));
        }
    }
    for (i = 0; i < MAX_ROUNDS; ++i) {
        WorldRound *r = &g->rounds[i];
        Projected3D a, b;
        Vec3 tail;
        if (!r->active) continue;
        tail = (Vec3){r->p.x - r->v.x * 0.025f, r->p.y - r->v.y * 0.025f,
                      r->p.z - r->v.z * 0.025f};
        a = project_world_point(g, r->p);
        b = project_world_point(g, tail);
        if (!a.visible || !b.visible) continue;
        if (abs(a.x - b.x) > 200 || abs(a.y - b.y) > 200) continue;
        line(s, a.x, a.y, b.x, b.y, r->owner == 1 ? PIX_PACK(255, 128, 64)
                                                  : PIX_PACK(255, 236, 160));
    }
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
    if (g->realistic_flight)
        ww1_flight_advance(&g->flight, &position, dt,
                           aileron_command, elevator_command);
    else
        arcade_flight_advance(&g->flight, &position, dt,
                              aileron_command, elevator_command,
                              1.0f + g->skill_flying * 0.22f);
    g->gun_recoil = flight_approach(g->gun_recoil, 0.0f, dt * 9.0f);
    g->shake = flight_approach(g->shake, 0.0f, dt * 2.6f);
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

    /* Guns jam now and then; R works the cocking handles to clear them. */
    if (g->guns_jammed && keys[SDL_SCANCODE_R] && !g->jam_clear_until)
        g->jam_clear_until = now + 1400u -
                             (uint32_t)(g->skill_engine * 600.0f);
    if (g->guns_jammed && g->jam_clear_until && now >= g->jam_clear_until) {
        g->guns_jammed = 0;
        g->jam_clear_until = 0;
        show_notice(g, "GUNS CLEAR");
    }
    if (!g->flight_doomed && keys[SDL_SCANCODE_SPACE] &&
        mission_armed(g) && !g->guns_jammed &&
        g->ammunition > 0 && now - g->last_shot >= 160u) {
        g->last_shot = now;
        --g->ammunition;
        g->audio.shot = 1500;
        g->gun_recoil = 1.0f;
        fire_player_guns(g);
        new_dogfight_particle(g, 298.0f, 245.0f,
                              -54.0f, -68.0f, 0.42f, 2);
        new_dogfight_particle(g, 344.0f, 245.0f,
                               58.0f, -62.0f, 0.42f, 2);
        if (random_unit(g) < 0.004f - g->skill_gunnery * 0.0025f) {
            g->guns_jammed = 1;
            g->jam_clear_until = 0;
            show_notice(g, "GUNS JAMMED - PRESS R");
        }
    } else if (g->guns_jammed && keys[SDL_SCANCODE_SPACE] &&
               (!g->notice_until || now >= g->notice_until)) {
        show_notice(g, "GUNS JAMMED - PRESS R");
    }
    /* H turns for home: the sortie ends as it stands. */
    if (keys[SDL_SCANCODE_H] && !g->heading_home_at && !g->flight_doomed &&
        !g->qa_hold) {
        g->heading_home_at = now + 2200u;
        show_notice(g, "TURNING FOR HOME");
    }
    if (g->heading_home_at && now >= g->heading_home_at) {
        finish_alternate_sortie(g);
        return;
    }
    update_wingmen(g, dt);
    update_rounds(g, dt);

    for (i = 0; i < MAX_ENEMIES; ++i) if (g->enemies[i].active) {
        Enemy *enemy = &g->enemies[i];
        float dx, dy, dz, horizontal, distance;
        float desired_heading = 0.0f, desired_pitch = 0.0f;
        if (enemy->crashing) {
            /* Preserve forward momentum, but force a real downward velocity.
             * The previous pitch decrement had the model's nose-up sign and
             * made a descending centre look as though the aircraft fell
             * upwards. */
            if (enemy->vy > -12.0f) enemy->vy = -12.0f;
            enemy->vy -= (31.0f + i * 1.7f) * dt;
            enemy->bank += dt * (1.7f + 0.16f * i);
            enemy->pitch = flight_approach(enemy->pitch, -0.82f, 0.64f * dt);
            enemy->heading += sinf(enemy->bank) * 0.24f * dt;
            /* The renderer reads the attitude from the quaternion. */
            flight_quaternion_from_euler(&enemy->flight, enemy->bank,
                                         enemy->pitch, enemy->heading);
            enemy->smoke_timer -= dt;
            if (enemy->smoke_timer <= 0.0f) {
                enemy->smoke_timer = 0.05f;
                new_spark(g, (Vec3){enemy->world_x, enemy->world_y,
                                    enemy->world_z},
                          (Vec3){0.0f, 2.0f, 0.0f}, 2, 3.0f);
            }
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
        if (distance < 18.0f && g->qa_hold) {
            /* Visual QA keeps the pilot alive: the attacker sheers off. */
            enemy->ai_state = AI_BREAK;
            enemy->ai_timer = 2.5f;
        } else if (distance < 18.0f) {
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
        {
            float max_bank, to_player;
            enemy_think(g, enemy, i, dt, &desired_heading, &desired_pitch,
                        &max_bank, &to_player);
            ai_fly(enemy, dt, desired_heading, desired_pitch, max_bank);
        }
        if (enemy->hp < enemy->max_hp) {
            enemy->smoke_timer -= dt;
            if (enemy->smoke_timer <= 0.0f) {
                enemy->smoke_timer = enemy->hp <= 1 ? 0.07f : 0.16f;
                new_spark(g, (Vec3){enemy->world_x, enemy->world_y,
                                    enemy->world_z},
                          (Vec3){0.0f, 1.5f, 0.0f}, 2, 2.4f);
            }
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
        enemy_maybe_fire(g, enemy, i);
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
    } else if (!g->qa_hold && !mission_armed(g) && active > 0 &&
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
    if (mission_armed(g)) {
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
            CANVAS_SCREEN_BITS, s->format->format);
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
        if (i == count - 1) {
            label = "THE BOOK OF PRESS CUTTINGS";
        } else if (has_continue && i == 0) {
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
        draw_victorian_plaque(g, 128, 112 + i * (count > 7 ? 28 : 31), 384,
                              label, i == g->menu_selection);
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

/* ---- Story presentation -------------------------------------------------
 *
 * Each chapter opens on a title card over the painted battlefield, the
 * diary entry is written out a few letters at a time, and decorations and
 * promotions arrive as typed citations.  When Thyne is lost the next
 * evening opens on his empty chair. */
#define JOURNAL_LETTERS_PER_SECOND 38
#define CITATION_LETTERS_PER_SECOND 46

static int revealed_letters(const Game *g, int per_second)
{
    uint32_t elapsed = SDL_GetTicks() - g->reveal_started;
    return (int)(elapsed * (uint32_t)per_second / 1000u);
}

static int story_reveal_done(const Game *g)
{
    const CampaignMission *mission = &campaign_missions[g->chapter][g->mission];
    return revealed_letters(g, JOURNAL_LETTERS_PER_SECOND) >=
           (int)strlen(mission->log);
}

static int current_award_index(const Game *g)
{
    int i;
    for (i = 0; i < 5; ++i)
        if (g->pending_award & (1 << i)) return i;
    return -1;
}

static const char *award_citation(const Game *g)
{
    int index = current_award_index(g);
    if (index >= 0) return decorations[index].citation;
    return "HAVING SHOWN GREAT SKILL AND COOLNESS IN COMMAND OF HIS "
           "MACHINE, HE IS PROMOTED WITH EFFECT FROM THIS DAY.";
}

static int award_reveal_done(const Game *g)
{
    return revealed_letters(g, CITATION_LETTERS_PER_SECOND) >=
           (int)strlen(award_citation(g));
}

/* Word-wrapped text which shows only its first 'limit' letters, laid out
 * as if complete so words never jump between lines as they are written. */
static int wrapped_text_limit(Game *g, int x, int y, const char *str,
                              int columns, int line_height, int limit,
                              int typewriter, uint32_t color)
{
    char line_buf[80];
    int line_len = 0, shown = 0;
    const char *p = str;
    while (*p) {
        const char *word;
        int word_len;
        while (*p == ' ') { ++p; ++shown; }
        if (!*p) break;
        word = p;
        while (*p && *p != ' ') ++p;
        word_len = (int)(p - word);
        if (line_len && line_len + 1 + word_len > columns) {
            line_buf[line_len] = '\0';
            if (typewriter) typewriter_text(g, x, y, line_buf, 7, 11, color);
            else journal_text(g, x, y, line_buf, color);
            y += line_height;
            line_len = 0;
        }
        if (line_len) line_buf[line_len++] = ' ';
        if (word_len > (int)sizeof(line_buf) - line_len - 1)
            word_len = (int)sizeof(line_buf) - line_len - 1;
        if (shown + word_len > limit) {
            int part = limit - shown;
            if (part > 0) {
                memcpy(line_buf + line_len, word, (size_t)part);
                line_len += part;
            }
            line_buf[line_len] = '\0';
            if (typewriter) typewriter_text(g, x, y, line_buf, 7, 11, color);
            else journal_text(g, x, y, line_buf, color);
            /* The pen nib at the end of the writing. */
            if (!typewriter)
                circle(g->screen, x + line_len * 8 + 2, y + 9, 1, color);
            return 0;
        }
        memcpy(line_buf + line_len, word, (size_t)word_len);
        line_len += word_len;
        shown += word_len;
    }
    if (line_len) {
        line_buf[line_len] = '\0';
        if (typewriter) typewriter_text(g, x, y, line_buf, 7, 11, color);
        else journal_text(g, x, y, line_buf, color);
    }
    return 1;
}

static void draw_victory_tally(Game *g, int x, int y, int count,
                               uint32_t ink)
{
    SDL_Surface *s = g->screen;
    int i;
    if (count > 40) count = 40;
    for (i = 0; i < count; ++i) {
        int cx = x + (i % 20) * 10, cy = y + (i / 20) * 12;
        line(s, cx - 3, cy, cx + 3, cy, ink);
        line(s, cx, cy - 3, cx, cy + 3, ink);
        line(s, cx - 3, cy + 1, cx + 3, cy + 1, ink);
    }
}

static void draw_chapter_card(Game *g)
{
    SDL_Surface *s = g->screen;
    const Chapter *chapter = &chapters[g->chapter];
    uint32_t elapsed = SDL_GetTicks() - g->reveal_started;
    char year[8];
    /* The day of the chapter's first sortie, so the card never runs
     * ahead of the newspaper and diary that follow it. */
    const char *date = mission_date(g);
    size_t len = strlen(date);
    int fade;
    rect(s, 0, 0, SCREEN_W, SCREEN_H, PIX_PACK(10, 8, 8));
    if (g->dogfight_background) {
        SDL_Rect src, dst;
        int span = g->dogfight_background->w - SCREEN_W;
        src.x = span > 0 ? (int)((elapsed / 60u) % (uint32_t)span) : 0;
        src.y = g->dogfight_background->h > SCREEN_H ?
                (g->dogfight_background->h - SCREEN_H) / 2 : 0;
        src.w = SCREEN_W;
        src.h = SCREEN_H;
        dst.x = 0; dst.y = 0; dst.w = SCREEN_W; dst.h = SCREEN_H;
        SDL_BlitSurface(g->dogfight_background, &src, s, &dst);
    }
    blend_rect(s, 0, 0, SCREEN_W, SCREEN_H, 8, 6, 6, 110);
    blend_rect(s, 0, 110, SCREEN_W, 170, 8, 6, 6, 120);
    snprintf(year, sizeof(year), "%s", len >= 4 ? date + len - 4 : "");
    if (elapsed > 500u)
        display_text(g, 320 - 2 * 36, 124, year, 36, 42,
                     PIX_PACK(236, 214, 160));
    if (elapsed > 1300u) {
        int w = (int)strlen(chapter->title) * 22;
        display_text(g, 320 - w / 2, 184, chapter->title, 22, 28,
                     PIX_PACK(214, 92, 62));
    }
    if (elapsed > 2000u) {
        int w = (int)strlen(date) * 10;
        typewriter_text(g, 320 - w / 2, 230, date, 10, 15,
                        PIX_PACK(226, 206, 166));
    }
    if (elapsed > 2800u)
        typewriter_text(g, 320 - 6 * 7, 360, "ENTER", 7, 11,
                        PIX_PACK(200, 180, 140));
    /* Fade up from black. */
    fade = elapsed < 1200u ? 255 - (int)(elapsed * 255u / 1200u) : 0;
    if (fade > 0) blend_rect(s, 0, 0, SCREEN_W, SCREEN_H, 0, 0, 0,
                             (uint8_t)fade);
}

static void draw_loss_card(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t elapsed = SDL_GetTicks() - g->reveal_started;
    static const char *lines[4] = {
        "THE MESS, THAT EVENING.",
        "THYNE'S CHAIR STOOD EMPTY BY THE STOVE.",
        "NOBODY MOVED HIS PIPE FROM THE TABLE,",
        "AND NOBODY SAT DOWN IN HIS PLACE."
    };
    int i;
    rect(s, 0, 0, SCREEN_W, SCREEN_H, PIX_PACK(12, 9, 8));
    /* A candle and the empty chair, drawn in a few strokes. */
    blend_circle(s, 470, 170, 70, 120, 80, 30, 60);
    rect(s, 468, 150, 4, 26, PIX_PACK(220, 206, 170));
    circle(s, 470, 146, 3, PIX_PACK(255, 214, 120));
    rect(s, 380, 210, 70, 8, PIX_PACK(70, 46, 30));
    rect(s, 380, 140, 8, 150, PIX_PACK(70, 46, 30));
    rect(s, 442, 218, 8, 72, PIX_PACK(70, 46, 30));
    rect(s, 380, 140, 70, 8, PIX_PACK(84, 56, 36));
    rect(s, 380, 218, 8, 72, PIX_PACK(70, 46, 30));
    for (i = 0; i < 4; ++i)
        if (elapsed > 600u + (uint32_t)i * 1100u)
            typewriter_text(g, 60, 150 + i * 26, lines[i], 7, 11,
                            i == 0 ? PIX_PACK(200, 90, 60)
                                   : PIX_PACK(220, 204, 170));
    if (elapsed > 5200u)
        typewriter_text(g, 60, 300, "GRIDPIPE THYNE, ROYAL FLYING CORPS", 7, 11,
                        PIX_PACK(160, 146, 120));
    if (elapsed > 6000u)
        typewriter_text(g, 320 - 6 * 7, 360, "ENTER", 7, 11,
                        PIX_PACK(200, 180, 140));
}

/* Grey bars standing in for the columns of small print. */
static void draw_newsprint(SDL_Surface *s, int x, int y, int w, int lines,
                           uint32_t seed, uint32_t colour)
{
    int i;
    for (i = 0; i < lines; ++i) {
        uint32_t h = scene_hash(seed + (uint32_t)i * 2654435761u);
        int len = i % 7 == 6 ? w / 3 + (int)(h % (uint32_t)(w / 3)) :
                  w - (int)(h % 14u);
        rect(s, x, y + i * 6, len, 2, colour);
    }
}

/* A front page of the morning paper. */
static void draw_newspaper(Game *g, int index, const char *footer)
{
    SDL_Surface *s = g->screen;
    const NewsItem *n = &news_items[index];
    uint32_t ink = PIX_PACK(36, 30, 26);
    uint32_t red = PIX_PACK(128, 36, 28);
    uint32_t grey = PIX_PACK(150, 140, 120);
    uint32_t paper = PIX_PACK(222, 210, 178);
    int len, w, i;
    rect(s, 0, 0, SCREEN_W, SCREEN_H, PIX_PACK(30, 22, 16));
    rect(s, 36, 20, 580, 372, PIX_PACK(16, 12, 10));
    rect(s, 30, 14, 580, 372, paper);
    blend_rect(s, 30, 14, 580, 372, 180, 150, 90, 40);
    w = (int)strlen(NEWS_MASTHEAD) * 22;
    display_text(g, 320 - w / 2, 22, NEWS_MASTHEAD, 22, 28, ink);
    rect(s, 44, 56, 552, 2, ink);
    typewriter_text(g, 46, 61, "PRICE ONE PENNY", 7, 11, ink);
    len = (int)strlen(n->date);
    typewriter_text(g, 594 - len * 7, 61, n->date, 7, 11, ink);
    rect(s, 44, 75, 552, 1, ink);
    rect(s, 44, 78, 552, 1, ink);
    len = (int)strlen(n->headline);
    if (len * 16 <= 548) {
        display_text(g, 320 - len * 8, 86, n->headline, 16, 23, red);
    } else {
        display_text(g, 320 - len * 6, 90, n->headline, 12, 18, red);
    }
    rect(s, 44, 116, 552, 1, ink);
    for (i = 0; i < 3; ++i)
        if (n->body[i])
            typewriter_text(g, 50, 126 + i * 17, n->body[i], 7, 11, ink);
    /* Three columns of the rest of the day's news. */
    for (i = 0; i < 3; ++i) {
        int cx = 50 + i * 184;
        draw_newsprint(s, cx, 188, 168, 18, (uint32_t)index * 97u + (uint32_t)i,
                       grey);
        if (i < 2) rect(s, cx + 176, 184, 1, 112, grey);
    }
    rect(s, 44, 300, 552, 1, ink);
    typewriter_text(g, 50, 306, "IN BRIEF", 7, 11, red);
    if (n->brief[0]) typewriter_text(g, 50, 324, n->brief[0], 7, 11, ink);
    if (n->brief[1]) typewriter_text(g, 330, 324, n->brief[1], 7, 11, ink);
    rect(s, 320, 304, 1, 36, grey);
    rect(s, 44, 344, 552, 1, ink);
    if (footer)
        typewriter_text(g, 320 - (int)strlen(footer) * 7 / 2, 356, footer,
                        7, 11, red);
}

static void draw_scrapbook(Game *g)
{
    SDL_Surface *s = g->screen;
    char buf[48];
    int i, total = 0, place = 0;
    for (i = 0; i < NEWS_COUNT; ++i)
        if (news_unlocked(g, i)) {
            ++total;
            if (i <= g->scrap_index) ++place;
        }
    if (!total) {
        rect(s, 0, 0, SCREEN_W, SCREEN_H, PIX_PACK(30, 22, 16));
        rect(s, 140, 150, 360, 90, PIX_PACK(222, 210, 178));
        display_text(g, 320 - 15 * 6, 168, "NO CUTTINGS YET", 12, 18,
                     PIX_PACK(128, 36, 28));
        typewriter_text(g, 320 - 30 * 7 / 2, 200,
                        "FLY A SORTIE TO BEGIN THE BOOK", 7, 11,
                        PIX_PACK(36, 30, 26));
        typewriter_text(g, 320 - 11 * 7 / 2, 220, "ENTER  BACK", 7, 11,
                        PIX_PACK(36, 30, 26));
        return;
    }
    draw_newspaper(g, g->scrap_index,
                   "LEFT / RIGHT  TURN THE PAGE    ENTER  BACK");
    snprintf(buf, sizeof(buf), "CUTTING %d OF %d", place, total);
    typewriter_text(g, 320 - (int)strlen(buf) * 7 / 2, 372, buf, 7, 11,
                    PIX_PACK(36, 30, 26));
}

static void draw_story(Game *g)
{
    SDL_Surface *s = g->screen;
    const Chapter *chapter = &chapters[g->chapter];
    const CampaignMission *mission = &campaign_missions[g->chapter][g->mission];
    char buf[48];
    uint32_t ink = rgb(s, 47, 37, 27);
    uint32_t red = rgb(s, 125, 39, 31);
    int done;
    if (g->story_stage == 0) { draw_chapter_card(g); return; }
    if (g->story_stage == 1) { draw_loss_card(g); return; }
    if (g->story_stage == 3) {
        int n = news_index_for(g->chapter, g->mission);
        if (n >= 0) {
            draw_newspaper(g, n, "ENTER - READ THE DIARY");
            return;
        }
    }
    rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 34, 24, 17));
    if (g->journal) SDL_BlitSurface(g->journal, NULL, s, NULL);
    journal_text(g, 112, journal_line_y(0), "FLIGHT LOG", red);
    snprintf(buf, sizeof(buf), "%s NEDDY SEAGOON", rank_short[g->rank & 3]);
    journal_text(g, 112, journal_line_y(2), buf, ink);
    journal_text(g, 112, journal_line_y(3), corps_name(g), ink);
    journal_text(g, 112, journal_line_y(5), mission_date(g), red);
    snprintf(buf, sizeof(buf), "CHAPTER %d / MISSION %d",
             g->chapter + 1, g->mission + 1);
    journal_text(g, 112, journal_line_y(7), buf, ink);
    journal_paragraph(g, 112, journal_line_y(9), chapter->title, 22, red);
    snprintf(buf, sizeof(buf), "VICTORIES %d", g->victories);
    journal_text(g, 112, journal_line_y(11), buf, ink);
    draw_victory_tally(g, 116, journal_line_y(12) + 6, g->victories, ink);
    journal_text(g, 348, journal_line_y(0), mission->title, red);
    journal_text(g, 348, journal_line_y(2), mission_mode_name(mission->mode), ink);
    done = wrapped_text_limit(g, 348, journal_line_y(4), mission->log, 25, 17,
                              revealed_letters(g, JOURNAL_LETTERS_PER_SECOND),
                              0, ink);
    if (!done) return;
    if (mission->mode == MISSION_DOGFIGHT_3D) {
        journal_text(g, 348, journal_line_y(11), "FLYING WITH:", red);
        if (wingman_sick(g, 2))
            snprintf(buf, sizeof(buf), "%s", allied_pilot_names[0]);
        else
            snprintf(buf, sizeof(buf), "%s / %s", allied_pilot_names[0],
                     allied_pilot_names[2]);
        journal_text(g, 348, journal_line_y(12), buf, ink);
        if (g->wingman_health[1] > 0)
            journal_text(g, 348, journal_line_y(13), allied_pilot_names[1], ink);
    } else if (mission->mode == MISSION_GUNNER_3D) {
        journal_text(g, 348, journal_line_y(11), "REAR COCKPIT:", red);
        journal_text(g, 348, journal_line_y(12), "TOM AT THE CONTROLS", ink);
        journal_text(g, 348, journal_line_y(13), "LEWIS GUN READY", ink);
    }
    typewriter_text(g, 348, 374, "ENTER - READ ORDERS", 7, 11,
                    PIX_PACK(226, 206, 166));
}

static void draw_medal(SDL_Surface *s, int x, int y, const Decoration *d)
{
    int i;
    if (d->shape == 0) {
        /* A bronze oak leaf. */
        ellipse(s, x, y + 40, 9, 22, PIX_PACK(150, 104, 52));
        line(s, x, y + 18, x, y + 64, PIX_PACK(96, 64, 30));
        return;
    }
    for (i = 0; i < 3; ++i)
        rect(s, x - 18 + i * 12, y, 12, 40,
             PIX_PACK(d->ribbon[i][0], d->ribbon[i][1], d->ribbon[i][2]));
    rect(s, x - 18, y + 40, 36, 3, PIX_PACK(150, 120, 60));
    if (d->shape == 1) {
        rect(s, x - 5, y + 46, 10, 34, PIX_PACK(200, 196, 188));
        rect(s, x - 17, y + 58, 34, 10, PIX_PACK(200, 196, 188));
        circle(s, x, y + 63, 5, PIX_PACK(176, 140, 64));
    } else {
        fill_triangle(s, x, y + 44, x - 16, y + 63, x + 16, y + 63,
                      PIX_PACK(214, 210, 200));
        fill_triangle(s, x, y + 82, x - 16, y + 63, x + 16, y + 63,
                      PIX_PACK(214, 210, 200));
        circle(s, x, y + 63, 7, PIX_PACK(176, 140, 64));
    }
}

static void draw_award(Game *g)
{
    SDL_Surface *s = g->screen;
    int index = current_award_index(g);
    const char *title = index >= 0 ? decorations[index].name :
                        rank_names[g->rank & 3];
    uint32_t ink = PIX_PACK(47, 37, 27);
    uint32_t red = PIX_PACK(125, 39, 31);
    int w;
    rect(s, 0, 0, SCREEN_W, SCREEN_H, PIX_PACK(24, 17, 12));
    if (g->journal) SDL_BlitSurface(g->journal, NULL, s, NULL);
    blend_rect(s, 0, 0, SCREEN_W, SCREEN_H, 10, 8, 6, 170);
    rect(s, 92, 58, 460, 270, PIX_PACK(28, 20, 14));
    rect(s, 88, 54, 460, 270, PIX_PACK(226, 212, 178));
    rect(s, 96, 62, 444, 1, PIX_PACK(150, 120, 80));
    rect(s, 96, 314, 444, 1, PIX_PACK(150, 120, 80));
    w = 22 * 12;
    display_text(g, 318 - w / 2, 72, "BY COMMAND OF THE KING", 12, 18, ink);
    typewriter_text(g, 210, 98, index >= 0 ? "IS AWARDED THE" : "IS PROMOTED",
                    7, 11, ink);
    w = (int)strlen(title) * 12;
    if (w > 420) w = 420;
    display_text(g, 318 - w / 2, 116, title, 12, 18, red);
    if (index >= 0) draw_medal(s, 140, 150, &decorations[index]);
    {
        char who[48];
        snprintf(who, sizeof(who), "%s N. SEAGOON, %s",
                 rank_short[g->rank & 3], corps_short(g));
        typewriter_text(g, 200, 150, who, 7, 11, red);
    }
    if (wrapped_text_limit(g, 200, 172, award_citation(g), 46, 16,
                           revealed_letters(g, CITATION_LETTERS_PER_SECOND),
                           1, ink))
        typewriter_text(g, 400, 296, "ENTER", 7, 11, red);
}

/* The squadron roster pinned over the map on the briefing board. */
static void draw_squadron_roster(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t ink = PIX_PACK(47, 37, 27);
    uint32_t red = PIX_PACK(125, 39, 31);
    char buf[48];
    int i, y = 222;
    rect(s, 404, 206, 196, 118, PIX_PACK(30, 22, 16));
    rect(s, 400, 202, 196, 118, PIX_PACK(228, 216, 184));
    circle(s, 498, 208, 3, PIX_PACK(150, 40, 30));
    typewriter_text(g, 410, y - 6, "SQUADRON ROSTER", 7, 11, red);
    y += 12;
    snprintf(buf, sizeof(buf), "%s SEAGOON %dV", rank_short[g->rank & 3],
             g->victories);
    typewriter_text(g, 410, y, buf, 7, 11, ink);
    for (i = 0; i < 3; ++i) {
        y += 15;
        snprintf(buf, sizeof(buf), "%-14s %s", allied_pilot_names[i],
                 g->wingman_health[i] <= 0 ? "LOST" :
                 wingman_sick(g, i) ? "SICK" : "FIT");
        typewriter_text(g, 410, y, buf, 7, 11,
                        g->wingman_health[i] > 0 && !wingman_sick(g, i) ?
                        ink : red);
    }
    y += 15;
    typewriter_text(g, 410, y, "TOM           OBSERVER", 7, 11, ink);
    y += 15;
    snprintf(buf, sizeof(buf), "FLY %d  GUN %d  ENG %d",
             (int)(g->skill_flying * 10.0f), (int)(g->skill_gunnery * 10.0f),
             (int)(g->skill_engine * 10.0f));
    typewriter_text(g, 410, y, buf, 7, 11, ink);
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
    if (mission->special == SPECIAL_TRUCE) {
        order0 = "FLY THE LINE ON CHRISTMAS DAY.";
        order1 = "NOBODY IS TO FIRE A SHOT TODAY.";
        order2 = "THE MESS SENDS A PUDDING TO THEIR FIELD.";
    } else if (mission->special == SPECIAL_AIRSHIP) {
        order0 = "NIGHT PATROL OVER ESSEX.";
        order1 = "FIND THE AIRSHIP IN THE SEARCHLIGHTS.";
        order2 = "BRING IT DOWN BEFORE IT REACHES LONDON.";
    } else if (mission->special == SPECIAL_GOTHA) {
        order0 = "GOTHA BOMBERS ARE OVER THE THAMES.";
        order1 = "BREAK UP THE FORMATION BEFORE THE CITY.";
        order2 = "MIND THEIR REAR GUNNERS.";
    } else if (mission->mode == MISSION_TOP_DOWN) {
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
    if (mission_armed(g)) {
        typewriter_text(g, 73, 237,
                        mission->mode == MISSION_GUNNER_3D ?
                        "SPACE       LEWIS GUN" :
                        "SPACE       MACHINE GUN", 7, 11, ink);
        typewriter_text(g, 73, 253,
                        mission->mode == MISSION_DOGFIGHT_3D ?
                        "R CLEAR JAM   H HOME" :
                        is_perspective_mode(mission->mode) ?
                        "              RETURN ALIVE" :
                        "B           DROP BOMB", 7, 11, ink);
    } else if (mission->special == SPECIAL_TRUCE) {
        typewriter_text(g, 73, 237, "B           DROP THE PUDDING", 7, 11, ink);
        typewriter_text(g, 73, 253, "NO GUNS ON CHRISTMAS DAY", 7, 11, red);
    } else {
        typewriter_text(g, 73, 237, "NO ARMAMENT FITTED", 7, 11, red);
        typewriter_text(g, 73, 253, "OBSERVE AND RETURN", 7, 11, ink);
    }
    draw_squadron_roster(g);
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
                uint32_t bg = surface_pixel(dst, tx, ty);
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
    if (target->type == 3) {
        /* Men of both armies out between the wire, and a football. */
        float t = (float)SDL_GetTicks() * 0.0016f + (float)x;
        int k;
        for (k = 0; k < 14; ++k) {
            uint32_t h = scene_hash((uint32_t)(k * 7919 + x));
            int dx = (int)(h % 90u) - 45, dy = (int)((h >> 8) % 40u) - 20;
            uint32_t coat = (k & 1) ? rgb(s, 120, 104, 66) : rgb(s, 104, 110, 100);
            dx += (int)(sinf(t * 0.7f + (float)k) * 3.0f);
            circle(s, x + dx, y + dy, 3, coat);
            circle(s, x + dx, y + dy - 1, 1, rgb(s, 214, 180, 150));
        }
        circle(s, x + (int)(sinf(t) * 26.0f), y + (int)(cosf(t * 1.3f) * 9.0f),
               2, rgb(s, 236, 230, 214));
        return;
    }
    if (target->type == 4) {
        /* The enemy aerodrome: two sheds and their machines. */
        rect(s, x - 46, y - 30, 40, 20, rgb(s, 96, 74, 50));
        rect(s, x + 6, y - 30, 40, 20, rgb(s, 96, 74, 50));
        rect(s, x - 44, y - 28, 36, 3, rgb(s, 130, 104, 72));
        rect(s, x + 8, y - 28, 36, 3, rgb(s, 130, 104, 72));
        draw_top_plane(s, x - 30, y + 18, 1);
        draw_top_plane(s, x + 32, y + 22, 1);
        if (g->pudding_delivered) {
            circle(s, x, y + 2, 6, rgb(s, 70, 40, 24));
            circle(s, x, y - 1, 3, rgb(s, 236, 232, 220));
            circle(s, x + 1, y - 3, 1, rgb(s, 170, 30, 30));
        }
        return;
    }
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
    if (g->special == SPECIAL_TRUCE)
        blend_rect(s, 0, 0, SCREEN_W, SCREEN_H, 232, 234, 238, 70);
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
    if (g->special == SPECIAL_TRUCE) {
        draw_field_sign(g, 8, 39, 112, "CHRISTMAS");
        draw_field_sign(g, 232, 8, 172,
                        g->pudding_delivered ? "PUDDING DELIVERED" :
                        g->bombs_remaining ? "PUDDING ABOARD" :
                                             "PUDDING MISSED");
    } else {
        snprintf(buf, sizeof(buf), "TARGETS %d/%d", g->objectives,
                 current_mission_goal(g));
        draw_field_sign(g, 8, 39, 112, buf);
        if (mission_armed(g))
            snprintf(buf, sizeof(buf), "AMMO %03d", g->ammunition);
        else
            snprintf(buf, sizeof(buf), "UNARMED");
        draw_field_sign(g, 264, 8, 108, buf);
        snprintf(buf, sizeof(buf), "BOMBS %02d", g->bombs_remaining);
        draw_field_sign(g, 264, 39, 108, buf);
    }
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

/* ---- Painted world for the cockpit view --------------------------------
 *
 * The sky is the painted sunset panorama wrapped round the horizon; the
 * ground is the painted battlefield map laid flat under the aircraft and
 * faded into haze with distance.  Every screen pixel is turned into a ray
 * through the flight quaternion, so both roll and pitch with the aircraft.
 * Rays are solved exactly every WORLD_SPAN pixels and interpolated between,
 * which keeps divides and arctangents out of the inner loops. */

static float fast_atan2f(float y, float x)
{
    float ax = fabsf(x), ay = fabsf(y), a, s2, r;
    if (ax < 1e-9f && ay < 1e-9f) return 0.0f;
    a = ax > ay ? ay / ax : ax / ay;
    s2 = a * a;
    r = ((-0.0464964749f * s2 + 0.15931422f) * s2 - 0.327622764f) * s2 * a + a;
    if (ay > ax) r = 1.57079637f - r;
    if (x < 0.0f) r = 3.14159274f - r;
    if (y < 0.0f) r = -r;
    return r;
}

static int convert_to_screen(Game *g, SDL_Surface **surface)
{
    SDL_Surface *converted;
    if (!*surface) return 0;
    if ((*surface)->format->format == g->screen->format->format) return 1;
    converted = SDL_ConvertSurface(*surface, g->screen->format, 0);
    if (!converted) return 0;
    SDL_FreeSurface(*surface);
    *surface = converted;
    return 1;
}

static pix_t average_pixels(SDL_Surface *s, int x0, int y0, int w, int h)
{
    uint32_t r = 0, gr = 0, b = 0, n = 0;
    int x, y;
    for (y = y0; y < y0 + h && y < s->h; ++y)
        for (x = x0; x < x0 + w && x < s->w; ++x) {
            uint8_t pr, pg, pb;
            unpack_rgb(s->format, surface_pixel(s, x, y), &pr, &pg, &pb);
            r += pr; gr += pg; b += pb; ++n;
        }
    if (!n) return PIX_PACK(128, 128, 128);
    return PIX_PACK(r / n, gr / n, b / n);
}

/* Builds the ground mip chain (1024 down to 32 texels) and the sky's
 * seamless wrap.  Distant ground reads a smaller copy, so it shimmers far
 * less as the aircraft moves. */
static void prepare_world_textures(Game *g)
{
    int level;
    if (g->topdown_battlefield && convert_to_screen(g, &g->topdown_battlefield) &&
        g->topdown_battlefield->w == 1024 && g->topdown_battlefield->h == 1024) {
        SDL_Surface *src = g->topdown_battlefield;
        int size = 1024;
        g->ground_mip[0] = (pix_t *)malloc(sizeof(pix_t) * 1024 * 1024);
        if (g->ground_mip[0]) {
            int y;
            for (y = 0; y < 1024; ++y)
                memcpy(g->ground_mip[0] + y * 1024, PIX_ROW(src, y),
                       sizeof(pix_t) * 1024);
            g->ground_mips = 1;
        }
        for (level = 1; level < GROUND_MIPS && g->ground_mips == level; ++level) {
            pix_t *prev = g->ground_mip[level - 1];
            int half = size / 2, x, y;
            pix_t *next = (pix_t *)malloc(sizeof(pix_t) * half * half);
            if (!next) break;
            for (y = 0; y < half; ++y)
                for (x = 0; x < half; ++x) {
                    pix_t a = prev[(y * 2) * size + x * 2];
                    pix_t b = prev[(y * 2) * size + x * 2 + 1];
                    pix_t c = prev[(y * 2 + 1) * size + x * 2];
                    pix_t d = prev[(y * 2 + 1) * size + x * 2 + 1];
                    next[y * half + x] = pix_mix(pix_mix(a, b, 16),
                                                 pix_mix(c, d, 16), 16);
                }
            g->ground_mip[level] = next;
            g->ground_mips = level + 1;
            size = half;
        }
        g->ground_haze_tint = average_pixels(src, 0, 0, 1024, 1024);
    }
    if (g->sky && convert_to_screen(g, &g->sky) && g->sky->w >= 256) {
        SDL_Surface *s = g->sky;
        int y, k;
        const int seam = 96;
        for (y = 0; y < s->h; ++y) {
            pix_t *row = PIX_ROW(s, y);
            for (k = 0; k < seam; ++k)
                row[s->w - seam + k] = pix_mix(row[s->w - seam + k],
                                               row[seam - k],
                                               k * 32 / seam);
        }
        g->sky_top = average_pixels(s, 0, 0, s->w, 4);
        g->sky_haze = average_pixels(s, 0, s->h - 10, s->w, 10);
    } else {
        g->sky_top = PIX_PACK(92, 104, 140);
        g->sky_haze = PIX_PACK(214, 150, 128);
    }
    /* Distant fields take on the colour of the low sky, so the ground
     * melts into the painted horizon instead of meeting it at a hard line. */
    g->world_haze = pix_mix(g->sky_haze, g->ground_haze_tint, 9);
    for (level = 0; level < MIDGROUND_ELEMENT_COUNT; ++level)
        if (g->midground[level]) {
            if (!convert_to_screen(g, &g->midground[level]))
                fprintf(stderr, "Midground element %d not converted\n", level);
            prepare_magenta_key(g->midground[level]);
        }
    for (level = 0; level < NEAR_ELEMENT_COUNT; ++level)
        if (g->near_props[level]) {
            if (!convert_to_screen(g, &g->near_props[level]))
                fprintf(stderr, "Near element %d not converted\n", level);
            prepare_magenta_key(g->near_props[level]);
        }
}

static void free_world_textures(Game *g)
{
    int i;
    for (i = 0; i < GROUND_MIPS; ++i) {
        free(g->ground_mip[i]);
        g->ground_mip[i] = NULL;
    }
    g->ground_mips = 0;
}

typedef struct {
    float n, e, d;
} WorldRay;

static void world_camera(const Game *g, WorldRay *origin, WorldRay *across,
                         WorldRay *down)
{
    Vec3 c0 = flight_rotate_to_ned(&g->flight, 1.0f,
                                   -320.0f / WORLD_FOCAL,
                                   -190.0f / WORLD_FOCAL);
    Vec3 cx = flight_rotate_to_ned(&g->flight, 0.0f,
                                   1.0f / WORLD_FOCAL, 0.0f);
    Vec3 cy = flight_rotate_to_ned(&g->flight, 0.0f, 0.0f,
                                   1.0f / WORLD_FOCAL);
    origin->n = c0.x; origin->e = c0.y; origin->d = c0.z;
    across->n = cx.x; across->e = cx.y; across->d = cx.z;
    down->n = cy.x; down->e = cy.y; down->d = cy.z;
}

static void draw_ground_run(Game *g, pix_t *row, int x_start, int x_end,
                            int step, WorldRay base, WorldRay across)
{
    float alt = g->flight_y > 2.0f ? g->flight_y : 2.0f;
    float east0 = fmodf(g->flight_x / GROUND_METRES_PER_TEXEL, 2048.0f);
    float north0 = fmodf(g->flight_z / GROUND_METRES_PER_TEXEL, 2048.0f);
    float min_down = alt / GROUND_MAX_RANGE;
    const float fog_scale = 32.0f / (GROUND_FOG_FAR - GROUND_FOG_NEAR);
    pix_t haze = g->world_haze;
    int x = x_start;
    float u0 = 0, v0 = 0, f0 = 0;
    {
        float dn = base.n + across.n * x, de = base.e + across.e * x;
        float dd = base.d + across.d * x, t, len;
        if (dd < min_down) dd = min_down;
        t = alt / dd;
        len = sqrtf(dn * dn + de * de + dd * dd);
        u0 = east0 + t * de / GROUND_METRES_PER_TEXEL;
        v0 = north0 + t * dn / GROUND_METRES_PER_TEXEL;
        f0 = (t * len - GROUND_FOG_NEAR) * fog_scale;
    }
    while (x < x_end) {
        int next = x + WORLD_SPAN;
        float dn, de, dd, t, len, u1, v1, f1, footprint, vertical;
        int level, size, shift, mask, n, i;
        int32_t u, v, fog, du, dv, dfog;
        const pix_t *tex;
        if (next > x_end) next = x_end;
        dn = base.n + across.n * next;
        de = base.e + across.e * next;
        dd = base.d + across.d * next;
        if (dd < min_down) dd = min_down;
        t = alt / dd;
        len = sqrtf(dn * dn + de * de + dd * dd);
        u1 = east0 + t * de / GROUND_METRES_PER_TEXEL;
        v1 = north0 + t * dn / GROUND_METRES_PER_TEXEL;
        f1 = (t * len - GROUND_FOG_NEAR) * fog_scale;
        n = next - x;
        if (f0 >= 32.0f && f1 >= 32.0f) {
            for (i = x; i < next; ++i) row[i] = haze;
            x = next; u0 = u1; v0 = v1; f0 = f1;
            continue;
        }
        /* Texels per pixel along the row, and down the screen where the
         * ground is seen at a grazing angle. */
        footprint = (fabsf(u1 - u0) + fabsf(v1 - v0)) / (float)n;
        vertical = alt * len * len /
                   (WORLD_FOCAL * GROUND_METRES_PER_TEXEL * dd * dd);
        if (vertical > footprint) footprint = vertical * 0.7f;
        footprint *= (float)step;
        level = 0;
        while (footprint > 1.6f && level + 1 < g->ground_mips) {
            footprint *= 0.5f;
            ++level;
        }
        size = 1024 >> level;
        shift = 10 - level;
        mask = size * 2 - 1;
        tex = g->ground_mip[level];
        {
            float scale = 65536.0f / (float)(1 << level);
            u = (int32_t)(u0 * scale);
            v = (int32_t)(v0 * scale);
            du = (int32_t)((u1 - u0) * scale / (float)n);
            dv = (int32_t)((v1 - v0) * scale / (float)n);
        }
        fog = (int32_t)(flight_clamp(f0, 0.0f, 32.0f) * 65536.0f);
        dfog = (int32_t)((flight_clamp(f1, 0.0f, 32.0f) -
                          flight_clamp(f0, 0.0f, 32.0f)) * 65536.0f /
                         (float)n);
        if (step == 2) { du *= 2; dv *= 2; dfog *= 2; }
        for (i = x; i < next; i += step) {
            int tu = (int)(u >> 16) & mask;
            int tv = (int)(v >> 16) & mask;
            pix_t c;
            tu ^= -((tu >> shift) & 1) & mask;
            tv ^= -((tv >> shift) & 1) & mask;
            c = tex[(tv << shift) + tu];
            if (fog > 0) c = pix_mix(c, haze, (int)(fog >> 16));
            row[i] = c;
            if (step == 2 && i + 1 < next) row[i + 1] = c;
            u += du; v += dv; fog += dfog;
        }
        x = next; u0 = u1; v0 = v1; f0 = f1;
    }
}

static void sky_coordinates(WorldRay d, float *column, float *row_out,
                            int sky_w, int sky_h)
{
    float horizontal = sqrtf(d.n * d.n + d.e * d.e);
    float elevation = fast_atan2f(-d.d, horizontal);
    float azimuth = fast_atan2f(d.e, d.n) + SKY_AZIMUTH_OFFSET;
    *column = azimuth * ((float)sky_w / (2.0f * (float)M_PI));
    *row_out = (float)(sky_h - 1) - elevation * SKY_ROWS_PER_RADIAN;
}

static void draw_sky_run(Game *g, pix_t *row, int x_start, int x_end,
                         int step, WorldRay base, WorldRay across)
{
    SDL_Surface *sky = g->sky;
    int sky_w, sky_h, x = x_start;
    float c0, r0;
    WorldRay d;
    if (!sky || sky->format->format != g->screen->format->format) {
        for (; x < x_end; ++x) row[x] = g->sky_top;
        return;
    }
    sky_w = sky->w;
    sky_h = sky->h;
    d.n = base.n + across.n * x;
    d.e = base.e + across.e * x;
    d.d = base.d + across.d * x;
    sky_coordinates(d, &c0, &r0, sky_w, sky_h);
    while (x < x_end) {
        int next = x + SKY_SPAN, n, i;
        float c1, r1;
        int32_t col, rr, dcol, drow;
        if (next > x_end) next = x_end;
        d.n = base.n + across.n * next;
        d.e = base.e + across.e * next;
        d.d = base.d + across.d * next;
        sky_coordinates(d, &c1, &r1, sky_w, sky_h);
        if (c1 - c0 > sky_w * 0.5f) c1 -= (float)sky_w;
        else if (c0 - c1 > sky_w * 0.5f) c1 += (float)sky_w;
        n = next - x;
        col = (int32_t)(c0 * 65536.0f);
        rr = (int32_t)(r0 * 65536.0f);
        dcol = (int32_t)((c1 - c0) * 65536.0f / (float)n) * step;
        drow = (int32_t)((r1 - r0) * 65536.0f / (float)n) * step;
        for (i = x; i < next; i += step) {
            int cx = (int)(col >> 16), ry = (int)(rr >> 16);
            pix_t c;
            while (cx < 0) cx += sky_w;
            while (cx >= sky_w) cx -= sky_w;
            if (ry >= sky_h) ry = sky_h - 1;
            if (ry >= 0) {
                c = PIX_ROW(sky, ry)[cx];
            } else {
                int fade = -ry / 5;
                c = PIX_ROW(sky, 0)[cx];
                if (fade > 0) c = pix_mix(c, g->sky_top, fade > 32 ? 32 : fade);
            }
            row[i] = c;
            if (step == 2 && i + 1 < next) row[i + 1] = c;
            col += dcol; rr += drow;
        }
        x = next; c0 = c1; r0 = r1;
        if (c0 < 0.0f) c0 += (float)sky_w;
        else if (c0 >= (float)sky_w) c0 -= (float)sky_w;
    }
}

static void draw_world_backdrop(Game *g)
{
    SDL_Surface *s = g->screen;
    WorldRay origin, across, down;
    int step = g->half_res_world ? 2 : 1;
    int y;
    world_camera(g, &origin, &across, &down);
    for (y = 0; y < SCREEN_H; y += step) {
        pix_t *row = PIX_ROW(s, y);
        WorldRay base;
        float split;
        int xh, ground_left;
        base.n = origin.n + down.n * y;
        base.e = origin.e + down.e * y;
        base.d = origin.d + down.d * y;
        /* The down component is linear along the row, so each row has at
         * most one horizon crossing: one sky run and one ground run. */
        ground_left = base.d > 0.0f;
        if (fabsf(across.d) > 1e-9f) {
            split = -base.d / across.d;
            if (split < 0.0f) split = 0.0f;
            if (split > (float)SCREEN_W) split = (float)SCREEN_W;
            xh = (int)split;
            if (step == 2) xh &= ~1;
        } else {
            xh = SCREEN_W;
        }
        if (ground_left) {
            if (xh > 0 && g->ground_mips)
                draw_ground_run(g, row, 0, xh, step, base, across);
            if (xh < SCREEN_W)
                draw_sky_run(g, row, xh, SCREEN_W, step, base, across);
        } else {
            if (xh > 0)
                draw_sky_run(g, row, 0, xh, step, base, across);
            if (xh < SCREEN_W && g->ground_mips)
                draw_ground_run(g, row, xh, SCREEN_W, step, base, across);
        }
        if (step == 2 && y + 1 < SCREEN_H)
            memcpy(PIX_ROW(s, y + 1), row, sizeof(pix_t) * SCREEN_W);
    }
}

/* An upright painted object standing in the world: the sprite is mapped
 * onto a quad from its projected foot to its projected top, so it rolls
 * with the horizon.  Distant objects take on the same haze as the fields. */
static void draw_world_sprite(Game *g, SDL_Surface *sprite, Projected3D foot,
                              Projected3D top, int fog)
{
    SDL_Surface *s = g->screen;
    float ux = (float)(top.x - foot.x), uy = (float)(top.y - foot.y);
    float height = sqrtf(ux * ux + uy * uy), aspect, rx, ry, det;
    float ox, oy, min_x, max_x, min_y, max_y;
    float cxs[4], cys[4];
    uint32_t key = 0;
    pix_t magenta;
    int has_key, x, y, x0, x1, y0, y1, i, sw, sh;
    if (!sprite || height < 3.0f || height > 700.0f) return;
    sw = sprite->w; sh = sprite->h;
    aspect = (float)sw / (float)sh;
    rx = -uy * aspect; ry = ux * aspect;
    ox = (float)foot.x - rx * 0.5f;
    oy = (float)foot.y - ry * 0.5f;
    cxs[0] = ox; cys[0] = oy;
    cxs[1] = ox + rx; cys[1] = oy + ry;
    cxs[2] = ox + rx + ux; cys[2] = oy + ry + uy;
    cxs[3] = ox + ux; cys[3] = oy + uy;
    min_x = max_x = cxs[0]; min_y = max_y = cys[0];
    for (i = 1; i < 4; ++i) {
        if (cxs[i] < min_x) min_x = cxs[i];
        if (cxs[i] > max_x) max_x = cxs[i];
        if (cys[i] < min_y) min_y = cys[i];
        if (cys[i] > max_y) max_y = cys[i];
    }
    x0 = (int)min_x; x1 = (int)max_x + 1;
    y0 = (int)min_y; y1 = (int)max_y + 1;
    if (x1 < 0 || y1 < 0 || x0 >= SCREEN_W || y0 >= SCREEN_H) return;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > SCREEN_W) x1 = SCREEN_W;
    if (y1 > SCREEN_H) y1 = SCREEN_H;
    det = rx * uy - ry * ux;
    if (fabsf(det) < 1e-3f) return;
    has_key = SDL_GetColorKey(sprite, &key) == 0;
    magenta = PIX_PACK(255, 0, 255);
    if (fog > 32) fog = 32;
    {
        /* a runs 0..1 across the sprite, b 0..1 from foot to top. */
        float da_dx = uy / det, db_dx = -ry / det;
        float da_dy = -ux / det, db_dy = rx / det;
        int32_t sa_dx = (int32_t)(da_dx * sw * 65536.0f);
        int32_t sb_dx = (int32_t)(-db_dx * sh * 65536.0f);
        for (y = y0; y < y1; ++y) {
            pix_t *row = PIX_ROW(s, y);
            float px = (float)x0 + 0.5f - ox, py = (float)y + 0.5f - oy;
            float a = (px * uy - py * ux) / det;
            float b = (rx * py - ry * px) / det;
            int32_t sa = (int32_t)(a * sw * 65536.0f);
            int32_t sb = (int32_t)((1.0f - b) * sh * 65536.0f);
            (void)da_dy; (void)db_dy;
            for (x = x0; x < x1; ++x, sa += sa_dx, sb += sb_dx) {
                int tx = (int)(sa >> 16), ty = (int)(sb >> 16);
                pix_t c;
                if (sa < 0 || sb < 0 || tx >= sw || ty >= sh) continue;
                c = PIX_ROW(sprite, ty)[tx];
                /* Converted sprites keep pure magenta as their clear
                 * colour; test it directly as well as the surface key. */
                if (c == magenta || (has_key && c == (pix_t)key)) continue;
                if (fog > 0) c = pix_mix(c, g->world_haze, fog);
                row[x] = c;
            }
        }
    }
}

typedef struct {
    SDL_Surface *sprite;
    Projected3D foot, top;
    float depth;
    int fog;
} WorldObject;

/* Farms, churches, poplar rows and the smoke of the line stand on a
 * hashed grid, so the same countryside is there when you circle back. */
static void draw_world_objects(Game *g)
{
    enum { CELL = 300, REACH = 9, MAX_OBJECTS = 96 };
    WorldObject list[MAX_OBJECTS];
    int count = 0, cx, cz, i, j;
    int base_x = (int)floorf(g->flight_x / CELL);
    int base_z = (int)floorf(g->flight_z / CELL);
    const float fog_scale = 32.0f / (GROUND_FOG_FAR - GROUND_FOG_NEAR);
    for (cz = base_z - REACH; cz <= base_z + REACH; ++cz)
    for (cx = base_x - REACH; cx <= base_x + REACH; ++cx) {
        uint32_t h = scene_hash((uint32_t)cx * 7919u ^
                                (uint32_t)cz * 104729u ^ 0x574f524cu);
        SDL_Surface *sprite;
        float height, wx, wz;
        int kind;
        Projected3D foot, top;
        if ((h & 3u) != 0u || count >= MAX_OBJECTS) continue;
        kind = (int)((h >> 4) % 10u);
        switch (kind) {
        case 0: case 1: case 2:
            sprite = g->near_props[2]; height = 13.0f; break;
        case 3: case 4:
            sprite = g->midground[2]; height = 22.0f; break;
        case 5:
            sprite = g->midground[0]; height = 10.0f; break;
        case 6:
            if ((h >> 12) & 3u) { sprite = g->midground[0]; height = 10.0f; }
            else { sprite = g->midground[1]; height = 24.0f; }
            break;
        case 7: case 8:
            sprite = g->midground[3]; height = 46.0f; break;
        default:
            sprite = g->midground[4]; height = 8.0f; break;
        }
        if (!sprite) continue;
        wx = ((float)cx + 0.15f + (float)((h >> 8) & 255u) / 365.0f) * CELL;
        wz = ((float)cz + 0.15f + (float)((h >> 16) & 255u) / 365.0f) * CELL;
        foot = project_world_point(g, (Vec3){wx, 0.0f, wz});
        if (!foot.visible || foot.depth > REACH * CELL * 1.1f) continue;
        top = project_world_point(g, (Vec3){wx, height, wz});
        if (!top.visible) continue;
        if ((foot.x < -120 && top.x < -120) ||
            (foot.x > SCREEN_W + 120 && top.x > SCREEN_W + 120) ||
            (foot.y < -120 && top.y < -120) ||
            (foot.y > SCREEN_H + 120 && top.y > SCREEN_H + 120)) continue;
        list[count].sprite = sprite;
        list[count].foot = foot;
        list[count].top = top;
        list[count].depth = foot.depth;
        {
            float dx = wx - g->flight_x, dz = wz - g->flight_z;
            float dist = sqrtf(dx * dx + dz * dz + g->flight_y * g->flight_y);
            list[count].fog = (int)flight_clamp((dist - GROUND_FOG_NEAR) *
                                                fog_scale, 0.0f, 32.0f);
        }
        ++count;
    }
    for (i = 1; i < count; ++i) {
        WorldObject item = list[i];
        j = i - 1;
        while (j >= 0 && list[j].depth < item.depth) {
            list[j + 1] = list[j];
            --j;
        }
        list[j + 1] = item;
    }
    for (i = 0; i < count; ++i)
        draw_world_sprite(g, list[i].sprite, list[i].foot, list[i].top,
                          list[i].fog);
}

static void draw_simulator_terrain(Game *g)
{
    SDL_Surface *s = g->screen;
    int pass;
    draw_world_backdrop(g);

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
                    pix_mix(PIX_PACK(64, 70, 66), g->world_haze, 21) :
                    pix_mix(PIX_PACK(58, 66, 46), g->world_haze, 15);
                fill_triangle(s, b0.x, b0.y, b1.x, b1.y,
                              t1.x, t1.y, hill);
                fill_triangle(s, b0.x, b0.y, t1.x, t1.y,
                              t0.x, t0.y, hill);
                if (pass == 1)
                    line(s, t0.x, t0.y, t1.x, t1.y,
                         pix_mix(PIX_PACK(96, 98, 74), g->world_haze, 14));
            }
        }
    }

    draw_world_objects(g);
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
                    radius / 4 + 1, rgb(s, 118, 100, 128));
            ellipse(s, p.x - radius / 3, p.y - radius / 10,
                    radius * 7 / 5, radius / 4 + 1,
                    rgb(s, 232, 176, 150));
        } else {
            ellipse(s, p.x, p.y + radius / 4,
                    cloud_type == 2 ? radius : radius * 6 / 5,
                    radius / 3 + 1, rgb(s, 108, 94, 122));
            ellipse(s, p.x - radius / 8, p.y,
                    cloud_type == 2 ? radius * 3 / 4 : radius,
                    radius / 3 + 1, rgb(s, 214, 160, 146));
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
                    rgb(s, 124, 104, 130));
            ellipse(s, px - pr / 8, py - pry / 5,
                    pr * 4 / 5, pry * 4 / 5,
                    rgb(s, shade, (uint8_t)(shade * 4 / 5), (uint8_t)(shade * 7 / 10)));
        }
    }
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

/* ---- Shaded aircraft ----------------------------------------------------
 *
 * Each aeroplane is a small faceted model lit by the low evening sun.  The
 * faces are sorted far to near, wings and tail show their doped upper or
 * pale under surfaces depending on which side faces the camera, and the
 * national markings sit on the panels as their own faces.  Model axes are
 * metres: x right, y up, z forward. */
#define MESH_MAX_VERTS 128
#define MESH_MAX_FACES 72
#define FACE_TWO_SIDED 1
#define FACE_OUTLINE 2

enum {
    MAT_WING_TOP, MAT_WING_UNDER, MAT_BODY, MAT_BODY_DARK, MAT_TAIL,
    MAT_METAL, MAT_MARK_LIGHT, MAT_MARK_DARK, MAT_MARK_RED, MAT_MARK_BLUE,
    MAT_COUNT
};

typedef struct {
    int count;
    unsigned char index[8];
    unsigned char material, under, flags;
    signed char parent;
    float bias;
} MeshFace;

typedef struct {
    int verts, faces, parent_for_new;
    float v[MESH_MAX_VERTS][3];
    MeshFace f[MESH_MAX_FACES];
} AircraftMesh;

typedef struct {
    uint8_t rgb[MAT_COUNT][3];
} AircraftLivery;

static const AircraftLivery enemy_liveries[4] = {
    {{{176, 150, 108}, {196, 188, 164}, {150, 42, 32}, {96, 30, 24},
      {150, 42, 32}, {70, 68, 62}, {236, 232, 220}, {20, 18, 18},
      {176, 40, 30}, {40, 60, 120}}},
    {{{104, 108, 70}, {190, 186, 160}, {164, 132, 62}, {110, 88, 42},
      {226, 222, 206}, {70, 68, 62}, {236, 232, 220}, {20, 18, 18},
      {176, 40, 30}, {40, 60, 120}}},
    {{{122, 100, 120}, {184, 176, 168}, {66, 78, 100}, {42, 50, 66},
      {184, 64, 44}, {70, 68, 62}, {236, 232, 220}, {20, 18, 18},
      {176, 40, 30}, {40, 60, 120}}},
    {{{170, 34, 28}, {150, 46, 40}, {176, 36, 30}, {110, 24, 20},
      {176, 36, 30}, {60, 58, 54}, {236, 232, 220}, {20, 18, 18},
      {176, 40, 30}, {40, 60, 120}}}
};

static const AircraftLivery allied_livery = {
    {{120, 108, 66}, {206, 196, 166}, {116, 104, 64}, {80, 72, 44},
     {116, 104, 64}, {80, 80, 76}, {236, 232, 220}, {20, 18, 18},
     {184, 40, 34}, {36, 58, 128}}
};

static int mesh_vertex(AircraftMesh *m, float x, float y, float z)
{
    if (m->verts >= MESH_MAX_VERTS) return MESH_MAX_VERTS - 1;
    m->v[m->verts][0] = x;
    m->v[m->verts][1] = y;
    m->v[m->verts][2] = z;
    return m->verts++;
}

static void mesh_face(AircraftMesh *m, int count, const int *index,
                      int material, int under, int flags, float bias)
{
    MeshFace *f;
    int i;
    if (m->faces >= MESH_MAX_FACES) return;
    f = &m->f[m->faces++];
    f->count = count;
    for (i = 0; i < count; ++i) f->index[i] = (unsigned char)index[i];
    f->material = (unsigned char)material;
    f->under = (unsigned char)under;
    f->flags = (unsigned char)flags;
    f->parent = (signed char)m->parent_for_new;
    f->bias = bias;
}

/* A flat rectangle in a horizontal (y) or vertical side (x) plane. */
static void mesh_panel_y(AircraftMesh *m, float x0, float x1, float y,
                         float z0, float z1, int material, int under,
                         int flags, float bias, int face_up)
{
    int q[4];
    q[0] = mesh_vertex(m, x0, y, z0);
    q[1] = mesh_vertex(m, x1, y, z0);
    q[2] = mesh_vertex(m, x1, y, z1);
    q[3] = mesh_vertex(m, x0, y, z1);
    if (face_up) {
        int t = q[1]; q[1] = q[3]; q[3] = t;
    }
    mesh_face(m, 4, q, material, under, flags, bias);
}

static void mesh_disc(AircraftMesh *m, int axis, float cx, float cy, float cz,
                      float radius, int material, float bias, int positive)
{
    int q[8], i;
    for (i = 0; i < 8; ++i) {
        float a = (float)i * (float)M_PI / 4.0f;
        float a1 = cosf(a) * radius, a2 = sinf(a) * radius;
        if (positive) a2 = -a2;
        if (axis == 1) q[i] = mesh_vertex(m, cx + a1, cy, cz + a2);
        else q[i] = mesh_vertex(m, cx, cy + a2, cz + a1);
    }
    mesh_face(m, 8, q, material, material, 0, bias);
}

static void mesh_cross_y(AircraftMesh *m, float cx, float y, float cz,
                         float size, int up)
{
    float s = size, b = size * 0.32f;
    mesh_panel_y(m, cx - s * 1.18f, cx + s * 1.18f, y, cz - s * 1.18f,
                 cz + s * 1.18f, MAT_MARK_LIGHT, MAT_MARK_LIGHT, 0, -0.02f, up);
    mesh_panel_y(m, cx - s, cx + s, y, cz - b, cz + b,
                 MAT_MARK_DARK, MAT_MARK_DARK, 0, -0.04f, up);
    mesh_panel_y(m, cx - b, cx + b, y, cz - s, cz + s,
                 MAT_MARK_DARK, MAT_MARK_DARK, 0, -0.05f, up);
}

static void build_aircraft_mesh(AircraftMesh *m, int allied)
{
    int q[4];
    int nose[4], mid[4], tail[4];
    int k;
    memset(m, 0, sizeof(*m));
    m->parent_for_new = -1;
    /* Upper and lower wings: one panel each, both sides painted. */
    mesh_panel_y(m, -8.7f, 8.7f, 1.35f, -1.25f, 1.8f, MAT_WING_TOP,
                 MAT_WING_UNDER, FACE_TWO_SIDED | FACE_OUTLINE, 0.0f, 1);
    mesh_panel_y(m, -7.8f, 7.8f, -0.95f, -1.05f, 1.75f, MAT_WING_TOP,
                 MAT_WING_UNDER, FACE_TWO_SIDED | FACE_OUTLINE, 0.0f, 1);
    /* Tailplane and fin. */
    q[0] = mesh_vertex(m, -2.9f, 0.15f, -6.75f);
    q[3] = mesh_vertex(m, 2.9f, 0.15f, -6.75f);
    q[2] = mesh_vertex(m, 2.2f, 0.15f, -5.2f);
    q[1] = mesh_vertex(m, -2.2f, 0.15f, -5.2f);
    mesh_face(m, 4, q, MAT_TAIL, MAT_WING_UNDER,
              FACE_TWO_SIDED | FACE_OUTLINE, 0.0f);
    q[0] = mesh_vertex(m, 0.0f, 0.25f, -5.0f);
    q[1] = mesh_vertex(m, 0.0f, 0.25f, -7.0f);
    q[2] = mesh_vertex(m, 0.0f, 2.15f, -6.8f);
    q[3] = mesh_vertex(m, 0.0f, 1.75f, -5.7f);
    mesh_face(m, 4, q, MAT_TAIL, MAT_TAIL, FACE_TWO_SIDED | FACE_OUTLINE, 0.0f);
    /* Fuselage: three box sections, closed so the back faces cull. */
    nose[0] = mesh_vertex(m, -0.82f, 0.72f, 4.4f);
    nose[1] = mesh_vertex(m, 0.82f, 0.72f, 4.4f);
    nose[2] = mesh_vertex(m, 0.74f, -0.82f, 4.4f);
    nose[3] = mesh_vertex(m, -0.74f, -0.82f, 4.4f);
    mid[0] = mesh_vertex(m, -0.78f, 0.7f, -0.6f);
    mid[1] = mesh_vertex(m, 0.78f, 0.7f, -0.6f);
    mid[2] = mesh_vertex(m, 0.62f, -0.78f, -0.6f);
    mid[3] = mesh_vertex(m, -0.62f, -0.78f, -0.6f);
    tail[0] = mesh_vertex(m, -0.14f, 0.34f, -6.9f);
    tail[1] = mesh_vertex(m, 0.14f, 0.34f, -6.9f);
    tail[2] = mesh_vertex(m, 0.1f, -0.06f, -6.9f);
    tail[3] = mesh_vertex(m, -0.1f, -0.06f, -6.9f);
    for (k = 0; k < 4; ++k) {
        int n = (k + 1) & 3;
        int material = k == 2 ? MAT_BODY_DARK : MAT_BODY;
        q[0] = nose[k]; q[1] = nose[n]; q[2] = mid[n]; q[3] = mid[k];
        mesh_face(m, 4, q, k == 0 ? MAT_METAL : material, 0, 0, 0.0f);
        q[0] = mid[k]; q[1] = mid[n]; q[2] = tail[n]; q[3] = tail[k];
        mesh_face(m, 4, q, material, 0, 0, 0.0f);
    }
    q[0] = nose[3]; q[1] = nose[2]; q[2] = nose[1]; q[3] = nose[0];
    mesh_face(m, 4, q, MAT_METAL, 0, 0, 0.0f);
    q[0] = tail[0]; q[1] = tail[1]; q[2] = tail[2]; q[3] = tail[3];
    mesh_face(m, 4, q, MAT_BODY, 0, 0, 0.0f);
    /* Markings on the wing tips, drawn straight after the wing they sit
     * on (faces 0 and 1) so depth sorting can never hide them. */
    if (allied) {
        m->parent_for_new = 0;
        mesh_disc(m, 1, -6.4f, 1.37f, 0.3f, 1.05f, MAT_MARK_BLUE, -0.02f, 1);
        mesh_disc(m, 1, -6.4f, 1.38f, 0.3f, 0.68f, MAT_MARK_LIGHT, -0.04f, 1);
        mesh_disc(m, 1, -6.4f, 1.39f, 0.3f, 0.34f, MAT_MARK_RED, -0.06f, 1);
        mesh_disc(m, 1, 6.4f, 1.37f, 0.3f, 1.05f, MAT_MARK_BLUE, -0.02f, 1);
        mesh_disc(m, 1, 6.4f, 1.38f, 0.3f, 0.68f, MAT_MARK_LIGHT, -0.04f, 1);
        mesh_disc(m, 1, 6.4f, 1.39f, 0.3f, 0.34f, MAT_MARK_RED, -0.06f, 1);
        m->parent_for_new = 1;
        mesh_disc(m, 1, -5.6f, -0.97f, 0.35f, 0.95f, MAT_MARK_BLUE, -0.02f, 0);
        mesh_disc(m, 1, -5.6f, -0.98f, 0.35f, 0.6f, MAT_MARK_LIGHT, -0.04f, 0);
        mesh_disc(m, 1, -5.6f, -0.99f, 0.35f, 0.3f, MAT_MARK_RED, -0.06f, 0);
        mesh_disc(m, 1, 5.6f, -0.97f, 0.35f, 0.95f, MAT_MARK_BLUE, -0.02f, 0);
        mesh_disc(m, 1, 5.6f, -0.98f, 0.35f, 0.6f, MAT_MARK_LIGHT, -0.04f, 0);
        mesh_disc(m, 1, 5.6f, -0.99f, 0.35f, 0.3f, MAT_MARK_RED, -0.06f, 0);
    } else {
        m->parent_for_new = 0;
        mesh_cross_y(m, -6.6f, 1.37f, 0.28f, 0.8f, 1);
        mesh_cross_y(m, 6.6f, 1.37f, 0.28f, 0.8f, 1);
        m->parent_for_new = 1;
        mesh_cross_y(m, -5.8f, -0.97f, 0.35f, 0.72f, 0);
        mesh_cross_y(m, 5.8f, -0.97f, 0.35f, 0.72f, 0);
    }
    m->parent_for_new = -1;
}

/* A raiding airship: an eight-sided hull, four fins and a gondola. */
static void build_airship_mesh(AircraftMesh *m)
{
    static const float ring_z[5] = {30.0f, 20.0f, 0.0f, -20.0f, -30.0f};
    static const float ring_r[5] = {2.5f, 7.0f, 8.0f, 6.5f, 1.8f};
    int ring[5][8], q[8], r, k;
    memset(m, 0, sizeof(*m));
    m->parent_for_new = -1;
    for (r = 0; r < 5; ++r)
        for (k = 0; k < 8; ++k) {
            /* Clockwise seen from the bow, like the scout fuselage, so
             * the outer faces are the front faces. */
            float a = -(float)k * (float)M_PI / 4.0f;
            ring[r][k] = mesh_vertex(m, cosf(a) * ring_r[r],
                                     sinf(a) * ring_r[r], ring_z[r]);
        }
    for (r = 0; r < 4; ++r)
        for (k = 0; k < 8; ++k) {
            int n = (k + 1) & 7;
            q[0] = ring[r][k]; q[1] = ring[r][n];
            q[2] = ring[r + 1][n]; q[3] = ring[r + 1][k];
            mesh_face(m, 4, q, (k == 1 || k == 2) ? MAT_BODY_DARK : MAT_BODY,
                      0, 0, 0.0f);
        }
    for (k = 0; k < 8; ++k) q[k] = ring[0][7 - k];
    mesh_face(m, 8, q, MAT_METAL, 0, 0, 0.0f);
    for (k = 0; k < 8; ++k) q[k] = ring[4][k];
    mesh_face(m, 8, q, MAT_BODY, 0, 0, 0.0f);
    /* Cruciform tail fins. */
    for (k = 0; k < 4; ++k) {
        float ax = k == 0 ? 1.0f : k == 2 ? -1.0f : 0.0f;
        float ay = k == 1 ? 1.0f : k == 3 ? -1.0f : 0.0f;
        q[0] = mesh_vertex(m, ax * 3.0f, ay * 3.0f, -29.0f);
        q[1] = mesh_vertex(m, ax * 6.0f, ay * 6.0f, -18.0f);
        q[2] = mesh_vertex(m, ax * 10.0f, ay * 10.0f, -24.0f);
        q[3] = mesh_vertex(m, ax * 10.0f, ay * 10.0f, -30.0f);
        mesh_face(m, 4, q, MAT_TAIL, MAT_TAIL,
                  FACE_TWO_SIDED | FACE_OUTLINE, 0.0f);
    }
    /* The control gondola slung under the bow. */
    mesh_panel_y(m, -1.2f, 1.2f, -9.2f, 8.0f, 14.0f, MAT_METAL, MAT_METAL,
                 FACE_TWO_SIDED | FACE_OUTLINE, 0.0f, 0);
    m->parent_for_new = -1;
}

/* Meshes: 0 enemy scout, 1 allied scout, 2 airship, 3 Gotha bomber. */
static const AircraftMesh *aircraft_mesh(int kind)
{
    static AircraftMesh meshes[4];
    static int built[4];
    if (kind < 0 || kind > 3) kind = 0;
    if (!built[kind]) {
        if (kind == 2) {
            build_airship_mesh(&meshes[2]);
        } else {
            int i;
            build_aircraft_mesh(&meshes[kind], kind == 1);
            /* The Gotha is the same rigging drawn half as large again. */
            if (kind == 3)
                for (i = 0; i < meshes[3].verts; ++i) {
                    meshes[3].v[i][0] *= 1.6f;
                    meshes[3].v[i][1] *= 1.4f;
                    meshes[3].v[i][2] *= 1.4f;
                }
        }
        built[kind] = 1;
    }
    return &meshes[kind];
}

static int enemy_mesh_kind(const Game *g)
{
    return g->special == SPECIAL_AIRSHIP ? 2 :
           g->special == SPECIAL_GOTHA ? 3 : 0;
}

static Vec3 sun_direction(void)
{
    static Vec3 sun;
    static int ready;
    if (!ready) {
        float ce = cosf(SUN_ELEVATION);
        sun.x = sinf(SUN_AZIMUTH) * ce;
        sun.y = sinf(SUN_ELEVATION) + 0.35f;
        sun.z = cosf(SUN_AZIMUTH) * ce;
        {
            float l = sqrtf(sun.x * sun.x + sun.y * sun.y + sun.z * sun.z);
            sun.x /= l; sun.y /= l; sun.z /= l;
        }
        ready = 1;
    }
    return sun;
}

typedef struct {
    float depth;
    int face;
    int under;
    pix_t colour;
} SortedFace;

static void draw_aircraft_far(Game *g, const AircraftPose *p,
                              Projected3D centre, pix_t body)
{
    SDL_Surface *s = g->screen;
    Projected3D wl = project_world_point(g, pose_point(p, -8.7f, 1.0f, 0.0f));
    Projected3D wr = project_world_point(g, pose_point(p, 8.7f, 1.0f, 0.0f));
    Projected3D nn = project_world_point(g, pose_point(p, 0.0f, 0.0f, 4.4f));
    Projected3D tt = project_world_point(g, pose_point(p, 0.0f, 0.3f, -6.9f));
    pix_t dark = pix_mix(PIX_PACK(30, 26, 24), g->world_haze,
                         centre.depth > 2500.0f ? 14 : 4);
    if (wl.visible && wr.visible) {
        line(s, wl.x, wl.y, wr.x, wr.y, dark);
        if (centre.depth < 1100.0f)
            line(s, wl.x, wl.y + 1, wr.x, wr.y + 1, dark);
    }
    if (nn.visible && tt.visible) line(s, nn.x, nn.y, tt.x, tt.y, body);
    if (wl.x == wr.x && wl.y == wr.y) pixel(s, centre.x, centre.y, dark);
}

/* Draws one aircraft.  Returns 0 when it is off screen. */
static int draw_aircraft_mesh(Game *g, const AircraftPose *pose,
                              const AircraftLivery *livery, int allied,
                              int smoking)
{
    SDL_Surface *s = g->screen;
    int kind = allied ? 1 : enemy_mesh_kind(g);
    const AircraftMesh *m = aircraft_mesh(kind);
    Vec3 world[MESH_MAX_VERTS];
    Projected3D screen[MESH_MAX_VERTS];
    SortedFace order[MESH_MAX_FACES];
    pix_t face_colour[MESH_MAX_FACES];
    unsigned char face_shown[MESH_MAX_FACES];
    pix_t base[MAT_COUNT];
    Vec3 sun = sun_direction();
    Projected3D centre = project_world_point(g, pose->pos);
    int i, j, count = 0;
    int haze;
    if (!centre.visible || centre.x < -260 || centre.x > SCREEN_W + 260 ||
        centre.y < -260 || centre.y > SCREEN_H + 260) return 0;
    haze = (int)flight_clamp((centre.depth - 900.0f) * 32.0f / 9000.0f,
                             0.0f, 26.0f);
    for (i = 0; i < MAT_COUNT; ++i)
        base[i] = PIX_PACK(livery->rgb[i][0], livery->rgb[i][1],
                           livery->rgb[i][2]);
    if (kind == 2 && centre.depth > 2400.0f) {
        Projected3D bow = project_world_point(g, pose_point(pose, 0.0f, 0.0f, 30.0f));
        Projected3D stern = project_world_point(g, pose_point(pose, 0.0f, 0.0f, -30.0f));
        if (bow.visible && stern.visible) {
            line(s, bow.x, bow.y, stern.x, stern.y, base[MAT_BODY]);
            line(s, bow.x, bow.y + 1, stern.x, stern.y + 1, base[MAT_BODY]);
        }
        return 1;
    }
    if (kind != 2 && centre.depth > (kind == 3 ? 1000.0f : 640.0f)) {
        draw_aircraft_far(g, pose, centre,
                          pix_mix(base[MAT_BODY], g->world_haze, haze));
        return 1;
    }
    for (i = 0; i < m->verts; ++i) {
        world[i] = pose_point(pose, m->v[i][0], m->v[i][1], m->v[i][2]);
        screen[i] = project_world_point(g, world[i]);
    }
    memset(face_shown, 0, sizeof(face_shown));
    for (i = 0; i < m->faces; ++i) {
        const MeshFace *f = &m->f[i];
        Vec3 a = world[f->index[0]], b = world[f->index[1]],
             c = world[f->index[2]];
        float ex = b.x - a.x, ey = b.y - a.y, ez = b.z - a.z;
        float fx = c.x - a.x, fy = c.y - a.y, fz = c.z - a.z;
        float nx = ey * fz - ez * fy, ny = ez * fx - ex * fz,
              nz = ex * fy - ey * fx;
        float vx = g->flight_x - a.x, vy = g->flight_y - a.y,
              vz = g->flight_z - a.z;
        float facing = nx * vx + ny * vy + nz * vz;
        float len, light, depth = 0.0f;
        int under = 0, material, k, visible = 1;
        for (k = 0; k < f->count; ++k) {
            if (!screen[f->index[k]].visible) { visible = 0; break; }
            depth += screen[f->index[k]].depth;
        }
        if (!visible) continue;
        if (facing < 0.0f) {
            if (!(f->flags & FACE_TWO_SIDED)) continue;
            nx = -nx; ny = -ny; nz = -nz;
            under = 1;
        }
        len = sqrtf(nx * nx + ny * ny + nz * nz);
        if (len < 1e-6f) continue;
        light = (nx * sun.x + ny * sun.y + nz * sun.z) / len;
        if (light < 0.0f) light = 0.0f;
        material = under ? f->under : f->material;
        order[count].colour = pix_mix(PIX_PACK(0, 0, 0), base[material],
                                      12 + (int)(light * 20.0f));
        order[count].colour = pix_mix(order[count].colour,
                                      PIX_PACK(255, 178, 126), 3);
        if (haze) order[count].colour = pix_mix(order[count].colour,
                                                g->world_haze, haze);
        face_colour[i] = order[count].colour;
        face_shown[i] = 1;
        if (f->parent >= 0) continue;
        order[count].depth = depth / (float)f->count + f->bias;
        order[count].face = i;
        order[count].under = under;
        ++count;
    }
    for (i = 1; i < count; ++i) {
        SortedFace item = order[i];
        j = i - 1;
        while (j >= 0 && order[j].depth < item.depth) {
            order[j + 1] = order[j];
            --j;
        }
        order[j + 1] = item;
    }
    for (i = 0; i < count; ++i) {
        const MeshFace *f = &m->f[order[i].face];
        Projected3D p0 = screen[f->index[0]];
        int k;
        for (k = 1; k + 1 < f->count; ++k) {
            Projected3D p1 = screen[f->index[k]];
            Projected3D p2 = screen[f->index[k + 1]];
            fill_triangle(s, p0.x, p0.y, p1.x, p1.y, p2.x, p2.y,
                          order[i].colour);
        }
        if ((f->flags & FACE_OUTLINE) && centre.depth < 420.0f) {
            pix_t edge = pix_mix(PIX_PACK(0, 0, 0), order[i].colour, 15);
            for (k = 0; k < f->count; ++k) {
                Projected3D a = screen[f->index[k]];
                Projected3D b = screen[f->index[(k + 1) % f->count]];
                line(s, a.x, a.y, b.x, b.y, edge);
            }
        }
        for (j = 0; j < m->faces; ++j) {
            const MeshFace *mark = &m->f[j];
            if (mark->parent != order[i].face || !face_shown[j]) continue;
            p0 = screen[mark->index[0]];
            for (k = 1; k + 1 < mark->count; ++k) {
                Projected3D p1 = screen[mark->index[k]];
                Projected3D p2 = screen[mark->index[k + 1]];
                fill_triangle(s, p0.x, p0.y, p1.x, p1.y, p2.x, p2.y,
                              face_colour[j]);
            }
        }
    }
    /* Struts, wires, undercarriage, pilot and the spinning propeller. */
    if (centre.depth < 520.0f) {
        static const float struts[][6] = {
            {-5.6f, 1.35f, 0.9f, -5.1f, -0.95f, 0.9f},
            {5.6f, 1.35f, 0.9f, 5.1f, -0.95f, 0.9f},
            {-5.6f, 1.35f, -0.4f, -5.1f, -0.95f, -0.4f},
            {5.6f, 1.35f, -0.4f, 5.1f, -0.95f, -0.4f},
            {-0.7f, 1.35f, 1.2f, -0.6f, 0.7f, 1.6f},
            {0.7f, 1.35f, 1.2f, 0.6f, 0.7f, 1.6f},
            {-0.6f, -0.8f, 2.6f, -0.9f, -2.1f, 2.4f},
            {0.6f, -0.8f, 2.6f, 0.9f, -2.1f, 2.4f}
        };
        pix_t strut = pix_mix(PIX_PACK(42, 34, 28), g->world_haze, haze);
        Projected3D head = project_world_point(g,
            pose_point(pose, 0.0f, 1.0f, -0.4f));
        Projected3D hub = project_world_point(g,
            pose_point(pose, 0.0f, 0.0f, 4.7f));
        Projected3D tip = project_world_point(g,
            pose_point(pose, 0.0f, 1.3f, 4.7f));
        for (i = 0; i < 8; ++i) {
            Projected3D a = project_world_point(g, pose_point(pose,
                struts[i][0], struts[i][1], struts[i][2]));
            Projected3D b = project_world_point(g, pose_point(pose,
                struts[i][3], struts[i][4], struts[i][5]));
            if (a.visible && b.visible) line(s, a.x, a.y, b.x, b.y, strut);
        }
        if (head.visible && centre.depth < 380.0f)
            circle(s, head.x, head.y, centre.depth < 160.0f ? 2 : 1,
                   PIX_PACK(48, 36, 30));
        if (hub.visible && tip.visible) {
            int dx = tip.x - hub.x, dy = tip.y - hub.y;
            int r = (int)sqrtf((float)(dx * dx + dy * dy));
            if (r >= 3)
                blend_circle(s, hub.x, hub.y, r, 210, 200, 180, 70);
        }
    }
    if (smoking) {
        Projected3D tail = project_world_point(g,
            pose_point(pose, 0.0f, 0.0f, -2.0f));
        if (tail.visible) {
            int r = (int)(900.0f / tail.depth);
            if (r < 2) r = 2;
            if (r > 18) r = 18;
            blend_circle(s, tail.x, tail.y, r, 40, 36, 34, 150);
        }
    }
    return 1;
}

/* ---- Living cockpit -----------------------------------------------------
 *
 * The painted cockpit is composed once with the underside of the upper
 * wing, its struts and wires.  The painted dial needles are cleaned off at
 * load and live needles are drawn every frame.  The whole composite shakes
 * with the engine and kicks when the guns fire. */
typedef struct {
    int x, y, radius;
    int dark_face;
} CockpitDial;

static const CockpitDial cockpit_dials[4] = {
    {242, 371, 21, 1},  /* airspeed */
    {321, 364, 24, 0},  /* altimeter */
    {379, 366, 16, 0},  /* engine revolutions */
    {427, 370, 12, 1}   /* fuel */
};

static void fill_quad(SDL_Surface *s, int x0, int y0, int x1, int y1,
                      int x2, int y2, int x3, int y3, uint32_t c)
{
    fill_triangle(s, x0, y0, x1, y1, x2, y2, c);
    fill_triangle(s, x0, y0, x2, y2, x3, y3, c);
}

static void draw_strut(SDL_Surface *s, int x0, int y0, int x1, int y1,
                       int top_width, int bottom_width)
{
    fill_quad(s, x0 - top_width / 2, y0, x0 + top_width / 2, y0,
              x1 + bottom_width / 2, y1, x1 - bottom_width / 2, y1,
              PIX_PACK(84, 56, 36));
    line(s, x0 - top_width / 2 + 1, y0, x1 - bottom_width / 2 + 1, y1,
         PIX_PACK(152, 104, 62));
    line(s, x0 + top_width / 2, y0, x1 + bottom_width / 2, y1,
         PIX_PACK(46, 30, 20));
}

static void erase_painted_needle(SDL_Surface *s, const CockpitDial *d)
{
    int r = d->radius, x, y;
    uint32_t sr = 0, sg = 0, sb = 0, n = 0;
    int inner = r * 72 / 100;
    uint8_t fr, fg, fb;
    pix_t face;
    for (y = -r; y <= r; ++y)
        for (x = -r; x <= r; ++x) {
            int dist2 = x * x + y * y;
            uint8_t pr, pg, pb;
            if (dist2 < (r * 30 / 100) * (r * 30 / 100) ||
                dist2 > (r * 60 / 100) * (r * 60 / 100)) continue;
            unpack_rgb(s->format, surface_pixel(s, d->x + x, d->y + y),
                       &pr, &pg, &pb);
            sr += pr; sg += pg; sb += pb; ++n;
        }
    if (!n) return;
    fr = (uint8_t)(sr / n); fg = (uint8_t)(sg / n); fb = (uint8_t)(sb / n);
    face = PIX_PACK(fr, fg, fb);
    for (y = -inner; y <= inner; ++y)
        for (x = -inner; x <= inner; ++x) {
            uint8_t pr, pg, pb;
            int diff;
            if (x * x + y * y > inner * inner) continue;
            unpack_rgb(s->format, surface_pixel(s, d->x + x, d->y + y),
                       &pr, &pg, &pb);
            diff = abs((int)pr - fr) + abs((int)pg - fg) + abs((int)pb - fb);
            if (diff > 54)
                PIX_ROW(s, d->y + y)[d->x + x] = face;
        }
}

static SDL_Surface *cockpit_composite_from(SDL_Surface *painting)
{
    SDL_Surface *c;
    pix_t key = PIX_PACK(255, 0, 255);
    int y, i;
    if (!painting) return NULL;
    c = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_W, SCREEN_H,
                                       CANVAS_SCREEN_BITS,
                                       CANVAS_SCREEN_FORMAT);
    if (!c) return NULL;
    SDL_FillRect(c, NULL, key);
    /* Underside of the upper wing: doped linen in its own shadow, warming
     * toward the leading edge where the low sun catches it. */
    for (y = 0; y < 46; ++y) {
        int t = y * 32 / 45;
        pix_t shade = pix_mix(PIX_PACK(96, 80, 70), PIX_PACK(178, 150, 118), t);
        pix_t *row = PIX_ROW(c, y);
        int x;
        for (x = 0; x < SCREEN_W; ++x) row[x] = shade;
    }
    for (y = 41; y < 47; ++y) {
        pix_t *row = PIX_ROW(c, y);
        int x;
        pix_t rim = y < 44 ? PIX_PACK(214, 178, 132) : PIX_PACK(120, 92, 70);
        for (x = 0; x < SCREEN_W; ++x) row[x] = rim;
    }
    /* Ribs run fore and aft, so they converge on the vanishing point. */
    for (i = -12; i <= 12; ++i) {
        float span = (float)i * 0.35f;
        int top_x = 320 + (int)(span * 305.0f);
        int bottom_x = 320 + (int)(span * 236.0f);
        line(c, top_x, 0, bottom_x, 41, PIX_PACK(186, 160, 126));
        line(c, top_x + 1, 0, bottom_x + 1, 41, PIX_PACK(84, 70, 60));
    }
    /* A small cut-out over the pilot's head shows the sky through it. */
    for (y = 0; y < 30; ++y) {
        int half = (int)(64.0f * sqrtf(1.0f - (float)(y * y) / 900.0f));
        pix_t *row = PIX_ROW(c, y);
        int x;
        for (x = 320 - half; x < 320 + half; ++x) row[x] = key;
        if (320 - half - 1 >= 0) row[320 - half - 1] = PIX_PACK(70, 54, 44);
        if (320 + half < SCREEN_W) row[320 + half] = PIX_PACK(70, 54, 44);
    }
    /* Flying wires, interplane and cabane struts. */
    line(c, 92, 46, 246, 262, PIX_PACK(58, 54, 50));
    line(c, 548, 46, 394, 262, PIX_PACK(58, 54, 50));
    line(c, 30, 46, 238, 270, PIX_PACK(66, 60, 54));
    line(c, 610, 46, 402, 270, PIX_PACK(66, 60, 54));
    draw_strut(c, 24, 46, 54, 318, 12, 9);
    draw_strut(c, 616, 46, 586, 318, 12, 9);
    draw_strut(c, 206, 46, 251, 236, 9, 7);
    draw_strut(c, 434, 46, 389, 236, 9, 7);
    SDL_BlitSurface(painting, NULL, c, NULL);
    for (i = 0; i < 4; ++i) erase_painted_needle(c, &cockpit_dials[i]);
    SDL_SetColorKey(c, SDL_TRUE, key);
    return c;
}

/* The armed cockpit, and the 1914 one with no guns on the decking. */
static void build_cockpit_composite(Game *g)
{
    g->cockpit_composite = cockpit_composite_from(g->pilot_cockpit);
    g->cockpit_composite_unarmed =
        cockpit_composite_from(g->pilot_cockpit_unarmed ?
                               g->pilot_cockpit_unarmed : g->pilot_cockpit);
}

static void draw_needle(SDL_Surface *s, const CockpitDial *d, int ox, int oy,
                        float fraction, float sweep)
{
    /* fraction 0..1 round a dial whose zero sits at the bottom-left and
     * sweeps clockwise through 'sweep' radians. */
    float start = sweep > 6.2f ? -(float)M_PI * 0.5f :
                  (float)M_PI * 0.5f + ((float)M_PI * 2.0f - sweep) * 0.5f;
    float angle = start + fraction * sweep;
    int length = d->radius * 78 / 100;
    int cx = d->x + ox, cy = d->y + oy;
    int tx = cx + (int)(cosf(angle) * (float)length);
    int ty = cy + (int)(sinf(angle) * (float)length);
    int bx = cx - (int)(cosf(angle) * (float)length * 0.22f);
    int by = cy - (int)(sinf(angle) * (float)length * 0.22f);
    pix_t colour = d->dark_face ? PIX_PACK(232, 214, 168) : PIX_PACK(28, 22, 20);
    line(s, bx, by, tx, ty, colour);
    line(s, bx + 1, by, tx + 1, ty, colour);
    circle(s, cx, cy, 2, PIX_PACK(166, 126, 60));
}

static void draw_living_cockpit(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t now = SDL_GetTicks();
    int ox = 0, oy = 0;
    float vibration = 0.35f + g->flight_throttle * 0.9f;
    float altitude_fraction;
    if (g->flight_crashed) vibration = 0.0f;
    oy = (int)(sinf((float)now * 0.061f) * vibration + 0.5f);
    if (g->gun_recoil > 0.0f) {
        oy += (int)(g->gun_recoil * 2.4f);
        ox += ((now / 23u) & 1u) ? 1 : -1;
    }
    if (g->shake > 0.0f) {
        ox += (int)(sinf((float)now * 0.093f) * g->shake * 5.0f);
        oy += (int)(cosf((float)now * 0.077f) * g->shake * 4.0f);
    }
    if (g->cockpit_composite) {
        SDL_Rect at;
        SDL_Surface *cockpit = mission_armed(g) || !g->cockpit_composite_unarmed ?
                               g->cockpit_composite : g->cockpit_composite_unarmed;
        at.x = ox; at.y = oy; at.w = SCREEN_W; at.h = SCREEN_H;
        SDL_BlitSurface(cockpit, NULL, s, &at);
    } else if (g->pilot_cockpit) {
        SDL_BlitSurface(g->pilot_cockpit, NULL, s, NULL);
        return;
    }
    draw_needle(s, &cockpit_dials[0], ox, oy,
                flight_clamp(g->airspeed / 160.0f, 0.0f, 1.0f), 5.2f);
    altitude_fraction = fmodf(g->flight_y * 3.28084f / 10000.0f, 1.0f);
    if (altitude_fraction < 0.0f) altitude_fraction += 1.0f;
    draw_needle(s, &cockpit_dials[1], ox, oy, altitude_fraction,
                (float)M_PI * 2.0f);
    draw_needle(s, &cockpit_dials[2], ox, oy,
                flight_clamp(g->flight_crashed ? 0.0f :
                             0.15f + g->flight_throttle * 0.8f +
                             sinf((float)now * 0.013f) * 0.01f, 0.0f, 1.0f),
                4.6f);
    draw_needle(s, &cockpit_dials[3], ox, oy,
                flight_clamp(g->fuel / 100.0f, 0.0f, 1.0f), 3.6f);
}

/* One small brass plate replaces the six signs of the old view. */
static void draw_cockpit_plate(Game *g)
{
    SDL_Surface *s = g->screen;
    char buf[64];
    uint32_t now = SDL_GetTicks();
    int health = g->health > 0 ? g->health : 0;
    blend_rect(s, 8, 54, 176, 38, 24, 18, 14, 170);
    rect(s, 8, 54, 176, 1, PIX_PACK(176, 134, 62));
    rect(s, 8, 91, 176, 1, PIX_PACK(120, 90, 44));
    if (perspective_enemy_total(g) > 0)
        snprintf(buf, sizeof(buf), "AMMO %03d  DOWNED %d/%d",
                 g->ammunition, g->kills, perspective_enemy_total(g));
    else
        snprintf(buf, sizeof(buf), "UNARMED PATROL");
    typewriter_text(g, 14, 59, buf, 7, 11, PIX_PACK(239, 215, 155));
    rect(s, 14, 76, 164, 8, PIX_PACK(40, 30, 24));
    rect(s, 15, 77, 162 * health / 100, 6,
         health > 50 ? PIX_PACK(176, 150, 92) :
         health > 25 ? PIX_PACK(200, 130, 50) : PIX_PACK(200, 60, 40));
    if (g->flight_stalled && ((now / 220u) & 1u))
        draw_field_sign(g, 260, 108, 120, "STALL");
    if (g->notice_until && now < g->notice_until)
        draw_field_sign(g, 200, 140, 240, g->notice);
}

static void show_notice(Game *g, const char *text)
{
    snprintf(g->notice, sizeof(g->notice), "%s", text);
    g->notice_until = SDL_GetTicks() + 1800u;
}

static const AircraftLivery airship_livery = {
    {{150, 150, 140}, {120, 120, 112}, {130, 128, 118}, {90, 90, 84},
     {150, 150, 140}, {70, 68, 62}, {236, 232, 220}, {20, 18, 18},
     {176, 40, 30}, {40, 60, 120}}
};

static const AircraftLivery *enemy_livery(const Game *g, int index)
{
    if (g->special == SPECIAL_AIRSHIP) return &airship_livery;
    if (index == 0 && g->chapter >= 3) return &enemy_liveries[3];
    return &enemy_liveries[(index + g->mission) % 3];
}

static void draw_dogfight_simulator(Game *g)
{
    SDL_Surface *s = g->screen;
    int order[MAX_ENEMIES + 3];
    float depth[MAX_ENEMIES + 3];
    int count = 0;
    int i, j;
    uint32_t now = SDL_GetTicks();
    draw_simulator_terrain(g);
    draw_simulator_clouds(g);
    if (g->special == SPECIAL_AIRSHIP) {
        /* Night over Essex: the world goes dark and the searchlights
         * reach up for the raider. */
        static const int beam_x[3] = {90, 330, 560};
        Projected3D ship = project_world_point(g, (Vec3){
            g->enemies[0].world_x, g->enemies[0].world_y,
            g->enemies[0].world_z});
        int caught = g->enemies[0].active && ship.visible &&
                     ship.x > -200 && ship.x < SCREEN_W + 200 &&
                     ship.y > -200 && ship.y < SCREEN_H;
        blend_rect(s, 0, 0, SCREEN_W, SCREEN_H, 6, 8, 22, 190);
        for (i = 0; i < 3; ++i) {
            float sweep = (float)now * 0.0004f + (float)i * 2.1f;
            int tx = caught ? ship.x + (i - 1) * 6 :
                     320 + (int)(sinf(sweep) * 300.0f);
            int ty = caught ? ship.y : 20 + (int)(cosf(sweep * 0.7f) * 20.0f);
            for (j = -2; j <= 2; ++j)
                line(s, beam_x[i] + j, SCREEN_H, tx + j * 3, ty,
                     rgb(s, 120 + 20 * (2 - abs(j)), 120 + 20 * (2 - abs(j)),
                         96 + 16 * (2 - abs(j))));
        }
    }
    for (i = 0; i < MAX_ENEMIES; ++i) if (g->enemies[i].active) {
        Projected3D p = project_world_point(g, (Vec3){g->enemies[i].world_x,
            g->enemies[i].world_y, g->enemies[i].world_z});
        if (p.visible) { order[count] = i; depth[count++] = p.depth; }
    }
    for (i = 0; i < 3; ++i) if (g->wingmen[i].active) {
        Projected3D p = project_world_point(g, (Vec3){g->wingmen[i].world_x,
            g->wingmen[i].world_y, g->wingmen[i].world_z});
        if (p.visible) { order[count] = MAX_ENEMIES + i; depth[count++] = p.depth; }
    }
    draw_combat_effects(g, 1);
    for (i = 0; i < count; ++i) for (j = i + 1; j < count; ++j) {
        if (depth[i] < depth[j]) {
            float td = depth[i]; int ti = order[i];
            depth[i] = depth[j]; order[i] = order[j];
            depth[j] = td; order[j] = ti;
        }
    }
    for (i = 0; i < count; ++i) {
        Enemy *enemy;
        Vec3 pos;
        AircraftPose pose;
        Projected3D p;
        if (order[i] >= MAX_ENEMIES) {
            Enemy *w = &g->wingmen[order[i] - MAX_ENEMIES];
            pose = pose_from_flight(&w->flight,
                                    (Vec3){w->world_x, w->world_y, w->world_z});
            draw_aircraft_mesh(g, &pose, &allied_livery, 1,
                               w->ai_state == AI_HOMEWARD);
            continue;
        }
        enemy = &g->enemies[order[i]];
        pos = (Vec3){enemy->world_x, enemy->world_y, enemy->world_z};
        pose = pose_from_flight(&enemy->flight, pos);
        p = project_world_point(g, pos);
        draw_aircraft_mesh(g, &pose, enemy_livery(g, order[i]), 0,
                           enemy->crashing || enemy->hp < enemy->max_hp);
        if (now >= enemy->last_shot && now - enemy->last_shot < 42u &&
            p.visible && p.x >= 0 && p.x < SCREEN_W &&
            p.y >= 0 && p.y < SCREEN_H) {
            circle(s, p.x, p.y, 3, rgb(s, 255, 124, 53));
            pixel(s, p.x, p.y, rgb(s, 255, 236, 164));
        }
    }
    draw_combat_effects(g, 0);
    draw_explosions(g);
    draw_dogfight_particles(g, 0);
    draw_living_cockpit(g);
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
    draw_cockpit_plate(g);
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
    if (gunner && mission_armed(g) && g->gunner_moving_guns) {
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
    } else if (gunner && !mission_armed(g)) {
        /* 1914: Tom rides in the back with a map and a pencil, no gun. */
    } else if (gunner && g->gunner_cockpit)
        SDL_BlitSurface(g->gunner_cockpit, NULL, s, NULL);
    else if (!gunner && g->pilot_cockpit)
        SDL_BlitSurface(g->pilot_cockpit, NULL, s, NULL);
    else {
        circle(s, 320, 421, 116, rgb(s, 50, 36, 27));
        circle(s, 320, 421, 96, rgb(s, 90, 61, 37));
        rect(s, 312, 318, 16, 82, rgb(s, 45, 44, 39));
    }
    if (gunner && mission_armed(g)) {
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
    if (mission_armed(g))
        snprintf(buf, sizeof(buf), "AMMO %03d", g->ammunition);
    else
        snprintf(buf, sizeof(buf), "UNARMED");
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
    if (mission_armed(g))
        snprintf(buf, sizeof(buf), "AMMO %03d", g->ammunition);
    else
        snprintf(buf, sizeof(buf), "UNARMED");
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
    display_text(g, is_raf(g) ? 72 : 54, 29, corps_name(g), 12, 18,
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
    typewriter_text(g, 310, 276, is_raf(g) ? "RAF" : "RFC", 7, 11,
                    inscription);
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

/* The paper's account of the sortie, in the careful voice of the censor. */
static void late_edition_text(const Game *g, char *out, size_t size)
{
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    if (g->special == SPECIAL_TRUCE) {
        snprintf(out, size, g->pudding_delivered ?
            "NOT A SHOT WAS FIRED ALONG THE LINE ON CHRISTMAS DAY. AN AIRMAN "
            "OF THE R.F.C. DROPPED A PLUM PUDDING ON THE ENEMY'S AERODROME." :
            "NOT A SHOT WAS FIRED ALONG THE LINE ON CHRISTMAS DAY. MEN OF BOTH "
            "SIDES MET BETWEEN THE TRENCHES.");
    } else if (g->fatal_sortie) {
        snprintf(out, size, "ONE OF OUR MACHINES DID NOT RETURN.");
    } else if (!g->success) {
        snprintf(out, size, "THERE WAS LITTLE ACTIVITY IN THE AIR, OWING "
                 "TO THE WEATHER.");
    } else if (g->special == SPECIAL_AIRSHIP) {
        snprintf(out, size, "AN ENEMY AIRSHIP WAS BROUGHT DOWN IN FLAMES IN "
                 "ESSEX LAST NIGHT. CROWDS CHEERED THE AIRMEN.");
    } else if (g->special == SPECIAL_GOTHA) {
        snprintf(out, size, "DAYLIGHT RAIDERS OVER LONDON. A SCHOOL IN POPLAR "
                 "WAS HIT. OUR AIRMEN BROUGHT DOWN %d OF THE BOMBERS.",
                 g->kills);
    } else if (mode == MISSION_DOGFIGHT_3D) {
        snprintf(out, size, "IN AIR FIGHTING YESTERDAY %d HOSTILE MACHINES "
                 "WERE BROUGHT DOWN. ALL OURS RETURNED.", g->kills);
    } else if (mode == MISSION_GUNNER_3D) {
        snprintf(out, size, "A TWO-SEATER OF THE %s DROVE OFF REPEATED "
                 "ATTACKS AND COMPLETED ITS WORK.", corps_short(g));
    } else {
        snprintf(out, size, "OUR AIRMEN CARRIED OUT MUCH USEFUL WORK BEHIND "
                 "THE ENEMY'S LINES.");
    }
}

static void draw_late_edition(Game *g)
{
    SDL_Surface *s = g->screen;
    char words[200];
    late_edition_text(g, words, sizeof(words));
    rect(s, 410, 62, 196, 172, PIX_PACK(30, 22, 16));
    rect(s, 406, 58, 196, 172, PIX_PACK(222, 210, 178));
    circle(s, 504, 64, 3, PIX_PACK(150, 40, 30));
    typewriter_text(g, 416, 72, "LATE EDITION", 7, 11, PIX_PACK(128, 36, 28));
    typewriter_text(g, 416, 86, NEWS_MASTHEAD, 7, 11, PIX_PACK(36, 30, 26));
    rect(s, 414, 100, 180, 1, PIX_PACK(36, 30, 26));
    wrapped_text_limit(g, 416, 106, words, 25, 15, 1000, 1,
                       PIX_PACK(36, 30, 26));
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
    draw_late_edition(g);
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
    typewriter_text(g, 73, 284, "IN SHREWSBURY, AS THE BELLS RANG, A TELEGRAM", 7, 11, ink);
    typewriter_text(g, 73, 300, "REACHED THE PARENTS OF WILFRED OWEN.", 7, 11, ink);
    typewriter_text(g, 73, 316, "HE HAD BEEN KILLED A WEEK BEFORE.", 7, 11, ink);
    typewriter_text(g, 73, 348, "ENTER  RETURN TO TITLE", 7, 11, ink);
}

/* A WAV as mono 16-bit 11 kHz, the mixer's own rate. */
static uint8_t *load_mono_wav(const char *path, uint32_t *len_out)
{
    SDL_AudioSpec spec;
    SDL_AudioCVT cvt;
    uint8_t *data = NULL;
    uint32_t len = 0;
    int result;
    if (!SDL_LoadWAV(path, &spec, &data, &len)) return NULL;
    result = SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq,
                               AUDIO_S16SYS, 1, 11025);
    if (result < 0) {
        SDL_FreeWAV(data);
        return NULL;
    }
    if (!cvt.needed) {
        *len_out = len;
        return data;
    }
    cvt.len = (int)len;
    cvt.buf = (uint8_t *)SDL_malloc((size_t)len * cvt.len_mult);
    if (!cvt.buf) {
        SDL_FreeWAV(data);
        return NULL;
    }
    memcpy(cvt.buf, data, len);
    SDL_FreeWAV(data);
    if (SDL_ConvertAudio(&cvt) != 0) {
        SDL_free(cvt.buf);
        return NULL;
    }
    *len_out = (uint32_t)cvt.len_cvt;
    return cvt.buf;
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
    g->audio.record_track = RECORD_NONE;
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
    g->screen = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_W, SCREEN_H,
                                               CANVAS_SCREEN_BITS,
                                               CANVAS_SCREEN_FORMAT);
    if (!g->window_surface || !g->screen ||
        g->screen->format->BytesPerPixel != (int)sizeof(pix_t)) {
        fprintf(stderr, "Canvas Skies could not create its framebuffer\n");
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
    g->pilot_cockpit_unarmed = SDL_LoadBMP(CANVAS_PILOT_COCKPIT_UNARMED_BMP);
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
            &g->pilot_cockpit, &g->pilot_cockpit_unarmed, &g->gunner_cockpit,
            &g->gunner_ring, &g->gunner_airframe,
            &g->gunner_moving_guns,
            &g->explosion_atlas, &g->topdown_explosion_atlas
        };
        int i;
        for (i = 0; i < 12; ++i) if (*convert[i]) {
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
                                g->pilot_cockpit, g->pilot_cockpit_unarmed,
                                g->gunner_cockpit,
                                g->gunner_ring, g->gunner_airframe,
                                g->gunner_moving_guns,
                                g->explosion_atlas,
                                g->topdown_explosion_atlas};
        int i;
        for (i = 0; i < 10; ++i) if (keyed[i])
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
    prepare_world_textures(g);
    build_cockpit_composite(g);
    open_joystick(g);
    init_audio(g);
    set_scene(g, SCENE_TITLE);
    return 1;
}

static void advance_after_debrief(Game *g)
{
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
    if (ev->type == SDL_JOYBUTTONDOWN && g->scene != SCENE_SORTIE) {
        /* A pad button works as Enter through the menus and story. */
        SDL_Event key;
        memset(&key, 0, sizeof(key));
        key.type = SDL_KEYDOWN;
        key.key.keysym.scancode = SDL_SCANCODE_RETURN;
        key.key.keysym.sym = SDLK_RETURN;
        g->key_latched[SDL_SCANCODE_RETURN] = 0;
        handle_event(g, &key);
        g->key_latched[SDL_SCANCODE_RETURN] = 0;
        return;
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
    case SDLK_F7:
        g->half_res_world = !g->half_res_world;
        show_notice(g, g->half_res_world ? "DETAIL: HALF" : "DETAIL: FULL");
        break;
    case SDLK_F8:
        g->realistic_flight = !g->realistic_flight;
        if (g->scene == SCENE_SORTIE) {
            /* Hand over cleanly: both models share the attitude, but the
             * full model also needs its airflow components. */
            g->flight.u = g->flight.speed > 20.0f ? g->flight.speed : 40.0f;
            g->flight.v = g->flight.w = 0.0f;
        }
        show_notice(g, g->realistic_flight ? "REALISTIC FLYING" :
                                             "ARCADE FLYING");
        break;
    case SDLK_ESCAPE:
        if (g->scene == SCENE_MENU && g->confirm_overwrite) {
            g->confirm_overwrite = 0;
            g->redraw_static = 1;
        } else if (g->scene == SCENE_TITLE) g->running = 0;
        else if (g->scene == SCENE_SCRAPBOOK) set_scene(g, SCENE_MENU);
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
            if (g->menu_selection == campaign_menu_count(g) - 1) {
                int i;
                g->confirm_overwrite = 0;
                g->scrap_index = 0;
                for (i = 0; i < NEWS_COUNT; ++i)
                    if (news_unlocked(g, i)) { g->scrap_index = i; break; }
                set_scene(g, SCENE_SCRAPBOOK);
                break;
            }
            if (has_continue && g->menu_selection == 0) {
                g->chapter = g->saved_chapter;
                g->mission = g->saved_mission;
                g->campaign_complete = g->saved_campaign_complete;
                g->lives = g->saved_lives;
                memcpy(g->wingman_health, g->saved_wingman_health,
                       sizeof(g->wingman_health));
                restore_saved_career(g);
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
                reset_career(g);
                for (pilot = 0; pilot < 3; ++pilot)
                    g->wingman_health[pilot] = 100;
                apply_story_wingmen(g);
                save_campaign(g);
            }
            set_scene(g, SCENE_STORY);
        }
        else if (g->scene == SCENE_BRIEFING) {
            reset_sortie(g);
            set_scene(g, SCENE_SORTIE);
        } else if (g->scene == SCENE_DEBRIEF) {
            if (g->mission_save_state < 0) {
                g->mission_save_state = 0;
                if (!save_mission_result(g)) break;
            }
            if (g->pending_award || g->pending_rank) {
                set_scene(g, SCENE_AWARD);
                break;
            }
            advance_after_debrief(g);
        } else if (g->scene == SCENE_AWARD) {
            if (!award_reveal_done(g)) {
                /* First press finishes the typing; the next moves on. */
                g->reveal_started = SDL_GetTicks() - 60000u;
                break;
            }
            if (g->pending_award) {
                g->pending_award &= g->pending_award - 1;
            } else {
                g->pending_rank = 0;
            }
            if (g->pending_award || g->pending_rank) {
                g->reveal_started = SDL_GetTicks();
                g->redraw_static = 1;
            } else {
                advance_after_debrief(g);
            }
        } else if (g->scene == SCENE_STORY) {
            if (g->story_stage == 2 && !story_reveal_done(g)) {
                g->reveal_started = SDL_GetTicks() - 60000u;
            } else if (g->story_stage != 2) {
                if (g->story_stage == 0 && thyne_lost_tonight(g))
                    g->story_stage = 1;
                else if (g->story_stage < 2 &&
                         news_index_for(g->chapter, g->mission) >= 0)
                    g->story_stage = 3;
                else
                    g->story_stage = 2;
                g->reveal_started = SDL_GetTicks();
            } else {
                set_scene(g, SCENE_BRIEFING);
            }
        } else if (g->scene == SCENE_EPILOGUE) set_scene(g, SCENE_TITLE);
        else if (g->scene == SCENE_SCRAPBOOK) set_scene(g, SCENE_MENU);
        break;
    case SDLK_LEFT:
    case SDLK_RIGHT:
        if (g->scene == SCENE_SCRAPBOOK) {
            int step = ev->key.keysym.sym == SDLK_LEFT ? -1 : 1;
            int i;
            for (i = g->scrap_index + step; i >= 0 && i < NEWS_COUNT; i += step)
                if (news_unlocked(g, i)) { g->scrap_index = i; break; }
            g->redraw_static = 1;
        }
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{
    Game g;
    uint32_t previous;
    if (argc > 1) use_qa_save_paths();
    if (!game_init(&g)) return 1;
    /* QA entry points keep the normal title/campaign flow untouched while
     * allowing each renderer to be launched directly on real Amiga hardware. */
    if (argc > 1 && strcmp(argv[1], "SIDE") == 0) {
        g.chapter = 2;
        g.mission = 4;
        reset_sortie(&g);
        set_scene(&g, SCENE_SORTIE);
    } else if (argc > 1 && (strcmp(argv[1], "TRUCE") == 0 ||
                            strcmp(argv[1], "AIRSHIP") == 0 ||
                            strcmp(argv[1], "GOTHA") == 0)) {
        /* The three special sorties of the war-news chapters. */
        g.chapter = argv[1][0] == 'T' ? 0 : argv[1][0] == 'A' ? 2 : 3;
        g.mission = argv[1][0] == 'T' ? 11 : 10;
        reset_sortie(&g);
        g.qa_hold = argv[1][0] != 'T';
        set_scene(&g, SCENE_SORTIE);
    } else if (argc > 1 && strcmp(argv[1], "NEWS") == 0) {
        /* The front page before the first day of the Somme. */
        g.chapter = 2;
        g.mission = 4;
        set_scene(&g, SCENE_STORY);
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
        {
            static uint8_t merged[SDL_NUM_SCANCODES];
            memcpy(merged, SDL_GetKeyboardState(NULL), sizeof(merged));
            merge_joystick_keys(&g, merged);
            keys = merged;
        }
        if (g.scene == SCENE_SORTIE) update_sortie(&g, dt, keys);
        /* The officers' menu is static: it is redrawn only when the selection
         * or prompt changes (redraw_static), not every frame. */
        if (g.scene == SCENE_TITLE ||
            g.scene == SCENE_SORTIE || g.scene == SCENE_DEBRIEF ||
            g.scene == SCENE_STORY || g.scene == SCENE_AWARD ||
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
            case SCENE_AWARD: draw_award(&g); break;
            case SCENE_SCRAPBOOK: draw_scrapbook(&g); break;
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
    if (g.pilot_cockpit_unarmed) SDL_FreeSurface(g.pilot_cockpit_unarmed);
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
    if (g.audio.record_data) SDL_free(g.audio.record_data);
    if (g.cockpit_composite) SDL_FreeSurface(g.cockpit_composite);
    if (g.cockpit_composite_unarmed) SDL_FreeSurface(g.cockpit_composite_unarmed);
    free_world_textures(&g);
    if (g.joystick) SDL_JoystickClose(g.joystick);
    if (g.screen) SDL_FreeSurface(g.screen);
    SDL_DestroyWindow(g.window);
    SDL_Quit();
    return 0;
}
