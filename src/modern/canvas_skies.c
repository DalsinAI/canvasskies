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
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "canvas_campaign.h"
#include "canvas_career.h"
#include "canvas_news.h"
#include "canvas_fonts.h"
#include "canvas_topdown_sprites.h"
#include "canvas_machine_sprites.h"
#include "canvas_side_civilians.h"
#include "canvas_rear_carbine.h"
#include "canvas_ticks.h"
#include "canvas_remember.h"
#include "canvas_riflemen.h"

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
#ifndef CANVAS_COCKPIT_REVOLVER_BMP
#define CANVAS_COCKPIT_REVOLVER_BMP "assets/canvas-skies/cockpit-revolver.bmp"
#endif
#ifndef CANVAS_REAR_CARBINE_BMP
#define CANVAS_REAR_CARBINE_BMP "assets/canvas-skies/rear-carbine.bmp"
#endif
#ifndef CANVAS_PILOT_COCKPIT_SINGLE_BMP
#define CANVAS_PILOT_COCKPIT_SINGLE_BMP "assets/canvas-skies/pilot-cockpit-single.bmp"
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
#ifndef CANVAS_MEDALS_BMP
#define CANVAS_MEDALS_BMP "assets/canvas-skies/medals.bmp"
#endif
#ifndef CANVAS_LOSS_CARD_BMP
#define CANVAS_LOSS_CARD_BMP "assets/canvas-skies/loss-card.bmp"
#endif
#ifndef CANVAS_MACHINE_CARDS_BMP
#define CANVAS_MACHINE_CARDS_BMP "assets/canvas-skies/machine-cards.bmp"
#endif
#ifndef CANVAS_OFFICE_PAPER_BMP
#define CANVAS_OFFICE_PAPER_BMP "assets/canvas-skies/office-paper.bmp"
#endif
#ifndef CANVAS_OFFICE_STAMP_BMP
#define CANVAS_OFFICE_STAMP_BMP "assets/canvas-skies/office-stamp.bmp"
#endif
#ifndef CANVAS_MESS_BMP
#define CANVAS_MESS_BMP "assets/canvas-skies/mess.bmp"
#endif
#ifndef CANVAS_MESS_ROWDY_BMP
#define CANVAS_MESS_ROWDY_BMP "assets/canvas-skies/mess-rowdy.bmp"
#endif
#ifndef CANVAS_MESS_WAKE_BMP
#define CANVAS_MESS_WAKE_BMP "assets/canvas-skies/mess-wake.bmp"
#endif
#ifndef CANVAS_COCKPIT_CELLULE_BMP
#define CANVAS_COCKPIT_CELLULE_BMP "assets/canvas-skies/cockpit-cellule.bmp"
#endif
#ifndef CANVAS_CINEMA_HUT_BMP
#define CANVAS_CINEMA_HUT_BMP "assets/canvas-skies/cinema-hut.bmp"
#endif
#ifndef CANVAS_NEWSREELS_BIN
#define CANVAS_NEWSREELS_BIN "assets/canvas-skies/newsreels.bin"
#endif
#ifndef CANVAS_PIANO_WAV
#define CANVAS_PIANO_WAV "assets/canvas-skies/cinema-piano.wav"
#endif
#ifndef CANVAS_NEWSPAPER_TABLE_BMP
#define CANVAS_NEWSPAPER_TABLE_BMP "assets/canvas-skies/newspaper-table.bmp"
#endif
#ifndef CANVAS_SIDE_CIVILIANS_BMP
#define CANVAS_SIDE_CIVILIANS_BMP "assets/canvas-skies/side-civilians.bmp"
#endif
#ifndef CANVAS_RIFLEMEN_BMP
#define CANVAS_RIFLEMEN_BMP "assets/canvas-skies/riflemen.bmp"
#endif
/* QA: one line per sortie flown, in RAM: where it costs nothing. */
#ifndef CANVAS_SORTIE_LOG
#define CANVAS_SORTIE_LOG "RAM:canvas-sortie.log"
#endif
#ifndef CANVAS_TOPDOWN_TARGETS_BMP
#define CANVAS_TOPDOWN_TARGETS_BMP "assets/canvas-skies/topdown-targets.bmp"
#endif
#ifndef CANVAS_TOPDOWN_EXPLOSION_ATLAS_BMP
#define CANVAS_TOPDOWN_EXPLOSION_ATLAS_BMP "assets/canvas-skies/topdown-explosion-atlas.bmp"
#endif
#ifndef CANVAS_FONTS_BMP
#define CANVAS_FONTS_BMP "assets/canvas-skies/fonts.bmp"
#endif
#ifndef CANVAS_PLAYER_TWOSEAT_BMP
#define CANVAS_PLAYER_TWOSEAT_BMP "assets/canvas-skies/aircraft-player-twoseat.bmp"
#endif
#ifndef CANVAS_PLAYER_LATE_BMP
#define CANVAS_PLAYER_LATE_BMP "assets/canvas-skies/aircraft-player-late.bmp"
#endif
#ifndef CANVAS_ENEMY_MONO_BMP
#define CANVAS_ENEMY_MONO_BMP "assets/canvas-skies/aircraft-enemy-mono.bmp"
#endif
#ifndef CANVAS_ENEMY_LATE_BMP
#define CANVAS_ENEMY_LATE_BMP "assets/canvas-skies/aircraft-enemy-late.bmp"
#endif
#ifndef CANVAS_TOPDOWN_TWOSEAT_BMP
#define CANVAS_TOPDOWN_TWOSEAT_BMP "assets/canvas-skies/topdown-player-twoseat.bmp"
#endif
#ifndef CANVAS_TOPDOWN_LATE_BMP
#define CANVAS_TOPDOWN_LATE_BMP "assets/canvas-skies/topdown-player-late.bmp"
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
#ifndef CANVAS_AIRFIELD_FILMS_BIN
#define CANVAS_AIRFIELD_FILMS_BIN "assets/canvas-skies/airfield-films.bin"
#endif
#ifndef CANVAS_REMEMBER_FIELD_BMP
#define CANVAS_REMEMBER_FIELD_BMP "assets/canvas-skies/remember-field.bmp"
#endif
#ifndef CANVAS_REMEMBER_POPPIES_BMP
#define CANVAS_REMEMBER_POPPIES_BMP "assets/canvas-skies/remember-poppies.bmp"
#endif
#ifndef CANVAS_REMEMBER_SKY_BMP
#define CANVAS_REMEMBER_SKY_BMP "assets/canvas-skies/remember-sky.bmp"
#endif
#ifndef CANVAS_REMEMBER_FAR_BMP
#define CANVAS_REMEMBER_FAR_BMP "assets/canvas-skies/remember-far.bmp"
#endif
#ifndef CANVAS_REMEMBER_MID_BMP
#define CANVAS_REMEMBER_MID_BMP "assets/canvas-skies/remember-mid.bmp"
#endif
#ifndef CANVAS_REMEMBER_NEAR_BMP
#define CANVAS_REMEMBER_NEAR_BMP "assets/canvas-skies/remember-near.bmp"
#endif
#ifndef CANVAS_REMEMBER_LIFE_BMP
#define CANVAS_REMEMBER_LIFE_BMP "assets/canvas-skies/remember-life.bmp"
#endif
#ifndef CANVAS_REMEMBER_WIND_WAV
#define CANVAS_REMEMBER_WIND_WAV "assets/canvas-skies/remember-wind.wav"
#endif
#ifndef CANVAS_WE_REMEMBER_TXT
#define CANVAS_WE_REMEMBER_TXT "PROGDIR:WeRemember.txt"
#endif
#ifndef CANVAS_PREFS_PATH
#define CANVAS_PREFS_PATH "PROGDIR:CanvasSkies.prefs"
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
#define MAX_TARGETS 20
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
    SCENE_SCRAPBOOK,
    SCENE_DAWN,     /* the hangars before take-off */
    SCENE_MESS,     /* the mess that evening, after a mission won */
    SCENE_CONDUCT,  /* the Officer Commanding's memorandum */
    SCENE_COURT_MARTIAL, /* the end of a career in disgrace */
    SCENE_CINEMA,   /* the picture house, a chapter's first morning */
    SCENE_SETTINGS, /* the officer's preferences */
    SCENE_FILM,     /* a take-off or a landing, on old film */
    SCENE_REMEMBER  /* the old front in 1919, and the roll */
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
      "RETURN TO THE WESTERN FIELD."}, 2, 4, 3, 1, 3800.0f},
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
      "BE HOME WHEN THE BELLS BEGIN."}, 1, 3, 10, 1, 3600.0f}
};

typedef struct {
    float x, y, vx, vy;
    float life, initial_life;
    int enemy;
    int type;
    int active;
    int missed;  /* an enemy round that went wide of the rear seat */
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
    float power, agility;   /* the machine's engine and handling, 1 = Camel */
} FlightState;

/* ---- The machines, year by year ---------------------------------------
 *
 * Our side flies a two-seat reconnaissance machine in 1914, a single-seat
 * scout with one synchronised Vickers from 1915, a twin-gun scout from the
 * Cambrai chapter and a faster late-war scout in 1918.  The enemy flies a
 * monoplane in 1915, a streamlined biplane scout through 1916 and 1917
 * and a square-winged biplane in 1918.  The shapes are our own; the
 * numbers follow the period: power and agility scale the flight models,
 * toughness divides the damage taken. */
typedef struct {
    const char *name;          /* as the squadron calls it */
    const char *arrival;       /* the diary on the day they arrive */
    float span, lower_span, chord, gap, stagger;
    int monoplane, two_seat;
    float power, agility, toughness;
    int guns, fire_ms;
} MachineType;

static const MachineType our_machines[4] = {
    {"THE TWO-SEATER", NULL, 11.2f, 10.4f, 1.7f, 1.9f, 0.3f, 0, 1,
     0.82f, 0.78f, 1.25f, 0, 0},
    {"THE SCOUT", "New scouts at the aircraft park: one Vickers each, "
     "firing through the propeller. We are fighting pilots now.",
     8.2f, 7.6f, 1.5f, 1.5f, 0.35f, 0, 0, 0.92f, 1.05f, 0.95f, 1, 150},
    {"THE TWIN-GUN SCOUT", "The new machines came up today: twin guns "
     "and a temper. Tom says it turns like a cat on a hot stove.",
     8.5f, 8.5f, 1.45f, 1.5f, 0.45f, 0, 0, 1.0f, 1.2f, 1.0f, 2, 160},
    {"THE LATE SCOUT", "Fresh machines from England, faster than "
     "anything we have flown. They do not forgive a clumsy hand.",
     8.1f, 8.1f, 1.55f, 1.6f, 0.4f, 0, 0, 1.18f, 1.08f, 1.15f, 2, 140}
};

static const MachineType their_machines[3] = {
    {"THE FOKKER MONOPLANE", "Their monoplane has a gun that fires "
     "through the propeller. Nobody here can explain it.",
     9.5f, 0.0f, 1.8f, 0.0f, 0.0f, 1, 0, 0.85f, 1.0f, 0.85f, 1, 0},
    {"THE ALBATROS", "Their new scouts are sleek as fish, and carry two "
     "guns. April will be hard.",
     9.0f, 8.6f, 1.6f, 1.5f, 0.3f, 0, 0, 1.0f, 1.0f, 1.0f, 2, 0},
    {"THE NEW FOKKER", "A square-winged Fokker has come to their front. "
     "It climbs as if the air were thicker for it.",
     8.9f, 7.0f, 1.6f, 1.7f, 0.5f, 0, 0, 1.12f, 1.12f, 1.1f, 2, 0}
};

static int mesh_chapter;   /* the chapter whose machines the meshes show */

static int our_machine_index(int chapter)
{
    return chapter == 0 ? 0 : chapter < 4 ? 1 : chapter < 5 ? 2 : 3;
}

static int their_machine_index(int chapter)
{
    return chapter <= 1 ? 0 : chapter < 5 ? 1 : 2;
}

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
    int player_hit;   /* our own rounds have struck it */
    float ai_timer, ai_side, smoke_timer;
} Enemy;

typedef struct {
    float world_x;
    int active;
    int type;
    uint32_t last_shot;
    uint32_t next_round;
    int burst_left;
    int civilian;       /* side-on: 0, or the SC_ sprite of a civilian place */
    int wrecked;
    int hazard;         /* a post met on the way, not one of the targets */
    int task;           /* side-on: TASK_ for 1914's work, or 0 */
    int done;           /* the task is done */
} Target;

/* ---- Top-down missions built from segments --------------------------------
 *
 * A top-down sortie from 1915 on is a run of ground segments, each with its
 * own targets: a road with a convoy, a railway with a train and a station,
 * a base, a village, a trench line.  The mix for each sortie is chosen by
 * date (see topdown_mixes).  Civilians, ambulances and our own line are on
 * the ground too, and hitting them is an offence. */
typedef enum {
    SEG_NONE, SEG_CONVOY, SEG_DEPOT, SEG_RAIL, SEG_STATION_VILLAGE,
    SEG_BASE, SEG_VILLAGE, SEG_TRENCH, SEG_AMBULANCES,
    SEG_COLUMN, SEG_HQ, SEG_BATTERY   /* the observers' work */
} SegmentKind;

typedef enum {
    TDK_LORRY, TDK_AMBULANCE, TDK_LOCO, TDK_WAGON, TDK_STATION, TDK_HANGAR,
    TDK_TENTS, TDK_DUMP, TDK_DEPOT, TDK_FARM, TDK_CHURCH, TDK_CART,
    TDK_REFUGEES, TDK_MG, TDK_DUGOUT, TDK_COLUMN, TDK_HQ, TDK_BATTERY,
    TDK_COUNT
} TdKind;

/* The observers' work, 1914 and the last days: mark a column on the map,
 * drop a message bag on a headquarters, expose a plate over a battery. */
enum { TASK_NONE, TASK_MARK, TASK_DROP, TASK_PHOTO };

enum { CIV_NONE, CIV_CIVILIAN, CIV_AMBULANCE, CIV_OWN_LINE };

typedef struct {
    float world_x, x, speed;   /* speed: world units a second, down-screen */
    int kind, hp, active, wrecked;
    uint32_t last_shot, wrecked_at;
    int hazard;                /* met on the way: not one of the targets */
    int done;                  /* a task: marked, delivered or photographed */
} TdObject;

typedef struct {
    int sprite_a, sprite_b;    /* tiles; b alternates with a when set */
    float x, from, to;         /* screen column and world range */
} TdStrip;

enum {
    MIX_NONE, MIX_CONVOY_DEPOT, MIX_RAILWAY, MIX_AERODROME, MIX_TRENCH_RAILHEAD,
    MIX_VILLAGE_STATION, MIX_TRENCH_RUN, MIX_BASE_AMBULANCES,
    MIX_CONVOY_AMBULANCES, MIX_FIRST_MESSAGE, MIX_COLUMNS, MIX_EMPTY_FLANK,
    MIX_TURN_OF_ARMIES, MIX_LAST_ORDERS, MIX_WHITE_TRAIN, MIX_COUNT
};

typedef struct {
    uint8_t *data;              /* the reel's bytes, read from disk */
    uint32_t len, cursor, started;
    int frames, shown, index;
    char title[41];
    uint8_t pix[192 * 120];     /* the frame being shown, 0..15 */
} Newsreel;

/* One of the aerodrome films (make_airfield_films.py): the newsreel codec
 * at the size its file names, with its own soundtrack. */
typedef struct {
    uint8_t *data;              /* the film's picture bytes */
    uint8_t *pix;               /* the frame being shown, 0..15 */
    int16_t *sound;             /* big-endian 16-bit mono */
    uint32_t len, cursor, started, sound_len;
    int w, h, fps, frames, shown, index;
} AirfieldFilm;

#define MAX_TD_OBJECTS 64
#define MAX_TD_STRIPS 10


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

/* ---- The shape of a sortie, and what is thrown at it on the way --------
 *
 * Every sortie is flown in legs: out over the lines to the task, the task
 * itself, then a short run home that is never empty, and in the side-on
 * and top-down views a landing at the aerodrome.  On the way the ground
 * and the sky keep the pilot busy: Archie ranging on him with one puff
 * and bursting where he would have been, riflemen standing up out of
 * trenches and shell holes for a pot shot, machine guns hosing tracer up
 * in arcs, kite balloons on their cables, trees at low level, and a squall
 * now and then to go round or through. */
enum { LEG_OUT, LEG_HOME, LEG_LANDING };

/* A round of Archie: a ranging puff that does no harm, and the aimed
 * burst that follows it to where the machine was going. */
enum { ARCHIE_RANGING = 1, ARCHIE_BURST };

typedef struct {
    float x, y;      /* side: world x, screen y; top-down: screen x, world y */
    float tx, ty;    /* where the aimed burst will come */
    Vec3 p, tp;      /* the same in the cockpit's world */
    float age, fuse;
    int kind;
    int active;
} Archie;

enum { RIFLE_HIDDEN, RIFLE_RISE, RIFLE_AIM, RIFLE_FIRED, RIFLE_DUCK, RIFLE_DOWN };

typedef struct {
    float x, y;      /* side: world x; top-down: screen x, world y */
    float timer, tracer;
    float aim_x, aim_y;   /* screen point the last round went towards */
    int state, stand, cover, shots;
    int active;
} Rifleman;

#define MAX_ARCHIE 10
#define MAX_RIFLEMEN 8
#define MAX_FABRIC_HOLES 8

typedef struct {
    float a, b;      /* side: world x span; top-down: world y span */
    float lo, hi;    /* side: the screen rows it fills; top: its columns */
    Vec3 centre;     /* in the cockpit: the storm's foot */
    int active;
} Squall;

typedef struct {
    float x, y;      /* side: world x and balloon height; top: screen x, world y */
    float drift;     /* a cut balloon rising away */
    int active, cut;
} KiteBalloon;

/* 1914 from the cockpit: one of the observer's jobs on the ground. */
typedef struct {
    float x, z;          /* where it lies */
    float progress;      /* seconds held level over it, for the plate */
    int kind, done;
} ObserverTask;

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
    volatile int crack;   /* a single rifle or revolver shot */
    volatile int plink;   /* a rifle bullet through the fabric or the wires */
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
    const int16_t *wind;        /* the wind over the old front, looped */
    uint32_t wind_len, wind_pos;
    const int16_t *film_sound;  /* the aerodrome film's soundtrack */
    uint32_t film_len, film_pos;
    float music_gain;     /* the settings' two sliders, 0..1 */
    float effects_gain;
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
    SDL_Surface *pilot_cockpit_single;  /* 1915 to spring 1917: one Vickers */
    SDL_Surface *cockpit_revolver;      /* 1914: two frames, rest and kick */
    SDL_Surface *rear_carbine;          /* 1914: the carbine at every aim */
    SDL_Surface *gunner_cockpit;
    SDL_Surface *gunner_ring;
    SDL_Surface *gunner_airframe;
    SDL_Surface *gunner_moving_guns;
    SDL_Surface *explosion_atlas;
    SDL_Surface *topdown_explosion_atlas;
    SDL_Surface *topdown_targets;   /* the painted mission targets */
    SDL_Surface *side_civilians;    /* farms, a church, carts, side-on */
    SDL_Surface *newspaper_table;   /* the paper on its bundle, untyped */
    SDL_Surface *cinema_hut;        /* the picture house, its screen dark */
    SDL_Surface *medals;            /* the decorations and the office crest */
    SDL_Surface *mess;              /* the squadron mess by lamplight */
    SDL_Surface *mess_rowdy;        /* the same, the night of a victory */
    SDL_Surface *mess_wake;         /* the same, the night of a loss */
    SDL_Surface *machine_cards;     /* the papers of the new-machines cards */
    SDL_Surface *office_paper;      /* foolscap on the adjutant's desk */
    SDL_Surface *office_stamp;      /* the rubber stamp's frame and its wear */
    SDL_Surface *loss_card;         /* the empty chair, by candlelight */
    Newsreel newsreel;
    AirfieldFilm film;
    SDL_Surface *remember_field;    /* the old front in summer 1919 */
    SDL_Surface *remember_layer[4]; /* or the same as sky, far, mid, near */
    SDL_Surface *remember_life;     /* butterflies, a lark, thistledown */
    SDL_Surface *remember_poppies;  /* poppies and grass in the wind */
    char **remember_names;          /* the roll, from WeRemember.txt */
    int remember_count;
    Scene film_next;        /* where the film leads when it ends */
    int cinema_reel_slot;
    int cinema_seen;                /* chapter + 1 whose reels were shown */
    SDL_Surface *fonts;     /* pre-rendered glyph coverage, 8 bits */
    SDL_Surface *player_sprites;
    SDL_Surface *enemy_sprites;
    /* Other years' machines: our two-seater and late scout, their
     * monoplane and late biplane (side-on), and our two from above. */
    SDL_Surface *machine_sprites[4];
    SDL_Surface *topdown_machines[2];
    SDL_Surface *cockpit_composite;
    SDL_Surface *dawn_backdrop;
    SDL_Surface *cockpit_composite_unarmed;
    SDL_Surface *cockpit_composite_single;
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
    float blast_flash;   /* side-on: our own bomb went off too close */
    float gun_recoil;
    SDL_Joystick *joystick;
    AudioState audio;
    Scene scene;
    int running;
    int fullscreen;
    /* Settings, kept in canvas-skies.prefs beside the save. */
    int cache_in_ram;       /* keep art and records once loaded */
    int music_volume;       /* 0..10 */
    int effects_volume;     /* 0..10 */
    int settings_row;
    Scene settings_from;    /* where the settings page returns to */
    unsigned assets_loaded; /* the AssetGroup bits held now */
    unsigned long fast_at_start;    /* free fast memory before loading */
    uint32_t window_restore_at;  /* back from full screen: tidy up then */
    int redraw_static;
    int qa_hold;
    int qa_diary_all;   /* test entry: turn every diary page in turn */
    int qa_tracers;     /* test entry: rounds fired at every angle */
    int qa_autopilot;   /* test entry: fly a top-down mix as a pilot would */
    int arms_rounds;        /* 1914: rounds left in the carbine or revolver */
    uint32_t arms_ready_at; /* the bolt worked, or the reload done */
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
    int saved_grounded;
    /* Pilot's record: victories, decorations, rank and growing skills. */
    int victories, sorties, rank;
    /* Conduct, kept with the career: the squadron's morale, offences
     * against civilians, ambulances or our own line (three is a court
     * martial), sorties left before an award may be made, rank lost. */
    int morale, offences, award_hold, rank_penalty, disgraced;
    int grounded_next;          /* the next sortie is flown without him */
    int conduct_pending;        /* a memorandum waits after the debrief */
    int offence_last_sortie;    /* the diary and the paper still remember */
    int court_martial_stage;
    unsigned awards;
    int saved_victories, saved_sorties, saved_rank;
    int saved_conduct[5];       /* morale, offences, hold, penalty, disgraced */
    unsigned saved_awards;
    float saved_skill[3];
    int pending_award, pending_rank;
    int career_counted;
    int story_stage;
    int mess_done;  /* this sortie's evening in the mess has been shown */
    int scrap_index;
    int special;           /* MissionSpecial of the sortie being flown */
    int pudding_delivered; /* Christmas 1914 */
    int raider_through;    /* the airship or Gothas reached London */
    uint32_t wreck_started; /* side, top-down and rear-seat crash hold */
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
    TdObject td[MAX_TD_OBJECTS];
    int td_count;
    TdStrip td_strips[MAX_TD_STRIPS];
    int td_strip_count;
    int td_mix, td_goal;
    int sortie_offences;       /* civilians, ambulances, our line: today */
    int sortie_offence_kind;   /* the last of them, CIV_... */
    BulletHole bullet_holes[MAX_BULLET_HOLES];
    int next_bullet_hole;
    /* The legs of the sortie and what is sent against it on the way. */
    int leg;                /* LEG_OUT, LEG_HOME, LEG_LANDING */
    int task_total;         /* the observers' tasks on this sortie */
    int order_met_told;     /* the "order met" notice has been shown */
    ObserverTask obs[4];    /* 1914 cockpit: Tom's jobs on the patrol */
    int obs_count, obs_bag_active, obs_signals;
    Vec3 obs_bag_p, obs_bag_v, obs_spot_at;
    float obs_spot_clock, obs_spot_pending, obs_spot_age;
    float leg_time;         /* seconds into the leg */
    float sortie_clock;     /* seconds since take-off */
    float front_at;         /* side, top-down: where their ground begins */
    float task_end;         /* where the last of the task lies */
    float home_line_at;     /* on the way home: where our lines begin */
    float field_at;         /* where the aerodrome lies */
    float home_progress;    /* cockpit: metres flown homeward */
    int landed;
    uint32_t landed_at;
    int wave, waves;        /* cockpit and rear seat: waves sent, and due */
    float wave_clock;
    float shadow_time;      /* 1914 cockpit: seconds spent on his tail */
    float hazard_clock, archie_clock, rifle_clock;
    float player_vx;        /* top-down: sideways drift, for the gunners */
    int bags;               /* message bags aboard */
    int photo_frames;       /* plates exposed on the battery so far */
    float photo_hold;       /* seconds held level over the battery */
    float tree_struck;      /* seconds since the last tree or cable */
    Archie archie[MAX_ARCHIE];
    Rifleman riflemen[MAX_RIFLEMEN];
    Squall squall;
    KiteBalloon balloon;
    float fabric_holes[MAX_FABRIC_HOLES][2];
    int fabric_hole_count;
    /* QA: how much of the sortie asked nothing of the pilot. */
    float qa_time, qa_idle, qa_idle_run, qa_idle_longest;
    int qa_logged;
    int qa_run;             /* test entry: a real sortie that ends */
    SDL_Surface *riflemen_sheet;
    /* The sortie's own clock: the same clamped steps the flying uses, in
     * milliseconds, so a slow frame never lets a gun fire faster than the
     * world moves.  Wall-clock time stays for films, scenes and sound. */
    uint32_t sim_ms;
    float sim_carry;
    /* QA: rounds fired each way, and damage taken, for the sortie log. */
    int qa_shots, qa_enemy_shots, qa_damage, qa_last_health;
    /* QA: the targets on offer, those left unreached when the run home
     * began, and the bombs carried. */
    int qa_offered, qa_unreached, qa_bombs_carried;
} Game;

/* The sortie clock starts here rather than at nought, so a time of nought
 * ("never") is always long past. */
#define SIM_EPOCH 10000u

static uint32_t sim_now(const Game *g)
{
    return g->sim_ms;
}

static void flight_state_init(FlightState *f, float heading, float pitch,
                              float bank, float speed, float throttle);
static void reset_wingmen(Game *g);
static void show_notice(Game *g, const char *text);
static float random_unit(Game *g);
static int personal_shot(Game *g, int revolver);
static void ensure_scene_assets(Game *g, Scene scene);
static void ensure_music(Game *g, int mood);
static int film_start_for(Game *g, Scene from, Scene to);
static void remember_start(Game *g);
static void remember_free(Game *g);
static void carbine_muzzle_position(const Game *g, float *x, float *y);
static void td_build_mission(Game *g);
static void qa_log_sortie(Game *g);
static int observer_next(const Game *g);
static void draw_puff_scaled(SDL_Surface *s, int size, int shape, int cx,
                             int cy, int w, int h, pix_t top, pix_t bottom,
                             int opacity);
static int update_gunner_legs(Game *g, float dt);
static void shift_frame(SDL_Surface *s, int ox, int oy);
static void cinema_start(Game *g);
static int cinema_reels_present(void);
static void newsreel_free(Newsreel *r);
static void td_bomb_on_own_line(Game *g, float x, float y);
static void td_update(Game *g, float dt, uint32_t now);
static void td_draw_ground(Game *g);
static void td_draw_objects(Game *g);
static void draw_top_task(Game *g, const TdObject *o, int sx, int sy);
static float wrap_angle(float angle);
static Vec3 flight_rotate_from_ned(const FlightState *f, float north,
                                   float east, float down);

/* QA: "SIDE n VILLAGE STILL" stops the scroll with the farm, the church
 * and the cart standing side by side in view. */
static int qa_still;

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

/* 1914: no machine guns yet.  The observer carries a cavalry carbine and
 * the pilot his service revolver; the enemy is armed the same way.  Single
 * aimed shots at short range, few of which strike, so bringing a machine
 * down is rare. */
#define CARBINE_ROUNDS 5
#define REVOLVER_ROUNDS 6
#define PERSONAL_HIT_CHANCE 0.35f
static int personal_arms(const Game *g)
{
    return !chapters[g->chapter].armed &&
           campaign_missions[g->chapter][g->mission].special != SPECIAL_TRUCE;
}

/* The last days of the war: "do not fire unless fired upon".  Apart from
 * "No Easy Truce", the Armistice chapter's air patrols meet an enemy who
 * waves and turns away.  The guns are fitted but nobody fights, and
 * flying the patrol out is the task, as in unarmed 1914. */
static int armistice_quiet(const Game *g)
{
    return g->chapter == CHAPTER_COUNT - 1 && g->mission != 2 &&
           is_perspective_mode(campaign_missions[g->chapter][g->mission].mode);
}

/* Tom is wounded from "The Empty Seat" (1916) until "Tom Returns"
 * (1917); a stand-in flies the two-seater meanwhile. */
static int tom_wounded(const Game *g)
{
    return (g->chapter == 2 && g->mission >= 8) ||
           (g->chapter == 3 && g->mission < 7);
}

/* The first wingman wounded and sent home today, or -1. */
static int wingman_hit(const Game *g)
{
    int i;
    for (i = 0; i < 3; ++i) if (g->wingman_hit_today[i]) return i;
    return -1;
}

/* Whether the enemy in the sky means to fight. */
static int enemies_hostile(const Game *g)
{
    return mission_armed(g) && !armistice_quiet(g);
}

/* QA entry points (DOGFIGHT, DEATH, CHAPTER_SUCCESS...) write their own
 * save file so testing on a player's install never replaces the campaign. */
static const char *save_path = CANVAS_SAVE_PATH;
static const char *save_new_path = CANVAS_SAVE_NEW_PATH;
static const char *save_backup_path = CANVAS_SAVE_BACKUP_PATH;

static int qa_save_in_use;

static void use_qa_save_paths(void)
{
    qa_save_in_use = 1;
    save_path = "PROGDIR:CanvasSkies-QA.save";
    save_new_path = "PROGDIR:CanvasSkies-QA.save.new";
    save_backup_path = "PROGDIR:CanvasSkies-QA.save.bak";
}

/* The wingmen carry their wounds from one sortie to the next
 * (canvas_career.h has the arithmetic); the story only adds what it has
 * written.  Gridpipe Thyne turns east and is lost in "Thyne's Last Turn"
 * (Chapter 5, Mission 9); from the next sortie onward his place stays
 * empty.  Whatever else the war has done to the flight is left alone. */
static void apply_story_wingmen(Game *g)
{
    career_story_wingmen(g->wingman_health,
                         g->chapter > 4 || (g->chapter == 4 && g->mission > 8));
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

/* Whether he is in the flight this morning: not lost, not down with the
 * influenza, and not still mending from an earlier sortie. */
static int wingman_fit(const Game *g, int pilot)
{
    return career_wingman_fit(g->wingman_health[pilot]) &&
           !wingman_sick(g, pilot);
}

/* QA: the flight and the continuation point as the save now has them,
 * printed to the shell for the lab's notes.  Only the test entries,
 * which keep their own save, say anything. */
static void qa_report_flight(const Game *g, const char *when)
{
    int i;
    if (!qa_save_in_use) return;
    printf("QA %s: CHAPTER %d MISSION %d GROUNDED %d", when,
           g->chapter + 1, g->mission + 1, g->grounded_next);
    for (i = 0; i < 3; ++i)
        printf("  %s %d %s", allied_pilot_names[i], g->wingman_health[i],
               g->wingman_health[i] <= 0 ? "LOST" :
               wingman_sick(g, i) ? "SICK" :
               career_wingman_fit(g->wingman_health[i]) ? "FIT" : "SICK BAY");
    printf("\n");
    fflush(stdout);
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
    CareerRecord r;
    int ok;
    if (!file) return 0;
    ok = career_record_read(file, &r, CHAPTER_COUNT, MISSIONS_PER_CHAPTER);
    fclose(file);
    if (!ok) return 0;
    g->saved_victories = r.victories;
    g->saved_sorties = r.sorties;
    g->saved_awards = r.awards;
    g->saved_rank = r.rank;
    g->saved_skill[0] = (float)r.skill[0] / 100.0f;
    g->saved_skill[1] = (float)r.skill[1] / 100.0f;
    g->saved_skill[2] = (float)r.skill[2] / 100.0f;
    memcpy(g->saved_conduct, r.conduct, sizeof(r.conduct));
    g->saved_chapter = r.chapter;
    g->saved_mission = r.mission;
    memcpy(g->saved_wingman_health, r.wingman, sizeof(r.wingman));
    g->saved_campaign_complete = r.complete;
    g->saved_lives = r.lives;
    g->saved_grounded = r.grounded;
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
    CareerRecord r;
    if (!file) return 0;
    career_record_defaults(&r);
    r.chapter = g->chapter;
    r.mission = g->mission;
    memcpy(r.wingman, g->wingman_health, sizeof(r.wingman));
    r.complete = g->campaign_complete;
    r.lives = g->lives;
    r.victories = g->victories;
    r.sorties = g->sorties;
    r.awards = g->awards;
    r.rank = g->rank;
    r.skill[0] = (int)(g->skill_flying * 100.0f + 0.5f);
    r.skill[1] = (int)(g->skill_gunnery * 100.0f + 0.5f);
    r.skill[2] = (int)(g->skill_engine * 100.0f + 0.5f);
    r.conduct[0] = g->morale;
    r.conduct[1] = g->offences;
    r.conduct[2] = g->award_hold;
    r.conduct[3] = g->rank_penalty;
    r.conduct[4] = g->disgraced;
    r.grounded = g->grounded_next;
    ok = career_record_write(file, &r);
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
    g->saved_grounded = g->grounded_next;
    g->saved_victories = g->victories;
    g->saved_sorties = g->sorties;
    g->saved_awards = g->awards;
    g->saved_rank = g->rank;
    g->saved_skill[0] = g->skill_flying;
    g->saved_skill[1] = g->skill_gunnery;
    g->saved_skill[2] = g->skill_engine;
    g->saved_conduct[0] = g->morale;
    g->saved_conduct[1] = g->offences;
    g->saved_conduct[2] = g->award_hold;
    g->saved_conduct[3] = g->rank_penalty;
    g->saved_conduct[4] = g->disgraced;
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

static int earned_rank_before_conduct(const Game *g)
{
    if (g->victories >= 25 && g->chapter >= 5) return 3;
    if (g->victories >= 12 && g->chapter >= 3) return 2;
    if (g->victories >= 3 || g->sorties >= 10) return 1;
    return 0;
}

static int earned_rank(const Game *g)
{
    int rank = earned_rank_before_conduct(g) - g->rank_penalty;
    return rank < 0 ? 0 : rank;
}

static void reset_career(Game *g)
{
    g->victories = g->sorties = g->rank = 0;
    g->awards = 0;
    g->skill_flying = g->skill_gunnery = g->skill_engine = 0.0f;
    g->pending_award = 0;
    g->pending_rank = 0;
    g->morale = 70;
    g->offences = g->award_hold = g->rank_penalty = g->disgraced = 0;
    g->grounded_next = g->conduct_pending = 0;
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
    g->morale = g->saved_conduct[0];
    g->offences = g->saved_conduct[1];
    g->award_hold = g->saved_conduct[2];
    g->rank_penalty = g->saved_conduct[3];
    g->disgraced = g->saved_conduct[4];
    /* A grounding owed when he last left the mess is still owed. */
    g->grounded_next = g->saved_grounded;
    g->conduct_pending = 0;
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
    /* The flight's wounds go into the books with the rest of the day.
     * Only the cockpit patrols put the wingmen in the air; on any other
     * sortie their machines stay on the field and the men simply rest.
     * A round that strikes a wingman takes one of his three lives of
     * fabric and wire (wingmen[].hp), and the third sends him home. */
    for (i = 0; i < 3; ++i) {
        const Enemy *w = &g->wingmen[i];
        int hits = w->max_hp - (w->hp < 0 ? 0 : w->hp);
        g->wingman_health[i] =
            career_wingman_after_sortie(g->wingman_health[i],
                                        w->max_hp > 0 ? hits : 0,
                                        g->wingman_hit_today[i]);
    }
    g->pending_award = 0;
    /* A pilot who goes down shows no decoration card that evening, so the
     * decoration waits for the next sortie he comes home from. */
    for (i = 0; i < 5 && !g->fatal_sortie; ++i)
        if (!(g->awards & (1u << i)) && g->victories >= decorations[i].victories) {
            g->awards |= 1u << i;
            g->pending_award |= 1 << i;
        }
    /* Conduct.  Each civilian place, ambulance or length of our own line
     * hit is an offence.  The first brings a reprimand on the record and
     * no award for three sorties; the second costs a rank, or grounds a
     * pilot who has none to lose; the third is a court martial. */
    if (g->award_hold > 0) {
        --g->award_hold;
        g->awards &= ~(unsigned)g->pending_award;
        g->pending_award = 0;
    }
    if (g->sortie_offences > 0) {
        int before = g->offences;
        g->morale -= 15 * g->sortie_offences;
        if (g->morale < 0) g->morale = 0;
        g->offences += g->sortie_offences;
        if (g->offences > 3) g->offences = 3;
        if (before < 1) g->award_hold = 3;
        g->awards &= ~(unsigned)g->pending_award;
        g->pending_award = 0;
        if (before < 2 && g->offences >= 2 && g->offences < 3) {
            if (g->rank > 0) {
                ++g->rank_penalty;
                --g->rank;
            } else {
                g->grounded_next = 1;
            }
        }
        g->conduct_pending = 1;
        g->offence_last_sortie = 1;
    } else if (g->success && g->morale < 90) {
        g->morale += 2;
    }
    rank = earned_rank(g);
    if (g->fatal_sortie) rank = g->rank;
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
    int active_grounded = g->grounded_next;
    int ok, game_over;
    if (g->fatal_sortie && !g->life_lost_this_sortie) {
        if (g->lives > 0) --g->lives;
        g->life_lost_this_sortie = 1;
    }
    count_sortie_in_record(g);
    /* The debrief still describes the sortie just flown, but the durable
     * continuation point must already describe what comes next.  A failed
     * sortie deliberately saves the same mission so Retry remains honest.
     * A grounded pilot's next sortie is struck off here too, by the same
     * rule the evening uses, so quitting after the debrief and continuing
     * cannot fly the sortie the squadron flew without him. */
    if (g->success &&
        career_advance(&g->chapter, &g->mission, &g->campaign_complete,
                       &g->grounded_next, CHAPTER_COUNT,
                       MISSIONS_PER_CHAPTER) == CAREER_NEXT_CHAPTER) {
        /* A fresh chapter brings a fresh start: three lives again, so
         * one bad spell costs a chapter rather than the whole war. */
        g->lives = 3;
    }
    game_over = !g->success && g->fatal_sortie && g->lives <= 0;
    if (game_over) {
        /* The last life lost: the war goes on from the start of this
         * chapter with three lives, and the pilot's record is kept, so one
         * bad spell costs a chapter rather than the whole career. */
        g->mission = 0;
        g->lives = 3;
    }
    apply_story_wingmen(g);
    ok = save_campaign(g);
    if (ok) qa_report_flight(g, "SAVED");
    if (game_over) g->lives = 0;   /* tonight still ends at the memorial */
    /* The evening's cards still belong to the sortie just flown: the
     * memorandum must still know he is grounded, and advance_after_debrief
     * moves on from here by the same rule as above. */
    g->chapter = active_chapter;
    g->mission = active_mission;
    g->campaign_complete = active_complete;
    g->grounded_next = active_grounded;
    apply_story_wingmen(g);
    g->mission_save_state = ok ? 1 : -1;
    return ok;
}

/* The new-campaign commissions, any saved campaign, and the cuttings book
 * which is always the last plaque. */
static int campaign_menu_count(const Game *g)
{
    /* The commissions, the book of press cuttings and the preferences. */
    return ((g->has_save && g->saved_lives > 0 && !g->saved_conduct[4]) ?
            7 : 6) + 2;
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
    if (dst->w * SCREEN_H < dst->h * SCREEN_W) {
        dest.w = dst->w;
        dest.h = dst->w * SCREEN_H / SCREEN_W;
    } else {
        dest.h = dst->h;
        dest.w = dst->h * SCREEN_W / SCREEN_H;
    }
    dest.x = (dst->w - dest.w) / 2;
    dest.y = (dst->h - dest.h) / 2;
    /* Only the letterbox bars need clearing: the frame covers the rest. */
    if (dest.w < dst->w || dest.h < dst->h) {
        SDL_Rect bar;
        black = SDL_MapRGB(dst->format, 0, 0, 0);
        bar.x = 0; bar.y = 0; bar.w = dst->w; bar.h = dest.y;
        if (bar.h > 0) SDL_FillRect(dst, &bar, black);
        bar.y = dest.y + dest.h; bar.h = dst->h - bar.y;
        if (bar.h > 0) SDL_FillRect(dst, &bar, black);
        bar.y = dest.y; bar.h = dest.h; bar.w = dest.x;
        if (bar.w > 0) SDL_FillRect(dst, &bar, black);
        bar.x = dest.x + dest.w; bar.w = dst->w - bar.x;
        if (bar.w > 0) SDL_FillRect(dst, &bar, black);
    }
    SDL_BlitScaled(g->screen, NULL, dst, &dest);
    SDL_UpdateWindowSurface(g->window);
}

/* Full screen covers the desktop at its own resolution and scales the
 * picture into it.  A real mode change (SDL_WINDOW_FULLSCREEN) is never
 * undone by SDL on AmigaOS: leaving it, or quitting from it, left the
 * display in 640x480 with Workbench's 800x600 bitmap behind it, so the
 * Workbench came back as four squashed copies in stripes. */
/* Leaving full screen restores Workbench's mode but not its contents:
 * whatever the game drew stays on the Workbench screen outside our window.
 * A borderless window over the whole screen, opened and closed at once,
 * makes Intuition repaint everything beneath it. */
static void refresh_desktop(void)
{
    SDL_DisplayMode mode;
    SDL_Window *cover;
    int w = 1920, h = 1200;
    if (SDL_GetDesktopDisplayMode(0, &mode) == 0 && mode.w > 0 && mode.h > 0) {
        w = mode.w;
        h = mode.h;
    }
    cover = SDL_CreateWindow("", 0, 0, w, h, SDL_WINDOW_BORDERLESS);
    if (cover) SDL_DestroyWindow(cover);
}

static void toggle_fullscreen(Game *g)
{
    if (g->fullscreen) {
        /* SDL brings a window back from full screen without its border,
         * drawing over where its title bar should be, so a fresh window
         * replaces it.  The game draws into its own surface, so nothing
         * else depends on the window. */
        SDL_Window *window = SDL_CreateWindow("Canvas Skies",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_W, SCREEN_H, SDL_WINDOW_HIDDEN);
        if (!window) {
            fprintf(stderr, "Canvas Skies window failed: %s\n",
                    SDL_GetError());
            return;
        }
        SDL_SetWindowFullscreen(g->window, 0);
        SDL_DestroyWindow(g->window);
        g->window = window;
        g->fullscreen = 0;
        /* SDL restores Workbench's mode a little after the call returns;
         * the desktop is repainted and the window shown once it has. */
        g->window_restore_at = SDL_GetTicks() + 400u;
        g->window_surface = NULL;
        g->redraw_static = 1;
        return;
    }
    if (SDL_SetWindowFullscreen(g->window, SDL_WINDOW_FULLSCREEN_DESKTOP) == 0) {
        g->fullscreen = 1;
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
    int dx, sx;
    /* Clip to the surface first (Liang-Barsky): a point just in front of
     * the camera can project hundreds of thousands of pixels away, and
     * stepping all of them one pixel at a time stalls the frame. */
    if (x0 < 0 || x0 >= s->w || y0 < 0 || y0 >= s->h ||
        x1 < 0 || x1 >= s->w || y1 < 0 || y1 >= s->h) {
        float fx0 = (float)x0, fy0 = (float)y0;
        float ddx = (float)(x1 - x0), ddy = (float)(y1 - y0);
        float t0 = 0.0f, t1 = 1.0f;
        float p[4], q[4];
        int k;
        p[0] = -ddx; q[0] = fx0;
        p[1] = ddx;  q[1] = (float)(s->w - 1) - fx0;
        p[2] = -ddy; q[2] = fy0;
        p[3] = ddy;  q[3] = (float)(s->h - 1) - fy0;
        for (k = 0; k < 4; ++k) {
            if (p[k] == 0.0f) {
                if (q[k] < 0.0f) return;
            } else {
                float r = q[k] / p[k];
                if (p[k] < 0.0f) { if (r > t1) return; if (r > t0) t0 = r; }
                else { if (r < t0) return; if (r < t1) t1 = r; }
            }
        }
        x1 = (int)(fx0 + ddx * t1 + 0.5f);
        y1 = (int)(fy0 + ddy * t1 + 0.5f);
        x0 = (int)(fx0 + ddx * t0 + 0.5f);
        y0 = (int)(fy0 + ddy * t0 + 0.5f);
    }
    dx = abs(x1 - x0); sx = x0 < x1 ? 1 : -1;
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
    if (s->format->format == CANVAS_SCREEN_FORMAT) {
        /* Our own pixel format: one pix_mix per pixel, no unpacking. */
        pix_t colour = PIX_PACK(r, g, b);
        int t = (alpha * 32 + 127) / 255;
        int x0 = x < 0 ? 0 : x, x1 = x + w > s->w ? s->w : x + w;
        for (py = y < 0 ? 0 : y; py < y + h && py < s->h; ++py) {
            pix_t *row = PIX_ROW(s, py);
            for (px = x0; px < x1; ++px) row[px] = pix_mix(row[px], colour, t);
        }
        if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
        return;
    }
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
    if (s->format->format == CANVAS_SCREEN_FORMAT) {
        /* Our own pixel format: work out each row's extent once and the
         * fall-off in 16.16 fixed point, with no divide per pixel. */
        pix_t colour = PIX_PACK(r, g, b);
        uint32_t r2 = (uint32_t)(radius * radius);
        uint32_t scale = ((uint32_t)alpha << 16) / r2;   /* alpha per unit d2 */
        for (py = -radius; py <= radius; ++py) {
            int yy = cy + py, half, x0, x1, xx;
            pix_t *row;
            if (yy < 0 || yy >= s->h) continue;
            half = (int)sqrtf((float)(r2 - (uint32_t)(py * py)));
            x0 = cx - half < 0 ? 0 : cx - half;
            x1 = cx + half >= s->w ? s->w - 1 : cx + half;
            row = PIX_ROW(s, yy);
            for (xx = x0; xx <= x1; ++xx) {
                int dx = xx - cx;
                uint32_t left = r2 - (uint32_t)(dx * dx + py * py);
                int aa = (int)((left * scale) >> 16);        /* 0..alpha */
                if (aa > 0) row[xx] = pix_mix(row[xx], colour, (aa * 32 + 127) / 255);
            }
        }
        if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
        return;
    }
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

/* The period faces, pre-rendered on the host at the size they are drawn
 * (ports/amigaos3/make_font_atlases.py).  Each glyph is a cell of 8-bit
 * coverage which is blended into the screen, so the letters keep their
 * antialiased edges and their own widths. */
static int font_width(FontId font, const char *str)
{
    const FontFace *f = &font_faces[font];
    int w = 0;
    while (*str) {
        unsigned int ch = (unsigned char)*str++;
        if (ch < FONT_FIRST || ch > FONT_LAST) ch = ' ';
        w += f->advance[ch - FONT_FIRST];
    }
    return w;
}

static int font_width_n(FontId font, const char *str, int n)
{
    const FontFace *f = &font_faces[font];
    int w = 0;
    while (n-- > 0 && *str) {
        unsigned int ch = (unsigned char)*str++;
        if (ch < FONT_FIRST || ch > FONT_LAST) ch = ' ';
        w += f->advance[ch - FONT_FIRST];
    }
    return w;
}

/* Draws str with its baseline on row 'baseline'; returns the end x. */
static int font_text(Game *g, FontId font, int x, int baseline,
                     const char *str, uint32_t color)
{
    const FontFace *f = &font_faces[font];
    SDL_Surface *s = g->screen, *atlas = g->fonts;
    pix_t ink = (pix_t)color;
    if (!atlas) {
        text(s, x, baseline - 7, str, 1, color);
        return x + (int)strlen(str) * 6;
    }
    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
    while (*str) {
        unsigned int ch = (unsigned char)*str++;
        int index, sx0, sy0, dx0, dy0, px, py;
        if (ch < FONT_FIRST || ch > FONT_LAST) ch = ' ';
        index = (int)ch - FONT_FIRST;
        if (ch != ' ') {
            sx0 = (index % FONT_COLUMNS) * f->cell_w;
            sy0 = f->top + (index / FONT_COLUMNS) * f->cell_h;
            dx0 = x - f->origin_x;
            dy0 = baseline - f->baseline;
            for (py = 0; py < f->cell_h; ++py) {
                const uint8_t *cov;
                pix_t *row;
                int dy = dy0 + py;
                if (dy < 0 || dy >= s->h) continue;
                cov = (const uint8_t *)atlas->pixels +
                      (sy0 + py) * atlas->pitch + sx0;
                row = PIX_ROW(s, dy);
                for (px = 0; px < f->cell_w; ++px) {
                    int c = cov[px], dx = dx0 + px;
                    if (c < 12 || dx < 0 || dx >= s->w) continue;
                    if (c > 243) row[dx] = ink;
                    else row[dx] = pix_mix(row[dx], ink, (c + 4) >> 3);
                }
            }
        }
        x += f->advance[index];
    }
    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
    return x;
}

/* The older calls name a face by its former cell size and give the top of
 * the line; the baseline sits four fifths of the way down that cell. */
static FontId display_font(int h)
{
    return h >= 42 ? FONT_TITLE : h >= 28 ? FONT_HEADING :
           h >= 23 ? FONT_SUBHEAD : FONT_LABEL;
}

static void display_text(Game *g, int x, int y, const char *str,
                         int w, int h, uint32_t color)
{
    (void)w;
    font_text(g, display_font(h), x, y + h * 4 / 5, str, color);
}

static int display_width(const char *str, int h)
{
    return font_width(display_font(h), str);
}

static FontId typewriter_font(int h)
{
    return h >= 15 ? FONT_TYPE : FONT_TYPE_SMALL;
}

/* Typed tables were laid out with runs of spaces on a fixed pitch.  A run
 * of two or more now acts as a tab: the text after it starts in the
 * column the old w-pixel cells gave it, so the columns still line up. */
static void typewriter_text(Game *g, int x, int y, const char *str,
                            int w, int h, uint32_t color)
{
    FontId font = typewriter_font(h);
    char part[96];
    const char *p = str;
    while (*p) {
        const char *start = p, *gap;
        int len;
        gap = strstr(p, "  ");
        len = gap ? (int)(gap - p) : (int)strlen(p);
        if (len > (int)sizeof(part) - 1) len = (int)sizeof(part) - 1;
        memcpy(part, start, (size_t)len);
        part[len] = '\0';
        font_text(g, font, x + (int)(start - str) * w, y + h * 4 / 5, part,
                  color);
        if (!gap) break;
        p = gap;
        while (*p == ' ') ++p;
    }
}

static int typewriter_width(const char *str, int h)
{
    /* Measured as typewriter_text lays it out, tab stops included; the
     * stops sit on the old 7- or 10-pixel pitch. */
    const char *last = str, *gap;
    while ((gap = strstr(last, "  ")) != NULL) {
        last = gap;
        while (*last == ' ') ++last;
    }
    return (int)(last - str) * (h >= 15 ? 10 : 7) +
           font_width(typewriter_font(h), last);
}

/* Diary writing, given the top of its line: the foot rests on the rule. */
#define JOURNAL_HAND_HEIGHT 11
/* Names, dates and headings shared with the typed pages are stored in
 * capitals; in the diary they are written in a hand, in title case.
 * Abbreviations such as R.F.C. keep their capitals. */
static const char *hand_case(const char *str, char *buf, int size)
{
    static const char *const small[] = {
        "a", "an", "and", "at", "by", "for", "in", "of", "on", "the", "to"
    };
    const char *p;
    int n = 0, word = 0;
    for (p = str; *p; ++p)
        if (*p >= 'a' && *p <= 'z') return str;
    while (*str && n < size - 1) {
        const char *end = str;
        int len, i, keep = 0, minor = 0;
        if (*str == ' ' || *str == '/') { buf[n++] = *str++; continue; }
        while (*end && *end != ' ' && *end != '/') ++end;
        len = (int)(end - str);
        for (i = 0; i + 1 < len; ++i)
            if (str[i] == '.' && str[i + 1] >= 'A' && str[i + 1] <= 'Z')
                keep = 1;
        if (word > 0)
            for (i = 0; i < (int)(sizeof(small) / sizeof(small[0])); ++i)
                if ((int)strlen(small[i]) == len) {
                    int k = 0;
                    while (k < len && str[k] - 'A' + 'a' == small[i][k]) ++k;
                    if (k == len) minor = 1;
                }
        for (i = 0; i < len && n < size - 1; ++i) {
            char c = str[i];
            int first = i == 0 || str[i - 1] == '-' || str[i - 1] == '(';
            if (!keep && c >= 'A' && c <= 'Z' && (!first || minor))
                c = (char)(c - 'A' + 'a');
            buf[n++] = c;
        }
        str = end;
        ++word;
    }
    buf[n] = '\0';
    return buf;
}

static void journal_text(Game *g, int x, int y, const char *str,
                         uint32_t color)
{
    char buf[96];
    font_text(g, FONT_HAND, x, y + JOURNAL_HAND_HEIGHT,
              hand_case(str, buf, (int)sizeof(buf)), color);
}

/* The journal's pages are ruled by the game, dead level, one rule to each
 * line of writing.  The painting has had its own rules taken out
 * (ports/amigaos3/make_journal_page.py): they ran uphill and bowed, so the
 * hand never sat on them.  Rule indices count half-lines, as before: the
 * writing goes on the even ones, which are the ruled ones. */
#define JOURNAL_LEFT 0
#define JOURNAL_RIGHT 1
#define JOURNAL_RULES 20
#define JOURNAL_LINE_STEP 20    /* one ruled line */
#define JOURNAL_FIRST_RULE 88
static const short journal_page_x[2][2] = {{100, 306}, {330, 550}};

/* The top of a line of writing whose foot rests on rule n of the page. */
static int journal_line_y(int page, int rule)
{
    (void)page;
    if (rule < 0) rule = 0;
    if (rule >= JOURNAL_RULES) rule = JOURNAL_RULES - 1;
    return JOURNAL_FIRST_RULE + rule * (JOURNAL_LINE_STEP / 2) -
           JOURNAL_HAND_HEIGHT;
}

static void draw_journal_rules(Game *g)
{
    SDL_Surface *s = g->screen;
    int page, y;
    for (page = 0; page < 2; ++page) {
        int x0 = journal_page_x[page][0], x1 = journal_page_x[page][1];
        for (y = JOURNAL_FIRST_RULE; y <= JOURNAL_FIRST_RULE + 200;
             y += JOURNAL_LINE_STEP)
            blend_rect(s, x0, y, x1 - x0, 1, 112, 124, 150, 84);
    }
}

static void journal_paragraph(Game *g, int x, int y, const char *str,
                              int columns, uint32_t color)
{
    int max_w = columns * 8;   /* the width the old fixed cells gave */
    char line_buf[128], cased[160];
    int line_len = 0;
    const char *p = hand_case(str, cased, (int)sizeof(cased));
    while (*p) {
        const char *word;
        int word_len;
        while (*p == ' ') ++p;
        if (!*p) break;
        word = p;
        while (*p && *p != ' ') ++p;
        word_len = (int)(p - word);
        if (word_len > (int)sizeof(line_buf) - 2)
            word_len = (int)sizeof(line_buf) - 2;
        if (line_len &&
            (line_len + 1 + word_len > (int)sizeof(line_buf) - 1 ||
             font_width_n(FONT_HAND, line_buf, line_len) +
             font_width_n(FONT_HAND, " ", 1) +
             font_width_n(FONT_HAND, word, word_len) > max_w)) {
            line_buf[line_len] = '\0';
            journal_text(g, x, y, line_buf, color);
            y += JOURNAL_LINE_STEP;
            line_len = 0;
        }
        if (line_len) line_buf[line_len++] = ' ';
        if (word_len > (int)sizeof(line_buf) - line_len - 1)
            word_len = (int)sizeof(line_buf) - line_len - 1;
        memcpy(line_buf + line_len, word, (size_t)word_len);
        line_len += word_len;
        line_buf[line_len] = '\0';
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
        float t = (float)a->sample_clock * (1.0f / 11025.0f);
        float cue_t = (float)a->cue_clock * (1.0f / 11025.0f);
        int mood_index = a->music_mood - MUSIC_RECON;
        float tempo = a->music_mood == MUSIC_TENSION ? 2.05f :
                      a->music_mood == MUSIC_MOURNING ? 0.92f :
                      a->music_mood == MUSIC_ARMISTICE ? 0.68f : 1.38f;
        int beat = (int)(cue_t * tempo);
        int bar = (beat / 4) & 7;
        float pad = 0.0f, bass, melody, drums = 0.0f, sample;
        int v;
        a->noise = a->noise * 1664525u + 1013904223u;
        if (a->scene == SCENE_DAWN) {
            /* A cold rotary being swung: two coughs that die, a third
             * that catches and settles into a rough idle. */
            float t = (float)a->cue_clock * (1.0f / 11025.0f);
            float level = 0.0f, hz = 34.0f;
            float noise_sample = (float)((a->noise >> 16) & 0xffff) /
                                 32768.0f - 1.0f;
            if (t > 1.6f && t < 1.95f) level = (1.95f - t) * 2.6f;
            else if (t > 2.9f && t < 3.3f) level = (3.3f - t) * 2.2f;
            else if (t > 4.3f) {
                float run = t - 4.3f;
                level = run < 0.8f ? run * 1.1f : 0.88f;
                hz = 30.0f + (run < 2.0f ? run * 7.0f : 14.0f);
                if (run < 1.6f && ((a->noise >> 13) & 7u) == 0u)
                    level *= 0.25f;     /* missing on a cylinder or two */
            }
            a->engine_phase += hz * (1.0f / 11025.0f);
            if (a->engine_phase >= 1.0f) a->engine_phase -= 1.0f;
            a->engine_sub_phase += hz * 0.5f * (1.0f / 11025.0f);
            if (a->engine_sub_phase >= 1.0f) a->engine_sub_phase -= 1.0f;
            sample = (triangle_wave(a->engine_phase) * 0.15f +
                      fast_sin_cycles(a->engine_sub_phase) * 0.10f +
                      noise_sample * 0.05f) * level;
        } else if (a->scene == SCENE_SORTIE) {
            float speed = (a->engine_speed - 62.0f) / 80.0f;
            float noise_sample = (float)((a->noise >> 16) & 0xffff) /
                                 32768.0f - 1.0f;
            float engine_hz;
            if (speed < 0.0f) speed = 0.0f;
            if (speed > 1.0f) speed = 1.0f;
            engine_hz = 46.0f + speed * 38.0f + a->engine_load * 7.0f;
            a->engine_phase += engine_hz * (1.0f / 11025.0f);
            if (a->engine_phase >= 1.0f) a->engine_phase -= 1.0f;
            a->engine_sub_phase += engine_hz * 0.5f * (1.0f / 11025.0f);
            if (a->engine_sub_phase >= 1.0f) a->engine_sub_phase -= 1.0f;
            sample = triangle_wave(a->engine_phase) * (0.17f + speed * 0.09f);
            /* This must have its own continuous phase. Deriving a half-cycle
             * from engine_phase inverted its slope at every wrap: the tick. */
            sample += fast_sin_cycles(a->engine_sub_phase) * 0.11f;
            sample += noise_sample * (0.012f + speed * 0.035f); /* slipstream */
            if (a->other_engine > 0.001f) {
                float hz = 53.0f + a->other_pitch * 31.0f;
                a->other_phase += hz * (1.0f / 11025.0f);
                if (a->other_phase >= 1.0f) a->other_phase -= 1.0f;
                sample += triangle_wave(a->other_phase) * a->other_engine * 0.16f;
            }
            if (a->vehicle_engine > 0.001f) {
                a->vehicle_phase += 29.0f * (1.0f / 11025.0f);
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
                a->synth_phase[4] += melody_hz * (1.0f / 11025.0f);
                a->synth_phase[3] += bass_hz * (1.0f / 11025.0f);
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
                a->synth_phase[0] += drone_d * (1.0f / 11025.0f);
                a->synth_phase[1] += drone_a * (1.0f / 11025.0f);
                a->synth_phase[4] += chanter_hz * (1.0f / 11025.0f);
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
                a->synth_phase[v] += f * (1.0f / 11025.0f);
                if (a->synth_phase[v] >= 1.0f) a->synth_phase[v] -= 1.0f;
                pad += triangle_wave(a->synth_phase[v]) * 0.072f;
            }
            {
                float bass_hz = note_hz(mood_chords[mood_index][bar][0] - 12);
                float melody_hz = note_hz(
                    mood_chords[mood_index][bar][(beat / 2) % 3] + 12);
                a->synth_phase[3] += bass_hz * (1.0f / 11025.0f);
                a->synth_phase[4] += melody_hz * (1.0f / 11025.0f);
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
        if (a->scene != SCENE_SORTIE && a->scene != SCENE_DAWN &&
            a->music_mood != MUSIC_PIPER) {
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
        /* Music on one slider; in a sortie the engines' drone goes with
         * the guns and bangs on the other. */
        float bed = a->scene == SCENE_FILM || a->scene == SCENE_REMEMBER ?
                    0.0f :
                    sample * (a->scene == SCENE_SORTIE ? a->effects_gain :
                                                        a->music_gain);
        sample = 0.0f;
        if (a->scene == SCENE_REMEMBER && a->wind && a->wind_len) {
            sample += (float)a->wind[a->wind_pos] / 32768.0f;
            if (++a->wind_pos >= a->wind_len) a->wind_pos = 0;
        }
        if (a->film_sound && a->film_pos < a->film_len)
            sample += (float)(int16_t)SDL_SwapBE16(
                          (uint16_t)a->film_sound[a->film_pos++]) / 32768.0f;
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
        if (a->crack > 0) {
            /* One sharp report: a hard click of noise that dies in a few
             * milliseconds over a short, dull thump.  No rattle. */
            float env = (float)a->crack / 2600.0f;
            float snap = (float)((a->noise >> 15) & 0x1ffff) / 65536.0f - 1.0f;
            float bite = env * env * env;
            sample += snap * bite * 0.62f;
            sample += fast_sin_cycles(t * 92.0f) * env * env * 0.18f;
            --a->crack;
        }
        if (a->plink > 0) {
            /* A bullet through a bracing wire or a fitting: a thin ring
             * that is gone in a moment. */
            float env = (float)a->plink / 900.0f;
            sample += fast_sin_cycles(t * 1870.0f) * env * env * 0.20f;
            sample += fast_sin_cycles(t * 2710.0f) * env * env * env * 0.10f;
            --a->plink;
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
        sample = bed + sample * a->effects_gain;
        if (sample > 1.0f) sample = 1.0f;
        if (sample < -1.0f) sample = -1.0f;
        out[i * 2] = (int16_t)(sample * 18000.0f);
        out[i * 2 + 1] = out[i * 2];
        /* Wrapped every 20 minutes so the effects clock never runs past
         * float precision (2^24 samples) and starts to sound stepped. */
        if (++a->sample_clock >= 11025u * 1200u) a->sample_clock = 0;
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
    RECORD_PIANO,       /* the picture-house pianist */
    RECORD_COUNT
};

static const char *const record_paths[RECORD_COUNT] = {
    CANVAS_TIPPERARY_WAV, CANVAS_HOME_FIRES_WAV, CANVAS_KIT_BAG_WAV,
    CANVAS_HOLYROOD_WAV, CANVAS_VOICE_OF_THE_GUNS_WAV, CANVAS_HOLST_WAV,
    CANVAS_PIANO_WAV
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
    if (scene == SCENE_CINEMA) return RECORD_PIANO;
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
    if (scene == SCENE_AWARD || scene == SCENE_MESS) return MUSIC_MESS;
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
    if (record == RECORD_NONE && !g->cache_in_ram && g->audio.device &&
        g->audio.record_data) {
        /* No record tonight: lend its memory back. */
        SDL_LockAudioDevice(g->audio.device);
        old = g->audio.record_data;
        g->audio.record_data = NULL;
        g->audio.record_len = 0;
        g->audio.record_track = RECORD_NONE;
        SDL_UnlockAudioDevice(g->audio.device);
        SDL_free(old);
        return;
    }
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
    /* Into the air and back down again on film, when there is film. */
    if (film_start_for(g, g->scene, scene)) return;
    /* A test entry's safety net never follows the player into a campaign. */
    if (scene == SCENE_TITLE) g->qa_hold = g->qa_run = 0;
    ensure_scene_assets(g, scene);
    ensure_music(g, music_mood_for_scene(g, scene));
    prepare_record(g, scene);
    if (scene == SCENE_STORY)
        g->story_stage = g->mission == 0 ? 0 : thyne_lost_tonight(g) ? 1 :
                         news_index_for(g->chapter, g->mission) >= 0 ? 3 : 2;
    if (scene == SCENE_STORY || scene == SCENE_AWARD ||
        scene == SCENE_DAWN || scene == SCENE_MESS)
        g->reveal_started = SDL_GetTicks();
    if (scene == SCENE_DEBRIEF && g->health <= 0) g->fatal_sortie = 1;
    if (scene == SCENE_CINEMA) cinema_start(g);
    if (g->scene == SCENE_REMEMBER && scene != SCENE_REMEMBER)
        remember_free(g);
    if (scene == SCENE_REMEMBER) remember_start(g);
    if (scene == SCENE_COURT_MARTIAL) {
        /* Cashiered: the career ends here and the save says so. */
        g->court_martial_stage = 0;
        g->disgraced = 1;
        save_campaign(g);
    }
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

/* Which painted atlas shows this year's machine: 0..3 index the newer
 * machine_sprites, -1 means the original Sopwith or Albatros. */
static int machine_atlas(int enemy)
{
    if (enemy) {
        int t = their_machine_index(mesh_chapter);
        return t == 0 ? 2 : t == 2 ? 3 : -1;
    } else {
        int t = our_machine_index(mesh_chapter);
        return t == 0 ? 0 : t == 3 ? 1 : -1;
    }
}

static void draw_plane(Game *g, int x, int y, int enemy, int pitch_frame)
{
    int which = machine_atlas(enemy);
    SDL_Surface *atlas = which >= 0 && g->machine_sprites[which] ?
                         g->machine_sprites[which] :
                         enemy ? g->enemy_sprites : g->player_sprites;
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

/* The synchronised gun's muzzle on top of the cowling, just behind the
 * propeller, in each of the five pitch frames of the 96x48 side-on art.
 * Our machines face right and frame 0 is nose up; theirs face left and
 * frame 0 is nose down.  Each frame turns the nose about 3.2 degrees. */
static const unsigned char side_muzzle[2][5][2] = {
    {{82, 10}, {83, 12}, {84, 15}, {84, 16}, {85, 18}},
    {{13, 18}, {13, 16}, {14, 14}, {14, 13}, {14, 12}}
};
#define SIDE_PITCH_STEP (3.2f * (float)M_PI / 180.0f)

static int side_pitch_frame(int frame)
{
    return frame < 0 ? 0 : frame > 4 ? 4 : frame;
}

/* Where a side-on machine drawn at (x, y) has its gun muzzle, and the
 * angle its nose points, radians above the horizontal it faces along. */
static void side_muzzle_at(int x, int y, int enemy, int frame,
                           float *mx, float *my, float *nose)
{
    frame = side_pitch_frame(frame);
    {
        int which = machine_atlas(enemy);
        const unsigned char *at = which >= 0 ? machine_muzzle[which][frame] :
                                  side_muzzle[enemy][frame];
        *mx = (float)(x - 36) + (float)at[0] * 0.75f;
        *my = (float)(y - 18) + (float)at[1] * 0.75f;
    }
    *nose = (float)(enemy ? frame - 2 : 2 - frame) * SIDE_PITCH_STEP;
}

/* The gun itself, a short dark barrel lying along the nose, with a flash
 * at the muzzle while it fires. */
static void draw_side_gun(Game *g, int x, int y, int enemy, int frame,
                          int firing)
{
    SDL_Surface *s = g->screen;
    float mx, my, nose, dir = enemy ? -1.0f : 1.0f;
    float ux, uy;
    side_muzzle_at(x, y, enemy, frame, &mx, &my, &nose);
    ux = cosf(nose) * dir;
    uy = -sinf(nose);
    line(s, (int)(mx - ux * 7.0f), (int)(my - uy * 7.0f), (int)mx, (int)my,
         rgb(s, 34, 32, 30));
    line(s, (int)(mx - ux * 7.0f), (int)(my - uy * 7.0f) - 1, (int)mx,
         (int)my - 1, rgb(s, 70, 66, 58));
    if (firing) {
        blend_circle(s, (int)(mx + ux * 3.0f), (int)(my + uy * 3.0f), 5,
                     255, 214, 120, 200);
        circle(s, (int)(mx + ux * 2.0f), (int)(my + uy * 2.0f), 1,
               rgb(s, 255, 250, 220));
    }
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
    if (armistice_quiet(g)) return 0;
    if (mode == MISSION_TOP_DOWN && g->td_mix != MIX_NONE) return g->td_goal;
    if (mode == MISSION_DOGFIGHT_3D && g->chapter == 0 && special == SPECIAL_NONE)
        return 3;                        /* three of Tom's four jobs */
    if (is_perspective_mode(mode)) {
        int goal = 1 + g->chapter / 2 + g->mission / 6;
        if (goal > chapters[g->chapter].enemy_count)
            goal = chapters[g->chapter].enemy_count;
        /* The rear seat meets a stream of tough attackers, most of whom
         * break off hurt: two brought down is a good day's work, one with
         * the carbine in 1914. */
        if (mode == MISSION_GUNNER_3D) goal = g->chapter == 0 ? 1 : 2;
        if (mode == MISSION_DOGFIGHT_3D && goal > 4) goal = 4;
        return goal;
    }
    if (mode == MISSION_SIDE) {
        /* Two groups of targets: the first holds one fewer than the
         * order (side_layout), so the second must be reached, and a
         * careful pilot has a target or two to spare.  1914's sorties
         * carry four tasks, three of them to do.  check_campaign.py
         * checks these orders against the targets and bombs on offer. */
        if (g->chapter == 0) return 3;
        return chapters[g->chapter].goal + 1;
    }
    return chapters[g->chapter].goal;
}

static int player_machine_gun_damage(const Game *g)
{
    int enemy_hits = 3 + g->chapter / 2;
    int player_hits = enemy_hits * PLAYER_DURABILITY_MULTIPLIER;
    int damage = player_hits > 0 ? 100 / player_hits : 4;
    /* A stouter machine takes the same rounds with less harm. */
    damage = (int)((float)damage /
                   our_machines[our_machine_index(g->chapter)].toughness + 0.5f);
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
    if (!enemies_hostile(g)) return 1;
    if (campaign_missions[g->chapter][g->mission].special == SPECIAL_AIRSHIP)
        return 1;
    if (campaign_missions[g->chapter][g->mission].special == SPECIAL_GOTHA)
        return 3;
    count = mode == MISSION_GUNNER_3D ? goal + 2 : goal;
    if (count > MAX_ENEMIES) count = MAX_ENEMIES;
    return count;
}

/* From 1915 the fifth and eleventh missions, when flown side-on, cross a
 * village whose people are still at home; the briefing names it. */
static const char *const side_village_names[CHAPTER_COUNT] = {
    NULL, "LA BASSEE", "MARTINPUICH", "MONCHY", "FLESQUIERES", "ALBERT",
    "VILLERS-BRETONNEUX", NULL
};

static int side_village_today(const Game *g)
{
    /* The fifth and eleventh missions, when they are flown side-on: the
     * places the village sorties have always fallen, whatever else in the
     * chapter is flown side-on. */
    if (!side_village_names[g->chapter] ||
        campaign_missions[g->chapter][g->mission].mode != MISSION_SIDE ||
        campaign_missions[g->chapter][g->mission].special != SPECIAL_NONE)
        return 0;
    return g->mission == 4 || g->mission == 10;
}

/* Side-on: an approach over the lines, a first group of targets, a long
 * stretch of their country, and a second group beyond it; in 1914 the
 * observers' three tasks instead, spread along the same road. */
#define SIDE_APPROACH 5600.0f
#define SIDE_BETWEEN 7000.0f
#define SIDE_SPACING 560.0f
static void side_layout(Game *g)
{
    const Chapter *chapter = &chapters[g->chapter];
    int goal = current_mission_goal(g), n, k, first;
    g->task_total = 0;
    if (g->chapter == 0) {
        /* Four pieces of work, a column always among them. */
        static const int tasks[3] = {TASK_MARK, TASK_DROP, TASK_PHOTO};
        for (k = 0; k < 4; ++k) {
            Target *t = &g->targets[k];
            t->active = 1;
            t->task = k == 3 ? TASK_MARK : tasks[(k + g->mission) % 3];
            t->world_x = SIDE_APPROACH + (float)k * 5000.0f;
        }
        g->task_end = g->targets[3].world_x + 300.0f;
        g->task_total = 4;
        return;
    }
    /* The first group falls one short of the order, so the second must
     * be reached; the second has at least three. */
    first = goal > 1 ? goal - 1 : 1;
    n = chapter->target_count;
    if (n < first + 3) n = first + 3;
    if (n > 14) n = 14;
    for (k = 0; k < n; ++k) {
        Target *t = &g->targets[k];
        t->active = 1;
        t->type = k % 3 == 2 ? 2 : k & 1;
        t->world_x = k < first ? SIDE_APPROACH + (float)k * SIDE_SPACING :
                     SIDE_APPROACH + (float)first * SIDE_SPACING + SIDE_BETWEEN +
                     (float)(k - first) * SIDE_SPACING;
    }
    g->task_end = g->targets[n - 1].world_x + 300.0f;
}


/* The legs, the hazards and the QA tally start afresh with each sortie. */
static void reset_sortie_legs(Game *g)
{
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    g->leg = LEG_OUT;
    g->order_met_told = 0;
    g->obs_count = 0;
    g->obs_spot_age = 99.0f;
    g->leg_time = 0.0f;
    g->sortie_clock = 0.0f;
    g->home_progress = 0.0f;
    g->landed = 0;
    g->landed_at = 0;
    g->wave = g->waves = 0;
    g->wave_clock = 0.0f;
    g->shadow_time = 0.0f;
    g->hazard_clock = 12.0f;
    g->archie_clock = 7.0f;
    g->rifle_clock = 5.0f;
    g->player_vx = 0.0f;
    g->photo_frames = 0;
    g->photo_hold = 0.0f;
    g->tree_struck = 10.0f;
    memset(g->archie, 0, sizeof(g->archie));
    memset(g->riflemen, 0, sizeof(g->riflemen));
    memset(&g->squall, 0, sizeof(g->squall));
    memset(&g->balloon, 0, sizeof(g->balloon));
    g->fabric_hole_count = 0;
    g->qa_time = g->qa_idle = g->qa_idle_run = g->qa_idle_longest = 0.0f;
    g->qa_logged = 0;
    g->qa_shots = g->qa_enemy_shots = g->qa_damage = 0;
    g->qa_last_health = 100;
    g->qa_unreached = 0;
    g->front_at = mode == MISSION_SIDE ? 1500.0f : 1400.0f;
    g->home_line_at = 1.0e9f;
    g->field_at = 1.0e9f;
    if (mode == MISSION_TOP_DOWN && g->td_mix == MIX_NONE)
        g->task_end = chapters[g->chapter].distance;
    /* Message bags for the sorties that carry one; td_build_mission
     * has already put them aboard a top-down sortie that needs them. */
    if (mode != MISSION_TOP_DOWN) g->bags = mode == MISSION_SIDE &&
                                            g->chapter == 0 ? 3 : 0;
}

static void reset_sortie(Game *g)
{
    int i;
    int perspective_enemy_count;
    const Chapter *chapter = &chapters[g->chapter];
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    g->sim_ms = SIM_EPOCH;
    g->sim_carry = 0.0f;
    g->mission_save_state = 0;
    g->mess_done = 0;
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
    mesh_chapter = g->chapter;
    flight_state_init(&g->flight, 0.0f, 0.0f, 0.0f, 43.81f,
                      g->flight_throttle);
    g->flight.power = our_machines[our_machine_index(g->chapter)].power;
    g->flight.agility = our_machines[our_machine_index(g->chapter)].agility;
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
    g->arms_rounds = 0;
    g->arms_ready_at = 0;
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
        g->enemies[i].flight.power =
            their_machines[their_machine_index(g->chapter)].power;
        g->enemies[i].flight.agility =
            their_machines[their_machine_index(g->chapter)].agility;
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
        g->enemies[i].last_shot = sim_now(g) + 650u + (uint32_t)i * 390u;
    }
    memset(g->rounds, 0, sizeof(g->rounds));
    memset(g->sparks, 0, sizeof(g->sparks));
    /* The dice are shaken by the wall clock, so no two sorties fall
     * out alike; only the sortie's timing runs on its own clock. */
    g->combat_seed ^= SDL_GetTicks() * 2654435761u + 12345u;
    g->home_heading = (float)M_PI;
    g->guns_jammed = 0;
    g->jam_clear_until = 0;
    g->heading_home_at = 0;
    g->notice_until = 0;
    g->shake = 0.0f;
    g->gun_recoil = 0.0f;
    reset_wingmen(g);
    memset(g->targets, 0, sizeof(g->targets));
    for (i = 0; i < MAX_TARGETS && mode != MISSION_SIDE; ++i) {
        g->targets[i].world_x = 560.0f + i * 480.0f;
        g->targets[i].active = !is_perspective_mode(mode) &&
                               i < chapter->target_count;
        g->targets[i].type = g->chapter == 0 ? 0 :
                             (i % 3 == 2 ? 2 : i & 1);
        g->targets[i].last_shot = 0;
        g->targets[i].next_round = 0;
        g->targets[i].burst_left = 0;
        g->targets[i].civilian = 0;
        g->targets[i].wrecked = 0;
    }
    if (mode == MISSION_SIDE) side_layout(g);
    if (mode == MISSION_SIDE && side_village_today(g)) {
        /* A village still lived in, among the gun pits: a farm, the
         * church and a cart on the road, placed between the targets. */
        static const int places[3] = {SC_FARM, SC_CHURCH, SC_CART};
        static const int pair[3] = {0, 1, -1};
        int k, n = 0;
        while (n < MAX_TARGETS && g->targets[n].active) ++n;
        for (k = 0; k < 3 && n + k < MAX_TARGETS; ++k) {
            Target *t = &g->targets[n + k];
            int a = pair[k] >= 0 ? pair[k] : (n + 1) / 2;
            t->active = 1;
            t->type = 0;
            t->civilian = places[k];
            /* between the guns */
            t->world_x = (g->targets[a].world_x + g->targets[a + 1].world_x) * 0.5f;
        }
    }
    g->special = campaign_missions[g->chapter][g->mission].special;
    g->sortie_offences = 0;
    g->sortie_offence_kind = CIV_NONE;
    g->offence_last_sortie = 0;
    td_build_mission(g);
    if (g->td_mix != MIX_NONE) {
        /* The painted targets replace the old gun pits, and Archie now
         * fires from batteries out of sight. */
        for (i = 0; i < MAX_TARGETS; ++i) g->targets[i].active = 0;
    }
    reset_sortie_legs(g);
    /* Bombs enough for the order and two that miss. */
    if (mission_armed(g) && g->bombs_remaining < current_mission_goal(g) + 2)
        g->bombs_remaining = current_mission_goal(g) + 2;
    g->pudding_delivered = 0;
    g->raider_through = 0;
    g->wreck_started = 0;
    if (g->special == SPECIAL_TRUCE) {
        /* Christmas Day along the line: a gathering of both armies in no
         * man's land every quarter of a minute or so, the footballers
         * among them, then their aerodrome for the pudding.  More of them
         * wave the machine home. */
        for (i = 0; i < MAX_TARGETS; ++i) {
            memset(&g->targets[i], 0, sizeof(g->targets[i]));
            if (i < 8) {
                g->targets[i].active = 1;
                g->targets[i].type = i < 7 ? 3 : 4;
                g->targets[i].world_x = 2200.0f + (float)i * 1450.0f;
            }
        }
        g->task_end = g->targets[7].world_x + 300.0f;
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
        g->notice_until = sim_now(g) + 4000u;
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
            b->enemy = enemy; b->type = type; b->missed = 0;
            b->life = life; b->initial_life = life;
            b->active = 1;
            if (type == 0 || type == 2 || type == 3) {
                if (enemy) ++g->qa_enemy_shots;
                else ++g->qa_shots;
            }
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
    uint32_t now = sim_now(g);
    float mx, my, nose;
    if (g->ammunition <= 0) return;
    if (now - g->last_shot < 130) return;
    side_muzzle_at((int)g->player_x, (int)g->player_y, 0,
                   2 + (int)(g->player_vy / 45.0f), &mx, &my, &nose);
    if (new_bullet(g, mx, my, 390.0f * cosf(nose),
                   -390.0f * sinf(nose) + g->player_vy * 0.12f, 0, 0, 2.0f)) {
        g->last_shot = now;
        --g->ammunition;
        g->audio.shot = 1700;
    }
}

static void drop_bomb(Game *g)
{
    uint32_t now = sim_now(g);
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
    f->power = f->agility = 1.0f;
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
             0.009f * f->aileron * effectiveness * f->agility +
             0.012f * f->rudder;
    pitch_c = -0.58f * alpha - 5.8f * qhat +
              0.095f * f->elevator * effectiveness * f->agility;
    yaw_c = 0.115f * beta - 0.24f * rhat +
            0.065f * f->rudder - 0.018f * f->aileron * effectiveness;

    lift = dynamic_pressure * wing_area * cl;
    drag = dynamic_pressure * wing_area * cd;
    side = dynamic_pressure * wing_area * cy;
    fx = f->throttle * 2050.0f * f->power - drag * cosf(alpha) +
         lift * sinf(alpha);
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
    target_speed = (30.0f + f->throttle * 30.0f) * (0.6f + 0.4f * f->power);
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
        arcade_flight_step(f, position, step, aileron, elevator,
                           agility * f->agility);
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
    if (campaign_missions[g->chapter][g->mission].mode == MISSION_GUNNER_3D &&
        depth >= 90.0f && index % 3 == 2) {
        /* Every third attacker works round a flank, out beside the tailplane
         * where the observer must swing right round to bear on him, and
         * comes in from there rather than from dead astern. */
        float flank = (index & 4) ? -1.0f : 1.0f;
        float swing = 1.0f - fabsf(sinf(g->scroll * 0.0016f +
                                        enemy->phase * 1.3f)) * 0.55f;
        *sx += (int)(flank * swing * 285.0f);
        *sy += (int)(swing * 28.0f);
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

/* London is about this far east of the patrol line. */
#define RAIDER_THROUGH_X 2200.0f

static int enemies_falling(const Game *g)
{
    int i;
    for (i = 0; i < MAX_ENEMIES; ++i)
        if (g->enemies[i].active && g->enemies[i].crashing) return 1;
    return 0;
}

/* Side-on, top-down and rear-seat deaths: hold on the burning wreck for a
 * moment before the debrief, instead of cutting straight to the hospital.
 * Returns 1 while the hold lasts. */
static int hold_for_wreck(Game *g, float x, float y)
{
    uint32_t now = sim_now(g);
    if (!g->wreck_started) {
        g->wreck_started = now ? now : 1u;
        new_explosion(g, x, y, 2);
        g->audio.blast = 7000;
        g->audio.hit_player = 1;
        g->shake = 1.0f;
        return 1;
    }
    return now - g->wreck_started < 2200u;
}

static void finish_alternate_sortie(Game *g)
{
    /* A sortie that ends in a crash is never also a success: shooting
     * away your own tail after the goal used to win the mission and cost
     * a life at once. */
    if (g->health <= 0 || g->tail_integrity <= 0) g->fatal_sortie = 1;
    if (is_perspective_mode(campaign_missions[g->chapter][g->mission].mode) &&
        g->obs_count == 0)
        g->success = g->kills >= current_mission_goal(g) &&
                     g->health > 0 && g->fuel > 0 && !g->fatal_sortie;
    else
        g->success = g->objectives >= current_mission_goal(g) &&
                     g->health > 0 && g->fuel > 0 && !g->fatal_sortie;
    if (g->special == SPECIAL_TRUCE && !g->pudding_delivered) g->success = 0;
    if (g->raider_through) g->success = 0;
    qa_log_sortie(g);
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

/* What each kind of ground target is: its painted whole and wrecked forms,
 * how many rounds it takes (bombs always finish it), whether only a bomb
 * will do, whether it counts towards the sortie's objective, and whether
 * hitting it is an offence. */
typedef struct {
    short whole, wreck;
    unsigned char hp, bomb_only, military, civ, fires;
    short half_w, half_h;
    unsigned char task;     /* TASK_ for the observers' work, or none */
} TdKindInfo;

static const TdKindInfo td_kinds[TDK_COUNT] = {
    {TD_LORRY_GREY, TD_LORRY_WRECK, 2, 0, 1, CIV_NONE, 0, 10, 28, 0},
    {TD_AMBULANCE, TD_AMBULANCE_WRECK, 1, 0, 0, CIV_AMBULANCE, 0, 10, 28, 0},
    {TD_LOCO, TD_LOCO_WRECK, 6, 0, 1, CIV_NONE, 0, 12, 36, 0},
    {TD_WAGON_GREY, TD_WAGON_WRECK, 3, 0, 1, CIV_NONE, 0, 11, 28, 0},
    {TD_STATION, TD_STATION_WRECK, 1, 1, 1, CIV_NONE, 0, 20, 58, 0},
    {TD_HANGAR, TD_HANGAR_WRECK, 1, 1, 1, CIV_NONE, 0, 46, 30, 0},
    {TD_TENTS, TD_TENTS_WRECK, 3, 0, 1, CIV_NONE, 0, 26, 26, 0},
    {TD_DUMP, TD_DUMP_WRECK, 1, 1, 1, CIV_NONE, 0, 34, 34, 0},
    {TD_DEPOT, TD_DEPOT_WRECK, 1, 1, 1, CIV_NONE, 0, 30, 20, 0},
    {TD_FARM, TD_FARM_WRECK, 1, 0, 0, CIV_CIVILIAN, 0, 22, 20, 0},
    {TD_CHURCH, TD_CHURCH_WRECK, 1, 0, 0, CIV_CIVILIAN, 0, 40, 56, 0},
    {TD_CART, TD_CART_WRECK, 1, 0, 0, CIV_CIVILIAN, 0, 10, 22, 0},
    {TD_REFUGEES, TD_REFUGEES_WRECK, 1, 0, 0, CIV_CIVILIAN, 0, 18, 28, 0},
    {TD_MG, TD_MG_WRECK, 2, 0, 1, CIV_NONE, 1, 16, 16, 0},
    {TD_DUGOUT, TD_DUGOUT_WRECK, 1, 1, 1, CIV_NONE, 0, 14, 14, 0},
    /* The observers' work: none of it is to be destroyed. */
    {TD_MAN_THEIRS, TD_MAN_THEIRS, 99, 1, 0, CIV_NONE, 0, 14, 60, TASK_MARK},
    {TD_TENTS, TD_TENTS_WRECK, 99, 1, 0, CIV_NONE, 0, 26, 26, TASK_DROP},
    {TD_AA_PIT, TD_AA_PIT, 99, 1, 0, CIV_NONE, 0, 40, 20, TASK_PHOTO}
};

/* A sortie's mix: up to three segments flown in order, its name for the
 * briefing, and the warning the briefing gives.  Each mix has a second
 * group of targets beyond the first, so the sortie is a raid with two
 * halves and a run home, not a single pass. */
typedef struct {
    unsigned char seg[3];
    const char *orders[3];
} TopdownMix;

static const TopdownMix topdown_mixes[MIX_COUNT] = {
    {{SEG_NONE, SEG_NONE, SEG_NONE}, {NULL, NULL, NULL}},
    {{SEG_CONVOY, SEG_DEPOT, SEG_NONE},
     {"STRAFE THE LORRY COLUMN ON THE ROAD EAST.",
      "FOLLOW THE ROAD TO ITS DEPOT AND BOMB THE SHEDS.", NULL}},
    {{SEG_RAIL, SEG_DEPOT, SEG_NONE},
     {"A TRAIN IS DUE ON THE LINE TO THE FRONT.",
      "STOP IT: BOMB THE ENGINE, THEN THE WAGONS.",
      "THEN THE STORES SHEDS AT THE RAILHEAD."}},
    {{SEG_BASE, SEG_CONVOY, SEG_NONE},
     {"THEIR AERODROME LIES BEHIND THE WOOD.",
      "BOMB THE HANGARS, THE DUMP AND THE STORES.",
      "STRAFE THE LORRIES ON THE ROAD BEYOND."}},
    {{SEG_TRENCH, SEG_RAIL, SEG_NONE},
     {"FLY ALONG THEIR FRONT LINE: GUN POSTS AND DUGOUTS.",
      "THEN THE RAILHEAD BEHIND IT.",
      "OUR OWN LINE IS IN KHAKI SANDBAGS. DO NOT BOMB IT."}},
    {{SEG_STATION_VILLAGE, SEG_DEPOT, SEG_NONE},
     {"BOMB THE STATION AND THE TRAIN STANDING IN IT.",
      "THE VILLAGE IS STILL OCCUPIED BY CIVILIANS:",
      "TAKE CARE. NO BOMBS ON THE HOUSES OR THE CHURCH."}},
    {{SEG_TRENCH, SEG_TRENCH, SEG_NONE},
     {"TRENCH RUN: FOLLOW THEIR LINE LOW AND HARD.",
      "GUN POSTS, DUGOUTS AND THE DUMPS BEHIND THEM.",
      "OUR OWN LINE IS IN KHAKI SANDBAGS. DO NOT BOMB IT."}},
    {{SEG_BASE, SEG_AMBULANCES, SEG_NONE},
     {"BOMB THE DEPOT AND ITS HANGARS.",
      "AMBULANCES ON THE ROAD: HOLD FIRE.", NULL}},
    {{SEG_CONVOY, SEG_VILLAGE, SEG_NONE},
     {"STRAFE THE COLUMNS ON THE ROAD WEST.",
      "AMBULANCES ON THE ROAD: HOLD FIRE.",
      "REFUGEES AND FARMS BEYOND: TAKE CARE."}},
    {{SEG_HQ, SEG_COLUMN, SEG_BATTERY},
     {"CARRY THE GENERAL'S NOTE TO CORPS HEADQUARTERS.",
      "B DROPS THE MESSAGE BAG ON THE TENTS.",
      "MARK ANY COLUMN ON THE ROAD BEYOND: FLY OVER IT."}},
    {{SEG_COLUMN, SEG_BATTERY, SEG_COLUMN},
     {"GREY COLUMNS ARE ON THE ROADS.",
      "MARK EACH ONE: FLY STRAIGHT OVER IT, AND",
      "PHOTOGRAPH THE BATTERY: HOLD LEVEL OVER IT."}},
    {{SEG_COLUMN, SEG_BATTERY, SEG_HQ},
     {"FIND THE GAP ON THEIR FLANK.",
      "MARK THE COLUMN, THEN PHOTOGRAPH THE BATTERY:",
      "HOLD THE MACHINE LEVEL OVER THE GUNS."}},
    {{SEG_BATTERY, SEG_HQ, SEG_COLUMN},
     {"PHOTOGRAPH THE BATTERY ON THE ROAD NORTH:",
      "HOLD THE MACHINE LEVEL OVER THE GUNS.",
      "THEN DROP THE REPORT AT HEADQUARTERS WITH B."}},
    {{SEG_HQ, SEG_BATTERY, SEG_COLUMN},
     {"THE ORDER SAYS THE FIRING ENDS AT ELEVEN.",
      "DROP IT ON THEIR HEADQUARTERS WITH B,",
      "THEN PHOTOGRAPH THE BATTERY STILL IN ACTION."}},
    {{SEG_COLUMN, SEG_HQ, SEG_BATTERY},
     {"THE NEGOTIATORS' TRAIN IS IN THE FOREST.",
      "MARK THE COLUMNS FALLING BACK BESIDE IT,",
      "THEN DROP OUR MESSAGE ON HEADQUARTERS WITH B."}}
};

/* By date: 1915's raids on railways and aerodromes; the Somme and Arras
 * trench lines and the stations behind them; Cambrai's trench runs; the
 * roads of the spring retreat crowded with ambulances and refugees; the
 * railways of the Hundred Days.  1914 and the Armistice are the
 * observers' war: messages, columns and photographs. */
static const unsigned char mixes_by_chapter[CHAPTER_COUNT][4] = {
    {MIX_FIRST_MESSAGE, MIX_COLUMNS, MIX_EMPTY_FLANK, MIX_TURN_OF_ARMIES},
    {MIX_CONVOY_DEPOT, MIX_RAILWAY, MIX_AERODROME, MIX_RAILWAY},
    {MIX_TRENCH_RAILHEAD, MIX_VILLAGE_STATION, MIX_TRENCH_RUN, MIX_AERODROME},
    {MIX_RAILWAY, MIX_BASE_AMBULANCES, MIX_TRENCH_RAILHEAD, MIX_TRENCH_RUN},
    {MIX_TRENCH_RUN, MIX_CONVOY_DEPOT, MIX_VILLAGE_STATION, MIX_TRENCH_RUN},
    {MIX_CONVOY_AMBULANCES, MIX_TRENCH_RUN, MIX_BASE_AMBULANCES,
     MIX_CONVOY_AMBULANCES},
    {MIX_RAILWAY, MIX_CONVOY_AMBULANCES, MIX_VILLAGE_STATION, MIX_TRENCH_RAILHEAD},
    {MIX_LAST_ORDERS, MIX_WHITE_TRAIN, MIX_LAST_ORDERS, MIX_EMPTY_FLANK}
};

static int topdown_mix_for(int chapter, int mission)
{
    int m, k = 0;
    if (campaign_missions[chapter][mission].mode != MISSION_TOP_DOWN ||
        campaign_missions[chapter][mission].special != SPECIAL_NONE)
        return MIX_NONE;
    for (m = 0; m < mission; ++m)
        if (campaign_missions[chapter][m].mode == MISSION_TOP_DOWN) ++k;
    if (k > 3) k = 3;
    return mixes_by_chapter[chapter][k];
}

static void td_add(Game *g, int kind, float world_x, float x, float speed)
{
    TdObject *o;
    if (g->td_count >= MAX_TD_OBJECTS) return;
    o = &g->td[g->td_count++];
    memset(o, 0, sizeof(*o));
    o->kind = kind;
    o->world_x = world_x;
    o->x = x;
    o->speed = speed;
    o->hp = td_kinds[kind].hp;
    o->active = 1;
}

static void td_strip(Game *g, int a, int b, float x, float from, float to)
{
    TdStrip *t;
    if (g->td_strip_count >= MAX_TD_STRIPS) return;
    t = &g->td_strips[g->td_strip_count++];
    t->sprite_a = a; t->sprite_b = b; t->x = x; t->from = from; t->to = to;
}

static void td_build_segment(Game *g, int seg, float from, float to)
{
    float mid = (from + to) * 0.5f, w;
    int k;
    switch (seg) {
    case SEG_CONVOY:
    case SEG_AMBULANCES:
        td_strip(g, TD_TILE_ROAD, -1, 430.0f, from - 200.0f, to + 400.0f);
        for (k = 0; k < 6; ++k) {
            int kind = seg == SEG_AMBULANCES ? (k & 1 ? TDK_AMBULANCE : -1) :
                       (g->td_mix == MIX_CONVOY_AMBULANCES && (k == 2 || k == 5)) ?
                       TDK_AMBULANCE : TDK_LORRY;
            if (kind < 0) continue;
            td_add(g, kind, from + 220.0f + (float)k * 74.0f, 430.0f, 22.0f);
        }
        if (seg == SEG_CONVOY)
            td_add(g, TDK_MG, mid + 160.0f, 520.0f, 0.0f);
        break;
    case SEG_DEPOT:
        td_strip(g, TD_TILE_ROAD, -1, 430.0f, from - 200.0f, to);
        td_add(g, TDK_DEPOT, mid - 120.0f, 520.0f, 0.0f);
        td_add(g, TDK_DEPOT, mid + 40.0f, 525.0f, 0.0f);
        td_add(g, TDK_DUMP, mid - 40.0f, 330.0f, 0.0f);
        td_add(g, TDK_MG, mid + 160.0f, 330.0f, 0.0f);
        break;
    case SEG_RAIL:
    case SEG_STATION_VILLAGE:
        td_strip(g, TD_TILE_RAIL, -1, 200.0f, from - 300.0f, to + 400.0f);
        w = seg == SEG_RAIL ? 26.0f : 0.0f;  /* a train under way, or standing */
        td_add(g, TDK_LOCO, from + 240.0f, 200.0f, w);
        for (k = 0; k < 4; ++k)
            td_add(g, TDK_WAGON, from + 302.0f + (float)k * 59.0f, 200.0f, w);
        td_add(g, TDK_STATION, mid + 140.0f, 238.0f, 0.0f);
        if (seg == SEG_STATION_VILLAGE) {
            td_add(g, TDK_FARM, mid + 30.0f, 330.0f, 0.0f);
            td_add(g, TDK_CHURCH, mid + 160.0f, 360.0f, 0.0f);
            td_add(g, TDK_FARM, mid + 280.0f, 320.0f, 0.0f);
            td_add(g, TDK_CART, mid + 90.0f, 420.0f, -12.0f);
            td_add(g, TDK_REFUGEES, mid + 230.0f, 290.0f, -6.0f);
        } else {
            td_add(g, TDK_DUMP, mid + 60.0f, 330.0f, 0.0f);
            td_add(g, TDK_MG, mid - 60.0f, 300.0f, 0.0f);
        }
        break;
    case SEG_BASE:
        td_add(g, TDK_HANGAR, mid - 140.0f, 260.0f, 0.0f);
        td_add(g, TDK_HANGAR, mid + 20.0f, 420.0f, 0.0f);
        td_add(g, TDK_TENTS, mid - 40.0f, 140.0f, 0.0f);
        td_add(g, TDK_DUMP, mid + 150.0f, 230.0f, 0.0f);
        td_add(g, TDK_DEPOT, mid + 120.0f, 470.0f, 0.0f);
        td_add(g, TDK_MG, mid - 210.0f, 360.0f, 0.0f);
        td_add(g, TDK_MG, mid + 230.0f, 340.0f, 0.0f);
        break;
    case SEG_VILLAGE:
        td_add(g, TDK_FARM, mid - 150.0f, 180.0f, 0.0f);
        td_add(g, TDK_FARM, mid - 40.0f, 240.0f, 0.0f);
        td_add(g, TDK_CHURCH, mid + 90.0f, 200.0f, 0.0f);
        td_add(g, TDK_REFUGEES, mid - 60.0f, 430.0f, 16.0f);
        td_add(g, TDK_CART, mid + 40.0f, 430.0f, 16.0f);
        td_add(g, TDK_MG, mid + 220.0f, 520.0f, 0.0f);
        td_strip(g, TD_TILE_ROAD, -1, 430.0f, from, to + 300.0f);
        break;
    case SEG_TRENCH:
        /* Their line down the middle, ours in khaki to the east. */
        td_strip(g, TD_TILE_TRENCH_A, TD_TILE_TRENCH_B, 300.0f, from, to);
        td_strip(g, TD_TILE_OURS_A, TD_TILE_OURS_B, 540.0f, from, to);
        for (k = 0; k < 4; ++k) {
            float at = from + 120.0f + (float)k * (to - from - 200.0f) / 4.0f;
            td_add(g, TDK_MG, at, 300.0f + (k & 1 ? 26.0f : -26.0f), 0.0f);
            td_add(g, TDK_DUGOUT, at + 80.0f, 300.0f + (k & 1 ? -40.0f : 40.0f),
                   0.0f);
        }
        td_add(g, TDK_DUMP, mid, 150.0f, 0.0f);
        break;
    case SEG_COLUMN:
        /* A column of infantry on the march down a road, and a second
         * coming up behind it on a road further over. */
        td_strip(g, TD_TILE_ROAD, -1, 250.0f, from - 200.0f, to + 300.0f);
        td_add(g, TDK_COLUMN, from + 380.0f, 250.0f, 9.0f);
        td_strip(g, TD_TILE_ROAD, -1, 470.0f, mid, to + 300.0f);
        td_add(g, TDK_COLUMN, mid + 420.0f, 470.0f, 9.0f);
        break;
    case SEG_HQ:
        td_strip(g, TD_TILE_ROAD, -1, 200.0f, from - 200.0f, to);
        td_add(g, TDK_FARM, mid - 60.0f, 260.0f, 0.0f);
        td_add(g, TDK_HQ, mid + 40.0f, 330.0f, 0.0f);
        td_add(g, TDK_TENTS, mid + 260.0f, 420.0f, 0.0f);
        td_add(g, TDK_MG, mid + 180.0f, 160.0f, 0.0f);
        break;
    case SEG_BATTERY:
        td_strip(g, TD_TILE_ROAD, -1, 430.0f, from - 200.0f, to + 200.0f);
        td_add(g, TDK_BATTERY, mid, 330.0f, 0.0f);
        td_add(g, TDK_MG, mid + 190.0f, 470.0f, 0.0f);
        td_add(g, TDK_DUGOUT, mid - 150.0f, 240.0f, 0.0f);
        break;
    default:
        break;
    }
}

/* How long each part of a top-down sortie is, in scroll: the approach
 * over the lines, each group of targets, and the stretch between them. */
#define TD_APPROACH 3800.0f
#define TD_SEGMENT 2900.0f
#define TD_BETWEEN 4000.0f

/* Lays out the sortie's segments and sets the objective: three in five of
 * its military targets, so it can be won without destroying everything,
 * and never by bombing anything it must not.  The observers' sorties
 * count their tasks instead, all but one of them. */
static void td_build_mission(Game *g)
{
    const TopdownMix *mix;
    float start = TD_APPROACH;
    float length = TD_SEGMENT + (float)g->chapter * 120.0f;
    int n = 0, k, military = 0, tasks = 0;
    g->td_count = 0;
    g->td_strip_count = 0;
    g->td_goal = 0;
    g->bags = 0;
    g->task_total = 0;
    g->td_mix = topdown_mix_for(g->chapter, g->mission);
    if (g->td_mix == MIX_NONE) return;
    mix = &topdown_mixes[g->td_mix];
    while (n < 3 && mix->seg[n] != SEG_NONE) ++n;
    for (k = 0; k < n; ++k) {
        float from = start + (float)k * (length + TD_BETWEEN);
        td_build_segment(g, mix->seg[k], from, from + length);
    }
    g->task_end = start + (float)n * length + (float)(n - 1) * TD_BETWEEN;
    for (k = 0; k < g->td_count; ++k) {
        if (td_kinds[g->td[k].kind].military) ++military;
        if (td_kinds[g->td[k].kind].task) ++tasks;
    }
    for (k = 0; k < g->td_count; ++k)
        if (td_kinds[g->td[k].kind].task == TASK_DROP) g->bags = 3;
    g->task_total = tasks;
    if (tasks > 0) {
        g->td_goal = tasks > 1 ? tasks - 1 : 1;
    } else {
        /* Half the military targets: enough that both groups matter,
         * with room for a bomb that falls wide. */
        g->td_goal = (military + 1) / 2;
        if (g->td_goal < 2) g->td_goal = 2;
    }
}

static void td_screen(const Game *g, const TdObject *o, float *sx, float *sy)
{
    *sx = o->x;
    *sy = SCREEN_H - (o->world_x - g->scroll);
}

static void td_offence(Game *g, int civ)
{
    ++g->sortie_offences;
    g->sortie_offence_kind = civ;
}

static void td_wreck(Game *g, TdObject *o, int by_bomb)
{
    const TdKindInfo *k = &td_kinds[o->kind];
    float sx, sy;
    td_screen(g, o, &sx, &sy);
    o->wrecked = 1;
    o->wrecked_at = sim_now(g);
    o->speed = 0.0f;
    new_ground_explosion(g, sx, sy, by_bomb || k->hp >= 3 ? 3 : 1);
    g->audio.blast = 7000;
    if (k->military && !o->hazard) {
        ++g->objectives;
        g->score += 500;
    } else if (k->military) {
        g->score += 150;
    }
    if (k->civ != CIV_NONE) td_offence(g, k->civ);
}

/* Our own bomb bursting on our own trench line. */
static void td_bomb_on_own_line(Game *g, float x, float y)
{
    int i;
    float world = g->scroll + (SCREEN_H - y);
    for (i = 0; i < g->td_strip_count; ++i) {
        const TdStrip *t = &g->td_strips[i];
        if (t->sprite_a != TD_TILE_OURS_A) continue;
        if (fabsf(x - t->x) < 36.0f && world >= t->from && world <= t->to) {
            td_offence(g, CIV_OWN_LINE);
            return;
        }
    }
}

static void td_update(Game *g, float dt, uint32_t now)
{
    int i, j;
    for (i = 0; i < g->td_count; ++i) {
        TdObject *o = &g->td[i];
        const TdKindInfo *k = &td_kinds[o->kind];
        float sx, sy;
        if (!o->active) continue;
        o->world_x -= o->speed * dt;
        td_screen(g, o, &sx, &sy);
        if (sy > SCREEN_H + 140.0f) { o->active = 0; continue; }
        if (o->wrecked || sy < -80.0f || k->task) continue;
        if (k->fires && sy > 18.0f && sy < SCREEN_H - 12.0f &&
            now - o->last_shot > 1500u + (uint32_t)(i * 131 % 700)) {
            aimed_fire(g, sx, sy, 245.0f, 0);
            o->last_shot = now;
        }
        for (j = 0; j < MAX_BULLETS; ++j) {
            Bullet *b = &g->bullets[j];
            if (!b->active || b->enemy || b->missed || b->type == 4) continue;
            if (b->type == 1) {
                /* A bomb strikes where it lands, in its last moments. */
                if (b->life > 0.30f) continue;
                if (fabsf(b->x - sx) > (float)k->half_w + 14.0f ||
                    fabsf(b->y - sy) > (float)k->half_h + 14.0f) continue;
                b->active = 0;
                td_wreck(g, o, 1);
                break;
            }
            if (k->bomb_only) continue;
            if (fabsf(b->x - sx) < (float)k->half_w + 3.0f &&
                fabsf(b->y - sy) < (float)k->half_h + 3.0f) {
                b->active = 0;
                if (--o->hp <= 0) {
                    td_wreck(g, o, 0);
                    break;
                }
            }
        }
    }
}

/* A sprite from the painted atlas with its shadow cast to the lower right,
 * dark and well out from it so each target stands up off the mud. */
static void td_blit(Game *g, int sprite, int cx, int cy)
{
    SDL_Surface *s = g->screen, *a = g->topdown_targets;
    const short *r = topdown_sprite_rect[sprite];
    SDL_Rect src = {r[0], r[1], r[2], r[3]};
    SDL_Rect dst = {cx - r[2] / 2, cy - r[3] / 2, r[2], r[3]};
    uint32_t key = 0;
    int x, y;
    if (!a) return;
    if (dst.y > SCREEN_H || dst.y + r[3] < 0) return;
    SDL_GetColorKey(a, &key);
    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
    for (y = 0; y < r[3]; ++y) {
        int ty = dst.y + y + 6;
        const pix_t *row;
        pix_t *out;
        if (ty < 0 || ty >= s->h) continue;
        row = (const pix_t *)((const uint8_t *)a->pixels +
                              (r[1] + y) * a->pitch) + r[0];
        out = PIX_ROW(s, ty);
        for (x = 0; x < r[2]; ++x) {
            int tx = dst.x + x + 5;
            if (row[x] == (pix_t)key || tx < 0 || tx >= s->w) continue;
            out[tx] = pix_mix(out[tx], PIX_PACK(16, 14, 10), 19);
        }
    }
    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
    SDL_BlitSurface(a, &src, s, &dst);
}

static void td_draw_ground(Game *g)
{
    int i;
    for (i = 0; i < g->td_strip_count; ++i) {
        const TdStrip *t = &g->td_strips[i];
        const short *r = topdown_sprite_rect[t->sprite_a];
        int tile = r[3] > 1 ? r[3] - 1 : 64;   /* rows overlap by one */
        int k0 = (int)floorf((g->scroll - 40.0f - t->from) / (float)tile);
        int k1 = (int)floorf((g->scroll + SCREEN_H + 80.0f - t->from) /
                             (float)tile);
        int k;
        if (k0 < 0) k0 = 0;
        for (k = k0; k <= k1; ++k) {
            float world = t->from + (float)k * (float)tile;
            int sy;
            int sprite = t->sprite_b >= 0 && (k & 1) ? t->sprite_b : t->sprite_a;
            const short *rr = topdown_sprite_rect[sprite];
            SDL_Rect src = {rr[0], rr[1], rr[2], rr[3]}, dst;
            if (world > t->to) break;
            sy = SCREEN_H - (int)(world - g->scroll);
            dst.x = (int)t->x - rr[2] / 2;
            dst.y = sy - rr[3];
            dst.w = rr[2];
            dst.h = rr[3];
            if (g->topdown_targets)
                SDL_BlitSurface(g->topdown_targets, &src, g->screen, &dst);
        }
    }
}

static void td_draw_objects(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t now = sim_now(g);
    int i;
    for (i = 0; i < g->td_count; ++i) {
        const TdObject *o = &g->td[i];
        const TdKindInfo *k = &td_kinds[o->kind];
        float sx, sy;
        if (!o->active) continue;
        td_screen(g, o, &sx, &sy);
        if (sy < -80.0f || sy > SCREEN_H + 80.0f) continue;
        if (k->task) {
            draw_top_task(g, o, (int)sx, (int)sy);
            continue;
        }
        td_blit(g, o->wrecked ? k->wreck : k->whole, (int)sx, (int)sy);
        if (o->wrecked && now - o->wrecked_at < 20000u) {
            /* Fire and smoke drifting downwind from the wreck. */
            int p;
            for (p = 0; p < 4; ++p) {
                uint32_t t = (now / 70u + (uint32_t)(p * 37 + i * 11)) % 40u;
                int r = 4 + (int)t / 4;
                blend_circle(s, (int)sx + (int)t / 2 + (p - 2) * 3,
                             (int)sy - (int)t, r, 40, 38, 36,
                             (uint8_t)(120 - (int)t * 3));
            }
            if (((now / 90u) + (uint32_t)i) % 3u)
                blend_circle(s, (int)sx - 2, (int)sy, 5, 255, 150, 60, 170);
        }
    }
}

/* ---- The way there and back: hazards, legs and the run home ------------
 *
 * The squadron's complaint about the early sorties was that they were a
 * pleasant drive in the country with a few minutes of war in the middle.
 * Now every leg has something to do or to dodge every ten or twenty
 * seconds, and the run home is short and still has its sting.  The pieces
 * are cheap: a handful of puffs and figures, each a few circles or a small
 * blit, so a 68040 hardly notices them. */

static Vec3 aircraft_velocity(const FlightState *f);
static int convert_to_screen(Game *g, SDL_Surface **surface);
static void draw_ground_gun(Game *g, int sx, int ty, Target *target);
static void draw_field_sign(Game *g, int x, int y, int w, const char *label);
static void td_blit_plain(Game *g, int sprite, int cx, int cy);

static MissionMode sortie_mode(const Game *g)
{
    return campaign_missions[g->chapter][g->mission].mode;
}

/* QA runs keep the pilot alive, as the held QA views always have, but a
 * QA run still flies its sortie to the end. */
/* QA: TIME on the command line; see main(). */
static int qa_frame_time;
/* QA: SLOWFRAMES holds every sortie frame to about eight a second, to show
 * that a slow machine meets the same sortie as a fast one. */
static int qa_slow_frames;

static int qa_spared(const Game *g)
{
    return g->qa_hold || g->qa_run;
}

/* 1914 and 1915 wear the spiked helmet under its grey cover; from 1916
 * the steel helmet. */
static int rifleman_side_base(const Game *g)
{
    return g->chapter <= 1 ? RF_SIDE_1914 : RF_SIDE_1916;
}

static int rifleman_top_base(const Game *g)
{
    return g->chapter <= 1 ? RF_TOP_1914 : RF_TOP_1916;
}

static void rifleman_blit(Game *g, int sprite, int x, int y, int crop)
{
    const short *r = rifleman_rect[sprite];
    SDL_Rect src = {r[0], r[1], r[2], r[3] - crop};
    SDL_Rect dst = {x, y, r[2], r[3] - crop};
    if (g->riflemen_sheet && src.h > 0)
        SDL_BlitSurface(g->riflemen_sheet, &src, g->screen, &dst);
}

/* Where the world position of a 2-D sortie is: side-on along the ground,
 * top-down up the map.  The machine's own position in the same terms. */
static float player_world(const Game *g)
{
    if (sortie_mode(g) == MISSION_SIDE) return g->scroll + g->player_x;
    return g->scroll + (float)SCREEN_H - g->player_y;
}

/* Their side of the wire: from the front line out, until the run home
 * crosses back over our own line. */
static int enemy_ground(const Game *g, float world)
{
    if (world < g->front_at) return 0;
    if (g->leg != LEG_OUT && world > g->home_line_at) return 0;
    return 1;
}

/* Near the lines or the targets, where the infantry is thickest. */
static int near_the_lines(const Game *g, float world)
{
    if (fabsf(world - g->front_at) < 1600.0f) return 1;
    if (g->leg == LEG_OUT && world > g->front_at && world < g->task_end + 400.0f)
        return world > g->task_end - 6000.0f;
    if (g->leg != LEG_OUT && fabsf(world - g->home_line_at) < 1400.0f) return 1;
    return 0;
}

/* Holes in our own fabric, side-on and from above: a few dark specks that
 * stay on the machine for the rest of the sortie. */
static void add_fabric_hole(Game *g)
{
    uint32_t h = scene_hash(sim_now(g) ^ (uint32_t)g->fabric_hole_count *
                            2654435761u);
    int k = g->fabric_hole_count % MAX_FABRIC_HOLES;
    int side = sortie_mode(g) == MISSION_SIDE;
    g->fabric_holes[k][0] = (float)((int)(h % (side ? 46u : 44u)) -
                                    (side ? 26 : 22));
    g->fabric_holes[k][1] = (float)((int)((h >> 8) % (side ? 14u : 18u)) -
                                    (side ? 8 : 10));
    ++g->fabric_hole_count;
}

static void draw_fabric_holes(Game *g, int x, int y)
{
    SDL_Surface *s = g->screen;
    int k, n = g->fabric_hole_count < MAX_FABRIC_HOLES ?
               g->fabric_hole_count : MAX_FABRIC_HOLES;
    for (k = 0; k < n; ++k) {
        int hx = x + (int)g->fabric_holes[k][0];
        int hy = y + (int)g->fabric_holes[k][1];
        rect(s, hx, hy, 2, 2, PIX_PACK(26, 20, 16));
        pixel(s, hx + 2, hy, PIX_PACK(224, 210, 170));
        pixel(s, hx - 1, hy + 2, PIX_PACK(224, 210, 170));
    }
}

/* A small hit from the ground: a rifle bullet or a splinter.  A plink on
 * the wires, a hole in the fabric, a little damage. */
static void small_hit(Game *g, int damage)
{
    MissionMode mode = sortie_mode(g);
    if (!qa_spared(g) || g->health > 30) g->health -= damage;
    g->last_damage = sim_now(g);
    g->audio.plink = 900;
    if (mode == MISSION_SIDE || mode == MISSION_TOP_DOWN) add_fabric_hole(g);
    else if (mode == MISSION_DOGFIGHT_3D)
        add_dogfight_bullet_hole(g, sim_now(g) * 7919u);
    else {
        BulletHole *hole = &g->bullet_holes[g->next_bullet_hole];
        uint32_t scatter = scene_hash(sim_now(g) * 31u);
        hole->x = 190.0f + (float)(scatter % 261u);
        hole->y = 205.0f + (float)((scatter >> 9) % 126u);
        hole->active = 1;
        g->next_bullet_hole = (g->next_bullet_hole + 1) % MAX_BULLET_HOLES;
    }
    if (g->shake < 0.15f) g->shake = 0.15f;
}

/* A heavy hit: an Archie burst close aboard, a cable, a tree. */
static void heavy_hit(Game *g, int damage)
{
    small_hit(g, damage);
    g->audio.hit_player = 1;
    g->audio.hit = 1400;
    if (g->shake < 0.8f) g->shake = 0.8f;
    if (sortie_mode(g) == MISSION_SIDE || sortie_mode(g) == MISSION_TOP_DOWN)
        new_explosion(g, g->player_x - 4.0f, g->player_y, 0);
}

/* ---- Archie ------------------------------------------------------------ */

static Archie *new_archie(Game *g)
{
    int i;
    for (i = 0; i < MAX_ARCHIE; ++i)
        if (!g->archie[i].active) {
            memset(&g->archie[i], 0, sizeof(g->archie[i]));
            g->archie[i].active = 1;
            return &g->archie[i];
        }
    return NULL;
}

/* How long the battery takes between its ranging round and the aimed one:
 * the warning the pilot has.  Better gunners later in the war. */
static float archie_warning(const Game *g)
{
    return 1.45f - (float)g->chapter * 0.04f;
}

/* Fires one ranging round and lays the next on where the machine will be
 * if it holds its course.  Side-on and top-down positions are kept on the
 * ground's own axis so the puffs stay where they burst. */
static void archie_fire_2d(Game *g)
{
    MissionMode mode = sortie_mode(g);
    float lead = archie_warning(g);
    float err = 6.0f + (float)(CHAPTER_COUNT - g->chapter) * 2.5f;
    float side_sign = random_unit(g) < 0.5f ? -1.0f : 1.0f;
    Archie *a = new_archie(g);
    if (!a) return;
    a->kind = ARCHIE_RANGING;
    a->fuse = lead;
    if (mode == MISSION_SIDE) {
        a->tx = g->scroll + g->player_x + g->airspeed * lead +
                (random_unit(g) - 0.5f) * 2.0f * err;
        a->ty = g->player_y + g->player_vy * lead * 0.5f +
                (random_unit(g) - 0.5f) * 2.0f * err;
        a->ty = flight_clamp(a->ty, 40.0f, 320.0f);
        a->x = a->tx + side_sign * (55.0f + random_unit(g) * 55.0f);
        a->y = flight_clamp(a->ty + (random_unit(g) - 0.5f) * 90.0f,
                            30.0f, 300.0f);
    } else if (mode == MISSION_TOP_DOWN) {
        a->tx = flight_clamp(g->player_x + g->player_vx * lead +
                             (random_unit(g) - 0.5f) * 2.0f * err,
                             20.0f, SCREEN_W - 20.0f);
        a->ty = player_world(g) + g->airspeed * lead +
                (random_unit(g) - 0.5f) * 2.0f * err;
        a->x = flight_clamp(a->tx + side_sign * (60.0f + random_unit(g) * 70.0f),
                            16.0f, SCREEN_W - 16.0f);
        a->y = a->ty + (random_unit(g) - 0.3f) * 140.0f;
    } else {
        /* Over the rear seat: the bursts walk in towards the tail. */
        a->tx = 250.0f + random_unit(g) * 140.0f;
        a->ty = 110.0f + random_unit(g) * 110.0f;
        a->x = 60.0f + random_unit(g) * 520.0f;
        a->y = 30.0f + random_unit(g) * 140.0f;
    }
    if (g->audio.blast < 1500) g->audio.blast = 1500;
}

/* The same in the cockpit's world: the ranging puff some tens of metres
 * off, the aimed burst where the machine will be. */
static void archie_fire_3d(Game *g)
{
    Vec3 v = aircraft_velocity(&g->flight);
    float lead = archie_warning(g) + 0.25f;
    float err = 3.0f + (float)(CHAPTER_COUNT - g->chapter) * 1.2f;
    float ang = random_unit(g) * 6.2832f;
    float off = 45.0f + random_unit(g) * 35.0f;
    Archie *a = new_archie(g);
    if (!a) return;
    a->kind = ARCHIE_RANGING;
    a->fuse = lead;
    a->tp.x = g->flight_x + v.x * lead + (random_unit(g) - 0.5f) * 2.0f * err;
    a->tp.y = g->flight_y + v.y * lead + (random_unit(g) - 0.5f) * 2.0f * err;
    a->tp.z = g->flight_z + v.z * lead + (random_unit(g) - 0.5f) * 2.0f * err;
    a->p.x = a->tp.x + cosf(ang) * off;
    a->p.y = a->tp.y + (random_unit(g) - 0.4f) * 40.0f;
    a->p.z = a->tp.z + sinf(ang) * off;
    if (g->audio.blast < 1500) g->audio.blast = 1500;
}

/* Bursts the aimed round of a ranging puff whose fuse has run, and judges
 * it against where the machine is now.  Holding a steady course is what
 * the battery hopes for. */
static void archie_burst(Game *g, Archie *ranging)
{
    MissionMode mode = sortie_mode(g);
    Archie *b = new_archie(g);
    float d;
    if (!b) return;
    b->kind = ARCHIE_BURST;
    b->x = ranging->tx;
    b->y = ranging->ty;
    b->p = ranging->tp;
    if (mode == MISSION_SIDE || mode == MISSION_TOP_DOWN) {
        float px = mode == MISSION_SIDE ? g->scroll + g->player_x : g->player_x;
        float py = mode == MISSION_SIDE ? g->player_y : player_world(g);
        d = sqrtf((b->x - px) * (b->x - px) + (b->y - py) * (b->y - py));
        if (d < 30.0f) heavy_hit(g, 6 + g->chapter / 3);
        else if (d < 54.0f) small_hit(g, 2);
    } else if (mode == MISSION_DOGFIGHT_3D) {
        float dx = b->p.x - g->flight_x, dy = b->p.y - g->flight_y;
        float dz = b->p.z - g->flight_z;
        d = sqrtf(dx * dx + dy * dy + dz * dz);
        if (d < 16.0f) heavy_hit(g, 6 + g->chapter / 3);
        else if (d < 30.0f) small_hit(g, 2);
    } else {
        /* Tom sees the ranging round and jinks; now and then not enough. */
        if (random_unit(g) < 0.22f + (float)g->chapter * 0.02f)
            heavy_hit(g, 4 + g->chapter / 3);
    }
    g->audio.blast = 3600;
}

static void update_archie(Game *g, float dt)
{
    int i;
    for (i = 0; i < MAX_ARCHIE; ++i) {
        Archie *a = &g->archie[i];
        if (!a->active) continue;
        a->age += dt;
        if (a->kind == ARCHIE_RANGING && a->fuse > 0.0f) {
            a->fuse -= dt;
            if (a->fuse <= 0.0f) archie_burst(g, a);
        }
        if (a->age > (a->kind == ARCHIE_RANGING ? 2.8f : 3.4f)) a->active = 0;
    }
}

static void archie_screen(const Game *g, const Archie *a, int *sx, int *sy)
{
    MissionMode mode = sortie_mode(g);
    if (mode == MISSION_SIDE) {
        *sx = (int)(a->x - g->scroll);
        *sy = (int)a->y;
    } else if (mode == MISSION_TOP_DOWN) {
        *sx = (int)a->x;
        *sy = SCREEN_H - (int)(a->y - g->scroll);
    } else {
        *sx = (int)a->x;
        *sy = (int)a->y;
    }
}

/* German Archie bursts black, the "woolly bear": a flash, then a ragged
 * ball of smoke that spreads and thins.  The ranging round is a smaller
 * grey puff. */
static void draw_archie_puff(SDL_Surface *s, int x, int y, int radius,
                             int kind, float age, int index)
{
    float life = kind == ARCHIE_RANGING ? 2.8f : 3.4f;
    int alpha = (int)(220.0f * (1.0f - age / life));
    uint32_t h = scene_hash((uint32_t)index * 2654435761u + 77u);
    if (alpha <= 8 || radius < 1) return;
    if (kind == ARCHIE_BURST && age < 0.14f) {
        blend_circle(s, x, y, radius + 6, 255, 196, 96, 230);
        blend_circle(s, x, y, radius / 2 + 2, 255, 246, 210, 255);
        return;
    }
    if (kind == ARCHIE_RANGING) {
        blend_circle(s, x, y, radius, 92, 88, 82, (uint8_t)alpha);
        blend_circle(s, x - radius / 3, y - radius / 3, radius * 2 / 3,
                     150, 146, 138, (uint8_t)(alpha / 2));
        return;
    }
    blend_circle(s, x, y, radius, 26, 24, 22, (uint8_t)alpha);
    blend_circle(s, x + (int)(h % 7u) - 3 + radius / 2,
                 y + (int)((h >> 4) % 5u) - 2, radius * 3 / 5,
                 34, 30, 28, (uint8_t)(alpha * 4 / 5));
    blend_circle(s, x - radius / 2, y + (int)((h >> 9) % 5u) - 1,
                 radius / 2 + 1, 38, 34, 30, (uint8_t)(alpha * 3 / 4));
}

static void draw_archie_2d(Game *g)
{
    int i;
    for (i = 0; i < MAX_ARCHIE; ++i) {
        const Archie *a = &g->archie[i];
        int sx, sy, r;
        if (!a->active) continue;
        archie_screen(g, a, &sx, &sy);
        sy -= (int)(a->age * 5.0f);   /* the smoke rises a little */
        if (sx < -40 || sx > SCREEN_W + 40 || sy < -40 || sy > SCREEN_H + 40)
            continue;
        r = a->kind == ARCHIE_RANGING ? 3 + (int)(fminf(a->age, 0.5f) * 14.0f) :
                                        7 + (int)(fminf(a->age, 0.7f) * 20.0f);
        draw_archie_puff(g->screen, sx, sy, r, a->kind, a->age, i);
    }
}

static void draw_archie_3d(Game *g)
{
    int i;
    for (i = 0; i < MAX_ARCHIE; ++i) {
        const Archie *a = &g->archie[i];
        Projected3D p;
        float metres;
        int r;
        if (!a->active) continue;
        p = project_world_point(g, (Vec3){a->kind == ARCHIE_RANGING ?
            a->p.x : a->p.x, a->p.y + a->age * 1.5f, a->p.z});
        if (!p.visible || p.depth < 4.0f || p.x < -60 || p.x > SCREEN_W + 60 ||
            p.y < -60 || p.y > SCREEN_H + 60) continue;
        metres = a->kind == ARCHIE_RANGING ? 3.0f + fminf(a->age, 0.5f) * 5.0f :
                                             5.0f + fminf(a->age, 0.8f) * 9.0f;
        r = (int)(metres * WORLD_FOCAL / p.depth);
        if (r < 1) r = 1;
        if (r > 70) r = 70;
        draw_archie_puff(g->screen, p.x, p.y, r, a->kind, a->age, i);
    }
}

/* ---- Riflemen ---------------------------------------------------------- */

static Rifleman *new_rifleman(Game *g)
{
    int i;
    for (i = 0; i < MAX_RIFLEMEN; ++i)
        if (!g->riflemen[i].active) {
            memset(&g->riflemen[i], 0, sizeof(g->riflemen[i]));
            g->riflemen[i].active = 1;
            return &g->riflemen[i];
        }
    return NULL;
}

/* A man or two in a trench, a shell hole or by the road ahead, side-on;
 * or in the fields beside the machine's track from above. */
static void spawn_riflemen(Game *g, float world, int count)
{
    int k;
    int side = sortie_mode(g) == MISSION_SIDE;
    int cover = (int)(random_unit(g) * 3.0f);   /* road, trench, shell hole */
    for (k = 0; k < count; ++k) {
        Rifleman *r = new_rifleman(g);
        if (!r) return;
        if (side) {
            r->x = world + (float)k * (16.0f + random_unit(g) * 20.0f);
            r->cover = cover == 1 ? 9 : cover == 2 ? 7 : 0;
            r->stand = cover == 0 || random_unit(g) < 0.3f;
        } else {
            float lane = g->player_x + (random_unit(g) - 0.5f) * 360.0f;
            r->x = flight_clamp(lane + (float)k * 22.0f, 30.0f, SCREEN_W - 30.0f);
            r->y = world + (float)k * 14.0f;
        }
        r->state = RIFLE_HIDDEN;
        r->timer = random_unit(g) * 0.8f;
    }
}

static void rifleman_screen(const Game *g, const Rifleman *r, int *sx, int *sy)
{
    if (sortie_mode(g) == MISSION_SIDE) {
        *sx = (int)(r->x - g->scroll);
        *sy = terrain_y(r->x) + 2;            /* his feet */
    } else {
        *sx = (int)r->x;
        *sy = SCREEN_H - (int)(r->y - g->scroll);
    }
}

/* Our own bomb or burst going off among them. */
static void riflemen_blast(Game *g, float sx, float sy, float radius)
{
    int i;
    for (i = 0; i < MAX_RIFLEMEN; ++i) {
        Rifleman *r = &g->riflemen[i];
        int x, y;
        if (!r->active || r->state == RIFLE_DOWN) continue;
        rifleman_screen(g, r, &x, &y);
        if (fabsf((float)x - sx) < radius && fabsf((float)y - sy) < radius) {
            r->state = RIFLE_DOWN;
            g->score += 25;
        }
    }
}

/* The odd soldier taking a pot shot: he rises, aims, fires once with a
 * puff of smoke and a faint streak, and ducks again.  Most rounds miss;
 * a few go through the fabric. */
static void update_riflemen(Game *g, float dt)
{
    int i, j;
    int side = sortie_mode(g) == MISSION_SIDE;
    for (i = 0; i < MAX_RIFLEMEN; ++i) {
        Rifleman *r = &g->riflemen[i];
        int sx, sy, in_range;
        float dx, dy;
        if (!r->active) continue;
        rifleman_screen(g, r, &sx, &sy);
        if ((side && sx < -40) || (!side && sy > SCREEN_H + 30)) {
            r->active = 0;
            continue;
        }
        if (r->tracer > 0.0f) r->tracer -= dt;
        if (r->state == RIFLE_DOWN) continue;
        for (j = 0; j < MAX_BULLETS; ++j) {
            Bullet *b = &g->bullets[j];
            if (!b->active || b->enemy || b->type != 0) continue;
            if (side ? (fabsf(b->x - (float)sx) < 8.0f &&
                        b->y > (float)(sy - 26) && b->y < (float)sy + 2.0f) :
                       (fabsf(b->x - (float)sx) < 9.0f &&
                        fabsf(b->y - (float)sy) < 9.0f)) {
                b->active = 0;
                r->state = RIFLE_DOWN;
                g->score += 25;
                break;
            }
        }
        if (r->state == RIFLE_DOWN) continue;
        dx = g->player_x - (float)sx;
        dy = g->player_y - (float)sy;
        in_range = side ? (dx < 30.0f && dx > -420.0f && dy < -30.0f) :
                          (dy > -40.0f && dx * dx + dy * dy < 300.0f * 300.0f);
        r->timer -= dt;
        switch (r->state) {
        case RIFLE_HIDDEN:
            if (in_range && r->timer <= 0.0f) {
                r->state = RIFLE_RISE;
                r->timer = 0.35f;
            }
            break;
        case RIFLE_RISE:
            if (r->timer <= 0.0f) {
                r->state = RIFLE_AIM;
                r->timer = 0.45f + random_unit(g) * 0.55f;
            }
            break;
        case RIFLE_AIM:
            if (r->timer <= 0.0f) {
                float chance = 0.07f + (float)g->chapter * 0.006f;
                int hit;
                if (side && g->player_y > 250.0f) chance += 0.05f;
                if (!in_range) chance = 0.0f;
                hit = random_unit(g) < chance;
                r->state = RIFLE_FIRED;
                r->timer = 0.3f;
                r->tracer = 0.24f;
                ++r->shots;
                r->aim_x = g->player_x + (hit ? 0.0f : (random_unit(g) - 0.5f) * 90.0f);
                r->aim_y = g->player_y + (hit ? 0.0f : (random_unit(g) - 0.5f) * 60.0f);
                if (g->audio.crack < 1100) g->audio.crack = 1100;
                if (hit) small_hit(g, 2);
            }
            break;
        case RIFLE_FIRED:
            if (r->timer <= 0.0f) {
                r->state = RIFLE_DUCK;
                r->timer = 1.6f + random_unit(g) * 1.4f;
            }
            break;
        case RIFLE_DUCK:
            if (r->timer <= 0.0f && r->shots < 2 && in_range) {
                r->state = RIFLE_RISE;
                r->timer = 0.35f;
            }
            break;
        default:
            break;
        }
    }
}

/* The muzzle in screen terms for a frame drawn with its top-left at x, y. */
static void rifleman_muzzle_at(int sprite, int x, int y, int *mx, int *my)
{
    *mx = x + rifleman_muzzle[sprite][0];
    *my = y + rifleman_muzzle[sprite][1];
}

static void draw_riflemen(Game *g)
{
    SDL_Surface *s = g->screen;
    int i;
    int side = sortie_mode(g) == MISSION_SIDE;
    for (i = 0; i < MAX_RIFLEMEN; ++i) {
        const Rifleman *r = &g->riflemen[i];
        int sx, sy, sprite, x, y, mx, my, crop = 0;
        if (!r->active) continue;
        rifleman_screen(g, r, &sx, &sy);
        if (sx < -30 || sx > SCREEN_W + 30 || sy < -30 || sy > SCREEN_H + 40)
            continue;
        if (side) {
            int base = rifleman_side_base(g);
            int aiming = r->state == RIFLE_AIM || r->state == RIFLE_FIRED;
            sprite = base + (r->state == RIFLE_DOWN ? 4 :
                             r->state == RIFLE_RISE ? 1 :
                             aiming ? (r->stand ? 3 : 2) : 0);
            /* Ducked in a trench or a shell hole, only his helmet shows
             * above the parapet. */
            if (r->cover && !aiming && r->state != RIFLE_RISE &&
                r->state != RIFLE_DOWN)
                crop = r->cover + 6;
            else if (r->cover && r->state != RIFLE_DOWN)
                crop = r->cover;
            x = sx - rifleman_rect[sprite][2] / 2;
            y = sy - rifleman_rect[sprite][3];
            rifleman_blit(g, sprite, x, y, crop);
            if (r->cover == 9) {
                int k;
                for (k = -2; k <= 2; ++k)
                    circle(s, sx + k * 5, sy - 4 - (k & 1), 3,
                           (k & 1) ? PIX_PACK(119, 96, 66) : PIX_PACK(104, 84, 58));
            } else if (r->cover == 7) {
                ellipse(s, sx, sy - 2, 13, 4, PIX_PACK(58, 46, 34));
                ellipse(s, sx - 3, sy - 4, 8, 2, PIX_PACK(86, 70, 50));
            }
        } else {
            int base = rifleman_top_base(g);
            if (r->state == RIFLE_DOWN) {
                sprite = base + 8;
            } else {
                float a = atan2f(g->player_x - (float)sx, (float)sy - g->player_y);
                int dir = (int)floorf(a / (float)(M_PI / 4.0) + 0.5f);
                sprite = base + (dir & 7);
            }
            x = sx - 8;
            y = sy - 8;
            rifleman_blit(g, sprite, x, y, 0);
        }
        rifleman_muzzle_at(sprite, x, y, &mx, &my);
        if (r->tracer > 0.0f) {
            /* The flash, the puff of smoke and the round's faint streak
             * going up at us. */
            float t = 1.0f - r->tracer / 0.24f;
            float hx = (float)mx + (r->aim_x - (float)mx) * t;
            float hy = (float)my + (r->aim_y - (float)my) * t;
            float len = sqrtf((r->aim_x - (float)mx) * (r->aim_x - (float)mx) +
                              (r->aim_y - (float)my) * (r->aim_y - (float)my));
            if (t < 0.25f) blend_circle(s, mx, my, 4, 255, 214, 130, 230);
            blend_circle(s, mx - 2, my - 2 - (int)(t * 6.0f), 4 + (int)(t * 5.0f),
                         206, 204, 196, (uint8_t)(150 - (int)(t * 110.0f)));
            if (len > 1.0f) {
                float ux = (r->aim_x - (float)mx) / len;
                float uy = (r->aim_y - (float)my) / len;
                line(s, (int)hx, (int)hy, (int)(hx - ux * 12.0f),
                     (int)(hy - uy * 12.0f), PIX_PACK(236, 226, 176));
            }
        }
    }
}

/* ---- Squalls, balloons, trees and posts -------------------------------- */

static void spawn_squall_2d(Game *g)
{
    Squall *q = &g->squall;
    if (sortie_mode(g) == MISSION_SIDE) {
        q->a = g->scroll + SCREEN_W + 40.0f;
        q->b = q->a + 480.0f + random_unit(g) * 320.0f;
        /* High cloud to fly under, or low cloud to climb over. */
        if (g->player_y < 200.0f) { q->lo = 36.0f; q->hi = 205.0f; }
        else { q->lo = 175.0f; q->hi = 330.0f; }
    } else {
        float centre = flight_clamp(g->player_x + (random_unit(g) - 0.5f) * 160.0f,
                                    150.0f, SCREEN_W - 150.0f);
        q->a = g->scroll + SCREEN_H + 40.0f;
        q->b = q->a + 380.0f + random_unit(g) * 260.0f;
        q->lo = centre - 120.0f - random_unit(g) * 50.0f;
        q->hi = centre + 120.0f + random_unit(g) * 50.0f;
    }
    q->active = 1;
}

/* Inside the squall: blind, thrown about, and battered. */
static int in_squall_2d(const Game *g)
{
    const Squall *q = &g->squall;
    float w = player_world(g);
    if (!q->active || w < q->a || w > q->b) return 0;
    if (sortie_mode(g) == MISSION_SIDE)
        return g->player_y > q->lo && g->player_y < q->hi;
    return g->player_x > q->lo && g->player_x < q->hi;
}

static void update_squall_2d(Game *g, float dt)
{
    Squall *q = &g->squall;
    static float battering;
    if (!q->active) return;
    if (player_world(g) > q->b + 300.0f) { q->active = 0; return; }
    if (!in_squall_2d(g)) return;
    if (sortie_mode(g) == MISSION_SIDE)
        g->player_vy += (random_unit(g) - 0.5f) * 900.0f * dt;
    else
        g->player_x += (random_unit(g) - 0.5f) * 260.0f * dt;
    battering += dt;
    if (battering > 1.1f) {
        battering = 0.0f;
        small_hit(g, 1);
        if (g->shake < 0.3f) g->shake = 0.3f;
    }
}

/* ---- Rain cloud, drawn cheaply -------------------------------------------
 *
 * A squall is drawn as a few overlapping soft blobs of cloud.  Each is laid
 * down in blocks of four by four pixels: the density of the block is the
 * fall-off towards the blob's edge times a small tiling noise, and an
 * ordered dither turns that density into a pattern of pixels.  The screen
 * is never read, so the cost is a table look-up and a few stores a block,
 * yet the cloud is ragged, uneven and lets the ground show through. */

static uint8_t cloud_noise[32][32];

static void make_cloud_noise(void)
{
    static int ready;
    uint8_t coarse[8][8];
    int x, y;
    if (ready) return;
    ready = 1;
    for (y = 0; y < 8; ++y)
        for (x = 0; x < 8; ++x)
            coarse[y][x] = (uint8_t)(scene_hash((uint32_t)(y * 8 + x) *
                                                2654435761u + 0x434c44u) & 255u);
    for (y = 0; y < 32; ++y)
        for (x = 0; x < 32; ++x) {
            /* Two octaves of smooth value noise, wrapping at the edges. */
            int cx = x >> 2, cy = y >> 2, fx = x & 3, fy = y & 3;
            int a = coarse[cy][cx], b = coarse[cy][(cx + 1) & 7];
            int c = coarse[(cy + 1) & 7][cx], d = coarse[(cy + 1) & 7][(cx + 1) & 7];
            int top = a * (4 - fx) + b * fx, bottom = c * (4 - fx) + d * fx;
            int smooth = (top * (4 - fy) + bottom * fy) / 16;
            int fine = (int)(scene_hash((uint32_t)(y * 32 + x) * 977u) & 63u);
            cloud_noise[y][x] = (uint8_t)(smooth * 3 / 4 + fine);
        }
}

/* The sixteen levels of a 4x4 ordered dither, one 4-bit mask a row. */
static const uint8_t dither_rows[17][4] = {
    {0x0, 0x0, 0x0, 0x0}, {0x1, 0x0, 0x0, 0x0}, {0x1, 0x0, 0x4, 0x0},
    {0x5, 0x0, 0x4, 0x0}, {0x5, 0x0, 0x5, 0x0}, {0x5, 0x2, 0x5, 0x0},
    {0x5, 0x2, 0x5, 0x8}, {0x5, 0xa, 0x5, 0x8}, {0x5, 0xa, 0x5, 0xa},
    {0x7, 0xa, 0x5, 0xa}, {0x7, 0xa, 0xd, 0xa}, {0xf, 0xa, 0xd, 0xa},
    {0xf, 0xa, 0xf, 0xa}, {0xf, 0xe, 0xf, 0xa}, {0xf, 0xe, 0xf, 0xb},
    {0xf, 0xf, 0xf, 0xb}, {0xf, 0xf, 0xf, 0xf}
};

/* The density, 0 to 'strength', at the middle of the 4-pixel block whose
 * left edge is bx, on row yy, and whether that place is on the lit side.
 * The noise is read in the blob's own coordinates, offset by (ox, oy), so
 * the cloud keeps its shape wherever it is drawn. */
static int cloud_level(int bx, int yy, int cx, int cy, int rx, int ry,
                       int ox, int oy, int strength, int *lit)
{
    int dx = (bx + 2 - cx) * 256 / rx, dy = (yy - cy) * 256 / ry;
    int e = 65536 - dx * dx - dy * dy, n, level;
    if (e <= 0) { *lit = 0; return 0; }
    n = cloud_noise[((dy * 12 >> 8) + 16 + oy) & 31][((dx * 12 >> 8) + 16 + ox) & 31];
    /* Fall-off shaped so the middle is thick and the edge frays into the
     * noise; lit above, shadowed below, the line between them broken by
     * the noise so it never runs straight. */
    level = (e >> 8) * (n + 40) / 300;                  /* 0 to about 256 */
    level = level * strength * 3 / 512 - 1;
    if (level > strength) level = strength;
    *lit = n;                       /* the caller decides light or dark */
    return level > 0 ? level : 0;
}

/* One soft blob of cloud centred on (cx, cy); strength is the densest
 * level, 0 to 16; the upper part is lit, the lower dark.  The density is
 * worked out on a grid every four pixels and eased row by row between
 * the grid lines, so the edges fray smoothly instead of in steps. */
static void draw_cloud_blob(SDL_Surface *s, int cx, int cy, int rx, int ry,
                            int ox, int oy, int strength, pix_t light,
                            pix_t dark)
{
    enum { MAX_BLOCKS = SCREEN_W / 4 + 2 };
    int top[MAX_BLOCKS], bottom[MAX_BLOCKS];
    unsigned char lit_top[MAX_BLOCKS], lit_bottom[MAX_BLOCKS];
    int bx, by, x0, x1, y0, y1, n, k, r;
    if (rx < 4 || ry < 4 || strength <= 0) return;
    make_cloud_noise();
    x0 = (cx - rx) & ~3; x1 = cx + rx;
    y0 = (cy - ry) & ~3; y1 = cy + ry;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > s->w - 4) x1 = s->w - 4;
    if (y1 > s->h - 4) y1 = s->h - 4;
    if (x1 < x0 || y1 < y0) return;
    n = (x1 - x0) / 4 + 1;
    if (n > MAX_BLOCKS) n = MAX_BLOCKS;
    for (k = 0; k < n; ++k) {
        int lit;
        bottom[k] = cloud_level(x0 + k * 4, y0, cx, cy, rx, ry, ox, oy,
                                strength, &lit);
        lit_bottom[k] = (unsigned char)lit;
    }
    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
    for (by = y0; by <= y1; by += 4) {
        int any = 0;
        for (k = 0; k < n; ++k) {
            int lit;
            top[k] = bottom[k];
            lit_top[k] = lit_bottom[k];
            bottom[k] = cloud_level(x0 + k * 4, by + 4, cx, cy, rx, ry, ox, oy,
                                    strength, &lit);
            lit_bottom[k] = (unsigned char)lit;
            any |= top[k] | bottom[k];
        }
        if (!any) continue;
        for (r = 0; r < 4; ++r) {
            int yy = by + r, rowdy;
            pix_t *row;
            if (yy >= s->h) break;
            row = PIX_ROW(s, yy);
            rowdy = (yy - cy) * 256 / ry;
            for (k = 0, bx = x0; k < n; ++k, bx += 4) {
                int level = (top[k] * (4 - r) + bottom[k] * r + 2) >> 2;
                uint8_t m;
                pix_t c;
                if (level <= 0) continue;
                m = dither_rows[level > 16 ? 16 : level][yy & 3];
                if (!m) continue;
                /* Lit above, shadowed below; the line between them is
                 * pushed about by the noise, row by row, so it never
                 * runs straight. */
                c = rowdy + ((int)lit_top[k] - 128) / 2 +
                    ((bx + yy * 3) & 15) - 8 < -50 ? light : dark;
                if (m & 1u) row[bx] = c;
                if (m & 2u) row[bx + 1] = c;
                if (m & 4u) row[bx + 2] = c;
                if (m & 8u) row[bx + 3] = c;
            }
        }
    }
    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
}

/* Rain under the cloud: faint slanting streaks, every other pixel. */
static void draw_rain(SDL_Surface *s, int x0, int x1, int top, int bottom,
                      int slant, uint32_t seed, pix_t c)
{
    int k, n = (x1 - x0) / 7;
    uint32_t now = SDL_GetTicks();
    if (x1 <= x0 || bottom <= top) return;
    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
    for (k = 0; k < n; ++k) {
        uint32_t h = scene_hash(seed + (uint32_t)k * 2654435761u);
        int x = x0 + (int)(h % (uint32_t)(x1 - x0 + 1));
        int len = 10 + (int)((h >> 8) & 15u);
        int y = top + (int)(((h >> 12) + now / 9u) % (uint32_t)(bottom - top + 1));
        int j;
        for (j = 0; j < len; j += 2) {
            int px = x - (j * slant) / 8, py = y + j;
            if (px < 0 || px >= s->w || py < 0 || py >= s->h || py > bottom) continue;
            PIX_ROW(s, py)[px] = c;
        }
    }
    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
}

/* Inside the cloud: grey everywhere, thickening and thinning. */
/* A quarter of the screen's own colour and three quarters of a grey that
 * changes from row to row: a rain haze, blended rather than dithered.  On
 * a 16-bit screen it is a shift and a mask per pixel, two pixels to a
 * 32-bit word, and one add of the row's grey taken to three quarters. */
static void haze_screen(SDL_Surface *s, const pix_t *row_tint)
{
    int y;
    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
    for (y = 0; y < s->h; ++y) {
#if CANVAS_SCREEN_BITS == 16
        uint32_t *w = (uint32_t *)PIX_ROW(s, y);
        uint32_t t = row_tint[y];
        uint32_t three = ((t >> 1) & 0x7BEFu) + ((t >> 2) & 0x39E7u);
        uint32_t add = three | (three << 16);
        int n = s->w / 2, x;
        for (x = 0; x < n; ++x)
            w[x] = ((w[x] >> 2) & 0x39E739E7u) + add;
#else
        pix_t *row = PIX_ROW(s, y);
        uint32_t t = row_tint[y];
        uint32_t add = ((t >> 1) & 0x7F7F7Fu) + ((t >> 2) & 0x3F3F3Fu);
        int x;
        for (x = 0; x < s->w; ++x)
            row[x] = (pix_t)(0xff000000u | (((row[x] >> 2) & 0x3F3F3Fu) + add));
#endif
    }
    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
}

/* Inside the cloud: everything half lost in grey rain haze, lighter
 * above and darker below, thickening and thinning in slow bands as the
 * cloud drives past, with the rain streaks over it. */
static void draw_blinding_cloud(Game *g)
{
    SDL_Surface *s = g->screen;
    static pix_t tint[SCREEN_H];
    float drift = (float)sim_now(g) * 0.0021f;
    int y;
    for (y = 0; y < SCREEN_H && y < s->h; ++y) {
        int v = 150 - y * 50 / SCREEN_H +
                (int)(sinf((float)y * 0.027f + drift) * 14.0f +
                      sinf((float)y * 0.071f - drift * 1.7f) * 7.0f);
        tint[y] = PIX_PACK(v, v + 3, v + 9);
    }
    haze_screen(s, tint);
    draw_rain(s, 0, SCREEN_W - 1, 0, SCREEN_H - 1, 3,
              sim_now(g) / 160u, PIX_PACK(186, 192, 202));
}

static void draw_squall_2d(Game *g)
{
    SDL_Surface *s = g->screen;
    const Squall *q = &g->squall;
    uint32_t seed = scene_hash((uint32_t)q->a);
    int k, blobs;
    if (!q->active) return;
    if (sortie_mode(g) == MISSION_SIDE) {
        int x0 = (int)(q->a - g->scroll), x1 = (int)(q->b - g->scroll);
        int lo = (int)q->lo, hi = (int)q->hi, w = x1 - x0, h = hi - lo;
        int ox = (int)q->a;
        if (x1 < -160 || x0 > SCREEN_W + 160) return;
        /* Rain falling from its foot to the ground, a curtain of it. */
        draw_rain(s, x0 + w / 8, x1 - w / 8, hi - 10, 360, 4, seed,
                  PIX_PACK(128, 136, 148));
        draw_rain(s, x0 + w / 5, x1 - w / 5, hi - 10, 360, 4, seed ^ 0x55u,
                  PIX_PACK(150, 158, 170));
        /* The body: big billows and smaller ones, each its own size. */
        blobs = 4 + w / 160;
        for (k = 0; k < blobs; ++k) {
            uint32_t r = scene_hash(seed + (uint32_t)k * 7919u);
            int bx = x0 + (int)(r % (uint32_t)(w + 1));
            int by = lo + h / 3 + (int)((r >> 8) % (uint32_t)(h / 3 + 1));
            int rx = 60 + (int)((r >> 4) % 90u);
            int ry = h / 3 + (int)((r >> 12) % (uint32_t)(h / 4 + 1));
            draw_cloud_blob(s, bx, by, rx, ry, (int)(r & 31u), k * 7,
                            11 + (int)((r >> 20) % 3u),
                            PIX_PACK(156, 158, 166), PIX_PACK(74, 78, 88));
        }
        /* Heavy, dark under-belly where the rain comes out, in a few
         * billows of their own rather than one long slab. */
        for (k = 0; k < 3; ++k)
            draw_cloud_blob(s, x0 + w / 6 + k * w / 3 + (int)((seed >> (k * 5)) % 40u) - 20,
                            hi - h / 5 - (int)((seed >> (k * 3 + 2)) % 12u),
                            w / 5 + 24, h / 4, ox + k * 9, 17 + k * 5, 12,
                            PIX_PACK(96, 100, 110), PIX_PACK(50, 54, 62));
    } else {
        int y1 = SCREEN_H - (int)(q->a - g->scroll);
        int y0 = SCREEN_H - (int)(q->b - g->scroll);
        int lo = (int)q->lo, hi = (int)q->hi, w = hi - lo, h = y1 - y0;
        if (y1 < -160 || y0 > SCREEN_H + 160) return;
        /* Its shadow on the ground, offset down-sun, then the cloud. */
        draw_cloud_blob(s, (lo + hi) / 2 + 26, (y0 + y1) / 2 + 30, w / 2,
                        h / 2, 5, 9, 7, PIX_PACK(30, 32, 28), PIX_PACK(30, 32, 28));
        blobs = 5 + (w * h) / 26000;
        for (k = 0; k < blobs; ++k) {
            uint32_t r = scene_hash(seed + (uint32_t)k * 7919u);
            int bx = lo + w / 6 + (int)(r % (uint32_t)(w * 2 / 3 + 1));
            int by = y0 + h / 6 + (int)((r >> 8) % (uint32_t)(h * 2 / 3 + 1));
            int rad = 50 + (int)((r >> 4) % 70u);
            draw_cloud_blob(s, bx, by, rad, rad * 4 / 5 + (int)((r >> 14) % 20u),
                            (int)(r & 31u), k * 7, 10 + (int)((r >> 20) % 5u),
                            PIX_PACK(176, 178, 182), PIX_PACK(98, 102, 112));
        }
        draw_rain(s, lo + 10, hi - 10, y0 > 0 ? y0 : 0, y1 < SCREEN_H ? y1 : SCREEN_H - 1,
                  3, seed, PIX_PACK(190, 196, 206));
    }
    if (in_squall_2d(g)) draw_blinding_cloud(g);
}

/* A kite balloon on its cable.  Side-on the cable runs from the winch to
 * the basket and must be climbed over; from above the balloon must be
 * flown round.  Either way it may be shot down. */
static void spawn_balloon(Game *g)
{
    KiteBalloon *k = &g->balloon;
    memset(k, 0, sizeof(*k));
    k->active = 1;
    if (sortie_mode(g) == MISSION_SIDE) {
        k->x = g->scroll + SCREEN_W + 80.0f;
        k->y = 68.0f + random_unit(g) * 40.0f;
    } else {
        k->x = flight_clamp(g->player_x + (random_unit(g) - 0.5f) * 120.0f,
                            60.0f, SCREEN_W - 60.0f);
        k->y = g->scroll + SCREEN_H + 60.0f;
    }
}

static void balloon_screen(const Game *g, int *sx, int *sy)
{
    const KiteBalloon *k = &g->balloon;
    if (sortie_mode(g) == MISSION_SIDE) {
        *sx = (int)(k->x - g->scroll);
        *sy = (int)(k->y - k->drift);
    } else {
        *sx = (int)k->x;
        *sy = SCREEN_H - (int)(k->y - g->scroll);
    }
}

static void update_balloon(Game *g, float dt)
{
    KiteBalloon *k = &g->balloon;
    int side = sortie_mode(g) == MISSION_SIDE;
    int sx, sy, j;
    if (!k->active) return;
    balloon_screen(g, &sx, &sy);
    if ((side && sx < -80) || (!side && sy > SCREEN_H + 80)) {
        k->active = 0;
        return;
    }
    if (k->cut) { k->drift += 30.0f * dt; return; }
    for (j = 0; j < MAX_BULLETS; ++j) {
        Bullet *b = &g->bullets[j];
        if (!b->active || b->enemy || b->type != 0 || b->missed) continue;
        if (side ? (fabsf(b->x - (float)sx) < 24.0f && fabsf(b->y - (float)sy) < 10.0f) :
                   (fabsf(b->x - (float)sx) < 11.0f && fabsf(b->y - (float)sy) < 24.0f)) {
            b->active = 0;
            k->cut = 1;
            new_explosion(g, (float)sx, (float)sy, 1);
            g->audio.blast = 5200;
            g->score += 150;
            show_notice(g, "BALLOON DOWN");
            return;
        }
    }
    if (g->tree_struck < 1.2f) return;
    if (side) {
        float dx = g->player_x - (float)sx;
        int cable = fabsf(dx) < 10.0f && g->player_y > (float)sy + 12.0f;
        int body = fabsf(dx) < 30.0f && fabsf(g->player_y - (float)sy) < 14.0f;
        if (cable || body) {
            heavy_hit(g, cable ? 16 : 10);
            g->player_vy = cable ? 70.0f : -60.0f;
            g->tree_struck = 0.0f;
            k->cut = 1;
            show_notice(g, "BALLOON CABLE!");
        }
    } else if (fabsf(g->player_x - (float)sx) < 30.0f &&
               fabsf(g->player_y - (float)sy) < 34.0f) {
        heavy_hit(g, 12);
        g->tree_struck = 0.0f;
        k->cut = 1;
        show_notice(g, "BALLOON CABLE!");
    }
}

static void draw_balloon(Game *g)
{
    SDL_Surface *s = g->screen;
    const KiteBalloon *k = &g->balloon;
    int sx, sy;
    if (!k->active) return;
    balloon_screen(g, &sx, &sy);
    if (sx < -60 || sx > SCREEN_W + 60 || sy < -60 || sy > SCREEN_H + 60) return;
    if (sortie_mode(g) == MISSION_SIDE) {
        int ground = terrain_y(k->x);
        if (!k->cut) {
            line(s, sx, sy + 10, sx + 2, ground - 6, PIX_PACK(36, 34, 30));
        } else {
            line(s, sx, sy + 10, sx - 6, sy + 70, PIX_PACK(36, 34, 30));
        }
        rect(s, sx - 9, ground - 8, 18, 8, PIX_PACK(64, 66, 52));      /* winch */
        rect(s, sx - 6, ground - 12, 7, 5, PIX_PACK(48, 50, 40));
        ellipse(s, sx, sy, 25, 10, PIX_PACK(150, 140, 104));
        ellipse(s, sx - 2, sy - 3, 20, 5, PIX_PACK(196, 186, 146));
        fill_triangle(s, sx + 18, sy - 2, sx + 30, sy - 12, sx + 26, sy,
                      PIX_PACK(140, 130, 96));
        fill_triangle(s, sx + 18, sy + 2, sx + 30, sy + 12, sx + 26, sy,
                      PIX_PACK(120, 110, 82));
        rect(s, sx - 3, sy + 11, 6, 4, PIX_PACK(80, 60, 40));          /* basket */
        if (k->cut)
            blend_circle(s, sx + 4, sy - 6, 9, 60, 54, 48, 160);
    } else {
        blend_circle(s, sx + 18, sy + 22, 14, 20, 22, 18, 90);         /* shadow */
        ellipse(s, sx, sy, 10, 25, PIX_PACK(150, 140, 104));
        ellipse(s, sx - 3, sy - 2, 5, 19, PIX_PACK(196, 186, 146));
        fill_triangle(s, sx - 2, sy + 18, sx - 12, sy + 30, sx, sy + 26,
                      PIX_PACK(130, 120, 90));
        fill_triangle(s, sx + 2, sy + 18, sx + 12, sy + 30, sx, sy + 26,
                      PIX_PACK(120, 110, 82));
        if (k->cut)
            blend_circle(s, sx, sy, 12, 50, 46, 40, 150);
    }
}

/* The painted poplars of the near ground are real: at low level the
 * machine must climb over them.  This finds the near-ground element at
 * the machine's position, as draw_element_layer places it. */
static void side_tree_check(Game *g)
{
    const int spacing = 235;
    int camera = (int)g->scroll;
    int segment = (camera + (int)g->player_x) / spacing;
    int n;
    if (g->tree_struck < 1.2f || !g->near_props[2]) return;
    for (n = segment - 1; n <= segment; ++n) {
        uint32_t seed = scene_hash((uint32_t)n * 2654435761u ^
                                   (uint32_t)(g->chapter + 1) * 2246822519u ^
                                   (uint32_t)(g->mission + 1) * 3266489917u ^
                                   0x4e454152u);
        int jitter = (int)((seed >> 9) % (uint32_t)(spacing / 3));
        int x = n * spacing - camera + jitter;
        int top = 390 - g->near_props[2]->h + 14;
        if (seed % (uint32_t)NEAR_ELEMENT_COUNT != 2u) continue;
        if (fabsf(g->player_x - (float)(x + g->near_props[2]->w / 2)) < 40.0f &&
            g->player_y + 12.0f > (float)top) {
            heavy_hit(g, 5);
            g->player_vy = -120.0f;
            g->tree_struck = 0.0f;
            show_notice(g, "TREES!");
            return;
        }
    }
}

/* Whether a near-ground poplar stands in the next two seconds of flight
 * and the machine is low enough to meet it. */
static int side_tree_ahead(const Game *g)
{
    const int spacing = 235;
    int camera = (int)g->scroll;
    int reach = (int)(g->airspeed * 2.0f);
    int first = (camera + (int)g->player_x) / spacing;
    int n;
    if (!g->near_props[2] || g->player_y < 390.0f - (float)g->near_props[2]->h - 10.0f)
        return 0;
    for (n = first; n <= first + reach / spacing + 1; ++n) {
        uint32_t seed = scene_hash((uint32_t)n * 2654435761u ^
                                   (uint32_t)(g->chapter + 1) * 2246822519u ^
                                   (uint32_t)(g->mission + 1) * 3266489917u ^
                                   0x4e454152u);
        int jitter = (int)((seed >> 9) % (uint32_t)(spacing / 3));
        int x = n * spacing - camera + jitter + g->near_props[2]->w / 2;
        if (seed % (uint32_t)NEAR_ELEMENT_COUNT != 2u) continue;
        if (x > (int)g->player_x - 20 && x < (int)g->player_x + reach) return 1;
    }
    return 0;
}

/* A machine-gun post met on the way, not one of the sortie's targets. */
static void spawn_mg_post(Game *g)
{
    int i;
    if (sortie_mode(g) == MISSION_SIDE) {
        for (i = 0; i < MAX_TARGETS; ++i) {
            Target *t = &g->targets[i];
            if (t->active) continue;
            memset(t, 0, sizeof(*t));
            t->active = 1;
            t->hazard = 1;
            t->type = 0;
            t->world_x = g->scroll + SCREEN_W + 60.0f + random_unit(g) * 80.0f;
            return;
        }
    } else {
        TdObject *o = NULL;
        for (i = 0; i < g->td_count; ++i)
            if (!g->td[i].active && g->td[i].hazard) { o = &g->td[i]; break; }
        if (!o && g->td_count < MAX_TD_OBJECTS) o = &g->td[g->td_count++];
        if (!o) return;
        memset(o, 0, sizeof(*o));
        o->kind = TDK_MG;
        o->hp = td_kinds[TDK_MG].hp;
        o->active = 1;
        o->hazard = 1;
        o->world_x = g->scroll + SCREEN_H + 50.0f;
        o->x = flight_clamp(g->player_x + (random_unit(g) - 0.5f) * 300.0f,
                            50.0f, SCREEN_W - 50.0f);
    }
}

/* A ground machine gun firing up at a climbing angle: its tracer curves
 * as it slows and drops, the arc the pilots watched come up at them. */
static void ground_mg_arc(Game *g, float x, float y)
{
    const float gravity = 70.0f;
    float dx = g->player_x - x, dy = g->player_y - y;
    float dist = sqrtf(dx * dx + dy * dy);
    float t = dist / 300.0f + 0.05f;
    float spread = (random_unit(g) - 0.5f) * 26.0f;
    float vx = (dx + spread) / t;
    float vy = (dy + spread * 0.6f) / t - 0.5f * gravity * t;
    if (new_bullet(g, x, y, vx, vy, 1, 3, t + 0.7f))
        g->audio.shot = 1200;
}

/* ---- Enemy scouts in the flat views ------------------------------------- */

/* A side-on enemy machine drawn facing right, for the ones that come up
 * from behind on the run home. */
static void draw_plane_mirrored(Game *g, int x, int y, int pitch_frame)
{
    int which = machine_atlas(1);
    SDL_Surface *atlas = which >= 0 && g->machine_sprites[which] ?
                         g->machine_sprites[which] : g->enemy_sprites;
    SDL_Surface *dst = g->screen;
    const int source_w = 96, source_h = 48, draw_w = 72, draw_h = 36;
    int dx, dy;
    if (!atlas) { draw_vector_plane(dst, x, y, 1); return; }
    pitch_frame = side_pitch_frame(pitch_frame);
    if (SDL_MUSTLOCK(atlas)) SDL_LockSurface(atlas);
    for (dy = 0; dy < draw_h; ++dy) {
        int sy = dy * source_h / draw_h;
        int ty = y - draw_h / 2 + dy;
        for (dx = 0; dx < draw_w; ++dx) {
            int sx = pitch_frame * source_w + (draw_w - 1 - dx) * source_w / draw_w;
            uint8_t r, green, b;
            uint32_t source = surface_pixel(atlas, sx, sy);
            unpack_rgb(atlas->format, source, &r, &green, &b);
            if (r > 180 && b > 180 && green < 130) continue;
            pixel(dst, x - draw_w / 2 + dx, ty,
                  atlas->format->format == dst->format->format ?
                  source : rgb(dst, r, green, b));
        }
    }
    if (SDL_MUSTLOCK(atlas)) SDL_UnlockSurface(atlas);
}

static int enemies_in_play(const Game *g)
{
    int i, n = 0;
    for (i = 0; i < MAX_ENEMIES; ++i) if (g->enemies[i].active) ++n;
    return n;
}

/* Side-on: a pair coming head-on out of the east, or, on the run home, a
 * pair overhauling us from behind.  Top-down the same, down or up the map. */
static void spawn_scouts_2d(Game *g, int count, int chasing)
{
    int i, k = 0;
    int side = sortie_mode(g) == MISSION_SIDE;
    for (i = 0; i < MAX_ENEMIES && k < count; ++i) {
        Enemy *e = &g->enemies[i];
        if (e->active) continue;
        memset(e, 0, sizeof(*e));
        e->active = 1;
        e->hp = 2 + g->chapter / 2;
        e->max_hp = e->hp;
        e->phase = random_unit(g) * 6.0f;
        e->ai_side = chasing ? 1.0f : -1.0f;
        e->last_shot = sim_now(g) + 600u + (uint32_t)k * 300u;
        if (side) {
            e->x = chasing ? g->scroll - 70.0f - (float)k * 110.0f :
                             g->scroll + SCREEN_W + 60.0f + (float)k * 150.0f;
            e->y = 80.0f + random_unit(g) * 170.0f;
            e->vx = chasing ? g->airspeed + 48.0f : -55.0f;
        } else {
            e->x = flight_clamp(g->player_x + (float)(k * 2 - 1) * 70.0f,
                                50.0f, SCREEN_W - 50.0f);
            e->y = chasing ? SCREEN_H + 50.0f + (float)k * 40.0f :
                             -50.0f - (float)k * 60.0f;
        }
        ++k;
    }
    if (k > 0) show_notice(g, chasing ? "SCOUTS ON OUR TAIL" : "ENEMY SCOUTS AHEAD");
}

/* The flat-view chasers come in from behind and fire along our line. */
static void update_side_chasers(Game *g, float dt, uint32_t now)
{
    int i;
    for (i = 0; i < MAX_ENEMIES; ++i) {
        Enemy *e = &g->enemies[i];
        float sx, sy;
        if (!e->active) continue;
        e->x += e->vx * dt;
        if (e->ai_side <= 0.0f) continue;
        sx = e->x - g->scroll;
        sy = e->y + sinf(g->scroll * 0.01f + e->phase) * 30.0f;
        /* Pursuit: close on our height, then sheer off past us. */
        if (sx < g->player_x - 30.0f)
            e->y += flight_clamp(g->player_y - sy, -60.0f, 60.0f) * dt;
        else
            e->y -= 70.0f * dt;
        if (sx > SCREEN_W + 70.0f) { e->active = 0; continue; }
        if (sx > -10.0f && g->player_x - sx > 50.0f && g->player_x - sx < 330.0f &&
            fabsf(g->player_y - sy) < 50.0f &&
            ticks_past(now, e->last_shot, (uint32_t)(900 - g->chapter * 40))) {
            float dx = g->player_x - sx, dy = g->player_y - sy;
            float len = sqrtf(dx * dx + dy * dy);
            if (new_bullet(g, sx + 30.0f, sy - 3.0f, dx / len * 280.0f,
                           dy / len * 280.0f, 1, 0, 2.0f))
                g->audio.shot = 1700;
            e->last_shot = now;
        }
    }
}

static void draw_top_scout(SDL_Surface *s, int x, int y, int dir)
{
    pix_t dark = PIX_PACK(39, 31, 25), body = PIX_PACK(126, 36, 31);
    pix_t cloth = PIX_PACK(201, 167, 132);
    blend_circle(s, x + 14, y + 18, 16, 20, 22, 18, 70);     /* its shadow */
    rect(s, x - 3, y - 24, 7, 49, body);
    rect(s, x - 25, dir > 0 ? y + 2 : y - 8, 51, 6, cloth);
    rect(s, x - 22, dir > 0 ? y - 9 : y + 4, 45, 5, cloth);
    rect(s, x - 10, dir > 0 ? y - 23 : y + 18, 21, 5, body);
    circle(s, x, y - 24 * dir, 4, dark);
    line(s, x - 7, y - 27 * dir, x + 7, y - 21 * dir, dark);
    line(s, x + 7, y - 27 * dir, x - 7, y - 21 * dir, dark);
    rect(s, x - 5, y - 10, 2, 20, PIX_PACK(20, 20, 20));      /* black crosses */
    rect(s, x - 14, y - 1, 28, 2, PIX_PACK(20, 20, 20));
}

static void update_top_scouts(Game *g, float dt, uint32_t now)
{
    int i, j;
    for (i = 0; i < MAX_ENEMIES; ++i) {
        Enemy *e = &g->enemies[i];
        int head_on;
        if (!e->active) continue;
        head_on = e->ai_side < 0.0f;
        e->phase += dt;
        e->y += (head_on ? 120.0f : -60.0f) * dt;
        e->x += (sinf(e->phase * 1.3f) * 50.0f +
                 flight_clamp(g->player_x - e->x, -80.0f, 80.0f) * 0.6f) * dt;
        if (e->y > SCREEN_H + 70.0f || e->y < -90.0f) { e->active = 0; continue; }
        if (fabsf(e->x - g->player_x) < 140.0f &&
            ((head_on && e->y > 10.0f && e->y < g->player_y - 70.0f) ||
             (!head_on && e->y > g->player_y + 50.0f && e->y < SCREEN_H)) &&
            ticks_past(now, e->last_shot, (uint32_t)(820 - g->chapter * 35))) {
            aimed_fire(g, e->x, e->y + (head_on ? 24.0f : -24.0f), 260.0f, 0);
            e->last_shot = now;
        }
        for (j = 0; j < MAX_BULLETS; ++j) {
            Bullet *b = &g->bullets[j];
            if (!b->active || b->enemy || b->type != 0 || b->missed) continue;
            if (fabsf(b->x - e->x) < 24.0f && fabsf(b->y - e->y) < 22.0f) {
                b->active = 0;
                if (--e->hp <= 0) {
                    e->active = 0;
                    new_explosion(g, e->x, e->y, 1);
                    g->audio.blast = 6500;
                    credit_air_kill(g);
                }
                break;
            }
        }
        if (e->active && fabsf(e->x - g->player_x) < 30.0f &&
            fabsf(e->y - g->player_y) < 30.0f) {
            heavy_hit(g, 18);
            e->active = 0;
            new_explosion(g, e->x, e->y, 1);
        }
    }
}

static void draw_top_scouts(Game *g)
{
    int i;
    for (i = 0; i < MAX_ENEMIES; ++i) {
        const Enemy *e = &g->enemies[i];
        if (!e->active) continue;
        draw_top_scout(g->screen, (int)e->x, (int)e->y, e->ai_side < 0.0f ? 1 : -1);
        if (sim_now(g) - e->last_shot < 60u)
            blend_circle(g->screen, (int)e->x,
                         (int)e->y + (e->ai_side < 0.0f ? 26 : -26), 4,
                         255, 190, 90, 230);
    }
}

/* ---- The director: something to do every ten or twenty seconds -------- */

static void sortie_director_2d(Game *g, float dt)
{
    int side = sortie_mode(g) == MISSION_SIDE;
    float here = player_world(g);
    float ahead = side ? g->scroll + SCREEN_W + 60.0f : g->scroll + SCREEN_H + 60.0f;
    float pace = 1.0f - (float)g->chapter * 0.035f;
    int ahead_enemy = enemy_ground(g, ahead);
    if (g->special == SPECIAL_TRUCE || g->landed) return;
    /* QA: a timed run meets a squall at 40 seconds and every minute after,
     * so its drawing cost can be read off the frame-time sign. */
    if (qa_frame_time && g->qa_run && !g->squall.active &&
        g->sortie_clock >= 40.0f &&
        fmodf(g->sortie_clock - 40.0f, 60.0f) < dt)
        spawn_squall_2d(g);
    /* Archie ranges on any machine over their lines, and from a long way
     * back on one that is high. */
    g->archie_clock -= dt;
    if (g->archie_clock <= 0.0f) {
        int high = !side || g->player_y < 240.0f;
        if ((enemy_ground(g, here) || (g->leg != LEG_OUT &&
             here < g->home_line_at + 1600.0f)) && g->leg != LEG_LANDING &&
            (high || random_unit(g) < 0.3f))
            archie_fire_2d(g);
        g->archie_clock = (5.0f + random_unit(g) * 4.5f) * pace *
                          (g->chapter == 0 ? 1.7f : 1.0f);
    }
    /* Riflemen: a few a minute, more near the lines. */
    g->rifle_clock -= dt;
    if (g->rifle_clock <= 0.0f) {
        if (ahead_enemy && g->leg != LEG_LANDING)
            spawn_riflemen(g, ahead, 1 + (int)(random_unit(g) * 2.2f));
        g->rifle_clock = near_the_lines(g, ahead) ?
                         4.0f + random_unit(g) * 4.0f :
                         9.0f + random_unit(g) * 8.0f;
    }
    /* The rest one at a time, sooner when the sky has gone quiet. */
    g->hazard_clock -= dt;
    if (g->qa_idle_run > 6.0f && g->hazard_clock > 0.5f) g->hazard_clock = 0.5f;
    if (g->hazard_clock <= 0.0f && g->leg != LEG_LANDING) {
        float pick = random_unit(g);
        int armed_air = mission_armed(g) && g->chapter >= 1;
        if (armed_air && enemies_in_play(g) == 0 && pick < 0.28f &&
            g->sortie_clock > 25.0f)
            spawn_scouts_2d(g, 1 + (g->chapter >= 3) + (random_unit(g) < 0.3f),
                            g->leg == LEG_HOME);
        else if (!g->balloon.active && pick < 0.42f && g->leg == LEG_OUT)
            spawn_balloon(g);
        else if (!g->squall.active && pick < 0.56f && g->sortie_clock > 30.0f)
            spawn_squall_2d(g);
        else if (ahead_enemy && pick < (g->chapter == 0 ? 0.68f : 0.84f))
            spawn_mg_post(g);
        else if (ahead_enemy)
            spawn_riflemen(g, ahead, 2 + (int)(random_unit(g) * 2.0f));
        else
            archie_fire_2d(g);
        g->hazard_clock = (8.0f + random_unit(g) * 6.0f) * pace;
    }
}

/* ---- Legs: out, the task, home, and down on the aerodrome --------------- */

static void start_run_home(Game *g)
{
    int i;
    int side = sortie_mode(g) == MISSION_SIDE;
    float view_end = side ? g->scroll + SCREEN_W + 40.0f :
                            g->scroll + SCREEN_H + 100.0f;
    if (g->leg != LEG_OUT) return;
    g->leg = LEG_HOME;
    g->leg_time = 0.0f;
    g->home_line_at = view_end + 2400.0f;
    g->field_at = view_end + 4300.0f;
    show_notice(g, g->special == SPECIAL_TRUCE ? "HAPPY CHRISTMAS: HOME" :
                g->objectives >= current_mission_goal(g) ?
                "TASK DONE: HOME" : "TURN FOR HOME");
    if (g->special == SPECIAL_TRUCE) {
        /* The way home runs along the line again, past more of them. */
        int placed = 0;
        g->field_at = view_end + 5600.0f;
        for (i = 0; i < MAX_TARGETS && placed < 3; ++i)
            if (!g->targets[i].active) {
                memset(&g->targets[i], 0, sizeof(g->targets[i]));
                g->targets[i].active = 1;
                g->targets[i].type = 3;
                g->targets[i].world_x = view_end + 700.0f + (float)placed * 1450.0f;
                ++placed;
            }
    }
    /* What lay further on is left to another day. */
    if (side) {
        for (i = 0; i < MAX_TARGETS; ++i)
            if (g->targets[i].active && g->targets[i].world_x > view_end) {
                if (!g->targets[i].civilian && !g->targets[i].hazard &&
                    !g->targets[i].done) ++g->qa_unreached;
                g->targets[i].active = 0;
            }
    } else {
        for (i = 0; i < g->td_count; ++i)
            if (g->td[i].active && g->td[i].world_x > view_end) {
                if (!g->td[i].hazard && !g->td[i].done && !g->td[i].wrecked &&
                    (td_kinds[g->td[i].kind].military ||
                     td_kinds[g->td[i].kind].task)) ++g->qa_unreached;
                g->td[i].active = 0;
            }
    }
    /* Their scouts come up after us. */
    if (mission_armed(g) && g->chapter >= 1)
        spawn_scouts_2d(g, 1 + (g->chapter >= 4), 1);
    g->hazard_clock = 9.0f;
    g->archie_clock = 2.0f;
}

/* Side-on and top-down: has the task been done, or flown past?  Then the
 * run home, and the field. */
static int update_legs_2d(Game *g, float dt)
{
    int side = sortie_mode(g) == MISSION_SIDE;
    float here = player_world(g);
    int goal = current_mission_goal(g);
    uint32_t now = sim_now(g);
    g->leg_time += dt;
    g->sortie_clock += dt;
    if (g->tree_struck < 10.0f) g->tree_struck += dt;
    if (g->leg == LEG_OUT) {
        /* The observers do all their work if they can; the order is met
         * with one task to spare.  A raid flies on through its second
         * group even when the order is met in the first: the order is
         * the least the squadron will accept, and what is left on the
         * ground is there for the taking. */
        int done = g->special == SPECIAL_TRUCE ? g->pudding_delivered :
                   g->task_total > 0 && g->objectives >= g->task_total;
        if (g->task_total == 0 && goal > 0 && g->objectives == goal &&
            !g->order_met_told) {
            g->order_met_told = 1;
            show_notice(g, "ORDER MET");
        }
        if (done || here > g->task_end)
            start_run_home(g);
    } else if (g->leg == LEG_HOME) {
        if (g->field_at - (side ? g->scroll + SCREEN_W : g->scroll + SCREEN_H) < 0.0f) {
            g->leg = LEG_LANDING;
            g->leg_time = 0.0f;
            show_notice(g, side ? "LAND ON THE FIELD" : "OVER THE FIELD: LAND");
        }
    } else if (!g->landed) {
        if (side) {
            float ground = (float)(terrain_y(here) - 19);
            if (here > g->field_at + 20.0f && here < g->field_at + 560.0f &&
                g->player_y >= ground - 3.0f) {
                g->landed = 1;
                g->landed_at = now;
                g->score += 200;
                show_notice(g, "DOWN SAFELY");
            } else if (here > g->field_at + 620.0f || g->leg_time > 30.0f) {
                show_notice(g, "DOWN IN A FIELD BEYOND");
                return 1;
            }
        } else {
            float fy = (float)SCREEN_H - (g->field_at - g->scroll);
            if (fabsf(fy - g->player_y) < 34.0f &&
                fabsf(g->player_x - 320.0f) < 80.0f) {
                g->landed = 1;
                g->landed_at = now;
                g->score += 200;
                show_notice(g, "DOWN SAFELY");
            } else if (fy > g->player_y + 120.0f || g->leg_time > 30.0f) {
                show_notice(g, "DOWN IN A FIELD BEYOND");
                return 1;
            }
        }
    } else if (now - g->landed_at > 1800u) {
        return 1;
    }
    return 0;
}

/* The aerodrome at the end of the run home: canvas hangars, a windsock and
 * the squadron's sign, side-on; from above the painted field. */
static void draw_field_2d(Game *g)
{
    SDL_Surface *s = g->screen;
    if (g->leg == LEG_OUT) return;
    if (sortie_mode(g) == MISSION_SIDE) {
        int x = (int)(g->field_at - g->scroll), k;
        int gy = terrain_y(g->field_at) + 2;
        if (x > SCREEN_W + 40 || x < -700) return;
        for (k = 0; k < 2; ++k) {
            int hx = x + 40 + k * 120;
            rect(s, hx, gy - 34, 90, 34, PIX_PACK(176, 164, 132));
            fill_triangle(s, hx - 6, gy - 34, hx + 45, gy - 54, hx + 96, gy - 34,
                          PIX_PACK(150, 138, 108));
            rect(s, hx + 30, gy - 26, 30, 26, PIX_PACK(54, 44, 34));
        }
        line(s, x + 300, gy, x + 300, gy - 46, PIX_PACK(60, 50, 40));
        fill_triangle(s, x + 300, gy - 46, x + 324, gy - 42, x + 300, gy - 38,
                      PIX_PACK(220, 130, 50));
        line(s, x + 20, gy - 2, x + 560, gy - 2, PIX_PACK(150, 160, 100));
        if (x > -100 && x < SCREEN_W - 100)
            draw_field_sign(g, x + 10, gy - 92, 112, "AERODROME");
    } else {
        int y = SCREEN_H - (int)(g->field_at - g->scroll);
        if (y < -80 || y > SCREEN_H + 80) return;
        td_blit(g, TD_AERODROME, 320, y);
    }
}

/* ---- The observers' work, side-on --------------------------------------- */

/* The column, the headquarters and the battery as they are drawn: the
 * column a file of men in field grey on the road, the headquarters a farm
 * with its flag, the battery two guns dug in. */
static void draw_side_task(Game *g, Target *t, int sx, int ty)
{
    SDL_Surface *s = g->screen;
    int k;
    if (t->task == TASK_MARK) {
        int base = rifleman_side_base(g);
        for (k = 0; k < 7; ++k) {
            int sprite = base + ((k + (int)(sim_now(g) / 400u)) % 3 == 0 ? 0 : 1);
            rifleman_blit(g, sprite, sx - 70 + k * 20, ty + 14 - rifleman_rect[sprite][3], 0);
        }
        if (t->done) draw_field_sign(g, sx - 50, ty - 70, 100, "MARKED");
    } else if (t->task == TASK_DROP) {
        const short *r = side_civilian_rect[SC_FARM];
        SDL_Rect src = {r[0], r[1], r[2], r[3]};
        SDL_Rect dst = {sx - r[2] / 2, ty + 14 - r[3], r[2], r[3]};
        if (g->side_civilians) SDL_BlitSurface(g->side_civilians, &src, s, &dst);
        line(s, sx + 30, ty - 40, sx + 30, ty - 76, PIX_PACK(60, 50, 40));
        rect(s, sx + 31, ty - 76, 18, 11, PIX_PACK(236, 232, 220));
        rect(s, sx + 31, ty - 72, 18, 3, PIX_PACK(176, 40, 32));
        rect(s, sx + 38, ty - 76, 3, 11, PIX_PACK(176, 40, 32));
        if (t->done) draw_field_sign(g, sx - 55, ty - 110, 110, "DELIVERED");
    } else if (t->task == TASK_PHOTO) {
        Target gun = *t;
        gun.type = 1;
        gun.last_shot = 0;
        draw_ground_gun(g, sx - 34, ty, &gun);
        draw_ground_gun(g, sx + 34, ty, &gun);
        if (t->done) draw_field_sign(g, sx - 50, ty - 110, 100, "PLATE TAKEN");
    }
}

static void task_done(Game *g, Target *t, const char *what)
{
    t->done = 1;
    ++g->objectives;
    g->score += 400;
    show_notice(g, what);
    g->audio.crack = 500;
}

/* Marking the column needs only a pass low overhead; the bag must land
 * by the flag; the plate needs the machine held level over the guns. */
static void update_side_task(Game *g, Target *t, float dt)
{
    float sx = t->world_x - g->scroll;
    float ground = (float)(terrain_y(t->world_x) - 19);
    if (t->done) return;
    if (t->task == TASK_MARK) {
        if (!t->burst_left && sx < SCREEN_W + 40.0f) {
            /* Some of them unsling their rifles. */
            t->burst_left = 1;
            spawn_riflemen(g, t->world_x - 30.0f, 2);
        }
        if (fabsf(sx - g->player_x) < 70.0f && g->player_y > ground - 150.0f)
            task_done(g, t, "COLUMN MARKED");
    } else if (t->task == TASK_PHOTO) {
        /* Near enough level for long enough over the guns.  A careful
         * pilot settles the machine as the battery comes up. */
        if (fabsf(sx - g->player_x) < 80.0f && g->player_y < ground - 50.0f &&
            fabsf(g->player_vy) < 50.0f) {
            g->photo_hold += dt;
            if (g->photo_hold > 0.7f) task_done(g, t, "PLATE EXPOSED");
        } else if (fabsf(sx - g->player_x) >= 80.0f) {
            g->photo_hold = 0.0f;
        }
    }
}

/* A message bag reaching the ground, side-on (x screen) or from above. */
static void bag_lands(Game *g, float x, float y)
{
    int i;
    if (sortie_mode(g) == MISSION_SIDE) {
        for (i = 0; i < MAX_TARGETS; ++i) {
            Target *t = &g->targets[i];
            if (!t->active || t->task != TASK_DROP || t->done) continue;
            if (fabsf(t->world_x - g->scroll - x) < 55.0f) {
                task_done(g, t, "MESSAGE DELIVERED");
                return;
            }
        }
    } else {
        for (i = 0; i < g->td_count; ++i) {
            TdObject *o = &g->td[i];
            float sx, sy;
            if (!o->active || td_kinds[o->kind].task != TASK_DROP || o->done)
                continue;
            td_screen(g, o, &sx, &sy);
            if (fabsf(sx - x) < 50.0f && fabsf(sy - y) < 50.0f) {
                o->done = 1;
                ++g->objectives;
                g->score += 400;
                show_notice(g, "MESSAGE DELIVERED");
                return;
            }
        }
    }
    show_notice(g, g->bags > 0 ? "THE BAG FELL WIDE" : "NO BAGS LEFT");
}

/* ---- Top-down: the observers' work and its drawing ---------------------- */

static void update_top_task(Game *g, TdObject *o, int index, float sx, float sy,
                            const uint8_t *keys, float dt)
{
    int task = td_kinds[o->kind].task;
    if (o->done) return;
    if (task == TASK_MARK) {
        if (!o->last_shot && sy > -20.0f) {
            o->last_shot = 1;
            spawn_riflemen(g, o->world_x + 40.0f, 2);
        }
        if (fabsf(sx - g->player_x) < 42.0f && fabsf(sy - g->player_y) < 60.0f) {
            o->done = 1;
            ++g->objectives;
            g->score += 400;
            show_notice(g, "COLUMN MARKED");
        }
    } else if (task == TASK_PHOTO) {
        if (fabsf(sx - g->player_x) < 60.0f && sy > g->player_y - 90.0f &&
            sy < g->player_y + 20.0f && !keys[SDL_SCANCODE_LEFT] &&
            !keys[SDL_SCANCODE_RIGHT]) {
            g->photo_hold += dt;
            if (g->photo_hold > 0.8f) {
                o->done = 1;
                ++g->objectives;
                g->score += 400;
                show_notice(g, "PLATE EXPOSED");
            }
        } else if (sy > g->player_y + 20.0f) {
            g->photo_hold = 0.0f;
        }
    }
    (void)index;
}

static void draw_top_task(Game *g, const TdObject *o, int sx, int sy)
{
    int task = td_kinds[o->kind].task, k;
    if (task == TASK_MARK) {
        /* Three abreast and eight ranks, marching. */
        int step = (int)(sim_now(g) / 300u) & 1;
        for (k = 0; k < 24; ++k)
            td_blit_plain(g, TD_MAN_THEIRS, sx - 10 + (k % 3) * 10,
                          sy - 50 + (k / 3) * 13 + ((k + step) & 1));
    } else if (task == TASK_DROP) {
        td_blit(g, TD_TENTS, sx, sy);
        line(g->screen, sx + 24, sy - 10, sx + 24, sy - 34, PIX_PACK(60, 50, 40));
        rect(g->screen, sx + 25, sy - 34, 14, 9, PIX_PACK(236, 232, 220));
        rect(g->screen, sx + 25, sy - 31, 14, 3, PIX_PACK(176, 40, 32));
        rect(g->screen, sx + 30, sy - 34, 3, 9, PIX_PACK(176, 40, 32));
    } else if (task == TASK_PHOTO) {
        td_blit(g, TD_AA_PIT, sx - 26, sy);
        td_blit(g, TD_AA_PIT, sx + 26, sy);
    }
    if (o->done)
        draw_field_sign(g, sx - 50, sy - 70, 100,
                        task == TASK_MARK ? "MARKED" :
                        task == TASK_DROP ? "DELIVERED" : "PLATE TAKEN");
}

/* ---- QA: how much of a sortie asked nothing of the pilot ---------------- */

/* A frame needs the pilot when an enemy machine is in view, a round or a
 * burst is in the air near him, a target or an obstacle lies within two
 * seconds' flying, or a task is within reach.  Anything else is idle. */
static int sortie_needs_input(Game *g)
{
    MissionMode mode = sortie_mode(g);
    int i;
    uint32_t now = sim_now(g);
    if (g->notice_until && now < g->notice_until &&
        (g->leg == LEG_LANDING || g->landed)) return 1;
    for (i = 0; i < MAX_ARCHIE; ++i) {
        const Archie *a = &g->archie[i];
        if (!a->active) continue;
        if (a->kind == ARCHIE_RANGING && a->fuse > 0.0f) return 1;
        if (a->kind == ARCHIE_BURST && a->age < 0.6f) return 1;
    }
    if (mode == MISSION_SIDE || mode == MISSION_TOP_DOWN) {
        int side = mode == MISSION_SIDE;
        float reach = g->airspeed * 2.0f + 60.0f;
        float here = player_world(g);
        if (g->leg == LEG_LANDING) return 1;
        for (i = 0; i < MAX_BULLETS; ++i) {
            const Bullet *b = &g->bullets[i];
            if (b->active && b->enemy &&
                fabsf(b->x - g->player_x) < 300.0f && fabsf(b->y - g->player_y) < 300.0f)
                return 1;
        }
        for (i = 0; i < MAX_ENEMIES; ++i) {
            const Enemy *e = &g->enemies[i];
            float sx = side ? e->x - g->scroll : e->x;
            float sy = e->y;
            if (e->active && sx > -60.0f && sx < SCREEN_W + 60.0f &&
                sy > -60.0f && sy < SCREEN_H + 60.0f) return 1;
        }
        for (i = 0; i < MAX_RIFLEMEN; ++i) {
            const Rifleman *r = &g->riflemen[i];
            int sx, sy;
            if (!r->active || r->state == RIFLE_DOWN) continue;
            rifleman_screen(g, r, &sx, &sy);
            if (sx > -20 && sx < SCREEN_W + 20 && sy > -20 && sy < SCREEN_H + 20)
                return 1;
        }
        if (g->squall.active && g->squall.a < here + reach &&
            g->squall.b > here) return 1;
        if (g->balloon.active && !g->balloon.cut) {
            int bx, by;
            balloon_screen(g, &bx, &by);
            if (side ? (bx > g->player_x - 30 && bx < g->player_x + reach) :
                       (by < g->player_y + 30 && by > g->player_y - reach))
                return 1;
        }
        if (side) {
            if (side_tree_ahead(g)) return 1;
            for (i = 0; i < MAX_TARGETS; ++i) {
                const Target *t = &g->targets[i];
                float sx = t->world_x - g->scroll;
                if (!t->active || t->wrecked || t->done) continue;
                if (sx > g->player_x - 40.0f && sx < g->player_x + reach) return 1;
            }
        } else {
            for (i = 0; i < g->td_count; ++i) {
                const TdObject *o = &g->td[i];
                float sx, sy;
                if (!o->active || o->wrecked || o->done) continue;
                td_screen(g, o, &sx, &sy);
                if (sy < g->player_y + 40.0f && sy > g->player_y - reach) return 1;
            }
            for (i = 0; i < MAX_TARGETS; ++i) {
                float sx, sy;
                if (!g->targets[i].active) continue;
                top_down_target_position(g, i, &sx, &sy);
                if (sy < g->player_y + 40.0f && sy > g->player_y - reach) return 1;
            }
        }
        return 0;
    }
    if (mode == MISSION_DOGFIGHT_3D) {
        Vec3 me = {g->flight_x, g->flight_y, g->flight_z};
        if (g->flight_y < 90.0f) return 1;
        if (g->squall.active) {
            float dx = g->squall.centre.x - me.x, dz = g->squall.centre.z - me.z;
            if (dx * dx + dz * dz < 700.0f * 700.0f) return 1;
        }
        for (i = 0; i < MAX_ENEMIES; ++i) {
            const Enemy *e = &g->enemies[i];
            Projected3D p;
            float dx, dy, dz;
            if (!e->active || e->crashing) continue;
            dx = e->world_x - me.x; dy = e->world_y - me.y; dz = e->world_z - me.z;
            if (dx * dx + dy * dy + dz * dz < 1100.0f * 1100.0f) return 1;
            p = project_world_point(g, (Vec3){e->world_x, e->world_y, e->world_z});
            if (p.visible && p.x > 0 && p.x < SCREEN_W && p.y > 0 && p.y < SCREEN_H &&
                p.depth < 2500.0f) return 1;
        }
        for (i = 0; i < MAX_ROUNDS; ++i) {
            const WorldRound *r = &g->rounds[i];
            float dx, dy, dz;
            if (!r->active || r->owner != 1) continue;
            dx = r->p.x - me.x; dy = r->p.y - me.y; dz = r->p.z - me.z;
            if (dx * dx + dy * dy + dz * dz < 400.0f * 400.0f) return 1;
        }
        if (g->obs_count && g->leg == LEG_OUT) {
            int k = observer_next(g);
            if (g->obs_spot_pending > 0.0f || g->obs_bag_active) return 1;
            if (k >= 0) {
                float dx = g->obs[k].x - me.x, dz = g->obs[k].z - me.z;
                if (dx * dx + dz * dz < 900.0f * 900.0f) return 1;
            }
        } else if (g->leg == LEG_OUT && !enemies_hostile(g) && g->wave > 0 &&
                   g->shadow_time < 35.0f) return 1;
        return 0;
    }
    for (i = 0; i < MAX_ENEMIES; ++i) {
        const Enemy *e = &g->enemies[i];
        int sx, sy, size;
        if (!e->active) continue;
        if (e->crashing) return 1;
        if (project_enemy_3d(g, e, i, &sx, &sy, &size) && sx > -40 &&
            sx < SCREEN_W + 40 && sy > -40 && sy < SCREEN_H + 40) return 1;
    }
    for (i = 0; i < MAX_BULLETS; ++i)
        if (g->bullets[i].active && g->bullets[i].enemy) return 1;
    return 0;
}

/* The targets a flat sortie offers towards its order: the gun pits or
 * the observers' tasks side-on, the military targets or the tasks from
 * above. */
static int targets_on_offer(const Game *g)
{
    int i, n = 0;
    for (i = 0; i < MAX_TARGETS; ++i)
        if (g->targets[i].active && !g->targets[i].civilian &&
            !g->targets[i].hazard && (sortie_mode(g) == MISSION_SIDE ||
                                      g->td_mix == MIX_NONE))
            ++n;
    for (i = 0; i < g->td_count; ++i)
        if (!g->td[i].hazard &&
            (td_kinds[g->td[i].kind].military || td_kinds[g->td[i].kind].task))
            ++n;
    return n;
}

static void sortie_tally(Game *g, float dt)
{
    if (g->qa_time == 0.0f) {
        g->qa_bombs_carried = g->bombs_remaining;
        g->qa_offered = targets_on_offer(g);
    }
    /* Damage taken, frame by frame.  A QA run keeps the pilot in the air
     * by patching him up below half, so every hit is counted, not only
     * those above the floor that keeps a held view alive. */
    if (g->health < g->qa_last_health)
        g->qa_damage += g->qa_last_health - g->health;
    if (g->qa_run && g->health > 0 && g->health < 50) g->health = 100;
    g->qa_last_health = g->health;
    g->qa_time += dt;
    if (sortie_needs_input(g)) {
        g->qa_idle_run = 0.0f;
    } else {
        g->qa_idle += dt;
        g->qa_idle_run += dt;
        if (g->qa_idle_run > g->qa_idle_longest)
            g->qa_idle_longest = g->qa_idle_run;
    }
}

/* One line in RAM: for each sortie flown: the mission, how long it took,
 * how much of it was idle and the longest idle stretch, in tenths. */
static void qa_log_sortie(Game *g)
{
    static const char *const modes[4] = {"SIDE", "TOPDOWN", "DOGFIGHT", "GUNNER"};
    FILE *f;
    int t = (int)(g->qa_time * 10.0f), idle = (int)(g->qa_idle * 10.0f);
    int longest = (int)(g->qa_idle_longest * 10.0f);
    if (g->qa_logged) return;
    g->qa_logged = 1;
    f = fopen(CANVAS_SORTIE_LOG, "a");
    if (!f) return;
    fprintf(f, "C%d M%02d %s MIX%d %s total=%d.%d idle=%d.%d longest=%d.%d "
            "success=%d objectives=%d/%d kills=%d health=%d fuel=%d "
            "shots=%d enemy_shots=%d damage=%d sim=%u offered=%d "
            "unreached=%d bombs=%d/%d\n",
            g->chapter + 1, g->mission + 1, modes[sortie_mode(g)], g->td_mix,
            personal_arms(g) ? "UNARMED" : mission_armed(g) ? "ARMED" : "QUIET",
            t / 10, t % 10, idle / 10, idle % 10, longest / 10, longest % 10,
            g->success, g->objectives, current_mission_goal(g), g->kills,
            g->health, (int)g->fuel, g->qa_shots, g->qa_enemy_shots,
            g->qa_damage, (unsigned)((g->sim_ms - SIM_EPOCH) / 1000u),
            g->qa_offered, g->qa_unreached,
            g->qa_bombs_carried - g->bombs_remaining, g->qa_bombs_carried);
    fclose(f);
}

static void update_top_down_sortie(Game *g, float dt, const uint8_t *keys)
{
    int i, j;
    uint32_t now = sim_now(g);
    float old_x = g->player_x;
    if (keys[SDL_SCANCODE_X]) g->airspeed += 42.0f * dt;
    if (keys[SDL_SCANCODE_Z]) g->airspeed -= 42.0f * dt;
    if (g->landed) {
        /* Rolling out across the field. */
        g->airspeed -= 60.0f * dt;
        if (g->airspeed < 0.0f) g->airspeed = 0.0f;
    } else {
        if (g->airspeed < 72.0f) g->airspeed = 72.0f;
        if (g->airspeed > 152.0f) g->airspeed = 152.0f;
    }
    if (g->qa_autopilot) {
        /* Line up on the nearest military target ahead; strafe what rounds
         * can stop, bomb what they cannot, and never bomb near people. */
        static uint8_t fake[SDL_NUM_SCANCODES];
        float best = 1e9f, aim_x = 320.0f;
        int k, bomb = 0;
        memcpy(fake, keys, sizeof(fake));
        fake[SDL_SCANCODE_SPACE] = fake[SDL_SCANCODE_B] = 0;
        fake[SDL_SCANCODE_LEFT] = fake[SDL_SCANCODE_RIGHT] = 0;
        for (k = 0; k < g->td_count; ++k) {
            const TdObject *o = &g->td[k];
            float sx, sy, ahead;
            int task = td_kinds[o->kind].task;
            if (!o->active || o->wrecked || o->done ||
                !(td_kinds[o->kind].military || task))
                continue;
            td_screen(g, o, &sx, &sy);
            ahead = g->player_y - sy;
            if (ahead < (task ? -20.0f : 40.0f) || ahead > 420.0f || ahead >= best)
                continue;
            best = ahead;
            aim_x = sx;
            if (task) {
                /* The observers' work: over the column, over the guns,
                 * or the bag let go about seventy pixels short. */
                bomb = 1;
                if (task == TASK_DROP && g->bags > 0 && ahead > 55.0f &&
                    ahead < 85.0f && fabsf(sx - g->player_x) < 24.0f)
                    fake[SDL_SCANCODE_B] = 1;
                continue;
            }
            bomb = td_kinds[o->kind].bomb_only || td_kinds[o->kind].hp >= 3;
            /* A bomb falls for about a second while the ground runs on
             * under it, and strikes in its last moments: let go when the
             * target is that second's run, less the bomb's own fall,
             * ahead. */
            if (bomb && fabsf(ahead - ((g->airspeed - 42.0f) * 0.97f + 4.0f)) < 10.0f &&
                fabsf(sx - g->player_x) < 16.0f) {
                int c, near_civ = 0;
                for (c = 0; c < g->td_count; ++c) {
                    float cx, cy;
                    if (!g->td[c].active || g->td[c].wrecked ||
                        td_kinds[g->td[c].kind].civ == CIV_NONE) continue;
                    td_screen(g, &g->td[c], &cx, &cy);
                    if (fabsf(cx - sx) < 60.0f && fabsf(cy - sy) < 70.0f)
                        near_civ = 1;
                }
                if (!near_civ) fake[SDL_SCANCODE_B] = 1;
            }
        }
        if (g->special == SPECIAL_TRUCE) {
            /* Over the next gathering to wave, then the pudding on their
             * field, let go a second's run short. */
            best = 1e9f;
            for (k = 0; k < MAX_TARGETS; ++k) {
                float sx, sy, ahead;
                if (!g->targets[k].active || g->targets[k].done) continue;
                top_down_target_position(g, k, &sx, &sy);
                ahead = g->player_y - sy;
                if (ahead < -30.0f || ahead > 420.0f || ahead >= best) continue;
                best = ahead;
                aim_x = sx;
                bomb = 0;
                if (g->targets[k].type == 4 && !g->pudding_delivered &&
                    fabsf(ahead - ((g->airspeed - 42.0f) * 0.97f + 4.0f)) < 10.0f &&
                    fabsf(sx - g->player_x) < 16.0f)
                    fake[SDL_SCANCODE_B] = 1;
            }
            if (best > 1e8f) aim_x = 320.0f;
        }
        if (aim_x < g->player_x - 4.0f) fake[SDL_SCANCODE_LEFT] = 1;
        if (aim_x > g->player_x + 4.0f) fake[SDL_SCANCODE_RIGHT] = 1;
        if (best < 1e8f && !bomb && fabsf(aim_x - g->player_x) < 14.0f)
            fake[SDL_SCANCODE_SPACE] = 1;
        keys = fake;
    }
    if (keys[SDL_SCANCODE_LEFT])
        g->player_x -= 175.0f *
            our_machines[our_machine_index(g->chapter)].agility * dt;
    if (keys[SDL_SCANCODE_RIGHT])
        g->player_x += 175.0f *
            our_machines[our_machine_index(g->chapter)].agility * dt;
    if (keys[SDL_SCANCODE_UP]) g->player_y -= 150.0f * dt;
    if (keys[SDL_SCANCODE_DOWN]) g->player_y += 150.0f * dt;
    if (g->player_x < 34.0f) g->player_x = 34.0f;
    if (g->player_x > SCREEN_W - 34.0f) g->player_x = SCREEN_W - 34.0f;
    if (g->player_y < 86.0f) g->player_y = 86.0f;
    if (g->player_y > SCREEN_H - 38.0f) g->player_y = SCREEN_H - 38.0f;
    if (dt > 0.0f)
        g->player_vx += ((g->player_x - old_x) / dt - g->player_vx) *
                        flight_clamp(dt * 4.0f, 0.0f, 1.0f);
    if (!qa_still) g->scroll += g->airspeed * dt;
    /* A tank for a raid with two halves and a run home. */
    g->fuel -= dt * 0.30f;
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
    } else if (personal_arms(g) && keys[SDL_SCANCODE_SPACE] &&
               personal_shot(g, 0)) {
        /* Tom's carbine over the side: one round, short range. */
        Bullet *b = new_bullet(g, g->player_x, g->player_y - 24.0f,
                               0.0f, -520.0f, 0, 0, 0.45f);
        if (b && random_unit(g) > PERSONAL_HIT_CHANCE) b->missed = 1;
    }
    if (g->bags > 0 && keys[SDL_SCANCODE_B] && now - g->last_bomb >= 650u) {
        /* A message bag with a long streamer, thrown over the side. */
        if (new_bullet(g, g->player_x, g->player_y - 4.0f,
                       0.0f, 42.0f, 0, 4, 1.12f)) {
            g->last_bomb = now;
            --g->bags;
        }
    } else if ((mission_armed(g) || g->special == SPECIAL_TRUCE) &&
        g->bombs_remaining > 0 && keys[SDL_SCANCODE_B] &&
        now - g->last_bomb >= 650u) {
        if (new_bullet(g, g->player_x, g->player_y - 4.0f,
                       0.0f, 42.0f, 0, 1, 1.12f)) {
            g->last_bomb = now;
            --g->bombs_remaining;
        }
    }
    if (g->qa_tracers && now - g->last_shot >= 700u) {
        /* A ring of rounds, turning 5 degrees each time, from mid-screen. */
        int k;
        float turn = (float)((now / 700u) % 3u) * 5.0f;
        for (k = 0; k < 24; ++k) {
            float a = ((float)k * 15.0f + turn) * (float)M_PI / 180.0f;
            new_bullet(g, 320.0f, 190.0f, cosf(a) * 300.0f,
                       sinf(a) * 300.0f, 0, 0, 0.55f);
        }
        g->last_shot = now;
    }
    age_explosions(g, dt);
    for (i = 0; i < MAX_BULLETS; ++i) if (g->bullets[i].active) {
        Bullet *b = &g->bullets[i];
        b->x += b->vx * dt;
        b->y += b->vy * dt;
        b->life -= dt;
        if (!b->enemy && b->type == 4 && b->life <= 0.0f) {
            bag_lands(g, b->x, b->y);
            b->active = 0;
        } else if (!b->enemy && b->type == 1 && b->life <= 0.0f) {
            if (g->special != SPECIAL_TRUCE) {
                new_ground_explosion(g, b->x, b->y, 2);
                g->audio.blast = 7000;
                td_bomb_on_own_line(g, b->x, b->y);
                riflemen_blast(g, b->x, b->y, 40.0f);
            }
            b->active = 0;
        } else if (b->life <= 0.0f || b->y < -30.0f || b->y > SCREEN_H + 30.0f) {
            b->active = 0;
        }
        if (b->active && b->enemy &&
            fabsf(b->x - g->player_x) < 24.0f &&
            fabsf(b->y - g->player_y) < 24.0f &&
            now - g->last_damage > 240u) {
            if (!qa_spared(g) || g->health > 30) g->health -= 4;
            g->last_damage = now;
            b->active = 0;
            new_explosion(g, g->player_x, g->player_y, 0);
        }
    }
    for (i = 0; i < MAX_TARGETS; ++i) if (g->targets[i].active) {
        Target *target = &g->targets[i];
        float sx, sy;
        top_down_target_position(g, i, &sx, &sy);
        if (target->type == 3 && !target->done &&
            fabsf(sx - g->player_x) < 80.0f && fabsf(sy - g->player_y) < 50.0f) {
            /* Low over the men in no man's land: caps in the air on both
             * sides, and Tom waves back. */
            static const char *const waves[4] = {
                "THEY WAVE BACK", "TOM WAVES HIS CAP", "A CHEER FROM BELOW",
                "HAPPY CHRISTMAS, FRITZ"
            };
            target->done = 1;
            g->score += 100;
            show_notice(g, waves[i & 3]);
        }
        if (sy > 18.0f && sy < SCREEN_H - 12.0f) {
            if (target->type < 2 && g->special != SPECIAL_TRUCE &&
                now - target->last_shot > 1600u + (uint32_t)(i * 97 % 620)) {
                aimed_fire(g, sx, sy, target->type ? 185.0f : 235.0f, 0);
                target->last_shot = now;
            }
            for (j = 0; j < MAX_BULLETS; ++j) {
                Bullet *b = &g->bullets[j];
                if (!b->active || b->enemy || b->missed || b->type == 4) continue;
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
                            show_notice(g, "PUDDING DELIVERED");
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
    td_update(g, dt, now);
    for (i = 0; i < g->td_count; ++i) {
        TdObject *o = &g->td[i];
        float sx, sy;
        if (!o->active || !td_kinds[o->kind].task) continue;
        td_screen(g, o, &sx, &sy);
        update_top_task(g, o, i, sx, sy, keys, dt);
    }
    update_top_scouts(g, dt, now);
    update_riflemen(g, dt);
    update_archie(g, dt);
    update_squall_2d(g, dt);
    update_balloon(g, dt);
    sortie_director_2d(g, dt);
    g->shake -= dt * 2.2f;
    if (g->shake < 0.0f) g->shake = 0.0f;
    if (update_legs_2d(g, dt) ||
        g->fuel <= 0.0f || g->health <= 0) {
        if (g->health <= 0 && hold_for_wreck(g, g->player_x, g->player_y))
            return;
        finish_alternate_sortie(g);
    }
}

static void update_perspective_sortie(Game *g, float dt,
                                      const uint8_t *keys, int gunner)
{
    int i;
    int active_enemies = 0;
    int crashing_enemies = 0;
    int tail_strike = 0;
    int legs_done = 0;
    uint32_t now = sim_now(g);
    int aim_x;
    int aim_y;
    int personal = !mission_armed(g) && personal_arms(g);
    int firing = keys[SDL_SCANCODE_SPACE] &&
                 (personal ? personal_shot(g, !gunner) :
                  mission_armed(g) && g->ammunition > 0 &&
                  now - g->last_shot >= (gunner ? 110u : 130u));
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
        if (personal) {
            /* The carbine's range: a rifle aimed lower than this would lie
             * under the signs, where nobody could see where it pointed. */
            if (g->player_y < (float)CARBINE_AIM_Y0)
                g->player_y = (float)CARBINE_AIM_Y0;
            if (g->player_y > (float)CARBINE_AIM_Y1)
                g->player_y = (float)CARBINE_AIM_Y1;
        }
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
        if (!personal) {
            --g->ammunition;
            g->audio.shot = gunner ? 1750 : 1500;
        }
        if (gunner && personal)
            carbine_muzzle_position(g, &muzzle_x, &muzzle_y);
        else if (gunner)
            gunner_muzzle_position(g, &muzzle_x, &muzzle_y);
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
    if (!qa_still) g->scroll += g->airspeed * dt * 1.16f;
    g->fuel -= dt * 0.3f;
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
        if (b->active && b->enemy && !b->missed &&
            fabsf(b->x - 320.0f) < 28.0f &&
            fabsf(b->y - 190.0f) < 28.0f &&
            now - g->last_damage > 520u) {
            /* Not every round in a burst finds the machine: about one in
             * two goes wide and flies on past. */
            if (scene_hash((uint32_t)i * 2654435761u ^ now) % 100u >= 55u) {
                b->missed = 1;
                continue;
            }
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
            if (!qa_spared(g) || g->health > 30)
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
        if (depth < -520.0f || ((i & 3) == 3 && depth > 3300.0f) ||
            (enemy->fled && depth > 3000.0f)) {
            enemy->active = 0;
            continue;
        }
        if (enemy->fled) {
            /* Hurt, he has had enough: opens the range and goes home
             * trailing smoke, jinking as he goes. */
            enemy->x += (personal || g->kills >= 4 ? 950.0f : 560.0f) * dt;
            enemy->phase += dt * 1.6f;
        }
        ++active_enemies;
        if (!project_enemy_3d(g, enemy, i, &sx, &sy, &size)) continue;
        /* Not every round of a Lewis burst finds the machine; the carbine's
         * single shots seldom do. */
        if (firing && random_unit(g) < (personal ? PERSONAL_HIT_CHANCE :
                                        gunner ? 0.5f : 1.0f)) {
            if (abs(aim_x - sx) < size / 2 + 10 &&
                abs(aim_y - sy) < size / 3 + 9) {
                --enemy->hp;
                /* They grow wary as their losses mount: with none of
                 * them down yet an attacker presses on until he is badly
                 * hurt; once several have fallen the next turns away at
                 * the first serious hits, and goes faster. */
                if (gunner && enemy->hp > 0 && !enemy->fled &&
                    enemy->hp * 10 <= enemy->max_hp *
                    (g->kills < 2 ? 4 : g->kills < 4 ? 6 : 9)) {
                    enemy->fled = 1;
                    g->score += 50;
                }
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
        if (enemy->crashing || enemy->fled) continue;
        if ((enemies_hostile(g) ||
             (personal_arms(g) && (scene_hash(now / 900u + (uint32_t)i) & 3u) == 0u)) &&
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
    if (gunner && !g->qa_hold) {
        update_archie(g, dt);
        legs_done = update_gunner_legs(g, dt);
    }
    g->shake -= dt * 2.2f;
    if (g->shake < 0.0f) g->shake = 0.0f;
    if (gunner && g->tail_integrity <= 0) {
        if (!hold_for_wreck(g, 320.0f, 300.0f)) finish_alternate_sortie(g);
    } else if (gunner && !g->qa_hold) {
        if (legs_done || g->fuel <= 0.0f || g->health <= 0) {
            if (!(g->health <= 0 && hold_for_wreck(g, 320.0f, 300.0f)))
                finish_alternate_sortie(g);
        }
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
        g->fuel <= 0.0f || g->health <= 0)) {
        if (!(g->health <= 0 && hold_for_wreck(g, 320.0f, 300.0f)))
            finish_alternate_sortie(g);
    }
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

/* Whether the carbine (or the revolver) will fire now; if it does, the
 * round is spent, the report sounds, and the bolt or the reload starts. */
static int personal_shot(Game *g, int revolver)
{
    uint32_t now = sim_now(g);
    if (now < g->arms_ready_at) return 0;
    if (g->arms_rounds <= 0)
        g->arms_rounds = revolver ? REVOLVER_ROUNDS : CARBINE_ROUNDS;
    --g->arms_rounds;
    g->audio.crack = 2600;
    if (g->arms_rounds == 0) {
        g->arms_ready_at = now + (revolver ? 4200u : 3000u);
        show_notice(g, revolver ? "RELOADING THE REVOLVER" :
                                  "A FRESH CLIP FOR THE CARBINE");
    } else {
        g->arms_ready_at = now + (revolver ? 480u : 1300u);
    }
    g->last_shot = now;
    return 1;
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
        if (owner == 0) ++g->qa_shots;
        else if (owner == 1) ++g->qa_enemy_shots;
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
/* The pilot's forward guns, as the year's machine carries them: none in
 * 1914, one synchronised Vickers from 1915, two from the Cambrai chapter. */
static int pilot_gun_count(const Game *g)
{
    if (!mission_armed(g)) return 0;
    return our_machines[our_machine_index(g->chapter)].guns;
}

static void fire_player_guns(Game *g)
{
    Vec3 me = {g->flight_x, g->flight_y, g->flight_z};
    AircraftPose pose = pose_from_flight(&g->flight, me);
    Vec3 carry = aircraft_velocity(&g->flight);
    Vec3 aim = pose_point(&pose, 0.0f, -150.0f * 20.0f / WORLD_FOCAL, 150.0f);
    float spread = 0.012f - g->skill_gunnery * 0.008f;
    int gun, guns = pilot_gun_count(g);
    for (gun = 0; gun < guns; ++gun) {
        /* A single gun is on the centreline, under the sight. */
        Vec3 from = pose_point(&pose, guns == 1 ? 0.0f : gun ? 0.42f : -0.42f,
                               -0.35f, 2.2f);
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

/* 1914: the pilot leans out with his revolver.  One round, thrown wide
 * by the slipstream, and spent within a hundred yards. */
static void fire_revolver(Game *g)
{
    Vec3 me = {g->flight_x, g->flight_y, g->flight_z};
    AircraftPose pose = pose_from_flight(&g->flight, me);
    Vec3 carry = aircraft_velocity(&g->flight);
    Vec3 from = pose_point(&pose, 0.5f, -0.2f, 0.6f);
    Vec3 aim = pose_point(&pose, 0.0f, -150.0f * 20.0f / WORLD_FOCAL, 150.0f);
    Vec3 dir = vec3_sub(aim, from);
    float len = vec3_length(dir), spread = 0.05f;
    dir.x = dir.x / len + (random_unit(g) - 0.5f) * spread;
    dir.y = dir.y / len + (random_unit(g) - 0.5f) * spread;
    dir.z = dir.z / len + (random_unit(g) - 0.5f) * spread;
    new_round(g, from, (Vec3){carry.x + dir.x * ROUND_SPEED * 0.7f,
                              carry.y + dir.y * ROUND_SPEED * 0.7f,
                              carry.z + dir.z * ROUND_SPEED * 0.7f}, 0, 0.45f);
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
    if (by_player) enemy->player_hit = 1;
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
        if (!qa_spared(g) || g->health > 30) {
            g->health -= player_machine_gun_damage(g);
            add_dogfight_bullet_hole(g, sim_now(g) ^ (uint32_t)(r - g->rounds));
        }
        g->audio.hit_player = 1;
        g->audio.hit = 1400;
        g->last_damage = sim_now(g);
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
            g->notice_until = sim_now(g) + 2200u;
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
    for (k = 0; k < (personal_arms(g) ? 1 : 3); ++k) {
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
    enemy->last_shot = sim_now(g);
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
        if (!wingman_fit(g, i)) continue;   /* lost, ill or mending */
        at = pose_point(&pose, offset[i][0], offset[i][1], offset[i][2]);
        w->world_x = at.x; w->world_y = at.y; w->world_z = at.z;
        w->heading = g->flight_heading;
        w->speed = 98.0f;
        w->throttle = 0.7f;
        flight_state_init(&w->flight, w->heading, 0.0f, 0.0f, 43.8f, 0.7f);
        w->flight.power = our_machines[our_machine_index(g->chapter)].power;
        w->flight.agility = our_machines[our_machine_index(g->chapter)].agility;
        w->hp = 3;
        w->max_hp = 3;
        w->active = 1;
        w->ai_state = AI_FORMATION;
        w->ai_timer = 7.0f + (float)i * 1.5f;
        w->target = -1;
        w->last_shot = sim_now(g) + 2000u;
    }
}

static void update_wingmen(Game *g, float dt)
{
    static const float slot[3][2] = {{-34.0f, 30.0f}, {40.0f, 45.0f},
                                     {-72.0f, 60.0f}};
    uint32_t now = sim_now(g);
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
                    ticks_past(now, w->last_shot, 900u)) {
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
    uint32_t now = sim_now(g);
    Vec3 me = {enemy->world_x, enemy->world_y, enemy->world_z};
    Vec3 target = {g->flight_x, g->flight_y, g->flight_z};
    Vec3 target_v = aircraft_velocity(&g->flight);
    int ace = index == 0 && g->chapter >= 3;
    float distance, chance, cooldown;
    if (g->special == SPECIAL_AIRSHIP) return;
    if (g->special == SPECIAL_GOTHA) {
        /* The rear gunner covers the tail of the bomber. */
        distance = vec3_length(vec3_sub(target, me));
        if (now - SIM_EPOCH < 5200u || now < enemy->last_shot ||
            now - enemy->last_shot < 1400u + (uint32_t)(index * 211 % 500) ||
            distance > 450.0f || off_nose(enemy, target) < 1.9f) return;
        enemy_burst(g, enemy, target, target_v, -1,
                    0.28f * (1.0f - distance / 520.0f));
        return;
    }
    if (!(enemies_hostile(g) || personal_arms(g)) ||
        enemy->ai_state != AI_ATTACK ||
        now - SIM_EPOCH < 5200u || now < enemy->last_shot) return;
    if (enemy->target >= 0) {
        Enemy *w = &g->wingmen[enemy->target];
        target = (Vec3){w->world_x, w->world_y, w->world_z};
        target_v = aircraft_velocity(&w->flight);
    }
    distance = vec3_length(vec3_sub(target, me));
    cooldown = 1150.0f - (float)g->chapter * 45.0f + (float)((index * 173) % 400);
    if (distance > 600.0f ||
        !ticks_past(now, enemy->last_shot, (uint32_t)cooldown) ||
        off_nose(enemy, target) > 0.1f) return;
    chance = (0.3f + (float)g->chapter * 0.035f + (ace ? 0.15f : 0.0f)) *
             (1.0f - distance / 700.0f);
    if (enemy->target < 0 && fabsf(g->view_bank) > 0.6f) chance *= 0.6f;
    if (qa_spared(g) && enemy->target < 0 && g->health <= 30) chance = 0.0f;
    if (personal_arms(g)) {
        /* 1914: the enemy observer's rifle, as seldom as ours. */
        if (!ticks_past(now, enemy->last_shot, (uint32_t)cooldown * 3u))
            return;
        chance *= 0.4f;
    }
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

/* ---- The cockpit and the rear seat: waves, Archie and the run home ------
 *
 * A patrol flies out over the lines under Archie, meets the enemy in
 * waves, and turns for home when the sky is clear or the time is up.  The
 * run home is a leg of its own: Archie on the way out of their country,
 * now and then a scout who follows, and the sortie ends when the machine
 * is back over our side.  In 1914 and the last days nobody fights, and the
 * task is to shadow the other machine for half a minute and see where it
 * goes. */

#define DF_HOME_METRES 1700.0f
#define DF_SHADOW_SECONDS 70.0f

static int live_enemies(const Game *g)
{
    int i, n = 0;
    for (i = 0; i < MAX_ENEMIES; ++i)
        if (g->enemies[i].active && !g->enemies[i].crashing) ++n;
    return n;
}

/* The common state of a fresh enemy machine, as reset_sortie sets it. */
static void enemy_fresh(Game *g, Enemy *e, int i)
{
    memset(e, 0, sizeof(*e));
    e->phase = (float)i * 0.8f;
    e->y = 90.0f + (float)((i * 47) % 120);
    e->ai_state = AI_ATTACK;
    e->ai_side = (i & 1) ? 1.0f : -1.0f;
    e->target = -1;
    e->speed = 84.0f + (float)((i * 13) % 24);
    e->throttle = 0.67f + (float)(i & 1) * 0.08f;
    e->hp = 3 + g->chapter / 2;
    e->max_hp = e->hp;
    e->active = 1;
    e->last_shot = sim_now(g) + 650u + (uint32_t)i * 390u;
}

/* An enemy machine some way off, turning in towards us; behind us on the
 * run home. */
static void df_place_enemy(Game *g, int i, float range, float across,
                           float height, int behind)
{
    Enemy *e = &g->enemies[i];
    float h = g->flight_heading, dir = behind ? -1.0f : 1.0f;
    enemy_fresh(g, e, i);
    e->world_x = g->flight_x + sinf(h) * range * dir + cosf(h) * across;
    e->world_z = g->flight_z + cosf(h) * range * dir - sinf(h) * across;
    e->world_y = flight_clamp(g->flight_y + height, 260.0f, 1500.0f);
    e->heading = wrap_angle(atan2f(g->flight_x - e->world_x,
                                   g->flight_z - e->world_z) +
                            ((i & 1) ? 0.25f : -0.25f));
    flight_state_init(&e->flight, e->heading, 0.0f, 0.0f,
                      e->speed * 0.44704f, e->throttle);
    e->flight.power = their_machines[their_machine_index(g->chapter)].power;
    e->flight.agility = their_machines[their_machine_index(g->chapter)].agility;
}

static void df_spawn_wave(Game *g, int count, int behind)
{
    int i, k = 0;
    for (i = 0; i < MAX_ENEMIES && k < count; ++i) {
        if (g->enemies[i].active) continue;
        df_place_enemy(g, i, (behind ? 900.0f : 1500.0f) + (float)k * 260.0f,
                       (float)((k & 1) ? 1 : -1) * (120.0f + (float)k * 90.0f),
                       (float)((k * 57) % 180) - 60.0f, behind);
        ++k;
    }
    g->wave_clock = 0.0f;
    if (k > 0) {
        ++g->wave;
        show_notice(g, behind ? "A SCOUT ON OUR TAIL" :
                    !enemies_hostile(g) ? "A MACHINE AHEAD" :
                    g->wave == 1 ? "ENEMY AHEAD" : "ANOTHER PATROL");
    }
}

static int df_wave_size(const Game *g, int wave)
{
    if (!enemies_hostile(g)) return 1;
    if (wave == 0) return perspective_spawn_count(g);
    if (wave == 1) return 1 + g->chapter / 3;
    return 2;
}

/* ---- The storm cell in the cockpit's world ---- */

static void spawn_squall_3d(Game *g)
{
    float h = g->flight_heading;
    float across = (random_unit(g) < 0.5f ? -1.0f : 1.0f) *
                   (60.0f + random_unit(g) * 160.0f);
    g->squall.centre.x = g->flight_x + sinf(h) * 1700.0f + cosf(h) * across;
    g->squall.centre.z = g->flight_z + cosf(h) * 1700.0f - sinf(h) * across;
    g->squall.centre.y = 0.0f;
    g->squall.active = 1;
}

static int in_squall_3d(const Game *g)
{
    float dx = g->squall.centre.x - g->flight_x;
    float dz = g->squall.centre.z - g->flight_z;
    return g->squall.active && g->flight_y < 1450.0f &&
           dx * dx + dz * dz < 270.0f * 270.0f;
}

static void update_squall_3d(Game *g, float dt)
{
    static float battering;
    float dx = g->squall.centre.x - g->flight_x;
    float dz = g->squall.centre.z - g->flight_z;
    if (!g->squall.active) return;
    if (dx * dx + dz * dz > 3200.0f * 3200.0f) { g->squall.active = 0; return; }
    if (!in_squall_3d(g)) return;
    g->flight.p += (random_unit(g) - 0.5f) * 14.0f * dt;
    g->flight.q += (random_unit(g) - 0.5f) * 6.0f * dt;
    battering += dt;
    if (battering > 0.7f) {
        battering = 0.0f;
        small_hit(g, 1);
        if (g->shake < 0.35f) g->shake = 0.35f;
    }
}

static void draw_squall_3d(Game *g)
{
    SDL_Surface *s = g->screen;
    int k;
    if (!g->squall.active) return;
    for (k = 0; k < 9; ++k) {
        uint32_t h = scene_hash((uint32_t)k * 2654435761u + 0x5351u);
        Vec3 at = {g->squall.centre.x + (float)((int)(h % 300u) - 150),
                   380.0f + (float)(k * 120) + (float)((h >> 9) % 60u),
                   g->squall.centre.z + (float)((int)((h >> 3) % 300u) - 150)};
        Projected3D p = project_world_point(g, at);
        int r;
        if (!p.visible || p.depth < 30.0f) continue;
        r = (int)((150.0f + (float)((h >> 14) % 80u)) * WORLD_FOCAL / p.depth);
        if (r < 4) r = 4;
        if (r > 150) r = 150;
        if (p.x < -r || p.x > SCREEN_W + r || p.y < -r || p.y > SCREEN_H + r)
            continue;
        if (k < 3) {
            /* Rain hanging under the cloud base. */
            Projected3D foot = project_world_point(g, (Vec3){at.x, 0.0f, at.z});
            if (foot.visible && foot.y > p.y && abs(foot.x - p.x) < 400)
                draw_rain(s, p.x - r / 2, p.x + r / 2, p.y, foot.y < SCREEN_H ?
                          foot.y : SCREEN_H - 1, 2, h, PIX_PACK(126, 134, 146));
        }
        /* Soft billows blended over the sky, as the low cloud from above,
         * darker towards the rain. */
        draw_puff_scaled(s, (int)((h >> 3) % 3u), (int)((h >> 5) & 1u), p.x, p.y,
                         r * 2, r * 4 / 3, PIX_PACK(150, 152, 160),
                         PIX_PACK(74, 78, 88), 22 + (int)((h >> 20) % 6u));
    }
    if (in_squall_3d(g)) draw_blinding_cloud(g);
}

static void df_turn_for_home(Game *g)
{
    int i;
    if (g->leg != LEG_OUT) return;
    g->leg = LEG_HOME;
    g->leg_time = 0.0f;
    g->home_progress = 0.0f;
    /* Those still about break off for their own lines. */
    for (i = 0; i < MAX_ENEMIES; ++i)
        if (g->enemies[i].active && !g->enemies[i].crashing) {
            g->enemies[i].ai_state = AI_FLEE;
            g->enemies[i].ai_timer = 40.0f;
        }
    show_notice(g, "TURN FOR HOME");
}

/* Returns 1 when the sortie is over: back over our side of the lines. */
/* ---- 1914 from the cockpit: the observers' work ----------------------
 *
 * In 1914 nobody in the air is armed to speak of, and an aeroplane's work
 * is to look.  Tom has four jobs on the patrol, laid out along the route:
 * a column on a road to mark on the map (fly over it low), a headquarters
 * to drop the general's message bag on (B), a battery to photograph (hold
 * the machine level over it), and our own guns to spot for (watch the fall
 * of shot and signal it with S).  Three of the four make the duty.  The
 * German two-seater shares the sky as before, now only for company. */

enum { OBS_COLUMN, OBS_HQ, OBS_BATTERY, OBS_SPOT };

static int observer_cockpit(const Game *g)
{
    return g->chapter == 0 &&
           campaign_missions[g->chapter][g->mission].mode == MISSION_DOGFIGHT_3D &&
           campaign_missions[g->chapter][g->mission].special == SPECIAL_NONE;
}

static const char *const obs_names[4] = {"COLUMN", "HQ", "BATTERY", "GUNS"};

/* Lays the four jobs along the patrol's course north, in an order that
 * depends on the mission, each a little to one side of the line. */
static void observer_layout(Game *g)
{
    static const int orders[2][4] = {
        {OBS_COLUMN, OBS_HQ, OBS_BATTERY, OBS_SPOT},
        {OBS_SPOT, OBS_COLUMN, OBS_BATTERY, OBS_HQ}
    };
    const int *order = orders[g->mission > 6];
    int k;
    memset(g->obs, 0, sizeof(g->obs));
    for (k = 0; k < 4; ++k) {
        g->obs[k].kind = order[k];
        g->obs[k].x = (k & 1 ? 230.0f : -190.0f) + (float)((k * 37) % 60);
        g->obs[k].z = 1700.0f + (float)k * 1800.0f;
    }
    g->obs_count = 4;
    g->task_total = 4;
    g->bags = 3;
    g->obs_bag_active = 0;
    g->obs_spot_clock = 0.0f;
    g->obs_spot_pending = 0.0f;
    g->obs_signals = 0;
}

static int observer_next(const Game *g)
{
    int k;
    for (k = 0; k < g->obs_count; ++k) if (!g->obs[k].done) return k;
    return -1;
}

static void observer_done(Game *g, ObserverTask *t, const char *line)
{
    t->done = 1;
    ++g->objectives;
    g->score += 400;
    show_notice(g, line);
    g->audio.crack = 500;
}

/* The work itself, frame by frame. */
static void update_observer_tasks(Game *g, float dt, const uint8_t *keys)
{
    uint32_t now = sim_now(g);
    int k;
    /* The message bag in flight: thrown over the side with the machine's
     * own speed, falling until it meets the ground. */
    if (keys[SDL_SCANCODE_B] && g->bags > 0 && !g->obs_bag_active &&
        ticks_past(now, g->last_bomb, 600u)) {
        g->obs_bag_p = (Vec3){g->flight_x, g->flight_y - 2.0f, g->flight_z};
        g->obs_bag_v = aircraft_velocity(&g->flight);
        g->obs_bag_active = 1;
        g->last_bomb = now;
        --g->bags;
    }
    if (g->obs_bag_active) {
        g->obs_bag_v.y -= 9.81f * dt;
        g->obs_bag_p.x += g->obs_bag_v.x * dt;
        g->obs_bag_p.y += g->obs_bag_v.y * dt;
        g->obs_bag_p.z += g->obs_bag_v.z * dt;
        if (g->obs_bag_p.y <= 0.0f) {
            int hit = 0;
            g->obs_bag_active = 0;
            for (k = 0; k < g->obs_count; ++k) {
                ObserverTask *t = &g->obs[k];
                float dx = g->obs_bag_p.x - t->x, dz = g->obs_bag_p.z - t->z;
                if (t->kind == OBS_HQ && !t->done && dx * dx + dz * dz < 70.0f * 70.0f) {
                    observer_done(g, t, "TOM: BAG ON THE TENTS");
                    hit = 1;
                }
            }
            if (!hit) show_notice(g, g->bags > 0 ? "THE BAG FELL WIDE" :
                                                   "NO BAGS LEFT");
        }
    }
    for (k = 0; k < g->obs_count; ++k) {
        ObserverTask *t = &g->obs[k];
        float dx = g->flight_x - t->x, dz = g->flight_z - t->z;
        float d = sqrtf(dx * dx + dz * dz);
        if (t->done) continue;
        switch (t->kind) {
        case OBS_COLUMN:
            if (d < 160.0f && g->flight_y < 460.0f)
                observer_done(g, t, "TOM: COLUMN ON THE MAP");
            break;
        case OBS_BATTERY:
            if (d < 140.0f && g->flight_y < 750.0f && fabsf(g->view_bank) < 0.22f) {
                t->progress += dt;
                if (t->progress > 0.9f)
                    observer_done(g, t, "TOM: PLATE EXPOSED");
            } else if (d >= 140.0f) {
                t->progress = 0.0f;
            }
            break;
        case OBS_SPOT:
            /* Our guns fire on their battery every few seconds while we are
             * near enough to see; each fall of shot wants a signal. */
            if (d < 1300.0f) {
                g->obs_spot_clock -= dt;
                if (g->obs_spot_clock <= 0.0f) {
                    uint32_t h = scene_hash(now);
                    g->obs_spot_clock = 5.0f;
                    g->obs_spot_at = (Vec3){t->x + (float)((int)(h % 160u) - 80),
                                            0.0f,
                                            t->z + (float)((int)((h >> 8) % 160u) - 80)};
                    g->obs_spot_age = 0.0f;
                    g->obs_spot_pending = 3.0f;
                    g->audio.blast = 2000;
                    show_notice(g, "FALL OF SHOT: S SIGNALS");
                }
            }
            if (g->obs_spot_pending > 0.0f) {
                g->obs_spot_pending -= dt;
                if (keys[SDL_SCANCODE_S]) {
                    static const char *const calls[3] = {
                        "TOM SIGNALS: SHORT", "TOM SIGNALS: OVER",
                        "TOM SIGNALS: ON TARGET"
                    };
                    g->obs_spot_pending = 0.0f;
                    if (++g->obs_signals >= 3)
                        observer_done(g, t, calls[2]);
                    else
                        show_notice(g, calls[g->obs_signals - 1]);
                }
            }
            break;
        default:
            break;
        }
    }
    g->obs_spot_age += dt;
}

/* What Tom has still to do, and where, for the cockpit plate. */
static int observer_text(const Game *g, char *buf, size_t size)
{
    int k = observer_next(g);
    float dx, dz, err;
    if (k < 0) return 0;
    dx = g->obs[k].x - g->flight_x;
    dz = g->obs[k].z - g->flight_z;
    err = wrap_angle(atan2f(dx, dz) - g->flight_heading);
    snprintf(buf, size, "%s %04d M %s", obs_names[g->obs[k].kind],
             (int)sqrtf(dx * dx + dz * dz),
             err > 0.3f ? "RIGHT" : err < -0.3f ? "LEFT" : "AHEAD");
    return 1;
}

/* A few specks and shapes on the ground, projected: men on a road, a
 * headquarters' tents and flag, guns in a row, and our shells falling. */
static void draw_ground_speck(Game *g, float x, float z, float metres, pix_t c)
{
    Projected3D p = project_world_point(g, (Vec3){x, 0.0f, z});
    int r;
    if (!p.visible || p.depth < 10.0f || p.x < -10 || p.x > SCREEN_W + 10 ||
        p.y < -10 || p.y > SCREEN_H + 10) return;
    r = (int)(metres * WORLD_FOCAL / p.depth);
    if (r < 1) r = 1;
    if (r > 12) r = 12;
    rect(g->screen, p.x - r / 2, p.y - r / 2, r, r > 1 ? r - 1 : 1, c);
}

static void draw_observer_tasks(Game *g)
{
    SDL_Surface *s = g->screen;
    int k, j;
    if (!g->obs_count) return;
    for (k = 0; k < g->obs_count; ++k) {
        const ObserverTask *t = &g->obs[k];
        Projected3D base = project_world_point(g, (Vec3){t->x, 0.0f, t->z});
        if (!base.visible || base.depth > 6000.0f) continue;
        if (!t->done) {
            /* Each job shows itself from afar by a smudge of smoke going
             * up: the column's dust, the cookhouse fires at headquarters,
             * the guns' own smoke.  It is what Tom looks for. */
            uint32_t tick = sim_now(g) / 90u;
            for (j = 0; j < 5; ++j) {
                float rise = (float)((tick + (uint32_t)(j * 7)) % 35u) / 35.0f;
                Projected3D p = project_world_point(g, (Vec3){
                    t->x + rise * 30.0f, 15.0f + rise * 170.0f, t->z});
                int r;
                if (!p.visible || p.depth < 30.0f) continue;
                r = (int)((16.0f + rise * 44.0f) * WORLD_FOCAL / p.depth);
                if (r < 2) r = 2;
                if (r > 30) r = 30;
                blend_circle(s, p.x, p.y, r,
                             t->kind == OBS_COLUMN ? 170 : 200,
                             t->kind == OBS_COLUMN ? 150 : 198,
                             t->kind == OBS_COLUMN ? 116 : 192,
                             (uint8_t)(190 - (int)(rise * 150.0f)));
            }
        }
        switch (t->kind) {
        case OBS_COLUMN: {
            Projected3D a = project_world_point(g, (Vec3){t->x, 0.0f, t->z - 260.0f});
            Projected3D b = project_world_point(g, (Vec3){t->x, 0.0f, t->z + 260.0f});
            if (a.visible && b.visible)
                line(s, a.x, a.y, b.x, b.y, PIX_PACK(150, 132, 100));
            for (j = 0; j < 16; ++j)
                draw_ground_speck(g, t->x + (float)((j & 1) * 4), t->z - 90.0f +
                                  (float)j * 12.0f, 3.0f, PIX_PACK(60, 64, 56));
            break;
        }
        case OBS_HQ: {
            Projected3D top = project_world_point(g, (Vec3){t->x + 14.0f, 18.0f, t->z});
            for (j = 0; j < 3; ++j)
                draw_ground_speck(g, t->x - 20.0f + (float)j * 18.0f, t->z,
                                  9.0f, PIX_PACK(214, 204, 176));
            if (top.visible && base.visible && base.depth < 3000.0f) {
                Projected3D foot = project_world_point(g, (Vec3){t->x + 14.0f, 0.0f, t->z});
                if (foot.visible) {
                    line(s, foot.x, foot.y, top.x, top.y, PIX_PACK(60, 50, 40));
                    rect(s, top.x, top.y, 5, 3, PIX_PACK(236, 232, 220));
                    rect(s, top.x, top.y + 1, 5, 1, PIX_PACK(176, 40, 32));
                }
            }
            break;
        }
        case OBS_BATTERY:
        case OBS_SPOT:
            for (j = 0; j < 4; ++j)
                draw_ground_speck(g, t->x - 36.0f + (float)j * 24.0f, t->z,
                                  6.0f, PIX_PACK(44, 46, 38));
            break;
        default:
            break;
        }
    }
    if (g->obs_spot_age < 2.5f) {
        /* Our shell bursting: a flash, then a column of brown smoke. */
        Projected3D p = project_world_point(g, (Vec3){g->obs_spot_at.x,
            6.0f + g->obs_spot_age * 8.0f, g->obs_spot_at.z});
        if (p.visible && p.depth > 20.0f) {
            int r = (int)((8.0f + g->obs_spot_age * 10.0f) * WORLD_FOCAL / p.depth);
            if (r < 2) r = 2;
            if (r > 40) r = 40;
            if (g->obs_spot_age < 0.15f)
                blend_circle(s, p.x, p.y, r + 2, 255, 200, 110, 230);
            blend_circle(s, p.x, p.y, r, 96, 80, 60,
                         (uint8_t)(200 - (int)(g->obs_spot_age * 70.0f)));
        }
    }
    if (g->obs_bag_active) {
        Projected3D p = project_world_point(g, g->obs_bag_p);
        if (p.visible) {
            rect(s, p.x - 1, p.y - 1, 3, 3, PIX_PACK(120, 86, 50));
            line(s, p.x, p.y, p.x + 4, p.y - 6, PIX_PACK(236, 230, 214));
        }
    }
}

static int update_df_legs(Game *g, float dt)
{
    int hostile = enemies_hostile(g), alive;
    float pace = 1.0f - (float)g->chapter * 0.035f;
    if (g->sortie_clock == 0.0f) {
        /* The patrol takes off with an empty sky; the enemy comes later. */
        int i;
        for (i = 0; i < MAX_ENEMIES; ++i) g->enemies[i].active = 0;
        g->waves = hostile ? (g->chapter >= 4 ? 3 : 2) : 1;
        g->archie_clock = 6.0f;
        if (observer_cockpit(g)) observer_layout(g);
        g->hazard_clock = 14.0f + random_unit(g) * 20.0f;
    }
    g->sortie_clock += dt;
    g->leg_time += dt;
    g->wave_clock += dt;
    alive = live_enemies(g);
    /* Archie, except in the last quiet hour of the war; sooner when the
     * sky has gone quiet. */
    g->archie_clock -= dt;
    if (g->qa_idle_run > 6.0f && g->archie_clock > 0.5f) g->archie_clock = 0.5f;
    if (g->archie_clock <= 0.0f) {
        if (!(armistice_quiet(g) && g->mission >= 9))
            archie_fire_3d(g);
        g->archie_clock = (5.0f + random_unit(g) * 4.0f) * pace *
                          (alive ? 1.6f : 1.0f);
    }
    g->hazard_clock -= dt;
    if (qa_frame_time && g->qa_run && g->sortie_clock < dt * 1.5f)
        g->hazard_clock = 40.0f;   /* QA: a timed run meets one at 40 s */
    if (g->hazard_clock <= 0.0f) {
        if (!g->squall.active) spawn_squall_3d(g);
        g->hazard_clock = 70.0f + random_unit(g) * 40.0f;
    }
    update_squall_3d(g, dt);
    if (g->leg == LEG_OUT) {
        if (g->wave == 0) {
            if (g->leg_time > (hostile ? 20.0f : 14.0f))
                df_spawn_wave(g, df_wave_size(g, 0), 0);
        } else if (hostile) {
            if (alive == 0 && !enemies_falling(g)) {
                if (g->wave >= g->waves) {
                    df_turn_for_home(g);
                } else {
                    /* A breath, then the next lot. */
                    if (!g->mission_clear_at) g->mission_clear_at = sim_now(g);
                    if (sim_now(g) - g->mission_clear_at > 6000u) {
                        g->mission_clear_at = 0;
                        df_spawn_wave(g, df_wave_size(g, g->wave), 0);
                    }
                }
            } else if (g->wave_clock > 70.0f) {
                if (g->wave < g->waves) df_spawn_wave(g, df_wave_size(g, g->wave), 0);
                else df_turn_for_home(g);
            }
            /* However the fight goes, the patrol's time over the lines
             * is limited by its fuel and its orders. */
            if (g->sortie_clock > 220.0f) df_turn_for_home(g);
        } else if (g->obs_count) {
            /* Home when Tom's work is done, or the patrol has gone on
             * beyond the last of it, or the time is up. */
            if (observer_next(g) < 0) {
                show_notice(g, "TOM: ALL DONE. HOME");
                df_turn_for_home(g);
            } else if (g->sortie_clock > 230.0f ||
                       g->flight_z > g->obs[g->obs_count - 1].z + 1500.0f) {
                df_turn_for_home(g);
            }
        } else {
            Enemy *e = &g->enemies[0];
            int k;
            for (k = 0; k < MAX_ENEMIES; ++k)
                if (g->enemies[k].active) { e = &g->enemies[k]; break; }
            if (e->active) {
                float dx = e->world_x - g->flight_x, dy = e->world_y - g->flight_y;
                float dz = e->world_z - g->flight_z;
                if (dx * dx + dy * dy + dz * dz < 750.0f * 750.0f)
                    g->shadow_time += dt;
            }
            if (g->shadow_time >= DF_SHADOW_SECONDS) {
                show_notice(g, "SEEN WHERE HE WENT");
                df_turn_for_home(g);
            } else if (!e->active || g->wave_clock > 120.0f) {
                df_turn_for_home(g);
            }
        }
    } else {
        Vec3 v = aircraft_velocity(&g->flight);
        float along = v.x * sinf(g->home_heading) + v.z * cosf(g->home_heading);
        if (along > 0.0f) g->home_progress += along * dt;
        if (hostile && g->chapter >= 2 && g->wave <= g->waves &&
            g->leg_time > 7.0f && g->leg_time - dt <= 7.0f) {
            ++g->waves;
            df_spawn_wave(g, 1, 1);
        }
        if (g->home_progress >= DF_HOME_METRES && !enemies_falling(g)) {
            show_notice(g, "OVER OUR LINES");
            return 1;
        }
        if (g->leg_time > 60.0f) return 1;
    }
    return 0;
}

/* The way home, on the cockpit plate. */
static void df_home_text(const Game *g, char *buf, size_t size)
{
    float err = wrap_angle(g->home_heading - g->flight_heading);
    int left = (int)(DF_HOME_METRES - g->home_progress);
    if (left < 0) left = 0;
    snprintf(buf, size, "HOME %s %04d M",
             err > 0.35f ? "RIGHT" : err < -0.35f ? "LEFT " : "AHEAD", left);
}

/* ---- The rear seat ---- */

/* Now and then Tom flies through a squall.  For a few seconds the rear
 * seat sees nothing but cloud and driving rain and the machine is thrown
 * about; then the storm draws away astern, shrinking towards the horizon
 * the gunner is looking at. */
#define GUNNER_SQUALL_INSIDE 5.0f
#define GUNNER_SQUALL_END 17.0f

static void update_squall_gunner(Game *g, float dt)
{
    static float battering;
    if (!g->squall.active) return;
    g->squall.a += dt;
    if (g->squall.a < GUNNER_SQUALL_INSIDE) {
        battering += dt;
        if (battering > 1.1f) {
            battering = 0.0f;
            small_hit(g, 1);
        }
    } else if (g->squall.a > GUNNER_SQUALL_END) {
        g->squall.active = 0;
    }
}

static void draw_squall_gunner(Game *g)
{
    SDL_Surface *s = g->screen;
    float t = g->squall.a, f, big;
    int vx = 320 - (int)(gunner_traverse(g) * 306.0f);
    int vy = 150 + (int)(gunner_elevation(g) * 118.0f);
    int k;
    if (!g->squall.active) return;
    if (t < GUNNER_SQUALL_INSIDE) {
        draw_blinding_cloud(g);
        return;
    }
    f = (t - GUNNER_SQUALL_INSIDE) / (GUNNER_SQUALL_END - GUNNER_SQUALL_INSIDE);
    big = 1.0f - f;
    draw_rain(s, vx - (int)(60.0f + 260.0f * big), vx + (int)(60.0f + 260.0f * big),
              vy, vy + 40 + (int)(200.0f * big), 3, 0x5351u,
              PIX_PACK(150, 158, 170));
    for (k = 0; k < 7; ++k) {
        uint32_t h = scene_hash((uint32_t)k * 2654435761u + 0x47u);
        float spread = 0.25f + big;
        int bx = vx + (int)((float)((int)(h % 400u) - 200) * spread);
        int by = vy - 20 + (int)((float)((int)((h >> 9) % 120u) - 60) * spread * 0.6f);
        int r = (int)((float)(40 + (int)((h >> 18) % 40u)) * (0.35f + big * 1.9f));
        if (r > 120) r = 120;
        /* Soft billows blended over the view, as the cockpit's storm. */
        draw_puff_scaled(s, (int)((h >> 3) % 3u), (int)((h >> 5) & 1u), bx, by,
                         r * 2, r * 4 / 3, PIX_PACK(156, 160, 168),
                         PIX_PACK(76, 80, 90), 26 - (int)(f * 18.0f));
    }
}

static void gunner_place_enemy(Game *g, int i, float depth)
{
    Enemy *e = &g->enemies[i];
    enemy_fresh(g, e, i);
    e->phase = random_unit(g) * 6.0f;
    /* Built to take punishment: it takes a long burst to bring one down,
     * and half of one sends him home. */
    e->hp = mission_armed(g) ? 10 + g->chapter / 2 : 3;
    e->max_hp = e->hp;
    e->x = 0.0f;
    e->x = depth - enemy_depth_3d(g, e, i);
}

static void gunner_spawn_wave(Game *g, int count, int behind)
{
    int i, k = 0;
    for (i = 0; i < MAX_ENEMIES && k < count; ++i) {
        if (g->enemies[i].active) continue;
        gunner_place_enemy(g, i, 2300.0f + (float)k * 480.0f);
        ++k;
    }
    g->wave_clock = 0.0f;
    if (k > 0) {
        ++g->wave;
        show_notice(g, behind ? "ONE FOLLOWS US HOME" :
                    g->wave == 1 ? "MACHINES ASTERN" : "MORE OF THEM");
    }
}

static int gunner_wave_size(const Game *g, int wave)
{
    if (!mission_armed(g)) return 1;
    if (wave == 0) return perspective_spawn_count(g) > 2 ? 2 :
                          perspective_spawn_count(g);
    return 1;
}

/* Returns 1 when the sortie is over. */
static int update_gunner_legs(Game *g, float dt)
{
    int alive;
    float pace = 1.0f - (float)g->chapter * 0.035f;
    if (g->sortie_clock == 0.0f) {
        int i;
        for (i = 0; i < MAX_ENEMIES; ++i) g->enemies[i].active = 0;
        g->archie_clock = 3.0f;
        g->hazard_clock = 45.0f + random_unit(g) * 20.0f;
    }
    g->sortie_clock += dt;
    g->leg_time += dt;
    g->wave_clock += dt;
    /* Those breaking off for home no longer count as attackers. */
    alive = 0;
    {
        int i;
        for (i = 0; i < MAX_ENEMIES; ++i)
            if (g->enemies[i].active && !g->enemies[i].crashing &&
                !g->enemies[i].fled) ++alive;
    }
    g->hazard_clock -= dt;
    if (qa_frame_time && g->qa_run && g->sortie_clock - dt < 40.0f &&
        g->sortie_clock >= 40.0f) g->hazard_clock = 0.0f;
    if (g->hazard_clock <= 0.0f && !g->squall.active && g->leg == LEG_OUT) {
        memset(&g->squall, 0, sizeof(g->squall));
        g->squall.active = 1;
        show_notice(g, "TOM FLIES INTO RAIN");
        g->hazard_clock = 60.0f + random_unit(g) * 30.0f;
    }
    update_squall_gunner(g, dt);
    g->archie_clock -= dt;
    if (g->qa_idle_run > 6.0f && g->archie_clock > 0.5f) g->archie_clock = 0.5f;
    if (g->archie_clock <= 0.0f) {
        if (!(armistice_quiet(g) && g->mission >= 9)) archie_fire_2d(g);
        g->archie_clock = (5.5f + random_unit(g) * 4.0f) * pace *
                          (alive ? 1.8f : 0.7f) * (g->chapter == 0 ? 1.3f : 1.0f);
    }
    if (g->leg == LEG_OUT) {
        /* A steady stream of attackers rather than set waves with lulls
         * between: the next comes in as soon as the sky astern is clear,
         * and one more joins if the last is taking his time. */
        if (g->wave == 0) {
            if (g->leg_time > 6.0f) gunner_spawn_wave(g, gunner_wave_size(g, 0), 0);
        } else if (g->sortie_clock > 175.0f ||
                   g->wave >= (mission_armed(g) ? 18 : 12)) {
            if (alive == 0 || g->wave_clock > 30.0f) {
                g->leg = LEG_HOME;
                g->leg_time = 0.0f;
                show_notice(g, "TOM TURNS FOR HOME");
            }
        } else if (alive == 0 && !enemies_falling(g)) {
            if (!g->mission_clear_at) g->mission_clear_at = sim_now(g);
            if (sim_now(g) - g->mission_clear_at > 2500u &&
                g->wave_clock > 7.0f) {
                g->mission_clear_at = 0;
                gunner_spawn_wave(g, gunner_wave_size(g, g->wave), 0);
            }
        } else if (alive < 2 && g->wave_clock > 14.0f) {
            g->mission_clear_at = 0;
            gunner_spawn_wave(g, 1, 0);
        }
    } else {
        if (mission_armed(g) && g->chapter >= 1 &&
            g->leg_time > 4.0f && g->leg_time - dt <= 4.0f)
            gunner_spawn_wave(g, 1, 1);
        if ((g->leg_time > 28.0f && alive == 0 && !enemies_falling(g)) ||
            g->leg_time > 45.0f)
            return 1;
    }
    return 0;
}

/* ---- QA autopilots ------------------------------------------------------
 *
 * "FLY" on the command line hands the controls to a plain pilot so a QA
 * run can fly a whole sortie unattended: he keeps a sensible height,
 * climbs over cables and round squalls, goes for what is ahead and lands
 * at the end.  He is no ace; he is there so the measurements cover a
 * sortie as a player would fly it. */

static const uint8_t *qa_side_keys(Game *g, const uint8_t *keys)
{
    static uint8_t fake[SDL_NUM_SCANCODES];
    float ground = (float)(terrain_y(g->scroll + g->player_x) - 19);
    float want = 190.0f, best = 1e9f;
    float here = g->scroll + g->player_x;
    int i, task_close = 0;
    memcpy(fake, keys, sizeof(fake));
    fake[SDL_SCANCODE_UP] = fake[SDL_SCANCODE_DOWN] = 0;
    fake[SDL_SCANCODE_SPACE] = fake[SDL_SCANCODE_B] = 0;
    fake[SDL_SCANCODE_X] = fake[SDL_SCANCODE_Z] = 0;
    for (i = 0; i < MAX_TARGETS; ++i) {
        const Target *t = &g->targets[i];
        float dx = t->world_x - g->scroll - g->player_x;
        if (!t->active || t->civilian || t->done || t->wrecked) continue;
        if (dx < -30.0f || dx > 360.0f || dx > best) continue;
        best = dx;
        if (t->task && dx < 260.0f) task_close = 1;
        if (t->task == TASK_MARK) {
            want = ground - 110.0f;
        } else if (t->task) {
            want = ground - 130.0f;
            if (t->task == TASK_PHOTO && dx < 220.0f) {
                /* Level over the guns while the plate is exposed. */
                want = g->player_y;
            }
            if (t->task == TASK_DROP && g->bags > 0) {
                float h = fmaxf(ground + 15.0f - g->player_y - 14.0f, 1.0f);
                float fall = (-50.0f + sqrtf(50.0f * 50.0f + 210.0f * h)) / 105.0f;
                float off = dx - (40.0f + g->airspeed) * fall;
                if (off > -8.0f && off < 6.0f)
                    fake[SDL_SCANCODE_B] = 1;
            }
        } else if (mission_armed(g)) {
            /* A bomb leaves at 55 down and falls at 105: when will it
             * reach the gun's height? */
            float h = fmaxf(ground + 19.0f - 12.0f - 32.0f - g->player_y, 1.0f);
            float fall = (-55.0f + sqrtf(55.0f * 55.0f + 210.0f * h)) / 105.0f;
            /* Let go on the mark, not at the edge of the window, or the
             * bomb falls a gun's length short. */
            float off = dx - (42.0f + g->airspeed) * fall;
            want = ground - 185.0f;
            if (off > -8.0f && off < 6.0f)
                fake[SDL_SCANCODE_B] = 1;
        }
    }
    for (i = 0; i < MAX_ENEMIES; ++i) {
        const Enemy *e = &g->enemies[i];
        float sx = e->x - g->scroll;
        float sy = e->y + sinf(g->scroll * 0.01f + e->phase) * 30.0f;
        if (!e->active) continue;
        if (e->ai_side <= 0.0f && sx > g->player_x + 60.0f &&
            sx < g->player_x + 420.0f) {
            want = sy;
            if (fabsf(sy - g->player_y) < 22.0f) fake[SDL_SCANCODE_SPACE] = 1;
        } else if (e->ai_side > 0.0f && sx < g->player_x &&
                   sx > g->player_x - 300.0f) {
            want = sy > 180.0f ? sy - 100.0f : sy + 100.0f;
        }
    }
    if (g->balloon.active && !g->balloon.cut) {
        float bx = g->balloon.x - here;
        /* Over the cable, unless the work is in hand and the cable is
         * not yet near: then the work first. */
        if (bx > -30.0f && bx < (task_close ? 110.0f : 380.0f))
            want = fminf(want, g->balloon.y - 26.0f);
    }
    if (!task_close && g->squall.active && g->squall.a - here < 420.0f &&
        g->squall.b > here &&
        want > g->squall.lo - 20.0f && want < g->squall.hi + 20.0f)
        want = g->squall.lo > 60.0f ? g->squall.lo - 28.0f : g->squall.hi + 28.0f;
    for (i = 0; i < MAX_ARCHIE; ++i) {
        const Archie *a = &g->archie[i];
        if (!task_close && a->active && a->kind == ARCHIE_RANGING &&
            a->fuse > 0.0f &&
            fabsf(a->ty - g->player_y) < 50.0f)
            want = g->player_y > 170.0f ? g->player_y - 90.0f : g->player_y + 90.0f;
    }
    if (want > ground - 75.0f && g->leg != LEG_LANDING) want = ground - 75.0f;
    if (g->leg == LEG_LANDING) {
        want = ground + 10.0f;
        fake[SDL_SCANCODE_Z] = 1;
    }
    want = flight_clamp(want, 50.0f, ground + 10.0f);
    if (g->player_y < want - 8.0f) fake[SDL_SCANCODE_DOWN] = 1;
    else if (g->player_y > want + 8.0f) fake[SDL_SCANCODE_UP] = 1;
    return fake;
}

/* The cockpit: turn after the nearest enemy, or for home, and fire when
 * he is on the nose. */
static void qa_fly_cockpit(Game *g, float *aileron, float *elevator, int *fire,
                           uint8_t *fake)
{
    Vec3 me = {g->flight_x, g->flight_y, g->flight_z};
    Vec3 fwd = aircraft_velocity(&g->flight);
    float want_heading = g->flight_heading, want_pitch = 0.0f, best = 1e9f;
    float err, bank_want, fl = sqrtf(fwd.x * fwd.x + fwd.y * fwd.y + fwd.z * fwd.z);
    int i;
    *fire = 0;
    if (g->leg == LEG_HOME) {
        want_heading = g->home_heading;
    } else {
        for (i = 0; i < MAX_ENEMIES; ++i) {
            const Enemy *e = &g->enemies[i];
            float dx, dy, dz, d, h;
            if (!e->active || e->crashing) continue;
            dx = e->world_x - me.x; dy = e->world_y - me.y; dz = e->world_z - me.z;
            d = sqrtf(dx * dx + dy * dy + dz * dz);
            if (d >= best) continue;
            best = d;
            h = sqrtf(dx * dx + dz * dz);
            want_heading = atan2f(dx, dz);
            want_pitch = flight_clamp(atan2f(dy, h > 1.0f ? h : 1.0f), -0.35f, 0.35f);
            if (fl > 1.0f && d < 450.0f &&
                (fwd.x * dx + fwd.y * dy + fwd.z * dz) / (fl * d) > 0.995f)
                *fire = 1;
        }
    }
    if (g->leg == LEG_OUT && g->obs_count && observer_next(g) >= 0) {
        /* 1914: Tom's next job.  Down to the height it wants, straight at
         * it, level over the battery, round and round over the guns. */
        const ObserverTask *t = &g->obs[observer_next(g)];
        float dx = t->x - me.x, dz = t->z - me.z, d = sqrtf(dx * dx + dz * dz);
        float want_y = t->kind == OBS_COLUMN ? 380.0f :
                       t->kind == OBS_BATTERY ? 520.0f : 560.0f;
        want_heading = atan2f(dx, dz);
        want_pitch = flight_clamp((want_y - me.y) * 0.004f, -0.25f, 0.25f);
        *fire = 0;
        if (t->kind == OBS_HQ && g->bags > 0 && !g->obs_bag_active) {
            float vh = sqrtf(fwd.x * fwd.x + fwd.z * fwd.z);
            float fall = sqrtf(2.0f * me.y / 9.81f);
            if (fabsf(wrap_angle(want_heading - g->flight_heading)) < 0.12f &&
                fabsf(d - vh * fall) < 25.0f)
                fake[SDL_SCANCODE_B] = 1;
        }
        if (t->kind == OBS_SPOT) {
            if (d < 1000.0f) want_heading += 1.35f;   /* circle the guns */
            if (g->obs_spot_pending > 0.0f && g->obs_spot_pending < 2.4f)
                fake[SDL_SCANCODE_S] = 1;
        }
    }
    if (g->squall.active) {
        float dx = g->squall.centre.x - me.x, dz = g->squall.centre.z - me.z;
        if (dx * dx + dz * dz < 600.0f * 600.0f &&
            fabsf(wrap_angle(atan2f(dx, dz) - g->flight_heading)) < 0.5f)
            want_heading = g->flight_heading + 0.8f;
    }
    if (g->flight_y < 320.0f) want_pitch = 0.3f;
    if (g->flight_y > 1500.0f) want_pitch = -0.15f;
    err = wrap_angle(want_heading - g->flight_heading);
    bank_want = flight_clamp(err * 1.3f, -0.85f, 0.85f);
    if (g->leg == LEG_OUT && g->obs_count && observer_next(g) >= 0 &&
        g->obs[observer_next(g)].kind == OBS_BATTERY) {
        const ObserverTask *t = &g->obs[observer_next(g)];
        float dx = t->x - me.x, dz = t->z - me.z;
        if (dx * dx + dz * dz < 260.0f * 260.0f) bank_want = 0.0f;
    }
    *aileron = flight_clamp((bank_want - g->view_bank) * 2.2f -
                            g->flight.p * 0.32f, -1.0f, 1.0f);
    *elevator = flight_clamp((want_pitch - g->view_pitch) * 2.4f -
                             g->flight.q * 0.28f + fabsf(g->view_bank) * 0.55f,
                             -1.0f, 1.0f);
}

/* The rear seat: swing the sight onto the nearest attacker. */
static const uint8_t *qa_gunner_keys(Game *g, const uint8_t *keys)
{
    static uint8_t fake[SDL_NUM_SCANCODES];
    int i, best = 1 << 30, tx = (int)g->player_x, ty = (int)g->player_y;
    memcpy(fake, keys, sizeof(fake));
    fake[SDL_SCANCODE_LEFT] = fake[SDL_SCANCODE_RIGHT] = 0;
    fake[SDL_SCANCODE_UP] = fake[SDL_SCANCODE_DOWN] = 0;
    fake[SDL_SCANCODE_SPACE] = 0;
    for (i = 0; i < MAX_ENEMIES; ++i) {
        int sx, sy, size, d;
        if (!g->enemies[i].active || g->enemies[i].crashing) continue;
        if (!project_enemy_3d(g, &g->enemies[i], i, &sx, &sy, &size)) continue;
        if (sx < 0 || sx > SCREEN_W || sy < 0 || sy > 300) continue;
        d = abs(sx - (int)g->player_x) + abs(sy - (int)g->player_y);
        if (d < best) { best = d; tx = sx; ty = sy; }
    }
    /* A man, not a machine: he follows the target a little behind it and
     * fires once he has held it in the sight for half a second. */
    {
        static uint32_t held_since;
        uint32_t now = sim_now(g);
        if (tx < (int)g->player_x - 14) fake[SDL_SCANCODE_LEFT] = 1;
        if (tx > (int)g->player_x + 14) fake[SDL_SCANCODE_RIGHT] = 1;
        if (ty < (int)g->player_y - 14) fake[SDL_SCANCODE_UP] = 1;
        if (ty > (int)g->player_y + 14) fake[SDL_SCANCODE_DOWN] = 1;
        if (best >= 30) held_since = now;
        if (best < 30 && now - held_since > 500u &&
            !gunner_tail_blocks_shot(g, (int)g->player_x, (int)g->player_y))
            fake[SDL_SCANCODE_SPACE] = 1;
    }
    return fake;
}

static void update_dogfight_3d_sortie(Game *g, float dt,
                                      const uint8_t *keys)
{
    uint32_t now = sim_now(g);
    int i;
    int active = 0;
    /* Campaign sorties fly in legs; the specials and the held QA views
     * keep their own endings. */
    int legs = !g->qa_hold && g->special == SPECIAL_NONE;
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
        if (g->qa_autopilot) {
            static uint8_t fake[SDL_NUM_SCANCODES];
            int fire;
            memcpy(fake, keys, sizeof(fake));
            fake[SDL_SCANCODE_B] = fake[SDL_SCANCODE_S] = 0;
            qa_fly_cockpit(g, &aileron_command, &elevator_command, &fire, fake);
            fake[SDL_SCANCODE_SPACE] = (uint8_t)fire;
            keys = fake;
        }
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
    if (!qa_still) g->scroll += g->airspeed * dt;
    /* A tank for the patrol out, its fights and the way home. */
    g->fuel -= dt * (0.36f + g->flight_throttle * 1.05f) / 3.6f;
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
        g->ammunition > 0 &&
        now - g->last_shot >=
            (uint32_t)our_machines[our_machine_index(g->chapter)].fire_ms) {
        g->last_shot = now;
        --g->ammunition;
        g->audio.shot = 1500;
        g->gun_recoil = 1.0f;
        fire_player_guns(g);
        if (pilot_gun_count(g) == 1) {
            new_dogfight_particle(g, 330.0f, 245.0f,
                                  58.0f, -62.0f, 0.42f, 2);
        } else {
            new_dogfight_particle(g, 298.0f, 245.0f,
                                  -54.0f, -68.0f, 0.42f, 2);
            new_dogfight_particle(g, 344.0f, 245.0f,
                                  58.0f, -62.0f, 0.42f, 2);
        }
        if (random_unit(g) < 0.004f - g->skill_gunnery * 0.0025f) {
            g->guns_jammed = 1;
            g->jam_clear_until = 0;
            show_notice(g, "GUNS JAMMED - PRESS R");
        }
    } else if (g->guns_jammed && keys[SDL_SCANCODE_SPACE] &&
               (!g->notice_until || now >= g->notice_until)) {
        show_notice(g, "GUNS JAMMED - PRESS R");
    } else if (!g->flight_doomed && keys[SDL_SCANCODE_SPACE] &&
               personal_arms(g) && personal_shot(g, 1)) {
        fire_revolver(g);
    }
    /* H turns for home: the sortie ends as it stands. */
    if (keys[SDL_SCANCODE_H] && !g->heading_home_at && !g->flight_doomed &&
        !g->qa_hold) {
        g->heading_home_at = now + 2200u;
        show_notice(g, "TURNING FOR HOME");
    }
    if (g->heading_home_at && now >= g->heading_home_at && !g->flight_doomed) {
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
        if (distance < 18.0f && qa_spared(g)) {
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
        if (g->special == SPECIAL_AIRSHIP || g->special == SPECIAL_GOTHA) {
            /* Raiders hold course and speed for London: an airship at
             * 40 mph, a Gotha at 70, not a scout's speed through the
             * scout flight model.  Past the line, they have got through. */
            float v = enemy->speed * 0.44704f;
            enemy->world_x += sinf(enemy->heading) * v * dt;
            enemy->world_z += cosf(enemy->heading) * v * dt;
            if (enemy->world_x > RAIDER_THROUGH_X && !g->qa_hold &&
                !g->flight_doomed && !g->flight_crashed) {
                g->raider_through = 1;
                show_notice(g, g->special == SPECIAL_AIRSHIP ?
                            "THE RAIDER HAS GOT THROUGH" :
                            "THE BOMBERS HAVE GOT THROUGH");
                if (!g->heading_home_at) g->heading_home_at = now + 2600u;
            }
        } else {
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
            /* A machine that hits the ground is ours only if our own
             * rounds struck it. */
            if (enemy->player_hit) credit_air_kill(g);
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
    if (legs && g->obs_count && g->leg == LEG_OUT && !g->flight_doomed)
        update_observer_tasks(g, dt, keys);
    if (legs) {
        update_archie(g, dt);
        if (!g->flight_doomed && !g->flight_crashed && update_df_legs(g, dt)) {
            finish_alternate_sortie(g);
            return;
        }
    } else if ((active == 0 || (!g->qa_hold && current_mission_goal(g) > 0 &&
                         g->kills >= current_mission_goal(g) &&
                         !enemies_falling(g))) &&
        !g->flight_doomed) {
        /* Hold the last impact for one beat so its explosion and crash sound
         * are seen and heard, then end both campaign and direct-test sorties.
         * Reaching the goal ends it too: two of three Gothas is the order. */
        if (!g->mission_clear_at) g->mission_clear_at = now;
        if (now - g->mission_clear_at >= 900u)
            finish_alternate_sortie(g);
    } else if (active > 0) {
        g->mission_clear_at = 0;
    }
    if (g->scene != SCENE_SORTIE) return;
    if (!g->qa_hold && g->health <= 0 &&
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
    if (!g->qa_hold && (active > 0 || legs) && g->fuel <= 0.0f &&
        !g->flight_doomed) {
        finish_alternate_sortie(g);
    } else if (!legs && !g->qa_hold && !enemies_hostile(g) && active > 0 &&
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
    if (g->qa_autopilot) keys = qa_gunner_keys(g, keys);
    update_perspective_sortie(g, dt, keys, 1);
}

/* A 112 lb bomb throws earth and steel a long way up.  Bombing from too
 * low hurts the machine that dropped it: the damage falls away with
 * distance and is gone beyond BOMB_BLAST_RADIUS, about a hundred feet of
 * height on this scale, which is the safe height the pilots learnt. */
#define BOMB_BLAST_RADIUS 104.0f
#define BOMB_BLAST_DAMAGE 30.0f
static void own_bomb_blast(Game *g, float x, float y, uint32_t now)
{
    float dx = g->player_x - x, dy = g->player_y - y;
    float distance = sqrtf(dx * dx + dy * dy);
    float reach;
    int damage;
    if (distance >= BOMB_BLAST_RADIUS) return;
    reach = 1.0f - distance / BOMB_BLAST_RADIUS;
    damage = (int)(BOMB_BLAST_DAMAGE * reach * reach + 0.5f);
    if (reach > g->shake) g->shake = reach;
    if (reach > g->blast_flash) g->blast_flash = reach;
    if (damage <= 0) return;
    if (!qa_spared(g) || g->health > damage) g->health -= damage;
    g->last_damage = now;
    new_explosion(g, g->player_x - 6.0f, g->player_y + 6.0f, 0);
}

static void update_side_sortie(Game *g, float dt, const uint8_t *keys)
{
    int i, j;
    uint32_t now = sim_now(g);
    int ground;
    if (g->qa_autopilot) keys = qa_side_keys(g, keys);
    {
        /* The year's machine sets the top speed and how hard it pulls. */
        const MachineType *ours = &our_machines[our_machine_index(g->chapter)];
        if (keys[SDL_SCANCODE_X]) g->airspeed += 42.0f * dt;
        if (keys[SDL_SCANCODE_Z]) g->airspeed -= 42.0f * dt;
        if (g->landed) {
            /* Down on the field and rolling to a stop. */
            g->airspeed -= 55.0f * dt;
            if (g->airspeed < 0.0f) g->airspeed = 0.0f;
        } else {
            if (g->airspeed < 62.0f) g->airspeed = 62.0f;
            if (g->airspeed > 142.0f * ours->power)
                g->airspeed = 142.0f * ours->power;
        }
        if (keys[SDL_SCANCODE_LEFT]) g->player_x -= 155.0f * ours->agility * dt;
        if (keys[SDL_SCANCODE_RIGHT]) g->player_x += 155.0f * ours->agility * dt;
    }
    if (g->player_x < 44.0f) g->player_x = 44.0f;
    if (g->player_x > 590.0f) g->player_x = 590.0f;
    if (keys[SDL_SCANCODE_UP])
        g->player_vy -= 245.0f *
            our_machines[our_machine_index(g->chapter)].agility * dt;
    if (keys[SDL_SCANCODE_DOWN])
        g->player_vy += 245.0f *
            our_machines[our_machine_index(g->chapter)].agility * dt;
    g->player_vy *= powf(0.10f, dt);
    if (g->player_vy < -120.0f) g->player_vy = -120.0f;
    if (g->player_vy > 120.0f) g->player_vy = 120.0f;
    g->player_y += g->player_vy * dt;
    if (g->player_y < 46.0f) { g->player_y = 46.0f; g->player_vy = 0.0f; }
    ground = terrain_y(g->scroll + g->player_x) - 19;
    if (g->player_y > ground || g->landed) {
        g->player_y = (float)ground;
        if (fabsf(g->player_vy) > 45.0f && !g->landed) g->health -= 3;
        g->player_vy = g->landed ? 0.0f : -8.0f;
    }
    if (!qa_still) g->scroll += g->airspeed * dt;
    /* A tank for the way out, both groups of targets and the way home. */
    g->fuel -= dt * 0.30f;
    g->audio.engine_speed = g->airspeed;
    g->audio.engine_load = fabsf(g->player_vy) / 120.0f;
    g->audio.other_engine = 0.0f;
    g->audio.other_pitch = 0.0f;
    if (g->bags > 0 && keys[SDL_SCANCODE_B] && now - g->last_bomb >= 550u) {
        /* The message bag, weighted, with its streamer. */
        if (new_bullet(g, g->player_x, g->player_y + 14.0f, 40.0f, 50.0f,
                       0, 4, 4.0f)) {
            g->last_bomb = now;
            --g->bags;
        }
    }
    if (mission_armed(g)) {
        if (keys[SDL_SCANCODE_SPACE]) fire(g);
        if (keys[SDL_SCANCODE_B]) drop_bomb(g);
    } else if (personal_arms(g) && keys[SDL_SCANCODE_SPACE] &&
               personal_shot(g, 0)) {
        /* Tom fires his carbine from the front seat, along the nose. */
        float mx, my, nose;
        Bullet *b;
        side_muzzle_at((int)g->player_x, (int)g->player_y, 0,
                       2 + (int)(g->player_vy / 45.0f), &mx, &my, &nose);
        b = new_bullet(g, mx - 14.0f, my - 4.0f, 520.0f * cosf(nose),
                       -520.0f * sinf(nose), 0, 0, 0.5f);
        if (b && random_unit(g) > PERSONAL_HIT_CHANCE) b->missed = 1;
    }

    for (i = 0; i < MAX_EXPLOSIONS; ++i) if (g->explosions[i].active) {
        g->explosions[i].age += dt;
        if (g->explosions[i].age >= g->explosions[i].duration)
            g->explosions[i].active = 0;
    }
    g->shake -= dt * 2.2f;
    if (g->shake < 0.0f) g->shake = 0.0f;
    g->blast_flash -= dt * 3.5f;
    if (g->blast_flash < 0.0f) g->blast_flash = 0.0f;

    for (i = 0; i < MAX_BULLETS; ++i) if (g->bullets[i].active) {
        Bullet *b = &g->bullets[i];
        if (b->type == 1 || b->type == 4) b->vy += 105.0f * dt;
        if (b->type == 2) b->vy += 92.0f * dt;
        if (b->type == 3) b->vy += 70.0f * dt;
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
                    if (!qa_spared(g) || g->health > 30) g->health -= damage;
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
            if (!qa_spared(g) || g->health > 30) g->health -= b->type == 2 ? 8 : 3;
            g->last_damage = now;
            b->active = 0;
            new_explosion(g, g->player_x - 2.0f, g->player_y, 0);
            if (b->type == 2) g->audio.blast = 5200;
            else g->audio.shot = 800;
        }
    }

    update_side_chasers(g, dt, now);
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
        if ((sx < -80 && g->enemies[i].ai_side <= 0.0f) || sx < -500.0f)
            g->enemies[i].active = 0;
        if (g->enemies[i].ai_side <= 0.0f &&
            sx > g->player_x + 65.0f && sx < SCREEN_W + 20.0f &&
            ticks_past(now, g->enemies[i].last_shot,
                       fire_delay + (uint32_t)(i * 47 % 260))) {
            /* A scout's guns are fixed: it fires along its nose, so it
             * holds fire until the nose is within about 15 degrees of us,
             * and the pilot's last touch of rudder bends the line half way. */
            float mx, my, nose, aim;
            side_muzzle_at((int)sx, (int)sy, 1,
                           2 + (int)(sinf(g->scroll * 0.02f + i) * 1.2f),
                           &mx, &my, &nose);
            aim = atan2f(my - g->player_y, mx - g->player_x);
            if (fabsf(aim - nose) < 0.26f) {
                float line_of_fire = (aim + nose) * 0.5f;
                if (new_bullet(g, mx, my, -cosf(line_of_fire) * shot_speed,
                               -sinf(line_of_fire) * shot_speed, 1, 0, 3.5f))
                    g->audio.shot = 1700;
                g->enemies[i].last_shot = now;
            }
        }
        for (j = 0; j < MAX_BULLETS; ++j) if (g->bullets[j].active) {
            if (!g->bullets[j].enemy && g->bullets[j].type == 0 &&
                !g->bullets[j].missed &&
                fabsf(g->bullets[j].x - sx) < 34.0f &&
                fabsf(g->bullets[j].y - sy) < 18.0f) {
                g->bullets[j].active = 0;
                if (--g->enemies[i].hp <= 0) {
                    g->enemies[i].active = 0;
                    credit_air_kill(g);
                    new_explosion(g, sx, sy, 1);
                    g->audio.blast = 6500;
                }
            }
        }
        if (g->enemies[i].active &&
            fabsf(sx - g->player_x) < 36.0f && fabsf(sy - g->player_y) < 18.0f) {
            if (!qa_spared(g) || g->health > 40) g->health -= 25;
            g->enemies[i].active = 0;
            new_explosion(g, g->player_x, g->player_y, 1);
            g->audio.blast = 6200;
        }
    }
    for (i = 0; i < MAX_TARGETS; ++i) if (g->targets[i].active) {
        float sx = g->targets[i].world_x - g->scroll;
        int ty = terrain_y(g->targets[i].world_x) - 12;
        Target *target = &g->targets[i];
        if (target->hazard && sx < -100.0f) {
            target->active = 0;
            continue;
        }
        if (target->task) {
            update_side_task(g, target, dt);
            continue;
        }
        if (target->civilian) {
            /* People's homes: struck by a bomb or a burst, they burn, and
             * the offence is entered against the pilot. */
            const short *r = side_civilian_rect[target->civilian];
            float half = (float)r[2] * 0.5f;
            if (target->wrecked) continue;
            for (j = 0; j < MAX_BULLETS; ++j) if (g->bullets[j].active) {
                Bullet *b = &g->bullets[j];
                if (b->enemy || b->missed) continue;
                if ((b->type == 0 && fabsf(b->x - sx) < half &&
                     b->y > (float)(ty + 12 - r[3]) && b->y < (float)ty + 12.0f) ||
                    (b->type == 1 && fabsf(b->x - sx) < half + 8.0f &&
                     b->y > (float)(ty + 12 - r[3]))) {
                    b->active = 0;
                    target->wrecked = 1;
                    new_ground_explosion(g, sx, (float)ty, 3);
                    g->audio.blast = 7000;
                    td_offence(g, CIV_CIVILIAN);
                    break;
                }
            }
            continue;
        }
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
                    ground_mg_arc(g, sx + 34.0f, (float)ty - 18.0f);
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
                if (target->hazard) {
                    g->score += 150;
                } else {
                    g->score += 500;
                    ++g->objectives;
                }
                new_ground_explosion(g, sx, (float)ty,
                                     b->type == 1 ? 3 : 1);
                g->audio.blast = 7000;
                if (b->type == 1) own_bomb_blast(g, sx, (float)ty, now);
            }
        }
    }
    /* A missed bomb still strikes the physical ground and detonates.  Target
     * collision is resolved first above so a direct hit gets the same larger
     * blast without being replaced by a generic terrain impact. */
    for (i = 0; i < MAX_BULLETS; ++i) {
        Bullet *b = &g->bullets[i];
        if (b->active && !b->enemy && b->type == 4 &&
            b->y >= terrain_y(g->scroll + b->x) - 4) {
            b->active = 0;
            bag_lands(g, b->x, b->y);
        }
        if (b->active && !b->enemy && b->type == 1) {
            int impact_y = terrain_y(g->scroll + b->x) - 2;
            if (b->y >= impact_y) {
                b->y = (float)impact_y;
                b->active = 0;
                new_ground_explosion(g, b->x, b->y, 2);
                g->audio.blast = 7000;
                own_bomb_blast(g, b->x, b->y, now);
                riflemen_blast(g, b->x, b->y, 45.0f);
            }
        }
    }
    update_riflemen(g, dt);
    update_archie(g, dt);
    update_squall_2d(g, dt);
    update_balloon(g, dt);
    side_tree_check(g);
    sortie_director_2d(g, dt);
    if (update_legs_2d(g, dt) || g->fuel <= 0.0f || g->health <= 0) {
        if (g->health <= 0 && hold_for_wreck(g, g->player_x, g->player_y))
            return;
        finish_alternate_sortie(g);
    }
}

/* Each view flies its own sortie; the QA tally watches them all. */
static void update_sortie(Game *g, float dt, const uint8_t *keys)
{
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    sim_advance(&g->sim_ms, &g->sim_carry, dt);
    if (mode == MISSION_TOP_DOWN) update_top_down_sortie(g, dt, keys);
    else if (mode == MISSION_DOGFIGHT_3D) update_dogfight_3d_sortie(g, dt, keys);
    else if (mode == MISSION_GUNNER_3D) update_gunner_3d_sortie(g, dt, keys);
    else update_side_sortie(g, dt, keys);
    if (g->scene == SCENE_SORTIE) sortie_tally(g, dt);
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
    int text_x = x + (w - typewriter_width(label, 11)) / 2;
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
        "JOIN THE SQUADRON - 1914",
        "JOIN THE SQUADRON - 1915",
        "JOIN THE SQUADRON - 1916",
        "JOIN THE SQUADRON - 1917",
        "JOIN THE SQUADRON - 1918"
    };
    SDL_Surface *s = g->screen;
    int count = campaign_menu_count(g);
    int has_continue = g->has_save && g->saved_lives > 0 &&
                       !g->saved_conduct[4];
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
            label = "THE OFFICER'S PREFERENCES";
        } else if (i == count - 2) {
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
        draw_victorian_plaque(g, 128, 112 + i * (count > 8 ? 25 :
                                                 count > 7 ? 28 : 31), 384,
                              label, i == g->menu_selection);
    }
    if (g->has_save && g->saved_conduct[4])
        typewriter_text(g, 320 - typewriter_width(
                            "THE LAST CAREER ENDED: CASHIERED", 11) / 2, 352,
                        "THE LAST CAREER ENDED: CASHIERED", 7, 11,
                        rgb(s, 214, 120, 96));
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
 * as if complete so words never jump between lines as they are written.
 * 'columns' is the width in the old fixed cells: 7 pixels typed, 8 written. */
static int wrapped_text_limit(Game *g, int x, int y, const char *str,
                              int columns, int line_height, int limit,
                              int typewriter, uint32_t color)
{
    FontId font = typewriter ? FONT_TYPE_SMALL : FONT_HAND;
    int max_w = columns * (typewriter ? 7 : 8);
    char line_buf[96];
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
        if (word_len > (int)sizeof(line_buf) - 2)
            word_len = (int)sizeof(line_buf) - 2;
        if (line_len &&
            (line_len + 1 + word_len > (int)sizeof(line_buf) - 1 ||
             font_width_n(font, line_buf, line_len) +
             font_width_n(font, " ", 1) +
             font_width_n(font, word, word_len) > max_w)) {
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
                circle(g->screen, x + font_width(font, line_buf) + 2,
                       y + JOURNAL_HAND_HEIGHT - 2, 1, color);
            return 0;
        }
        memcpy(line_buf + line_len, word, (size_t)word_len);
        line_len += word_len;
        line_buf[line_len] = '\0';
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
        display_text(g, 320 - display_width(year, 42) / 2, 124, year, 36, 42,
                     PIX_PACK(236, 214, 160));
    if (elapsed > 1300u) {
        int w = display_width(chapter->title, 28);
        display_text(g, 320 - w / 2, 184, chapter->title, 22, 28,
                     PIX_PACK(214, 92, 62));
    }
    if (elapsed > 2000u) {
        int w = typewriter_width(date, 15);
        typewriter_text(g, 320 - w / 2, 230, date, 10, 15,
                        PIX_PACK(226, 206, 166));
    }
    if (elapsed > 2800u)
        typewriter_text(g, 320 - typewriter_width("ENTER", 11) / 2, 360,
                        "ENTER", 7, 11,
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
    if (g->loss_card) {
        /* The rendered room: his empty chair, the candle, his pipe. */
        SDL_BlitSurface(g->loss_card, NULL, s, NULL);
    } else {
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
    }
    for (i = 0; i < 4; ++i)
        if (elapsed > 600u + (uint32_t)i * 1100u)
            typewriter_text(g, 60, 150 + i * 26, lines[i], 7, 11,
                            i == 0 ? PIX_PACK(200, 90, 60)
                                   : PIX_PACK(220, 204, 170));
    if (elapsed > 5200u)
        typewriter_text(g, 60, 300, "GRIDPIPE THYNE, ROYAL FLYING CORPS", 7, 11,
                        PIX_PACK(160, 146, 120));
    if (elapsed > 6000u)
        typewriter_text(g, 320 - typewriter_width("ENTER", 11) / 2, 360,
                        "ENTER", 7, 11,
                        PIX_PACK(200, 180, 140));
}

/* Grey bars standing in for the columns of small print. */
/* A front page of the morning paper. */
/* ---- The morning paper ---------------------------------------------------
 *
 * Laid out like the London dailies of 1914-1918: a blackletter masthead
 * between rules with an ear each side, a dateline with the number, a
 * main head over stacked decks, then narrow justified columns divided by
 * hairline rules, a screened photograph and small notices.  The paper,
 * its bundle and string are painted (make_newspaper_table.py); the type
 * is set here.  The cable copy is stored in capitals and set in upper and
 * lower case, with the names that keep their capitals listed below. */
static const char *const news_proper[] = {
    "ABOUKIR", "ADMIRALTY", "ADRIATIC", "ALLIES", "ALLIES'", "AMERICA",
    "AMERICA'S", "AMIENS", "ANZAC", "APRIL", "ARMENIAN", "AUGUST", "AUSTRIA",
    "BALDWIN", "BALKANS", "BALUCHISTAN", "BARON", "BEATTY'S", "BEAUMONT",
    "BELGIAN", "BELGIUM", "BERLIN", "BOELCKE", "BOLSHEVIKS", "BRAZIL",
    "BRITAIN", "BRITISH", "BRUSSELS", "BRYAN'S", "BULGARIAN", "BULLECOURT",
    "CAMBRAI", "CANADIAN", "CANADIANS", "CAPT", "CAVELL", "CHRISTMAS",
    "COLOGNE", "COMMONS", "CONGRESS", "COURCELLETTE", "COURTRAI", "CRESSY",
    "CUBA", "CUNARD", "DECEMBER", "DOUGLAS", "DOWNING", "DUBLIN", "EDITH",
    "ENGLAND", "ERZBERGER", "ESSEN", "ESSEX", "EXCELLENCY", "FEBRUARY",
    "FINLAND", "FORMIDABLE", "FRANCE", "FRENCH", "GALLIPOLI", "GAZA",
    "GEORGE", "GERMAN", "GERMANS", "GERMANY", "GHENT", "GIBRALTAR", "GORIZIA",
    "HAIG", "HAIG'S", "HAMEL", "HAMPSHIRE", "HARTLEPOOL", "HINDENBURG",
    "HOGUE", "I", "IRELAND", "JAMES", "JANUARY", "JERUSALEM", "JOHN", "JULY",
    "JUNE", "KAISER", "KAISER'S", "KIEL", "KING", "KINSALE", "KITCHENER",
    "KITCHENER'S", "LLOYD", "LONDON", "LOOS", "LORD", "LUSITANIA", "MANNOCK",
    "MARY'S", "MASSEY'S", "MCCUDDEN", "MESSINES", "MONDAY", "MONS",
    "MOORHOUSE", "MOSCOW", "MR", "NGAURUHOE", "NORFOLK", "NORWAY", "NOVEMBER",
    "OCTOBER", "ORKNEY", "PARIS", "PASSCHENDAELE", "PIRONGIA", "QUENTIN",
    "REUTER", "RICHTHOFEN", "RUSSIA", "SANDRINGHAM", "SCARBOROUGH", "SCARPE",
    "SEPTEMBER", "SERVIANS", "SIBERIAN", "SIR", "SOMME", "SPAIN", "ST",
    "STATES", "SUVLA", "TATIANA", "TONGARIRO", "UNITED", "VENICE", "VERDUN",
    "VIMY", "VON", "WARNEFORD", "WARNEFORD'S", "WESTMINSTER", "WILSON",
    "WINDSOR", "ZEPPELIN", "ZEPPELINS", "ZEPPS"
};

/* Capitals to sentence case, keeping names and initials ("A.M.", "R.F.C."). */
static void news_case(const char *str, char *buf, int size)
{
    int n = 0, sentence = 1;
    while (*str && n < size - 1) {
        const char *end = str;
        int len, i, keep = 0, k, initials;
        if (*str == ' ') { buf[n++] = *str++; continue; }
        while (*end && *end != ' ') ++end;
        len = (int)(end - str);
        while (len > 1 && (str[len - 1] == '.' || str[len - 1] == ',' ||
                           str[len - 1] == ';' || str[len - 1] == ':'))
            --len;                              /* the word without its stop */
        initials = 0;
        for (i = 0; i + 1 < len; ++i)
            if (str[i] == '.' && str[i + 1] != ' ') keep = initials = 1;
        for (k = 0; !keep && k < (int)(sizeof(news_proper) /
                                       sizeof(news_proper[0])); ++k)
            if ((int)strlen(news_proper[k]) == len &&
                strncmp(news_proper[k], str, (size_t)len) == 0)
                keep = 1;
        for (i = 0; i < (int)(end - str) && n < size - 1; ++i) {
            char c = str[i];
            /* Names keep their capital letter; initials keep them all. */
            int capital = i == 0 && (sentence || keep);
            int dotted = initials && i + 1 < (int)(end - str) &&
                         str[i + 1] == '.';
            if (!capital && !dotted && c >= 'A' && c <= 'Z' &&
                !(len == 1 && str[0] == 'I'))
                c = (char)(c - 'A' + 'a');
            buf[n++] = c;
        }
        sentence = end[-1] == '.' || end[-1] == '!' || end[-1] == '?';
        str = end;
    }
    buf[n] = '\0';
}

/* Sets text in a column 'w' wide, justified, from baseline 'y'; returns
 * the baseline after the last line, or stops at 'bottom'. */
static int news_column(Game *g, FontId font, int x, int y, int w, int line_h,
                       int bottom, const char *text, uint32_t ink)
{
    const char *p = text;
    while (*p && y <= bottom) {
        const char *words[24];
        int lens[24], count = 0, width = 0, space = font_width(font, " ");
        int k, gap_total, last;
        while (*p == ' ') ++p;
        while (*p && count < 24) {
            const char *q = p;
            int wl, ww;
            while (*q && *q != ' ') ++q;
            wl = (int)(q - p);
            ww = font_width_n(font, p, wl);
            if (count && width + space + ww > w) break;
            words[count] = p; lens[count] = wl;
            width += (count ? space : 0) + ww;
            ++count;
            p = q;
            while (*p == ' ') ++p;
        }
        if (!count) break;
        last = !*p;
        gap_total = w - (width - space * (count - 1));
        {
            int cx = x;
            for (k = 0; k < count; ++k) {
                char word[48];
                int wl = lens[k] < 47 ? lens[k] : 47;
                memcpy(word, words[k], (size_t)wl);
                word[wl] = '\0';
                font_text(g, font, cx, y, word, ink);
                cx += font_width_n(font, words[k], lens[k]);
                if (k + 1 < count)
                    cx += last || count == 1 ? space :
                          gap_total / (count - 1) +
                          (k < gap_total % (count - 1) ? 1 : 0);
            }
        }
        y += line_h;
    }
    return y;
}

/* Filler for the inside columns: war news of the period kind, written for
 * the game. */
static const char *const news_filler[][2] = {
    {"CASUALTIES", "The War Office issues a further list of officers "
     "and men killed, wounded and missing. Relatives are asked not to call "
     "at the Office in person but to await the post."},
    {"FOOD PRICES", "The price of bread is unchanged this week. Sugar remains "
     "short in the northern towns, and grocers are asked to ration their "
     "customers fairly."},
    {"MUNITIONS", "Another national shell factory opened yesterday. Women "
     "now make up the greater part of its hands, and the output is said to "
     "exceed every estimate."},
    {"THE FRONT", "A correspondent with the army writes of mud to the "
     "knee and rain that has not stopped for a week. The men are cheerful and "
     "ask for socks."},
    {"AIRMEN", "Our airmen continue to be active over the "
     "enemy's lines. Several of our machines did not return. The enemy lost "
     "more."},
    {"LONDON", "The theatres are full of men on leave. A concert for "
     "wounded soldiers at the Albert Hall raised a handsome sum for the "
     "hospitals."}
};

static const char *const news_notices[][3] = {
    {"WAR SAVINGS", "Certificates 15s. 6d.", "at any Post Office."},
    {"PARCELS", "Cake, tobacco, socks", "for him: from 2s. 6d."},
    {"TRENCH COATS", "Proofed, 3 guineas.", "Outfitters, Strand."}
};

static void draw_newspaper(Game *g, int index, const char *footer)
{
    SDL_Surface *s = g->screen;
    const NewsItem *n = &news_items[index];
    pix_t ink = PIX_PACK(30, 28, 26), grey = PIX_PACK(120, 116, 108);
    char buf[200], lead[400];
    int i, y, col_w = 96, gap = 9, x0 = 56, colx[5];
    if (g->newspaper_table) SDL_BlitSurface(g->newspaper_table, NULL, s, NULL);
    else rect(s, 0, 0, SCREEN_W, SCREEN_H, PIX_PACK(232, 230, 220));
    for (i = 0; i < 5; ++i) colx[i] = x0 + i * (col_w + gap);
    /* Ears and masthead between rules. */
    rect(s, 56, 16, 74, 34, ink); rect(s, 57, 17, 72, 32, PIX_PACK(232, 230, 220));
    font_text(g, FONT_NEWS_DECK, 62, 30, "ONE", ink);
    font_text(g, FONT_NEWS_DECK, 62, 45, "PENNY", ink);
    rect(s, 510, 16, 74, 34, ink); rect(s, 511, 17, 72, 32, PIX_PACK(232, 230, 220));
    font_text(g, FONT_NEWS_SMALL, 516, 29, "WAR", ink);
    font_text(g, FONT_NEWS_SMALL, 516, 43, "EDITION", ink);
    font_text(g, FONT_MASTHEAD,
              320 - font_width(FONT_MASTHEAD, "The Western Courier") / 2, 50,
              "The Western Courier", ink);
    rect(s, 52, 56, 536, 2, ink);
    snprintf(buf, sizeof(buf), "No. %d", 3800 + index * 37);
    font_text(g, FONT_NEWS_SMALL, 56, 67, buf, ink);
    font_text(g, FONT_NEWS_SMALL, 320 - font_width(FONT_NEWS_SMALL, n->date) / 2,
              67, n->date, ink);
    font_text(g, FONT_NEWS_SMALL, 584 - font_width(FONT_NEWS_SMALL,
              "[REGISTERED AS A NEWSPAPER]"), 67,
              "[REGISTERED AS A NEWSPAPER]", ink);
    rect(s, 52, 71, 536, 1, ink);
    rect(s, 52, 73, 536, 1, ink);
    /* The main head across the page, decks beneath it. */
    i = font_width(FONT_NEWS_HEAD, n->headline);
    if (i <= 530)
        font_text(g, FONT_NEWS_HEAD, 320 - i / 2, 96, n->headline, ink);
    else
        font_text(g, FONT_NEWS_DECK, 320 - font_width(FONT_NEWS_DECK,
                  n->headline) / 2, 94, n->headline, ink);
    if (n->brief[0])
        font_text(g, FONT_NEWS_DECK, 320 - font_width(FONT_NEWS_DECK,
                  n->brief[0]) / 2, 113, n->brief[0], ink);
    if (n->brief[1]) {
        news_case(n->brief[1], buf, (int)sizeof(buf));
        font_text(g, FONT_NEWS_ITALIC, 320 - font_width(FONT_NEWS_ITALIC,
                  buf) / 2, 127, buf, ink);
    }
    rect(s, 52, 131, 536, 1, ink);
    for (i = 1; i < 5; ++i)                             /* column rules */
        if (i == 3) rect(s, colx[i] - gap / 2 - 1, 268, 1, 96, grey);
        else rect(s, colx[i] - gap / 2 - 1, 136, 1, 228, grey);
    /* Columns 1 and 2: the day's story, then a filler item. */
    lead[0] = '\0';
    for (i = 0; i < 3 && n->body[i]; ++i) {
        strncat(lead, n->body[i], sizeof(lead) - strlen(lead) - 2);
        strncat(lead, " ", sizeof(lead) - strlen(lead) - 1);
    }
    news_case(lead, buf, (int)sizeof(buf));
    y = news_column(g, FONT_NEWS_SMALL, colx[0], 146, col_w, 11, 362, buf, ink);
    y += 6;
    font_text(g, FONT_NEWS_DECK, colx[0], y, news_filler[index % 6][0], ink);
    news_column(g, FONT_NEWS_SMALL, colx[0], y + 13, col_w, 11, 362,
                news_filler[index % 6][1], ink);
    y = news_column(g, FONT_NEWS_SMALL, colx[1], 146, col_w, 11, 362,
                    news_filler[(index + 1) % 6][1], ink);
    /* Columns 3 and 4: the screened photograph (painted in), its caption,
     * and more news. */
    font_text(g, FONT_NEWS_ITALIC, 366 - font_width(FONT_NEWS_ITALIC,
              "Our airmen: a machine at the hangars.") / 2, 252,
              "Our airmen: a machine at the hangars.", ink);
    font_text(g, FONT_NEWS_DECK, colx[2], 280, news_filler[(index + 2) % 6][0], ink);
    news_column(g, FONT_NEWS_SMALL, colx[2], 293, col_w, 11, 362,
                news_filler[(index + 2) % 6][1], ink);
    font_text(g, FONT_NEWS_DECK, colx[3], 280, news_filler[(index + 3) % 6][0], ink);
    news_column(g, FONT_NEWS_SMALL, colx[3], 293, col_w, 11, 362,
                news_filler[(index + 3) % 6][1], ink);
    /* Column 5: the notices, boxed and centred, as the small display
     * advertisements were set. */
    y = 140;
    for (i = 0; i < 3; ++i) {
        const char *const *ad = news_notices[(index + i) % 3];
        int cx = colx[4] + col_w / 2, h = 54;
        rect(s, colx[4] - 2, y, col_w + 4, 1, ink);
        rect(s, colx[4] - 2, y + h, col_w + 4, 1, ink);
        rect(s, colx[4] - 2, y, 1, h, ink);
        rect(s, colx[4] + col_w + 1, y, 1, h, ink);
        font_text(g, font_width(FONT_NEWS_DECK, ad[0]) < col_w - 4 ?
                  FONT_NEWS_DECK : FONT_NEWS_SMALL,
                  cx - font_width(font_width(FONT_NEWS_DECK, ad[0]) < col_w - 4 ?
                                  FONT_NEWS_DECK : FONT_NEWS_SMALL, ad[0]) / 2,
                  y + 17, ad[0], ink);
        font_text(g, FONT_NEWS_SMALL, cx - font_width(FONT_NEWS_SMALL, ad[1]) / 2,
                  y + 32, ad[1], ink);
        font_text(g, FONT_NEWS_SMALL, cx - font_width(FONT_NEWS_SMALL, ad[2]) / 2,
                  y + 45, ad[2], ink);
        y += h + 10;
    }
    if (footer)          /* in the foot margin, clear of the string */
        typewriter_text(g, 300 - typewriter_width(footer, 11) / 2, 374, footer,
                        7, 11, PIX_PACK(128, 36, 28));
}

static void draw_scrapbook(Game *g)
{
    SDL_Surface *s = g->screen;
    char buf[80];
    int i, total = 0, place = 0;
    for (i = 0; i < NEWS_COUNT; ++i)
        if (news_unlocked(g, i)) {
            ++total;
            if (i <= g->scrap_index) ++place;
        }
    if (!total) {
        rect(s, 0, 0, SCREEN_W, SCREEN_H, PIX_PACK(30, 22, 16));
        rect(s, 140, 150, 360, 90, PIX_PACK(222, 210, 178));
        display_text(g, 320 - display_width("NO CUTTINGS YET", 18) / 2, 168,
                     "NO CUTTINGS YET", 12, 18,
                     PIX_PACK(128, 36, 28));
        typewriter_text(g, 320 - typewriter_width(
                            "FLY A SORTIE TO BEGIN THE BOOK", 11) / 2, 200,
                        "FLY A SORTIE TO BEGIN THE BOOK", 7, 11,
                        PIX_PACK(36, 30, 26));
        typewriter_text(g, 320 - typewriter_width("ENTER  BACK", 11) / 2, 220,
                        "ENTER  BACK", 7, 11,
                        PIX_PACK(36, 30, 26));
        return;
    }
    snprintf(buf, sizeof(buf), "CUTTING %d OF %d    LEFT / RIGHT  TURN    ENTER  BACK",
             place, total);
    draw_newspaper(g, g->scrap_index, buf);
}

/* ---- Before and after --------------------------------------------------
 *
 * Dawn at the hangars before every sortie: the engine is swung and
 * coughs, and a few lines of nerves are typed out.  The evening in the
 * mess after a mission won: the gramophone, toasts, and the day's
 * victories chalked on the squadron board.  All words are our own. */

static const char *const dawn_lines[CHAPTER_COUNT][3] = {
    {"MIST ON THE FIELD. THE MECHANICS SWING THE PROPELLER.",
     "TOM CHECKS THE MAP CASE TWICE. NEITHER OF US EATS.",
     "NOBODY YET KNOWS WHAT AN AEROPLANE IS FOR."},
    {"SOMEONE SAYS THE FOKKER CAN FIRE THROUGH ITS PROPELLER.",
     "I WRITE A LETTER HOME AND LEAVE IT ON MY BED.",
     "TOM'S HAND RESTS ON THE COWLING FOR A MOMENT."},
    {"THE GUNS HAVE NOT STOPPED ALL NIGHT.",
     "WE DO NOT TALK ABOUT THE EMPTY BEDS.",
     "THE RIGGER TAPS EVERY WIRE AND LISTENS TO IT SING."},
    {"THREE NEW PILOTS CAME LAST NIGHT. I DID NOT ASK THEIR NAMES.",
     "THE SQUADRON LEADER WISHES US LUCK AND DOES NOT SMILE.",
     "MY HANDS WILL NOT KEEP STILL UNTIL THE ENGINE CATCHES."},
    {"FROST ON THE WINGS. THE RIGGERS SCRAPE IT OFF BY HAND.",
     "WE HEARD THE TANKS GO UP IN THE DARK.",
     "TOM SAYS NOTHING. HE HAS SAID NOTHING SINCE THYNE."},
    {"THE LINE BROKE IN THE NIGHT. THE AERODROME IS PACKING.",
     "LORRIES ON THE ROAD, ALL OF THEM GOING THE WRONG WAY.",
     "WE TAKE OFF OVER OUR OWN TENTS."},
    {"FOR ONCE THE MAPS ARE OUT OF DATE BECAUSE WE ARE WINNING.",
     "THE BOYS IN THE MESS GET YOUNGER EVERY WEEK.",
     "I STILL CANNOT EAT BEFORE A PATROL."},
    {"THEY SAY IT WILL BE OVER ANY DAY NOW.",
     "NOBODY WANTS TO BE THE LAST.",
     "TOM SHAKES MY HAND BEFORE I CLIMB IN. HE HAS NEVER DONE THAT."}
};

static void draw_dawn(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t elapsed = SDL_GetTicks() - g->reveal_started;
    const CampaignMission *mission = &campaign_missions[g->chapter][g->mission];
    int night = mission->special == SPECIAL_AIRSHIP;
    int i;
    char buf[64];
    if (!g->dawn_backdrop) {
        SDL_Surface *b = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_W, SCREEN_H,
            CANVAS_SCREEN_BITS, s->format->format);
        if (b) {
            rect(b, 0, 0, SCREEN_W, SCREEN_H, rgb(b, 14, 18, 30));
            if (g->title) SDL_BlitSurface(g->title, NULL, b, NULL);
            /* First light: cold blue over the warm painting, darkest low
             * down where the text sits. */
            blend_rect(b, 0, 0, SCREEN_W, SCREEN_H, 18, 26, 48, 120);
            blend_rect(b, 0, 250, SCREEN_W, SCREEN_H - 250, 8, 10, 18, 170);
            g->dawn_backdrop = b;
        }
    }
    if (g->dawn_backdrop) SDL_BlitSurface(g->dawn_backdrop, NULL, s, NULL);
    else rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 14, 18, 30));
    if (night)
        blend_rect(s, 0, 0, SCREEN_W, 250, 4, 6, 16, 150);
    snprintf(buf, sizeof(buf), "%s, %s", night ? "NIGHT" : "DAWN",
             mission_date(g));
    typewriter_text(g, 40, 262, buf, 10, 15, PIX_PACK(232, 210, 160));
    for (i = 0; i < 3; ++i)
        if (elapsed > 700u + (uint32_t)i * 1500u) {
            const char *words = dawn_lines[g->chapter][i];
            /* Gridpipe Thyne is lost late in 1917; before that, Tom is
             * only quiet. */
            if (g->chapter == 4 && i == 2 && g->mission <= 8)
                words = "TOM CHECKS EVERY WIRE TWICE AND SAYS NOTHING.";
            typewriter_text(g, 40, 290 + i * 18, words,
                            7, 11, PIX_PACK(214, 200, 168));
        }
    /* The third cough catches. */
    if (elapsed > 4300u)
        display_text(g, 470, 254, "CONTACT", 16, 23, PIX_PACK(214, 92, 62));
    if (elapsed > 1200u)
        typewriter_text(g, 40, 374, "ENTER - TAKE OFF", 7, 11,
                        PIX_PACK(200, 180, 140));
}

#define MESS_LINES 4

/* How the evening goes, which picks the painted room (make_mess.py).
 * A wake when the day cost us someone, or soured us: a wingman sent to
 * the sick bay, the evening Thyne does not come back, a day we are
 * ashamed of, or a mess that has turned against us.  A party when we
 * brought machines down.  Otherwise an ordinary good evening. */
enum { MESS_QUIET, MESS_ROWDY, MESS_WAKE };

static int mess_mood(const Game *g)
{
    if (g->sortie_offences > 0 || wingman_hit(g) >= 0 ||
        thyne_lost_tonight(g) || g->morale < 30)
        return MESS_WAKE;
    return g->kills > 0 ? MESS_ROWDY : MESS_QUIET;
}

/* The squadron board in the painting: its slate, inside the frame, runs
 * from (59, 47) to (215, 153).  The chalk goes on in fives, four strokes
 * and one across, twenty to a row, three rows. */
#define MESS_SLATE_X 59
#define MESS_SLATE_Y 47

static void draw_mess(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t elapsed = SDL_GetTicks() - g->reveal_started;
    const CampaignMission *mission = &campaign_missions[g->chapter][g->mission];
    uint32_t pick = scene_hash((uint32_t)(g->chapter * 31 + g->mission) * 2654435761u);
    int mood = mess_mood(g);
    SDL_Surface *room = mood == MESS_WAKE && g->mess_wake ? g->mess_wake :
                        mood == MESS_ROWDY && g->mess_rowdy ? g->mess_rowdy :
                        g->mess;
    const char *lines[MESS_LINES];
    char first[64], board[32];
    static const char *const toasts[4] = {
        "ECCLES RAISES HIS GLASS: TO THE MACHINES THAT CAME HOME.",
        "BLUEBOTTLE STANDS ON A CHAIR AND MAKES A SPEECH NOBODY HEARS.",
        "TOM SINGS. NOBODY HAS THE HEART TO STOP HIM.",
        "THE COLONEL LOOKS IN, SAYS WELL DONE, AND LEAVES US TO IT."
    };
    /* A night with an empty chair in it: the squadron still drinks, but
     * soberly, and to him.  One toast a night, from these. */
    static const char *const wake_toasts[4] = {
        "%s STANDS AND RAISES HIS GLASS: TO ABSENT FRIENDS.",
        "%s FILLS HIS GLASS AND SETS IT AT THE EMPTY PLACE.",
        "THE COLONEL RAISES HIS GLASS TO THE BOARD. WE STAND WITH HIM.",
        "%s SINGS ONE VERSE, QUIETLY. NOBODY JOINS IN, AND NOBODY STOPS HIM."
    };
    char toast[96];
    const char *toaster = "TOM";
    static const char *const close_lines[3] = {
        "THE GRAMOPHONE PLAYS UNTIL THE NEEDLE GIVES OUT.",
        "SOMEONE OPENS THE LAST BOTTLE FROM THE CHRISTMAS HAMPER.",
        "WE SING EVERY SONG WE KNOW, AND THEN THE RUDE ONES."
    };
    int i, x;
    if (wingman_hit(g) >= 0)
        snprintf(first, sizeof(first), "%s IS IN THE SICK BAY TONIGHT.",
                 allied_pilot_names[wingman_hit(g)]);
    else if (g->kills > 0 && personal_arms(g))
        snprintf(first, sizeof(first),
                 "A MACHINE DOWN WITH A CARBINE. NOBODY BELIEVES IT.");
    else if (g->kills > 0)
        snprintf(first, sizeof(first), "%s CHALKS %d MORE ON THE BOARD.",
                 wingman_fit(g, 2) ? "BLUEBOTTLE" : "ECCLES", g->kills);
    else if (mission->special == SPECIAL_TRUCE)
        snprintf(first, sizeof(first), "A CHRISTMAS TOAST TO THE MEN ACROSS THE WIRE.");
    else if (!mission_armed(g))
        snprintf(first, sizeof(first), "TOM'S PHOTOGRAPHS GO UP ON THE WALL.");
    else
        snprintf(first, sizeof(first), "WE DRINK TO THE DAY AND DO NOT COUNT THE CHAIRS.");
    lines[0] = "THE MESS, THAT EVENING.";
    lines[1] = first;
    /* Bluebottle's speech waits until he is out of the sick bay. */
    if (mood == MESS_WAKE) {
        /* Whoever is still at the table gives it, never the man missing. */
        int k;
        for (k = 0; k < 3; ++k)
            if (k != wingman_hit(g) && wingman_fit(g, k)) {
                toaster = allied_pilot_names[k];
                break;
            }
        if ((pick & 3u) == 3u) toaster = "TOM";
        snprintf(toast, sizeof(toast), wake_toasts[pick & 3u], toaster);
        lines[2] = toast;
    } else {
        lines[2] = toasts[(pick & 3u) == 1u && !wingman_fit(g, 2) ? 0u :
                          (pick & 3u)];
    }
    /* A man sent home wounded today is kept out of tomorrow's show. */
    lines[3] = wingman_hit(g) >= 0 ? "THE DOCTOR KEEPS HIM OUT OF THE NEXT SHOW." :
               mood == MESS_WAKE ? "NOBODY SITS IN HIS CHAIR." :
                                   close_lines[(pick >> 4) % 3u];
    if (g->sortie_offences > 0) {
        /* Nobody drinks to a day like that. */
        lines[1] = "NOBODY TOASTS ANYTHING. THE GRAMOPHONE STAYS SHUT.";
        lines[2] = "TOM TAKES HIS GLASS TO THE FAR END OF THE TABLE.";
        lines[3] = "I GO TO BED EARLY. NOBODY SAYS GOOD NIGHT.";
    } else if (g->morale < 30) {
        lines[3] = "THE TALK IS SHORT AND SOUR. THEY HAVE NOT FORGOTTEN.";
    }
    if (room) {
        /* The painted room for the evening's mood; its board is left
         * clean for the chalk, and our own table across the foot of the
         * picture is kept dark for the lines. */
        SDL_BlitSurface(room, NULL, s, NULL);
    } else {
        rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 30, 20, 14));
        /* Lamplight and the gramophone horn. */
        blend_circle(s, 372, 96, 110, 140, 96, 40, 70);
        circle(s, 372, 94, 4, PIX_PACK(255, 220, 130));
        fill_triangle(s, 560, 150, 620, 110, 620, 190, PIX_PACK(150, 112, 50));
        rect(s, 540, 146, 30, 16, PIX_PACK(80, 52, 30));
        /* Bunting strung across the ceiling. */
        for (x = 0; x < SCREEN_W; x += 40) {
            uint32_t c = (x / 40) % 3 == 0 ? PIX_PACK(176, 40, 34) :
                         (x / 40) % 3 == 1 ? PIX_PACK(230, 224, 210) :
                                             PIX_PACK(40, 60, 128);
            int sag = 8 - abs((x % 160) - 80) / 10;
            fill_triangle(s, x, 14 + sag, x + 30, 14 + sag, x + 15, 38 + sag, c);
        }
        line(s, 0, 14, SCREEN_W, 14, PIX_PACK(120, 100, 70));
        /* The squadron board, and our own table at the foot. */
        rect(s, MESS_SLATE_X - 7, MESS_SLATE_Y - 7, 170, 120,
             PIX_PACK(96, 62, 34));
        rect(s, MESS_SLATE_X, MESS_SLATE_Y, 156, 106, PIX_PACK(26, 38, 30));
        rect(s, 0, 300, SCREEN_W, SCREEN_H - 300, PIX_PACK(36, 24, 16));
    }
    typewriter_text(g, MESS_SLATE_X + 9, MESS_SLATE_Y + 5, "SQUADRON BOARD",
                    7, 11, PIX_PACK(232, 228, 214));
    snprintf(board, sizeof(board), "VICTORIES %d", g->victories);
    typewriter_text(g, MESS_SLATE_X + 9, MESS_SLATE_Y + 21, board, 7, 11,
                    PIX_PACK(220, 218, 204));
    /* Every victory in chalk, today's in yellow. */
    for (i = 0; i < g->victories && i < 60; ++i) {
        int cx = MESS_SLATE_X + 12 + ((i % 20) / 5) * 38;
        int cy = MESS_SLATE_Y + 42 + (i / 20) * 18;
        int stroke = i % 5;
        int today = i >= g->victories - g->kills;
        uint32_t chalk = today && ((elapsed / 400u) & 1u) ?
                         PIX_PACK(250, 230, 120) :
                         today ? PIX_PACK(240, 214, 96) : PIX_PACK(214, 214, 204);
        if (stroke < 4) {
            line(s, cx + stroke * 7, cy, cx + stroke * 7, cy + 12, chalk);
            line(s, cx + stroke * 7 + 1, cy, cx + stroke * 7 + 1, cy + 12, chalk);
        } else {
            /* The fifth goes across the four. */
            line(s, cx - 3, cy + 11, cx + 24, cy + 1, chalk);
            line(s, cx - 2, cy + 11, cx + 25, cy + 1, chalk);
        }
    }
    for (i = 0; i < MESS_LINES; ++i)
        if (elapsed > 300u + (uint32_t)i * 1100u)
            typewriter_text(g, 40, 304 + i * 19, lines[i], 7, 11,
                            i == 0 ? PIX_PACK(214, 92, 62) : PIX_PACK(226, 210, 172));
    if (elapsed > MESS_LINES * 1100u + 600u)
        typewriter_text(g, 300, 381, "ENTER", 7, 11, PIX_PACK(200, 180, 140));
}


/* ---- New machines -------------------------------------------------------
 *
 * The first morning of a chapter whose machines differ from the last
 * chapter's: the new type stands at the aerodrome, from the aircraft park
 * for us or in the intelligence summary for theirs, with the diary's line. */
static int new_machines_today(const Game *g)
{
    if (g->mission != 0 || g->chapter == 0) return 0;
    return our_machine_index(g->chapter) != our_machine_index(g->chapter - 1) ||
           their_machine_index(g->chapter) != their_machine_index(g->chapter - 1);
}

static void draw_machine_card(Game *g, int x, int y, int enemy,
                              const MachineType *t, const char *heading)
{
    SDL_Surface *s = g->screen;
    int which = machine_atlas(enemy);
    SDL_Surface *atlas = which >= 0 && g->machine_sprites[which] ?
                         g->machine_sprites[which] :
                         enemy ? g->enemy_sprites : g->player_sprites;
    pix_t ink = PIX_PACK(36, 30, 26), red = PIX_PACK(128, 36, 28);
    char buf[48];
    if (g->machine_cards) {
        /* The painted card (make_cards.py): a record card from the park,
         * a grey form from intelligence. */
        SDL_Rect src = {enemy ? 269 : 0, 0, 268, 250};
        SDL_Rect dst = {x, y, 268, 250};
        blend_rect(s, x + 6, y + 7, 268, 250, 6, 4, 2, 150);
        SDL_BlitSurface(g->machine_cards, &src, s, &dst);
    } else {
        rect(s, x + 5, y + 5, 268, 250, PIX_PACK(14, 10, 8));
        rect(s, x, y, 268, 250, PIX_PACK(222, 210, 178));
        blend_rect(s, x, y, 268, 250, 180, 150, 90, 40);
        rect(s, x + 10, y + 26, 248, 1, ink);
    }
    typewriter_text(g, x + 12, y + 10, heading, 7, 11, red);
    if (atlas) {
        /* The level frame, two and a half times size, magenta left out. */
        int dx, dy;
        if (SDL_MUSTLOCK(atlas)) SDL_LockSurface(atlas);
        for (dy = 0; dy < 120; ++dy)
            for (dx = 0; dx < 240; ++dx) {
                uint8_t r, gr, b;
                uint32_t p = surface_pixel(atlas, 2 * 96 + dx * 2 / 5, dy * 2 / 5);
                unpack_rgb(atlas->format, p, &r, &gr, &b);
                if (r > 180 && b > 180 && gr < 130) continue;
                pixel(s, x + 14 + dx, y + 34 + dy, rgb(s, r, gr, b));
            }
        if (SDL_MUSTLOCK(atlas)) SDL_UnlockSurface(atlas);
    }
    display_text(g, x + 12, y + 160, t->name, 12, 18, ink);
    snprintf(buf, sizeof(buf), "%s  %s", t->monoplane ? "MONOPLANE" :
             t->two_seat ? "TWO-SEATER" : "SINGLE-SEAT SCOUT",
             t->guns == 0 ? "NO GUN" : t->guns == 1 ? "ONE GUN" : "TWO GUNS");
    typewriter_text(g, x + 12, y + 186, buf, 7, 11, ink);
    snprintf(buf, sizeof(buf), "SPEED %s   TURN %s",
             t->power >= 1.1f ? "FAST" : t->power >= 0.9f ? "GOOD" : "SLOW",
             t->agility >= 1.15f ? "QUICK" : t->agility >= 1.0f ? "FAIR" :
             "STEADY");
    typewriter_text(g, x + 12, y + 204, buf, 7, 11, ink);
}

static void draw_new_machines(Game *g)
{
    SDL_Surface *s = g->screen;
    const MachineType *ours = &our_machines[our_machine_index(g->chapter)];
    const MachineType *theirs = &their_machines[their_machine_index(g->chapter)];
    int ours_new = our_machine_index(g->chapter) !=
                   our_machine_index(g->chapter - 1);
    const char *line = ours_new ? ours->arrival : theirs->arrival;
    mesh_chapter = g->chapter;   /* the cards show this chapter's machines */
    rect(s, 0, 0, SCREEN_W, SCREEN_H, PIX_PACK(20, 31, 48));
    if (g->title) SDL_BlitSurface(g->title, NULL, s, NULL);
    blend_rect(s, 0, 0, SCREEN_W, SCREEN_H, 10, 8, 6, 120);
    if (ours_new)
        draw_machine_card(g, 30, 30, 0, ours, "FROM THE AIRCRAFT PARK");
    if (their_machine_index(g->chapter) != their_machine_index(g->chapter - 1))
        draw_machine_card(g, ours_new ? 342 : 186, 30, 1, theirs,
                          "INTELLIGENCE SUMMARY");
    if (line && g->machine_cards) {
        SDL_Rect src = {538, 0, 600, 70}, dst = {20, 296, 600, 70};
        blend_rect(s, 26, 303, 600, 70, 6, 4, 2, 150);
        SDL_BlitSurface(g->machine_cards, &src, s, &dst);
        journal_paragraph(g, 34, 304, line, 70, PIX_PACK(47, 37, 27));
    } else if (line) {
        blend_rect(s, 20, 296, 600, 70, 222, 210, 178, 220);
        journal_paragraph(g, 34, 304, line, 70, PIX_PACK(47, 37, 27));
    }
    typewriter_text(g, 320 - typewriter_width("ENTER", 11) / 2, 378, "ENTER",
                    7, 11, PIX_PACK(226, 206, 166));
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
    if (g->qa_diary_all) {
        /* Every page of the campaign, fully written, a second and a half
         * each, so a capture can check the whole diary in one run. */
        int n = (int)((SDL_GetTicks() - g->scene_started) / 1500u) %
                (8 * MISSIONS_PER_CHAPTER);
        g->chapter = n / MISSIONS_PER_CHAPTER;
        g->mission = n % MISSIONS_PER_CHAPTER;
        g->story_stage = 2;
        g->reveal_started = SDL_GetTicks() - 600000u;
    }
    if (g->story_stage == 0) { draw_chapter_card(g); return; }
    if (g->story_stage == 4) { draw_new_machines(g); return; }
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
    draw_journal_rules(g);
    journal_text(g, 112, journal_line_y(JOURNAL_LEFT, 0), "FLIGHT LOG", red);
    snprintf(buf, sizeof(buf), "%s NEDDY SEAGOON", rank_short[g->rank & 3]);
    journal_text(g, 112, journal_line_y(JOURNAL_LEFT, 2), buf, ink);
    journal_text(g, 112, journal_line_y(JOURNAL_LEFT, 4), corps_name(g), ink);
    journal_text(g, 112, journal_line_y(JOURNAL_LEFT, 6), mission_date(g), red);
    snprintf(buf, sizeof(buf), "CHAPTER %d / MISSION %d",
             g->chapter + 1, g->mission + 1);
    journal_text(g, 112, journal_line_y(JOURNAL_LEFT, 8), buf, ink);
    journal_paragraph(g, 112, journal_line_y(JOURNAL_LEFT, 10), chapter->title,
                      22, red);
    snprintf(buf, sizeof(buf), "VICTORIES %d", g->victories);
    journal_text(g, 112, journal_line_y(JOURNAL_LEFT, 14), buf, ink);
    if (g->offence_last_sortie)
        journal_text(g, 110, journal_line_y(JOURNAL_LEFT, 18),
                     "The Major's letter is in my pocket.", red);
    draw_victory_tally(g, 116, journal_line_y(JOURNAL_LEFT, 16) + 5,
                       g->victories, ink);
    journal_text(g, 338, journal_line_y(JOURNAL_RIGHT, 0), mission->title, red);
    journal_text(g, 338, journal_line_y(JOURNAL_RIGHT, 2),
                 mission_mode_name(mission->mode), ink);
    done = wrapped_text_limit(g, 338, journal_line_y(JOURNAL_RIGHT, 4),
                              mission->log, 25, JOURNAL_LINE_STEP,
                              revealed_letters(g, JOURNAL_LETTERS_PER_SECOND),
                              0, ink);
    if (!done) return;
    if (mission->mode == MISSION_DOGFIGHT_3D) {
        /* The whole flight fills the page as it always did.  Short of a
         * man, the page has room for who is flying on one line and who
         * is in the sick bay on the next. */
        static const char *const surname[3] = {"ECCLES", "THYNE", "BLUEBOTTLE"};
        int k, fit = 0, abed = -1, abed_count = 0;
        journal_text(g, 338, journal_line_y(JOURNAL_RIGHT, 14), "FLYING WITH:", red);
        buf[0] = '\0';
        for (k = 0; k < 3; ++k) {
            if (wingman_fit(g, k)) {
                if (fit++) strcat(buf, " / ");
                strcat(buf, surname[k]);
            } else if (g->wingman_health[k] > 0) {
                if (abed < 0) abed = k;
                ++abed_count;
            }
        }
        if (fit == 3) {
            snprintf(buf, sizeof(buf), "%s / %s", allied_pilot_names[0],
                     allied_pilot_names[2]);
            journal_text(g, 338, journal_line_y(JOURNAL_RIGHT, 16), buf, ink);
            journal_text(g, 338, journal_line_y(JOURNAL_RIGHT, 18),
                         allied_pilot_names[1], ink);
        } else {
            journal_text(g, 338, journal_line_y(JOURNAL_RIGHT, 16),
                         fit ? buf : "NOBODY. I GO ALONE.", ink);
            if (abed_count > 1)
                journal_text(g, 338, journal_line_y(JOURNAL_RIGHT, 18),
                             "THE REST IN THE SICK BAY", red);
            else if (abed >= 0) {
                snprintf(buf, sizeof(buf), "%s IN THE SICK BAY", surname[abed]);
                journal_text(g, 338, journal_line_y(JOURNAL_RIGHT, 18), buf, red);
            }
        }
    } else if (mission->mode == MISSION_GUNNER_3D) {
        journal_text(g, 338, journal_line_y(JOURNAL_RIGHT, 14), "REAR COCKPIT:", red);
        journal_text(g, 338, journal_line_y(JOURNAL_RIGHT, 16),
                     tom_wounded(g) ? "A STAND-IN AT THE CONTROLS" :
                                      "TOM AT THE CONTROLS", ink);
        journal_text(g, 338, journal_line_y(JOURNAL_RIGHT, 18),
                     mission_armed(g) ? "LEWIS GUN READY" : "A CARBINE ABOARD", ink);
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
    w = display_width("BY COMMAND OF THE KING", 18);
    display_text(g, 318 - w / 2, 72, "BY COMMAND OF THE KING", 12, 18, ink);
    typewriter_text(g, 210, 98, index >= 0 ? "IS AWARDED THE" : "IS PROMOTED",
                    7, 11, ink);
    w = display_width(title, 18);
    if (w > 420) w = 420;
    display_text(g, 318 - w / 2, 116, title, 12, 18, red);
    if (index >= 0) {
        /* The painted decoration on its ribbon (make_medals.py). */
        if (g->medals) {
            SDL_Rect src = {index * 72, 0, 72, 112}, dst = {104, 146, 72, 112};
            SDL_BlitSurface(g->medals, &src, s, &dst);
        } else {
            draw_medal(s, 140, 150, &decorations[index]);
        }
    }
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
    /* Two columns: the name, then the state of the man. */
    snprintf(buf, sizeof(buf), "%s SEAGOON", rank_short[g->rank & 3]);
    typewriter_text(g, 410, y, buf, 7, 11, ink);
    snprintf(buf, sizeof(buf), "VICT. %d", g->victories);
    typewriter_text(g, 530, y, buf, 7, 11, ink);
    /* A man still mending from the last show is WOUNDED and stays on the
     * ground; one back in the flight but not yet himself is BRUISED. */
    for (i = 0; i < 3; ++i) {
        uint32_t c = wingman_fit(g, i) ? ink : red;
        y += 15;
        typewriter_text(g, 410, y, allied_pilot_names[i], 7, 11, c);
        typewriter_text(g, 530, y, g->wingman_health[i] <= 0 ? "LOST" :
                        wingman_sick(g, i) ? "SICK" :
                        !wingman_fit(g, i) ? "WOUNDED" :
                        g->wingman_health[i] < 75 ? "BRUISED" : "FIT", 7, 11, c);
    }
    y += 15;
    typewriter_text(g, 410, y, "TOM", 7, 11, ink);
    typewriter_text(g, 530, y, "OBSERVER", 7, 11, ink);
    y += 15;
    snprintf(buf, sizeof(buf), "FLY %d  GUN %d  ENG %d",
             (int)(g->skill_flying * 10.0f), (int)(g->skill_gunnery * 10.0f),
             (int)(g->skill_engine * 10.0f));
    typewriter_text(g, 410, y, buf, 7, 11, ink);
    y += 15;
    typewriter_text(g, 410, y, "MORALE", 7, 11, ink);
    typewriter_text(g, 530, y, g->morale >= 65 ? "GOOD" : g->morale >= 45 ?
                    "FAIR" : g->morale >= 25 ? "LOW" : "SULLEN", 7, 11,
                    g->morale >= 45 ? ink : red);
    if (g->offences > 0) {
        y += 15;
        snprintf(buf, sizeof(buf), "REPRIMANDS %d", g->offences);
        typewriter_text(g, 410, y, buf, 7, 11, red);
    }
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
    } else if (mission->mode == MISSION_TOP_DOWN &&
               topdown_mix_for(g->chapter, g->mission) != MIX_NONE) {
        const TopdownMix *mix =
            &topdown_mixes[topdown_mix_for(g->chapter, g->mission)];
        order0 = mix->orders[0] ? mix->orders[0] : chapter->orders[0];
        order1 = mix->orders[1] ? mix->orders[1] : chapter->orders[1];
        order2 = mix->orders[2] ? mix->orders[2] : chapter->orders[2];
    } else if (mission->mode == MISSION_TOP_DOWN) {
        order0 = "FOLLOW THE MARKED MAP GRID.";
        order1 = "OBSERVE OR BOMB THE ASSIGNED SECTOR.";
        order2 = "RETURN ALONE WITH YOUR REPORT.";
    } else if (mission->mode == MISSION_DOGFIGHT_3D && g->chapter == 0) {
        order0 = "MARK THE COLUMN: FLY LOW OVER IT. B DROPS";
        order1 = "THE BAG ON H.Q. HOLD LEVEL OVER THE BATTERY,";
        order2 = "AND S SIGNALS OUR GUNS' FALL OF SHOT.";
    } else if (mission->mode == MISSION_DOGFIGHT_3D) {
        order0 = "RENDEZVOUS WITH THE ALLIED FLIGHT.";
        order1 = "ENGAGE THE ENEMY SQUADRON.";
        order2 = "BRING YOUR WINGMEN HOME.";
    } else if (mission->mode == MISSION_GUNNER_3D) {
        order0 = tom_wounded(g) ? "TOM'S STAND-IN WILL HOLD THE COURSE." :
                                  "TOM WILL HOLD THE RECONNAISSANCE COURSE.";
        order1 = mission_armed(g) ? "DEFEND THE REAR QUARTER WITH THE LEWIS GUN." :
                                    "WATCH THE REAR QUARTER. YOU HAVE A CARBINE.";
        order2 = "KEEP THE TWO-SEATER IN THE AIR.";
    } else if (side_village_today(g)) {
        static char warning[64];
        snprintf(warning, sizeof(warning), "%s IS STILL OCCUPIED: TAKE CARE.",
                 side_village_names[g->chapter]);
        order0 = chapter->orders[0];
        order1 = chapter->orders[1];
        order2 = warning;
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
        typewriter_text(g, 73, 237,
                        mission->mode == MISSION_DOGFIGHT_3D ?
                        "SPACE       REVOLVER, SIX ROUNDS" :
                        "SPACE       CARBINE, WORK THE BOLT", 7, 11, ink);
        typewriter_text(g, 73, 253, "ARMAMENT: RIFLE AND REVOLVER", 7, 11, red);
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
    int flash = sim_now(g) - target->last_shot < 180;
    if (sprite) {
        SDL_Rect dst = {sx - sprite->w / 2, ty + 12 - sprite->h, 0, 0};
        SDL_BlitSurface(sprite, NULL, s, &dst);
        if (flash) {
            /* The muzzle flash as a glow round a white-hot core. */
            int fx = target->type ? sx + 20 : sx + 49;
            int fy = target->type ? ty - 57 : ty - 24;
            blend_circle(s, fx, fy, target->type ? 16 : 11, 255, 170, 60, 230);
            blend_circle(s, fx, fy, target->type ? 6 : 4, 255, 246, 200, 255);
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

/* Small clouds over the side-on sky: a few soft puffs each, lit from
 * above and blue-grey underneath like the painted ones round them. */
static void draw_sky_detail(Game *g)
{
    SDL_Surface *s = g->screen;
    int camera = (int)(g->scroll * 0.075f);
    int first = camera / 190 - 2;
    int i, k;
    for (i = 0; i < 7; ++i) {
        int segment = first + i;
        uint32_t seed = scene_hash((uint32_t)segment * 2654435761u ^
                                   (uint32_t)(g->chapter + 3) * 3266489917u);
        int x = segment * 190 - camera + (int)(seed % 105u) - 46;
        int y = 34 + (int)((seed >> 8) % 112u);
        int width = 64 + (int)((seed >> 17) % 88u);
        for (k = 0; k < 3; ++k) {
            uint32_t h = scene_hash(seed + (uint32_t)k * 7919u);
            int w = width / 2 + (int)(h % (uint32_t)(width / 2 + 1));
            draw_puff_scaled(s, (int)((h >> 6) % 3u), (int)((h >> 9) & 1u),
                             x + k * width / 3 + (int)((h >> 12) % 17u) - 8,
                             y + (int)((h >> 18) % 9u) - 4, w, w * 2 / 5,
                             PIX_PACK(232, 224, 220), PIX_PACK(126, 118, 140),
                             12 + (int)((h >> 24) % 6u));
        }
    }
}

/* ---- Soft cloud puffs ----------------------------------------------------
 *
 * A handful of alpha masks, made once at start-up: soft puffs of cloud at
 * three sizes and two shapes each, from the cloud noise table with a
 * radial fall-off, so the outline is rounded and ragged and the edge
 * feathers away to nothing.  Drawing one is a look-up and a blend per
 * pixel over the puff's own small area, skipping the clear corners. */

enum { PUFF_SIZES = 3, PUFF_SHAPES = 2 };
static const unsigned char puff_dims[PUFF_SIZES][2] = {{36, 22}, {56, 34}, {84, 50}};
static uint8_t *puff_mask[PUFF_SIZES][PUFF_SHAPES];

static void make_puff_masks(void)
{
    static int ready;
    int size, shape, x, y;
    if (ready) return;
    ready = 1;
    make_cloud_noise();
    for (size = 0; size < PUFF_SIZES; ++size)
        for (shape = 0; shape < PUFF_SHAPES; ++shape) {
            int w = puff_dims[size][0], h = puff_dims[size][1];
            uint8_t *m = (uint8_t *)malloc((size_t)(w * h));
            puff_mask[size][shape] = m;
            if (!m) continue;
            for (y = 0; y < h; ++y)
                for (x = 0; x < w; ++x) {
                    float dx = ((float)x + 0.5f - (float)w * 0.5f) / ((float)w * 0.5f);
                    float dy = ((float)y + 0.5f - (float)h * 0.5f) / ((float)h * 0.5f);
                    /* The noise, sampled at a scale to suit the puff and at
                     * a different place for each shape, pushes the edge in
                     * and out and thins the middle in places. */
                    int nx = (x * 24 / w + shape * 11 + size * 5) & 31;
                    int ny = (y * 16 / h + shape * 7 + size * 3) & 31;
                    float n = (float)cloud_noise[ny][nx] / 255.0f;
                    float r = sqrtf(dx * dx + dy * dy) * (0.72f + n * 0.5f);
                    float a = (1.0f - r) * 2.2f;
                    if (a < 0.0f) a = 0.0f;
                    if (a > 1.0f) a = 1.0f;
                    a = a * a * (3.0f - 2.0f * a) * (0.75f + n * 0.25f);
                    m[y * w + x] = (uint8_t)(a * 32.0f + 0.5f);
                }
        }
}

/* Blends a puff over the screen at (x, y), its top left, toward the
 * colour 'top' at its upper edge and 'bottom' at its lower; opacity is
 * out of 32. */
static void draw_puff(SDL_Surface *s, int size, int shape, int x, int y,
                      pix_t top, pix_t bottom, int opacity)
{
    const uint8_t *m = puff_mask[size][shape];
    int w = puff_dims[size][0], h = puff_dims[size][1];
    int row, col, c0 = x < 0 ? -x : 0, c1 = x + w > s->w ? s->w - x : w;
    if (!m || c0 >= c1) return;
    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
    for (row = 0; row < h; ++row) {
        const uint8_t *mr;
        pix_t *out, colour;
        if (y + row < 0 || y + row >= s->h) continue;
        mr = m + row * w;
        out = PIX_ROW(s, y + row) + x;
        colour = pix_mix(top, bottom, row * 32 / h);
        for (col = c0; col < c1; ++col) {
            int t = mr[col];
            if (!t) continue;
            t = (t * opacity) >> 5;
            if (t) out[col] = pix_mix(out[col], colour, t);
        }
    }
    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
}

/* A puff drawn at any size, centred on (cx, cy), w by h: the mask is
 * stepped through to fit.  Large puffs are blended in 2x2 blocks, which
 * the eye does not see in soft cloud and which quarters the cost. */
static void draw_puff_scaled(SDL_Surface *s, int size, int shape, int cx,
                             int cy, int w, int h, pix_t top, pix_t bottom,
                             int opacity)
{
    const uint8_t *m;
    int mw = puff_dims[size][0], mh = puff_dims[size][1];
    int step = w > 70 ? 2 : 1, x0 = cx - w / 2, y0 = cy - h / 2, x, y;
    make_puff_masks();
    m = puff_mask[size][shape];
    if (!m || w < 2 || h < 2) return;
    if (x0 + w < 0 || x0 >= s->w || y0 + h < 0 || y0 >= s->h) return;
    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
    for (y = 0; y < h; y += step) {
        int sy = y0 + y, my = y * mh / h, k;
        const uint8_t *mr = m + my * mw;
        pix_t colour = pix_mix(top, bottom, y * 32 / h);
        if (sy < 0 || sy >= s->h) continue;
        for (x = 0; x < w; x += step) {
            int sx = x0 + x, t = mr[x * mw / w];
            if (!t || sx < 0 || sx >= s->w) continue;
            t = (t * opacity) >> 5;
            if (!t) continue;
            for (k = 0; k < step * step; ++k) {
                int px = sx + (k & 1), py = sy + (k >> 1);
                pix_t *row;
                if (px >= s->w || py >= s->h) continue;
                row = PIX_ROW(s, py);
                row[px] = pix_mix(row[px], colour, t);
            }
        }
    }
    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
}

/* Low cloud seen from above: a few puffs of grey-white each, of varied
 * size and shape, lit from a sun in the upper right, drifting a little
 * faster than the ground beneath because they are nearer to us, and each
 * casting its soft shadow down and to the left. */
static void draw_top_down_clouds(Game *g)
{
    SDL_Surface *s = g->screen;
    int pass, i, k;
    make_puff_masks();
    for (pass = 0; pass < 2; ++pass)
    for (i = 0; i < 5; ++i) {
        uint32_t seed = scene_hash((uint32_t)(i + 1) * 2654435761u ^
                                   (uint32_t)(g->chapter + 1) * 2246822519u ^
                                   (uint32_t)(g->mission + 7) * 3266489917u);
        float travel = (float)(seed % 900u) + g->scroll *
                       (1.08f + (float)((seed >> 10) & 15u) * 0.006f);
        int y = (int)fmodf(travel, 900.0f) - 220;
        int x = (int)((seed >> 12) % 640u) +
                (int)(g->scroll * (float)((int)((seed >> 3) & 7u) - 3) * 0.004f);
        int puffs = 3 + (int)((seed >> 27) % 3u);
        x = ((x % 760) + 760) % 760 - 60;
        if (y < -120 || y > SCREEN_H + 120) continue;
        for (k = 0; k < puffs; ++k) {
            uint32_t h = scene_hash(seed + (uint32_t)k * 7919u);
            int size = k == 0 ? 2 : (int)(h % 3u);
            int shape = (int)((h >> 4) & 1u);
            int px = x + (k - puffs / 2) * 34 + (int)((h >> 8) % 31u) - 15 -
                     puff_dims[size][0] / 2;
            int py = y + (int)((h >> 16) % 40u) - 20 - puff_dims[size][1] / 2;
            if (pass == 0)
                draw_puff(s, size, shape, px - 30, py + 36,
                          PIX_PACK(28, 30, 24), PIX_PACK(28, 30, 24), 11);
            else
                draw_puff(s, size, shape, px, py, PIX_PACK(244, 244, 240),
                          PIX_PACK(172, 176, 184), 18 + (int)((h >> 24) % 6u));
        }
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
    /* Four by four frames; the atlases are cut at 160 pixels a cell
     * (make_explosion_atlases.py) so big bursts are barely scaled up. */
    const int cell = atlas ? atlas->w / 4 : 96;
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
        int radius = 6 + (int)(phase * (health < 42 ? 16.0f : 12.0f));
        /* Soft puffs that thin as they age: a dark body of smoke, its
         * crown lit by the sky, the oldest drifting off almost clear. */
        int alpha = (int)((1.0f - phase * 0.8f) * (health < 42 ? 255.0f : 215.0f));
        uint8_t grey = (uint8_t)(34 + (int)(phase * 66.0f));
        blend_circle(s, px, py, radius, grey, grey, (uint8_t)(grey - 4),
                     (uint8_t)alpha);
        blend_circle(s, px - trail_direction * radius / 4, py - radius / 3,
                     radius * 2 / 3, (uint8_t)(grey + 40), (uint8_t)(grey + 38),
                     (uint8_t)(grey + 32), (uint8_t)(alpha * 3 / 5));
    }
}

static void draw_player_damage(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t now = sim_now(g);
    if (g->health >= 80) return;
    draw_aircraft_smoke(g, (int)g->player_x, (int)g->player_y,
                        g->health, -1, 0x4e454444u);
    if (g->health < 42) {
        /* Fire licking back from the engine: a soft glow, a hot core. */
        int flicker = (now / 45) & 3;
        blend_circle(s, (int)g->player_x - 28, (int)g->player_y + 2,
                     12 + flicker, 237, 89, 24, 220);
        blend_circle(s, (int)g->player_x - 26, (int)g->player_y,
                     6 + (flicker & 1), 255, 226, 120, 255);
    }
}

static void draw_field_sign(Game *g, int x, int y, int w, const char *label)
{
    SDL_Surface *s = g->screen;
    int text_width = font_width(FONT_STENCIL, label);
    int text_x = x + (w - text_width) / 2;
    uint32_t wood = rgb(s, 54, 38, 27);
    uint32_t brass = rgb(s, 180, 137, 63);
    rect(s, x + 2, y + 3, w, 25, rgb(s, 25, 25, 24));
    rect(s, x, y, w, 25, brass);
    rect(s, x + 2, y + 2, w - 4, 21, wood);
    circle(s, x + 6, y + 6, 2, brass);
    circle(s, x + w - 7, y + 6, 2, brass);
    if (text_x < x + 4) text_x = x + 4;
    /* Stencilled, like the squadron's crates and petrol tins. */
    font_text(g, FONT_STENCIL, text_x, y + 17, label, rgb(s, 239, 215, 155));
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

/* A small atlas sprite set straight down, with no shadow of its own. */
static void td_blit_plain(Game *g, int sprite, int cx, int cy)
{
    const short *r = topdown_sprite_rect[sprite];
    SDL_Rect src = {r[0], r[1], r[2], r[3]};
    SDL_Rect dst = {cx - r[2] / 2, cy - r[3] / 2, r[2], r[3]};
    if (g->topdown_targets) SDL_BlitSurface(g->topdown_targets, &src, g->screen, &dst);
}

static void draw_top_target(Game *g, int x, int y, Target *target)
{
    SDL_Surface *s = g->screen;
    uint32_t earth = rgb(s, 74, 58, 39);
    uint32_t iron = rgb(s, 47, 48, 42);
    uint32_t sand = rgb(s, 139, 112, 73);
    if (g->topdown_targets) {
        /* The painted sprites (make_topdown_targets.py). */
        if (target->type == 3) {
            /* Christmas in no man's land, from the air: men of both armies,
             * khaki and field grey, round a brazier with its smoke going
             * up, and a few more kicking a ball about beside them. */
            float t = (float)sim_now(g) * 0.0016f + (float)x;
            uint32_t tick = sim_now(g) / 110u;
            int k;
            for (k = 0; k < 3; ++k) {
                int rise = (int)((tick + (uint32_t)(k * 9)) % 27u);
                blend_circle(s, x + rise / 3 + 2, y - rise, 3 + rise / 4,
                             214, 212, 206, (uint8_t)(200 - rise * 6));
            }
            for (k = 0; k < 16; ++k) {
                uint32_t h = scene_hash((uint32_t)(k * 7919 + x));
                int ours = k & 1, mx, my;
                pix_t coat = ours ? PIX_PACK(150, 130, 84) : PIX_PACK(108, 116, 106);
                if (k < 9) {
                    /* round the fire */
                    float a = (float)k * 0.70f + (float)(h % 7u) * 0.05f;
                    mx = x + (int)(cosf(a) * 13.0f);
                    my = y + (int)(sinf(a) * 9.0f);
                } else {
                    /* the football, off to one side */
                    mx = x + 34 + (int)(h % 30u) - 15 +
                         (int)(sinf(t * 0.8f + (float)k) * 4.0f);
                    my = y + (int)((h >> 8) % 22u) - 11 +
                         (int)(cosf(t * 0.6f + (float)k) * 3.0f);
                }
                blend_rect(s, mx, my + 1, 5, 3, 24, 22, 18, 110);     /* shadow */
                rect(s, mx - 2, my - 2, 5, 5, coat);
                rect(s, mx - 1, my - 1, 3, 3, PIX_PACK(206, 172, 140)); /* head */
            }
            blend_circle(s, x, y, 5, 255, 150, 50, 220);               /* fire */
            rect(s, x - 1, y - 1, 2, 2, PIX_PACK(255, 236, 160));
            circle(s, x + 34 + (int)(sinf(t) * 14.0f),
                   y + (int)(cosf(t * 1.3f) * 7.0f), 1,
                   PIX_PACK(240, 236, 224));                             /* ball */
            return;
        }
        if (target->type == 4) {
            td_blit(g, TD_AERODROME, x, y);
            if (g->pudding_delivered) {
                blend_circle(s, x, y + 2, 8, 70, 40, 24, 255);
                blend_circle(s, x, y - 1, 4, 236, 232, 220, 255);
                circle(s, x + 1, y - 3, 1, rgb(s, 170, 30, 30));
            }
            return;
        }
        td_blit(g, target->type == 2 ? TD_CONVOY :
                   target->type == 1 ? TD_AA_PIT : TD_MG, x, y);
        return;
    }
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

/* Adds 'weight' (0..256) of colour c at a sub-pixel position, shared
 * between the four pixels round it. */
static void splat(SDL_Surface *s, float x, float y, pix_t c, int weight)
{
    int ix = (int)floorf(x), iy = (int)floorf(y);
    int fx = (int)((x - (float)ix) * 16.0f), fy = (int)((y - (float)iy) * 16.0f);
    int w[4], i;
    w[0] = (16 - fx) * (16 - fy);
    w[1] = fx * (16 - fy);
    w[2] = (16 - fx) * fy;
    w[3] = fx * fy;
    for (i = 0; i < 4; ++i) {
        int px = ix + (i & 1), py = iy + (i >> 1);
        int t = (w[i] * weight) >> 11;          /* 0..32 */
        pix_t *row;
        if (t <= 0 || px < 0 || py < 0 || px >= s->w || py >= s->h) continue;
        if (t > 32) t = 32;
        row = PIX_ROW(s, py);
        row[px] = pix_mix(row[px], c, t);
    }
}

/* A tracer seen from above: a bright head and a tail that fades out
 * behind it, two pixels wide, pointing exactly along its velocity. */
static void draw_tracer(SDL_Surface *s, float x, float y, float vx, float vy,
                        uint8_t r, uint8_t g, uint8_t b)
{
    float speed = sqrtf(vx * vx + vy * vy);
    float dx, dy, length, step;
    pix_t core = PIX_PACK(255, 252, 230), colour = PIX_PACK(r, g, b);
    int n, i;
    if (speed < 0.01f) return;
    dx = vx / speed;
    dy = vy / speed;
    length = speed * 0.035f;
    if (length < 9.0f) length = 9.0f;
    if (length > 18.0f) length = 18.0f;
    n = (int)(length * 2.0f);
    step = length / (float)n;
    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
    /* A dark edge first, so the streak reads on pale mud and dark grass. */
    for (i = 0; i <= n; i += 2) {
        float along = (float)i * step;
        float px = x - dx * along, py = y - dy * along;
        splat(s, px - dy * 1.5f, py + dx * 1.5f, PIX_PACK(24, 20, 14), 150);
        splat(s, px + dy * 1.5f, py - dx * 1.5f, PIX_PACK(24, 20, 14), 150);
    }
    for (i = 0; i <= n; ++i) {
        float along = (float)i * step;
        float px = x - dx * along, py = y - dy * along;
        int fade = 256 - i * 220 / n;
        splat(s, px - dy * 0.5f, py + dx * 0.5f, colour, fade);
        splat(s, px + dy * 0.5f, py - dx * 0.5f, colour, fade);
        if (i < n / 3) splat(s, px, py, core, 256);
    }
    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
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
    td_draw_ground(g);
    draw_field_2d(g);
    td_draw_objects(g);
    draw_riflemen(g);
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
    if (g->special == SPECIAL_TRUCE) {
        /* A few flakes of Christmas snow drifting across, below us. */
        uint32_t t = sim_now(g);
        int k;
        for (k = 0; k < 48; ++k) {
            uint32_t h = scene_hash((uint32_t)k * 2654435761u + 0x534e4fu);
            int fx = (int)((h % 680u) + t / (40u + (h >> 28)) % 680u) % 680 - 20;
            int fy = (int)(((h >> 10) % 420u) + t / 25u) % 420 - 10;
            rect(s, fx, fy, 2, 2, PIX_PACK(240, 242, 246));
        }
    }
    draw_balloon(g);
    for (i = 0; i < MAX_BULLETS; ++i) if (g->bullets[i].active) {
        Bullet *b = &g->bullets[i];
        if (b->type == 4) {
            /* The message bag falling, its streamer flying out behind. */
            float fall = 1.0f - b->life / b->initial_life;
            rect(s, (int)b->x - 2, (int)b->y - 2, 5, 5, PIX_PACK(120, 86, 50));
            line(s, (int)b->x, (int)b->y, (int)b->x + 6 - (int)(fall * 4.0f),
                 (int)b->y - 14, PIX_PACK(236, 230, 214));
        } else if (b->type == 1) {
            float fall = 1.0f - b->life / b->initial_life;
            int shadow_y = (int)b->y + 28 - (int)(fall * 22.0f);
            int body = 3 + (int)(fall * 4.0f);
            blend_circle(s, (int)b->x + 8, shadow_y, 6 + body,
                         28, 30, 27, 65);
            circle(s, (int)b->x, (int)b->y, body, rgb(s, 48, 43, 35));
            circle(s, (int)b->x - 1, (int)b->y - body / 2,
                   body / 2, rgb(s, 178, 145, 79));
        } else {
            /* A round is a tracer streak along its own line of flight, at
             * any angle, drawn with sub-pixel weights so it glides rather
             * than steps.  Its length is about one frame's travel. */
            if (b->enemy)
                draw_tracer(s, b->x, b->y, b->vx, b->vy, 255, 120, 60);
            else
                draw_tracer(s, b->x, b->y, b->vx, b->vy, 255, 236, 160);
        }
    }
    draw_top_scouts(g);
    draw_archie_2d(g);
    draw_explosions(g);
    if (g->health < 80)
        draw_aircraft_smoke(g, (int)g->player_x, (int)g->player_y,
                            g->health, 1, 0x544f5044u);
    {
        int t = our_machine_index(g->chapter);
        SDL_Surface *plane = t == 0 && g->topdown_machines[0] ?
                             g->topdown_machines[0] :
                             t == 3 && g->topdown_machines[1] ?
                             g->topdown_machines[1] : g->topdown_player;
        SDL_Rect dst;
        if (plane) {
            dst.x = (int)g->player_x - plane->w / 2;
            dst.y = (int)g->player_y - plane->h / 2;
            dst.w = dst.h = 0;
            SDL_BlitSurface(plane, NULL, s, &dst);
        } else {
            draw_top_plane(s, (int)g->player_x, (int)g->player_y, 0);
        }
        draw_fabric_holes(g, (int)g->player_x, (int)g->player_y);
    }
    draw_squall_2d(g);
    if (g->shake > 0.0f) {
        uint32_t t = sim_now(g);
        shift_frame(s, (int)(sinf((float)t * 0.093f) * g->shake * 6.0f),
                    (int)(cosf((float)t * 0.077f) * g->shake * 5.0f));
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
        if (g->leg != LEG_OUT)
            snprintf(buf, sizeof(buf), g->leg == LEG_HOME ? "HOMEWARD" : "LAND");
        else
            snprintf(buf, sizeof(buf), g->bags > 0 || g->td_mix >= MIX_FIRST_MESSAGE ?
                     "TASKS %d/%d" : "TARGETS %d/%d", g->objectives,
                     current_mission_goal(g));
        draw_field_sign(g, 8, 39, 112, buf);
        if (mission_armed(g))
            snprintf(buf, sizeof(buf), "AMMO %03d", g->ammunition);
        else
            snprintf(buf, sizeof(buf), personal_arms(g) ? "CARBINE" : "UNARMED");
        draw_field_sign(g, 264, 8, 108, buf);
        if (g->bags > 0)
            snprintf(buf, sizeof(buf), "BAGS %d", g->bags);
        else
            snprintf(buf, sizeof(buf), "BOMBS %02d", g->bombs_remaining);
        draw_field_sign(g, 264, 39, 108, buf);
    }
    snprintf(buf, sizeof(buf), "FUEL %02d", (int)g->fuel);
    draw_field_sign(g, 526, 8, 106, buf);
    snprintf(buf, sizeof(buf), "AIRFRAME %03d", g->health > 0 ? g->health : 0);
    draw_field_sign(g, 502, 39, 130, buf);
    if (g->notice_until && sim_now(g) < g->notice_until)
        draw_field_sign(g, 200, 140, 240, g->notice);
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
/* The ground's average colour, for the haze of the far fields in the
 * cockpit view.  Measured once at start-up from the top-down painting. */
static void measure_ground_tint(Game *g)
{
    SDL_Surface *ground = SDL_LoadBMP(CANVAS_TOPDOWN_BATTLEFIELD_BMP);
    if (ground && convert_to_screen(g, &ground) && ground->w >= 64)
        g->ground_haze_tint = average_pixels(ground, 0, 0, ground->w,
                                             ground->h);
    if (ground) SDL_FreeSurface(ground);
}

/* The top-down painting does not tile from its foot back to its head:
 * the long sorties scroll past the join, where fields and craters were cut
 * off along a straight line.  Its last rows are faded into its first, so
 * it wraps without a seam. */
static void blend_vertical_seam(SDL_Surface *s)
{
    const int band = 128;
    int y, x;
    if (!s || s->h < band * 4 || s->format->BytesPerPixel != sizeof(pix_t))
        return;
    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
    /* The foot fades into the head seen in a mirror, so that its last row
     * is the first row again and the wrap carries straight on. */
    for (y = 0; y < band; ++y) {
        pix_t *row = PIX_ROW(s, s->h - band + y);
        const pix_t *head = PIX_ROW(s, band - 1 - y);
        int weight = (y + 1) * 32 / band;
        for (x = 0; x < s->w; ++x)
            row[x] = pix_mix(row[x], head[x], weight);
    }
    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
}

/* The cockpit view's ground: the top-down painting and its mipmaps, so
 * the fields far off are filtered rather than sparkling. */
static void build_ground_mips(Game *g, SDL_Surface *src)
{
    int level, size = 1024;
    if (!src || src->w != 1024 || src->h != 1024 || g->ground_mips) return;
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
}

static void free_ground_mips(Game *g)
{
    int i;
    for (i = 0; i < GROUND_MIPS; ++i) {
        free(g->ground_mip[i]);
        g->ground_mip[i] = NULL;
    }
    g->ground_mips = 0;
}

/* The sky panorama's ends are blended so it wraps without a seam, and its
 * top and horizon colours are taken for the haze. */
static void prepare_sky(Game *g)
{
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
    } else if (!g->sky_top) {
        g->sky_top = PIX_PACK(92, 104, 140);
        g->sky_haze = PIX_PACK(214, 150, 128);
    }
    /* Distant fields take on the colour of the low sky, so the ground
     * melts into the painted horizon instead of meeting it at a hard line. */
    g->world_haze = pix_mix(g->sky_haze, g->ground_haze_tint, 9);
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
        /* A crossing off either end of the row means the whole row is
         * what its left end is.  (Clamping a crossing left of the screen
         * to x = 0 used to paint the far ground's haze across the sky,
         * or the sky across the ground, whenever the aircraft banked.) */
        if (fabsf(across.d) > 1e-9f) {
            split = -base.d / across.d;
            if (split <= 0.0f || split >= (float)SCREEN_W) {
                xh = SCREEN_W;
            } else {
                xh = (int)split;
                if (step == 2) xh &= ~1;
            }
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
     * quaternion projection as the fields rather than sliding on the screen.
     * The ring's bearings and heights never change: they are worked out
     * once, and each ring point is projected once a frame and shared by
     * the two segments that meet there. */
    {
        enum { RIDGE_SEGMENTS = 64 };
        static float ring_sin[RIDGE_SEGMENTS], ring_cos[RIDGE_SEGMENTS];
        static float ring_height[2][RIDGE_SEGMENTS];
        static int ring_ready;
        Projected3D base[RIDGE_SEGMENTS], top[RIDGE_SEGMENTS];
        int segment;
        if (!ring_ready) {
            for (segment = 0; segment < RIDGE_SEGMENTS; ++segment) {
                float a = (float)segment *
                          (2.0f * (float)M_PI / RIDGE_SEGMENTS);
                ring_sin[segment] = sinf(a);
                ring_cos[segment] = cosf(a);
                for (pass = 0; pass < 2; ++pass) {
                    uint32_t seed = scene_hash((uint32_t)segment * 92821u +
                                               (uint32_t)pass * 0x48494c4cu);
                    ring_height[pass][segment] =
                        (pass == 0 ? 430.0f : 260.0f) +
                        (float)(seed % (pass == 0 ? 690u : 430u));
                }
            }
            ring_ready = 1;
        }
        for (pass = 0; pass < 2; ++pass) {
            float distance = pass == 0 ? 14200.0f : 9400.0f;
            uint32_t hill = pass == 0 ?
                pix_mix(PIX_PACK(64, 70, 66), g->world_haze, 21) :
                pix_mix(PIX_PACK(58, 66, 46), g->world_haze, 15);
            uint32_t crest = pix_mix(PIX_PACK(96, 98, 74), g->world_haze, 14);
            for (segment = 0; segment < RIDGE_SEGMENTS; ++segment) {
                Vec3 w = {g->flight_x + ring_sin[segment] * distance, 0.0f,
                          g->flight_z + ring_cos[segment] * distance};
                base[segment] = project_world_point(g, w);
                w.y = ring_height[pass][segment];
                top[segment] = project_world_point(g, w);
            }
            for (segment = 0; segment < RIDGE_SEGMENTS; ++segment) {
                int next = (segment + 1) % RIDGE_SEGMENTS;
                Projected3D b0 = base[segment], b1 = base[next];
                Projected3D t0 = top[segment], t1 = top[next];
                if (b0.visible && b1.visible && t0.visible && t1.visible &&
                    abs(b0.x) < 1500 && abs(b1.x) < 1500) {
                    fill_triangle(s, b0.x, b0.y, b1.x, b1.y,
                                  t1.x, t1.y, hill);
                    fill_triangle(s, b0.x, b0.y, t1.x, t1.y,
                                  t0.x, t0.y, hill);
                    if (pass == 1)
                        line(s, t0.x, t0.y, t1.x, t1.y, crest);
                }
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
        /* No hard dark base: a flat purple underside under a row of
         * puffs read as a flying saucer against the painted sky.  The
         * shadowed side is only a little deeper than the lit one. */
        if (cloud_type == 1) {
            ellipse(s, p.x - radius / 3, p.y - radius / 10,
                    radius * 7 / 5, radius / 4 + 1,
                    rgb(s, 222, 168, 150));
        } else {
            ellipse(s, p.x - radius / 8, p.y,
                    cloud_type == 2 ? radius * 3 / 4 : radius,
                    radius / 3 + 1, rgb(s, 206, 154, 146));
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
            ellipse(s, px, py + pry / 4, pr, pry * 4 / 5,
                    rgb(s, 176, 132, 140));
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
    age = (float)(sim_now(g) - g->flight_crash_started) / 1000.0f;
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
/* Every decal panel has its own four corners: the rib tapes alone take
 * 176, so the limit must leave room for the crosses and struts after. */
#define MESH_MAX_VERTS 320
#define MESH_MAX_FACES 96
#define FACE_TWO_SIDED 1
#define FACE_OUTLINE 2

enum {
    MAT_WING_TOP, MAT_WING_UNDER, MAT_BODY, MAT_BODY_DARK, MAT_TAIL,
    MAT_METAL, MAT_MARK_LIGHT, MAT_MARK_DARK, MAT_MARK_RED, MAT_MARK_BLUE,
    MAT_RIB,                    /* rib tapes: the wing colour, lighter */
    MAT_RIB_UNDER,              /* and beneath, a shade darker */
    MAT_COUNT
};

typedef struct {
    int count;
    unsigned short index[8];
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
    for (i = 0; i < count; ++i) f->index[i] = (unsigned short)index[i];
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

/* A strut from (x0,y0,z0) to (x1,y1,z1): two thin crossed blades, so it
 * shows edge-on from no angle. */
static void mesh_strut(AircraftMesh *m, float x0, float y0, float z0,
                       float x1, float y1, float z1, float width)
{
    int q[4], k;
    for (k = 0; k < 2; ++k) {
        float dx = k == 0 ? width * 0.5f : 0.0f;
        float dz = k == 0 ? 0.0f : width * 0.5f;
        q[0] = mesh_vertex(m, x0 - dx, y0, z0 - dz);
        q[1] = mesh_vertex(m, x0 + dx, y0, z0 + dz);
        q[2] = mesh_vertex(m, x1 + dx, y1, z1 + dz);
        q[3] = mesh_vertex(m, x1 - dx, y1, z1 - dz);
        mesh_face(m, 4, q, MAT_BODY_DARK, MAT_BODY_DARK, FACE_TWO_SIDED,
                  -0.1f);
    }
}

/* Model units are half a metre: the machine's span in metres is its half
 * span in units. */
static void build_aircraft_mesh(AircraftMesh *m, int allied,
                                const MachineType *t)
{
    int q[4];
    int nose[4], mid[4], tail[4];
    int k;
    float half = t->span, low_half = t->lower_span;
    float chord = t->chord * 2.0f, gap = t->gap * 1.55f;
    float up_y = t->monoplane ? 0.35f : gap * 0.58f;
    float low_y = -gap * 0.42f;
    float up_le = 1.8f + t->stagger * 2.0f, low_le = 1.75f;
    float strut_x = half * 0.64f;
    float length = t->two_seat ? 1.15f : 1.0f;
    memset(m, 0, sizeof(*m));
    m->parent_for_new = -1;
    /* Upper and lower wings: one panel each, both sides painted. */
    mesh_panel_y(m, -half, half, up_y, up_le - chord, up_le, MAT_WING_TOP,
                 MAT_WING_UNDER, FACE_TWO_SIDED | FACE_OUTLINE, 0.0f, 1);
    if (!t->monoplane)
        mesh_panel_y(m, -low_half, low_half, low_y, low_le - chord * 0.95f,
                     low_le, MAT_WING_TOP, MAT_WING_UNDER,
                     FACE_TWO_SIDED | FACE_OUTLINE, 0.0f, 1);
    else
        /* Keep the face numbering: an empty-sized stand-in for face 1. */
        mesh_panel_y(m, -0.01f, 0.01f, 0.3f, 0.0f, 0.01f, MAT_WING_UNDER,
                     MAT_WING_UNDER, FACE_TWO_SIDED, 0.0f, 1);
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
    tail[0] = mesh_vertex(m, -0.14f, 0.34f, -6.9f * length);
    tail[1] = mesh_vertex(m, 0.14f, 0.34f, -6.9f * length);
    tail[2] = mesh_vertex(m, 0.1f, -0.06f, -6.9f * length);
    tail[3] = mesh_vertex(m, -0.1f, -0.06f, -6.9f * length);
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
    /* Rib tapes across the chord of each wing, as decals on its top
     * surface, so the planes read as stretched fabric and not as boards. */
    {
        int n;
        m->parent_for_new = 0;
        for (n = 1; n <= 6; ++n) {
            float x = half * (float)n / 6.6f;
            for (k = -1; k <= 1; k += 2) {
                mesh_panel_y(m, (float)k * x - 0.07f, (float)k * x + 0.07f,
                             up_y + 0.01f, up_le - chord + 0.12f,
                             up_le - 0.08f, MAT_RIB, MAT_RIB, 0, -0.01f, 1);
                mesh_panel_y(m, (float)k * x - 0.07f, (float)k * x + 0.07f,
                             up_y - 0.01f, up_le - chord + 0.12f,
                             up_le - 0.08f, MAT_RIB_UNDER, MAT_RIB_UNDER, 0,
                             -0.01f, 0);
            }
        }
        if (!t->monoplane) {
            m->parent_for_new = 1;
            for (n = 1; n <= 5; ++n) {
                float x = low_half * (float)n / 5.6f;
                for (k = -1; k <= 1; k += 2) {
                    mesh_panel_y(m, (float)k * x - 0.07f, (float)k * x + 0.07f,
                                 low_y + 0.01f, low_le - chord * 0.95f + 0.12f,
                                 low_le - 0.08f, MAT_RIB, MAT_RIB, 0, -0.01f, 1);
                    mesh_panel_y(m, (float)k * x - 0.07f, (float)k * x + 0.07f,
                                 low_y - 0.01f, low_le - chord * 0.95f + 0.12f,
                                 low_le - 0.08f, MAT_RIB_UNDER, MAT_RIB_UNDER,
                                 0, -0.01f, 0);
                }
            }
        }
    }
    /* Markings on the wing tips, drawn straight after the wing they sit
     * on (faces 0 and 1) so depth sorting can never hide them. */
    {
        float mz = up_le - chord * 0.5f;            /* mid-chord */
        float tip = half * 0.74f, low_tip = low_half * 0.72f;
        if (allied) {
            m->parent_for_new = 0;
            for (k = -1; k <= 1; k += 2) {
                mesh_disc(m, 1, (float)k * tip, up_y + 0.02f, mz, 1.05f,
                          MAT_MARK_BLUE, -0.02f, 1);
                mesh_disc(m, 1, (float)k * tip, up_y + 0.03f, mz, 0.68f,
                          MAT_MARK_LIGHT, -0.04f, 1);
                mesh_disc(m, 1, (float)k * tip, up_y + 0.04f, mz, 0.34f,
                          MAT_MARK_RED, -0.06f, 1);
            }
            if (!t->monoplane) {
                m->parent_for_new = 1;
                for (k = -1; k <= 1; k += 2) {
                    mesh_disc(m, 1, (float)k * low_tip, low_y - 0.02f, mz,
                              0.95f, MAT_MARK_BLUE, -0.02f, 0);
                    mesh_disc(m, 1, (float)k * low_tip, low_y - 0.03f, mz,
                              0.6f, MAT_MARK_LIGHT, -0.04f, 0);
                    mesh_disc(m, 1, (float)k * low_tip, low_y - 0.04f, mz,
                              0.3f, MAT_MARK_RED, -0.06f, 0);
                }
            }
        } else {
            m->parent_for_new = 0;
            mesh_cross_y(m, -tip, up_y + 0.02f, mz, 0.8f, 1);
            mesh_cross_y(m, tip, up_y + 0.02f, mz, 0.8f, 1);
            if (!t->monoplane) {
                m->parent_for_new = 1;
                mesh_cross_y(m, -low_tip, low_y - 0.02f, mz, 0.72f, 0);
                mesh_cross_y(m, low_tip, low_y - 0.02f, mz, 0.72f, 0);
            }
        }
    }
    m->parent_for_new = -1;
    if (t->monoplane) {
        /* A shoulder-wing monoplane: a pylon above the cockpit and the
         * wires that brace the wing to it and to the undercarriage. */
        mesh_strut(m, 0.0f, up_y, 0.6f, 0.0f, 1.9f, 0.6f, 0.1f);
        mesh_strut(m, -strut_x, up_y, 0.6f, 0.0f, 1.9f, 0.6f, 0.04f);
        mesh_strut(m, strut_x, up_y, 0.6f, 0.0f, 1.9f, 0.6f, 0.04f);
        mesh_strut(m, -strut_x, up_y, 0.6f, 0.0f, -1.6f, 0.9f, 0.04f);
        mesh_strut(m, strut_x, up_y, 0.6f, 0.0f, -1.6f, 0.9f, 0.04f);
        return;
    }
    /* Interplane struts outboard, cabane struts over the fuselage. */
    {
        float fs = up_le - chord * 0.3f, rs = up_le - chord * 0.75f;
        float st = t->stagger * 2.0f;
        mesh_strut(m, -strut_x, up_y, fs, -strut_x * 0.95f, low_y, fs - st,
                   0.16f);
        mesh_strut(m, strut_x, up_y, fs, strut_x * 0.95f, low_y, fs - st,
                   0.16f);
        mesh_strut(m, -strut_x, up_y, rs, -strut_x * 0.95f, low_y, rs - st,
                   0.16f);
        mesh_strut(m, strut_x, up_y, rs, strut_x * 0.95f, low_y, rs - st,
                   0.16f);
        mesh_strut(m, -0.7f, up_y, fs, -0.6f, 0.7f, fs + 0.4f, 0.12f);
        mesh_strut(m, 0.7f, up_y, fs, 0.6f, 0.7f, fs + 0.4f, 0.12f);
    }
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

/* Meshes: 0 enemy scout, 1 allied scout, 2 airship, 3 Gotha bomber.  The
 * scouts are the machines of the chapter being flown (mesh_chapter), and
 * are rebuilt when it changes. */
static const AircraftMesh *aircraft_mesh(int kind)
{
    static AircraftMesh meshes[4];
    static int built[4];
    if (kind < 0 || kind > 3) kind = 0;
    if (built[kind] != mesh_chapter + 1) {
        if (kind == 2) {
            build_airship_mesh(&meshes[2]);
        } else {
            int i;
            const MachineType *t = kind == 1 ?
                &our_machines[our_machine_index(mesh_chapter)] : kind == 3 ?
                &their_machines[1] :
                &their_machines[their_machine_index(mesh_chapter)];
            build_aircraft_mesh(&meshes[kind], kind == 1, t);
            /* The Gotha is the same rigging drawn half as large again. */
            if (kind == 3)
                for (i = 0; i < meshes[3].verts; ++i) {
                    meshes[3].v[i][0] *= 1.6f;
                    meshes[3].v[i][1] *= 1.4f;
                    meshes[3].v[i][2] *= 1.4f;
                }
        }
        built[kind] = mesh_chapter + 1;
    }
    return &meshes[kind];
}

/* QA: TIME after any sortie entry shows the average drawing time of a
 * frame (declared with the QA arguments above). */

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
    for (i = 0; i < MAT_RIB; ++i)       /* the livery's own colours */
        base[i] = PIX_PACK(livery->rgb[i][0], livery->rgb[i][1],
                           livery->rgb[i][2]);
    base[MAT_RIB] = pix_mix(base[MAT_WING_TOP], PIX_PACK(236, 228, 200), 9);
    base[MAT_RIB_UNDER] = pix_mix(base[MAT_WING_UNDER], PIX_PACK(0, 0, 0), 7);
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
    /* Struts, wires, undercarriage, pilot and the spinning propeller.  The
     * struts follow the machine's own rigging and are drawn as lines at
     * every range, so a distant biplane never shows floating wings. */
    if (centre.depth < 1600.0f) {
        float struts[10][6];
        int count = 0;
        pix_t strut = pix_mix(PIX_PACK(42, 34, 28), g->world_haze, haze);
        {
            const MachineType *t = kind == 1 ?
                &our_machines[our_machine_index(mesh_chapter)] :
                &their_machines[kind == 3 ? 1 : their_machine_index(mesh_chapter)];
            float scale = kind == 3 ? 1.5f : 1.0f;
            float chord = t->chord * 2.0f, gap = t->gap * 1.55f;
            float up_y = t->monoplane ? 0.35f : gap * 0.58f, low_y = -gap * 0.42f;
            float up_le = 1.8f + t->stagger * 2.0f, sx = t->span * 0.64f;
            float fs = up_le - chord * 0.3f, rs = up_le - chord * 0.75f;
            float st = t->stagger * 2.0f;
            int side;
#define STRUT(a, b, c2, d, e, f) do { if (count < 10) { \
    struts[count][0] = (a) * scale; struts[count][1] = (b) * scale; \
    struts[count][2] = (c2) * scale; struts[count][3] = (d) * scale; \
    struts[count][4] = (e) * scale; struts[count][5] = (f) * scale; \
    ++count; } } while (0)
            for (side = -1; side <= 1; side += 2) {
                float x = (float)side * sx;
                if (t->monoplane) {
                    STRUT(0.0f, 1.9f, 0.6f, x, up_y, 0.6f);
                    STRUT(0.0f, -1.6f, 0.9f, x, up_y, 0.6f);
                } else {
                    STRUT(x, up_y, fs, x * 0.95f, low_y, fs - st);
                    STRUT(x, up_y, rs, x * 0.95f, low_y, rs - st);
                    STRUT((float)side * 0.7f, up_y, fs, (float)side * 0.6f,
                          0.7f, fs + 0.4f);
                }
                STRUT((float)side * 0.6f, -0.8f, 2.6f, (float)side * 0.9f,
                      -2.1f, 2.4f);
            }
            if (t->monoplane) STRUT(0.0f, up_y, 0.6f, 0.0f, 1.9f, 0.6f);
#undef STRUT
        }
        Projected3D head = project_world_point(g,
            pose_point(pose, 0.0f, 1.0f, -0.4f));
        Projected3D hub = project_world_point(g,
            pose_point(pose, 0.0f, 0.0f, 4.7f));
        Projected3D tip = project_world_point(g,
            pose_point(pose, 0.0f, 1.3f, 4.7f));
        for (i = 0; i < count; ++i) {
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

/* The wing cellule seen from the pilot's seat, built in the airframe's own
 * axes (x forward, y right, z down, metres from the pilot's eye) to the
 * Camel's measurements: the top wing 0.53 m over the eye with its leading
 * edge 1.1 m ahead, so its underside runs across the top of the view and
 * its trailing edge passes overhead; the lower wing a gap of 1.55 m below,
 * staggered back.  The cabane struts stand close either side of the gun,
 * from the centre section down past the coaming to the upper longerons;
 * the interplane pairs are far out at the sides with the flying, landing
 * and incidence wires crossing in each bay.  It is projected once, wide
 * (112 degrees, as a pilot takes in his wings at a glance), into the
 * cockpit composite, so it is fixed to the machine: the world rolls behind
 * it when the machine banks. */
typedef struct { float x, y, z; } CellPoint;
#define CELL_FOCAL 216.0f
#define CELL_CY 184.0f
#define CELL_NEAR 0.06f
#define CELL_UP_LE 1.10f
#define CELL_UP_TE (-0.27f)
#define CELL_UP_Z (-0.53f)
#define CELL_UP_SPAN 4.27f
#define CELL_CUTOUT_Y 0.38f
#define CELL_CUTOUT_TO 0.28f       /* the cut-out runs back from here */
#define CELL_LO_LE 0.64f
#define CELL_LO_TE (-0.73f)
#define CELL_LO_Z 1.02f
#define CELL_LO_ROOT 0.42f
#define CELL_DIHEDRAL 0.0875f      /* five degrees */
#define CELL_FRONT_SPAR 0.25f
#define CELL_REAR_SPAR 0.70f
#define CELL_CABANE_Y 0.36f
#define CELL_CABANE_FOOT_Y 0.46f
#define CELL_CABANE_FOOT_Z 0.77f

static const float cell_strut_y[2] = {1.25f, 2.75f};

static float cell_lower_z(float y)
{
    return CELL_LO_Z - fabsf(y) * CELL_DIHEDRAL;
}

static float cell_chord(float le, float te, float frac)
{
    return le - (le - te) * frac;
}

/* Clips a segment to the near plane; returns 0 if it lies behind the eye. */
static int cell_clip(float *x0, float *y0, float *z0, float *x1, float *y1,
                     float *z1)
{
    float t;
    if (*x0 < CELL_NEAR && *x1 < CELL_NEAR) return 0;
    if (*x0 < CELL_NEAR) {
        t = (CELL_NEAR - *x0) / (*x1 - *x0);
        *y0 += (*y1 - *y0) * t; *z0 += (*z1 - *z0) * t; *x0 = CELL_NEAR;
    }
    if (*x1 < CELL_NEAR) {
        t = (CELL_NEAR - *x1) / (*x0 - *x1);
        *y1 += (*y0 - *y1) * t; *z1 += (*z0 - *z1) * t; *x1 = CELL_NEAR;
    }
    return 1;
}

static void cell_project(float x, float y, float z, int *sx, int *sy)
{
    float px = 320.0f + y * CELL_FOCAL / x, py = CELL_CY + z * CELL_FOCAL / x;
    /* Far off screen is still drawn toward the right edge. */
    if (px < -4000.0f) px = -4000.0f;
    if (px > 4000.0f) px = 4000.0f;
    if (py < -4000.0f) py = -4000.0f;
    if (py > 4000.0f) py = 4000.0f;
    *sx = (int)floorf(px + 0.5f);
    *sy = (int)floorf(py + 0.5f);
}

static void cell_line(SDL_Surface *s, float x0, float y0, float z0,
                      float x1, float y1, float z1, pix_t c)
{
    int ax, ay, bx, by;
    if (!cell_clip(&x0, &y0, &z0, &x1, &y1, &z1)) return;
    cell_project(x0, y0, z0, &ax, &ay);
    cell_project(x1, y1, z1, &bx, &by);
    line(s, ax, ay, bx, by, c);
}

static void cell_strut(SDL_Surface *s, float x0, float y0, float z0,
                       float x1, float y1, float z1, float width)
{
    int ax, ay, bx, by, wa, wb;
    if (!cell_clip(&x0, &y0, &z0, &x1, &y1, &z1)) return;
    cell_project(x0, y0, z0, &ax, &ay);
    cell_project(x1, y1, z1, &bx, &by);
    wa = (int)(width * CELL_FOCAL / x0 + 0.5f);
    wb = (int)(width * CELL_FOCAL / x1 + 0.5f);
    if (wa > 60) wa = 60;
    if (wb > 60) wb = 60;
    draw_strut(s, ax, ay, bx, by, wa < 2 ? 2 : wa, wb < 2 ? 2 : wb);
}

/* Both wings' visible surfaces, cast pixel by pixel once when the
 * composite is built.  The top wing's underside is clear-doped linen with
 * rib tapes, glowing where the low sun is behind it, its leading edge
 * lit and the centre-section cut-out open to the sky over the pilot; the
 * lower wing's top is the same PC10 khaki as the painted roots. */
static int cellule_sample(float px, float py, float *r, float *gr, float *b)
{
    float dx = (px - 320.0f) / CELL_FOCAL, dz = (py - CELL_CY) / CELL_FOCAL;
    if (dz < -0.001f) {
        float x = CELL_UP_Z / dz, y = x * dx, chord, rib, tape, glow, edge;
        if (x > CELL_UP_LE || fabsf(y) > CELL_UP_SPAN) return 0;
        if (fabsf(y) < CELL_CUTOUT_Y && x < CELL_CUTOUT_TO) return 0;
        chord = (x - CELL_UP_TE) / (CELL_UP_LE - CELL_UP_TE);
        rib = fabsf(y) / (CELL_UP_SPAN / 17.0f);
        rib -= floorf(rib);
        tape = rib < 0.06f || rib > 0.94f ? 1.0f : 0.0f;
        glow = 0.5f + 0.5f * (y / CELL_UP_SPAN);    /* the sun on the right */
        edge = chord > 0.92f ? (chord - 0.92f) * 12.0f : 0.0f;
        *r = 120.0f + glow * 52.0f + tape * 22.0f - edge * 30.0f;
        *gr = 102.0f + glow * 38.0f + tape * 18.0f - edge * 26.0f;
        *b = 82.0f + glow * 18.0f + tape * 12.0f - edge * 20.0f;
        if (chord > 0.985f) { *r += 60.0f; *gr += 50.0f; *b += 34.0f; }
        if (fabsf(y) < CELL_CUTOUT_Y + 0.03f && x < CELL_CUTOUT_TO + 0.03f) {
            *r *= 0.55f; *gr *= 0.55f; *b *= 0.55f;  /* the cut-out's rim */
        }
        return 1;
    }
    if (dz > 0.001f) {
        float k = fabsf(dx) * CELL_DIHEDRAL;
        float x = CELL_LO_Z / (dz + k), y = x * dx, chord, rib, tape, light, fade;
        if (x > CELL_LO_LE || fabsf(y) < CELL_LO_ROOT ||
            fabsf(y) > CELL_UP_SPAN * 0.95f)
            return 0;
        chord = (x - CELL_LO_TE) / (CELL_LO_LE - CELL_LO_TE);
        rib = fabsf(y) / (CELL_UP_SPAN / 17.0f);
        rib -= floorf(rib);
        tape = rib < 0.07f || rib > 0.93f ? 1.0f : 0.0f;
        light = 0.6f + 0.4f * (1.0f - chord);
        fade = (fabsf(y) - CELL_LO_ROOT) / (CELL_UP_SPAN - CELL_LO_ROOT);
        {
            uint32_t h = scene_hash((uint32_t)(px * 7.0f) * 7919u ^
                                    (uint32_t)(py * 5.0f) * 104729u);
            float grain = (float)(h & 15u) - 7.5f;
            *r = 92.0f + light * 40.0f + tape * 18.0f - fade * 30.0f + grain;
            *gr = 80.0f + light * 26.0f + tape * 14.0f - fade * 20.0f + grain;
            *b = 48.0f + light * 14.0f + tape * 8.0f - fade * 6.0f + grain * 0.6f;
        }
        if (chord > 0.96f) { *r += 30.0f; *gr += 24.0f; *b += 14.0f; }
        if (y > 0.0f) { *r *= 0.78f; *gr *= 0.86f; *b *= 1.02f; }
        return 1;
    }
    return 0;
}

#define CELLULE_ROWS 330
static void cellule_cast_pixel(int x, int y, pix_t *out, uint8_t *hit);
static void draw_cellule_surfaces(SDL_Surface *c)
{
    /* Cast once and kept: the cockpit composites share it, and the 68040
     * need only do the arithmetic a single time. */
    static pix_t *cast;
    static uint8_t *covered;
    int x, y;
    if (!cast) {
        cast = (pix_t *)malloc(sizeof(pix_t) * SCREEN_W * CELLULE_ROWS);
        covered = (uint8_t *)calloc(SCREEN_W * CELLULE_ROWS, 1);
        if (!cast || !covered) {
            free(cast); free(covered);
            cast = NULL; covered = NULL;
            return;
        }
        for (y = 0; y < CELLULE_ROWS; ++y)
            for (x = 0; x < SCREEN_W; ++x)
                cellule_cast_pixel(x, y, &cast[y * SCREEN_W + x],
                                   &covered[y * SCREEN_W + x]);
    }
    for (y = 0; y < CELLULE_ROWS; ++y) {
        pix_t *row = PIX_ROW(c, y);
        for (x = 0; x < SCREEN_W; ++x)
            if (covered[y * SCREEN_W + x]) row[x] = cast[y * SCREEN_W + x];
    }
}

static void cellule_cast_pixel(int x, int y, pix_t *out, uint8_t *hit)
{
    float r = 0.0f, gr = 0.0f, b = 0.0f;
    int n = 0, k;
    for (k = 0; k < 4; ++k) {
        float sr, sg, sb;
        if (cellule_sample((float)x + 0.25f + 0.5f * (float)(k & 1),
                           (float)y + 0.25f + 0.5f * (float)(k >> 1),
                           &sr, &sg, &sb)) {
            r += sr; gr += sg; b += sb; ++n;
        }
    }
    *hit = 0;
    if (n < 2) return;
    r /= (float)n; gr /= (float)n; b /= (float)n;
    if (n < 4) { r *= 0.8f; gr *= 0.8f; b *= 0.8f; }   /* soft edge */
    *out = PIX_PACK((int)flight_clamp(r, 0.0f, 255.0f),
                    (int)flight_clamp(gr, 0.0f, 255.0f),
                    (int)flight_clamp(b, 0.0f, 255.0f));
    *hit = 1;
}

static void draw_cockpit_cellule(SDL_Surface *c)
{
    pix_t wire = PIX_PACK(48, 44, 40), landing = PIX_PACK(62, 56, 50);
    int side, k, bay;
    draw_cellule_surfaces(c);
    for (side = -1; side <= 1; side += 2) {
        float sy = (float)side;
        float root = sy * CELL_LO_ROOT;
        float inner = sy * cell_strut_y[0];
        /* Wires first, so the struts stand over them.  Flying wires climb
         * from the lower wing root to the top of the inner struts, landing
         * wires fall from the centre section to their feet; in the outer
         * bay they run from strut to strut; incidence wires cross between
         * each front and rear pair. */
        for (k = 0; k < 2; ++k) {
            float ux = cell_chord(CELL_UP_LE, CELL_UP_TE,
                                  k ? CELL_REAR_SPAR : CELL_FRONT_SPAR);
            float lx = cell_chord(CELL_LO_LE, CELL_LO_TE,
                                  k ? CELL_REAR_SPAR : CELL_FRONT_SPAR);
            cell_line(c, lx, root, cell_lower_z(root), ux, inner, CELL_UP_Z,
                      wire);
            cell_line(c, ux, sy * CELL_CABANE_Y, CELL_UP_Z, lx, inner,
                      cell_lower_z(inner), landing);
            cell_line(c, lx, inner, cell_lower_z(inner), ux,
                      sy * cell_strut_y[1], CELL_UP_Z, wire);
            cell_line(c, ux, inner, CELL_UP_Z, lx, sy * cell_strut_y[1],
                      cell_lower_z(cell_strut_y[1]), landing);
        }
        for (bay = 0; bay < 2; ++bay) {
            float y = sy * cell_strut_y[bay];
            float fu = cell_chord(CELL_UP_LE, CELL_UP_TE, CELL_FRONT_SPAR);
            float ru = cell_chord(CELL_UP_LE, CELL_UP_TE, CELL_REAR_SPAR);
            float fl = cell_chord(CELL_LO_LE, CELL_LO_TE, CELL_FRONT_SPAR);
            float rl = cell_chord(CELL_LO_LE, CELL_LO_TE, CELL_REAR_SPAR);
            cell_line(c, fu, y, CELL_UP_Z, rl, y, cell_lower_z(y), wire);
            cell_line(c, ru, y, CELL_UP_Z, fl, y, cell_lower_z(y), wire);
            /* Interplane struts, rear first, raked with the stagger. */
            cell_strut(c, ru, y, CELL_UP_Z, rl, y, cell_lower_z(y), 0.11f);
            cell_strut(c, fu, y, CELL_UP_Z, fl, y, cell_lower_z(y), 0.11f);
        }
        /* Cabane struts from the centre section down past the coaming to
         * the upper longerons; the painted cockpit hides their feet. */
        for (k = 1; k >= 0; --k) {
            float cx = cell_chord(CELL_UP_LE, CELL_UP_TE,
                                  k ? CELL_REAR_SPAR : CELL_FRONT_SPAR);
            cell_strut(c, cx, sy * CELL_CABANE_Y, CELL_UP_Z, cx - 0.15f,
                       sy * CELL_CABANE_FOOT_Y, CELL_CABANE_FOOT_Z, 0.10f);
        }
    }
}

static SDL_Surface *cockpit_composite_from(SDL_Surface *painting)
{
    SDL_Surface *c;
    pix_t key = PIX_PACK(255, 0, 255);
    int i;
    if (!painting) return NULL;
    c = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_W, SCREEN_H,
                                       CANVAS_SCREEN_BITS,
                                       CANVAS_SCREEN_FORMAT);
    if (!c) return NULL;
    SDL_FillRect(c, NULL, key);
    /* The painted cellule (make_cockpit_cellule.py), or the drawn one. */
    {
        SDL_Surface *cell = SDL_LoadBMP(CANVAS_COCKPIT_CELLULE_BMP);
        SDL_Surface *conv = cell ? SDL_ConvertSurface(cell, c->format, 0) : NULL;
        if (cell) SDL_FreeSurface(cell);
        if (conv) {
            SDL_SetColorKey(conv, SDL_TRUE, SDL_MapRGB(conv->format, 255, 0, 255));
            SDL_BlitSurface(conv, NULL, c, NULL);
            SDL_FreeSurface(conv);
        } else {
            draw_cockpit_cellule(c);
        }
    }
    SDL_BlitSurface(painting, NULL, c, NULL);
    for (i = 0; i < 4; ++i) erase_painted_needle(c, &cockpit_dials[i]);
    SDL_SetColorKey(c, SDL_TRUE, key);
    /* Nearly two thirds of the composite is clear: run-length encoding
     * lets the per-frame blit skip it instead of testing every pixel. */
    SDL_SetSurfaceRLE(c, 1);
    return c;
}

/* The twin-gun cockpit, the single-gun one and the 1914 one with no
 * guns on the decking. */
static void build_cockpit_composite(Game *g)
{
    g->cockpit_composite = cockpit_composite_from(g->pilot_cockpit);
    g->cockpit_composite_unarmed =
        cockpit_composite_from(g->pilot_cockpit_unarmed ?
                               g->pilot_cockpit_unarmed : g->pilot_cockpit);
    g->cockpit_composite_single =
        cockpit_composite_from(g->pilot_cockpit_single ?
                               g->pilot_cockpit_single : g->pilot_cockpit);
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

/* 1914's weapons, held: painted on the host (make_personal_arms.py).  They
 * kick up when fired and settle as the bolt is worked or the next round
 * comes round. */
static float personal_kick(const Game *g)
{
    uint32_t since = sim_now(g) - g->last_shot;
    return since < 260u ? 1.0f - (float)since / 260.0f : 0.0f;
}

static void draw_revolver_in_hand(Game *g)
{
    SDL_Rect src = {0, 0, 220, 170}, dst = {420, 230, 220, 170};
    if (!g->cockpit_revolver) return;
    if (personal_kick(g) > 0.55f) src.x = 220;
    SDL_BlitSurface(g->cockpit_revolver, &src, g->screen, &dst);
}

/* The carbine is rendered in perspective for a grid of aims over its
 * whole range (make_rear_carbine.py): astern it runs away into the
 * picture, on a flank across it, raised it climbs it.  The frame for the
 * nearest aim is drawn where it was rendered on the screen. */
static const short *carbine_frame(const Game *g)
{
    float fx = (g->player_x - CARBINE_AIM_X0) * (CARBINE_AIM_COLUMNS - 1) /
               (float)(CARBINE_AIM_X1 - CARBINE_AIM_X0);
    float fy = (g->player_y - CARBINE_AIM_Y0) * (CARBINE_AIM_ROWS - 1) /
               (float)(CARBINE_AIM_Y1 - CARBINE_AIM_Y0);
    int column = (int)(fx + 0.5f), row = (int)(fy + 0.5f);
    if (column < 0) column = 0;
    if (column > CARBINE_AIM_COLUMNS - 1) column = CARBINE_AIM_COLUMNS - 1;
    if (row < 0) row = 0;
    if (row > CARBINE_AIM_ROWS - 1) row = CARBINE_AIM_ROWS - 1;
    return carbine_frames[row * CARBINE_AIM_COLUMNS + column];
}

/* The kick throws the rifle back along its own line, towards the butt. */
static void carbine_kick_offset(const Game *g, const short *f,
                                int *dx, int *dy)
{
    float kick = personal_kick(g);
    float bx = (float)f[4] + (float)f[2] * 0.5f - (float)f[6];
    float by = (float)f[5] + (float)f[3] - (float)f[7];
    float length = sqrtf(bx * bx + by * by);
    if (length < 1.0f) length = 1.0f;
    *dx = (int)(bx / length * kick * 9.0f);
    *dy = (int)(by / length * kick * 9.0f);
}

static void carbine_muzzle_position(const Game *g, float *x, float *y)
{
    const short *f = carbine_frame(g);
    int dx, dy;
    carbine_kick_offset(g, f, &dx, &dy);
    *x = (float)(f[6] + dx);
    *y = (float)(f[7] + dy);
}

static void draw_carbine_in_hand(Game *g)
{
    const short *f = carbine_frame(g);
    SDL_Rect src, dst;
    int dx, dy;
    float kick = personal_kick(g);
    if (!g->rear_carbine) return;
    carbine_kick_offset(g, f, &dx, &dy);
    src.x = f[0]; src.y = f[1]; src.w = f[2]; src.h = f[3];
    dst.x = f[4] + dx; dst.y = f[5] + dy; dst.w = f[2]; dst.h = f[3];
    SDL_BlitSurface(g->rear_carbine, &src, g->screen, &dst);
    if (kick > 0.6f) {
        /* The flash at the muzzle, along the line the round takes. */
        int mx = f[6] + dx, my = f[7] + dy;
        blend_circle(g->screen, mx, my, 9 + (int)(kick * 7.0f),
                     255, 196, 110, 150);
        blend_circle(g->screen, mx, my, 4, 255, 246, 220, 230);
    }
}

static void draw_living_cockpit(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t now = sim_now(g);
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
        SDL_Surface *cockpit = g->cockpit_composite;
        int guns = pilot_gun_count(g);
        if (guns == 0 && g->cockpit_composite_unarmed)
            cockpit = g->cockpit_composite_unarmed;
        else if (guns == 1 && g->cockpit_composite_single)
            cockpit = g->cockpit_composite_single;
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
    uint32_t now = sim_now(g);
    int health = g->health > 0 ? g->health : 0;
    blend_rect(s, 8, 54, 176, 38, 24, 18, 14, 170);
    rect(s, 8, 54, 176, 1, PIX_PACK(176, 134, 62));
    rect(s, 8, 91, 176, 1, PIX_PACK(120, 90, 44));
    if (g->leg == LEG_HOME && g->special == SPECIAL_NONE && !g->qa_hold)
        df_home_text(g, buf, sizeof(buf));
    else if (g->obs_count && g->leg == LEG_OUT && observer_text(g, buf, sizeof(buf)))
        ;
    else if (!enemies_hostile(g) && g->wave > 0 && g->special == SPECIAL_NONE)
        snprintf(buf, sizeof(buf), "SHADOWED %02d/%02d S",
                 (int)g->shadow_time, (int)DF_SHADOW_SECONDS);
    else if (perspective_enemy_total(g) > 0)
        snprintf(buf, sizeof(buf), "AMMO %03d  DOWNED %d/%d",
                 g->ammunition, g->kills, perspective_enemy_total(g));
    else
        snprintf(buf, sizeof(buf), mission_armed(g) ? "HOLD YOUR FIRE" :
                 personal_arms(g) ? "RIFLE AND REVOLVER" : "UNARMED PATROL");
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
    g->notice_until = sim_now(g) + 1800u;
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
    uint32_t now = sim_now(g);
    draw_simulator_terrain(g);
    draw_observer_tasks(g);
    draw_simulator_clouds(g);
    draw_squall_3d(g);
    draw_archie_3d(g);
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
    if (personal_arms(g)) draw_revolver_in_hand(g);
    draw_dogfight_bullet_holes(g);
    draw_dogfight_particles(g, 1);
    if (pilot_gun_count(g) > 0 && now >= g->last_shot &&
        now - g->last_shot < 45u) {
        /* A flash at each Vickers muzzle: the single gun's on the
         * centreline, the twin guns' either side of it. */
        if (pilot_gun_count(g) == 1) {
            fill_triangle(s, 316, 226, 320, 216, 325, 226,
                          rgb(s, 255, 140, 45));
            circle(s, 320, 222, 2, rgb(s, 255, 238, 157));
        } else {
            fill_triangle(s, 288, 232, 292, 222, 297, 232,
                          rgb(s, 255, 140, 45));
            circle(s, 292, 228, 2, rgb(s, 255, 238, 157));
            fill_triangle(s, 345, 232, 350, 222, 354, 232,
                          rgb(s, 255, 140, 45));
            circle(s, 350, 228, 2, rgb(s, 255, 238, 157));
        }
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
        draw_archie_2d(g);
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
    if (gunner) draw_squall_gunner(g);
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
        /* 1914: Neddy in the back with the carbine, and where it points. */
        if (personal_arms(g)) {
            int ax = (int)g->player_x, ay = (int)g->player_y;
            uint32_t sight = gunner_tail_blocks_shot(g, ax, ay) ?
                             rgb(s, 242, 72, 45) : rgb(s, 239, 221, 164);
            draw_carbine_in_hand(g);
            line(s, ax - 14, ay, ax - 6, ay, sight);
            line(s, ax + 6, ay, ax + 14, ay, sight);
            line(s, ax, ay - 14, ax, ay - 6, sight);
            line(s, ax, ay + 6, ax, ay + 14, sight);
        }
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
        snprintf(buf, sizeof(buf), mission_armed(g) ? "HOLD YOUR FIRE" :
                 personal_arms(g) ? "RIFLE AND REVOLVER" : "UNARMED PATROL");
    draw_field_sign(g, 8, 342, 130, buf);
    if (mission_armed(g))
        snprintf(buf, sizeof(buf), "AMMO %03d", g->ammunition);
    else
        snprintf(buf, sizeof(buf), personal_arms(g) ? "CARBINE" : "UNARMED");
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
    if (g->notice_until && sim_now(g) < g->notice_until)
        draw_field_sign(g, 200, 140, 240, g->notice);
}

/* Moves the whole picture by (ox, oy) for a shake, filling the edge it
 * leaves with the row or column beside it. */
static void shift_frame(SDL_Surface *s, int ox, int oy)
{
    int y;
    if (ox == 0 && oy == 0) return;
    if (ox > 8) ox = 8;
    if (ox < -8) ox = -8;
    if (oy > 8) oy = 8;
    if (oy < -8) oy = -8;
    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
    if (oy > 0)
        for (y = s->h - 1; y >= oy; --y)
            memcpy(PIX_ROW(s, y), PIX_ROW(s, y - oy), (size_t)s->w * sizeof(pix_t));
    else if (oy < 0)
        for (y = 0; y < s->h + oy; ++y)
            memcpy(PIX_ROW(s, y), PIX_ROW(s, y - oy), (size_t)s->w * sizeof(pix_t));
    if (ox != 0)
        for (y = 0; y < s->h; ++y) {
            pix_t *row = PIX_ROW(s, y);
            if (ox > 0) memmove(row + ox, row, (size_t)(s->w - ox) * sizeof(pix_t));
            else memmove(row, row - ox, (size_t)(s->w + ox) * sizeof(pix_t));
        }
    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
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
    draw_field_2d(g);
    for (i = 0; i < MAX_TARGETS; ++i) if (g->targets[i].active) {
        int sx = (int)(g->targets[i].world_x - g->scroll);
        if (sx > -60 && sx < SCREEN_W + 60) {
            int ty = terrain_y(g->targets[i].world_x) - 12;
            if (g->targets[i].civilian) {
                int sprite = g->targets[i].civilian + g->targets[i].wrecked;
                const short *r = side_civilian_rect[sprite];
                SDL_Rect src = {r[0], r[1], r[2], r[3]};
                SDL_Rect dst = {sx - r[2] / 2, ty + 14 - r[3], r[2], r[3]};
                if (g->side_civilians)
                    SDL_BlitSurface(g->side_civilians, &src, s, &dst);
                if (g->targets[i].wrecked)
                    blend_circle(s, sx + (int)(sim_now(g) / 60u % 12u),
                                 ty - r[3] / 2 - (int)(sim_now(g) / 60u % 12u),
                                 10, 40, 38, 36, 110);
            } else if (g->targets[i].task) {
                draw_side_task(g, &g->targets[i], sx, ty);
            } else {
                draw_ground_gun(g, sx, ty, &g->targets[i]);
            }
        }
    }
    draw_riflemen(g);
    draw_balloon(g);
    for (i = 0; i < MAX_ENEMIES; ++i) if (g->enemies[i].active) {
        int sx = (int)(g->enemies[i].x - g->scroll);
        int sy = (int)(g->enemies[i].y + sinf(g->scroll * 0.01f + g->enemies[i].phase) * 30.0f);
        if (sx > -50 && sx < SCREEN_W + 50) {
            if (g->enemies[i].hp < g->enemies[i].max_hp) {
                int health = g->enemies[i].hp * 100 / g->enemies[i].max_hp;
                draw_aircraft_smoke(g, sx, sy, health, 1,
                                    0x454e454du + (uint32_t)i * 97u);
            }
            int frame = 2 + (int)(sinf(g->scroll * 0.02f + i) * 1.2f);
            if (g->enemies[i].ai_side > 0.0f) {
                /* One of those come up from behind, facing our way. */
                draw_plane_mirrored(g, sx, sy, 2);
                if (sim_now(g) - g->enemies[i].last_shot < 70u)
                    blend_circle(s, sx + 32, sy - 3, 5, 255, 190, 90, 230);
                continue;
            }
            draw_plane(g, sx, sy, 1, frame);
            if (mission_armed(g))
                draw_side_gun(g, sx, sy, 1, frame,
                              sim_now(g) - g->enemies[i].last_shot < 70u);
        }
    }
    for (i = 0; i < MAX_BULLETS; ++i) if (g->bullets[i].active) {
        Bullet *b = &g->bullets[i];
        if (b->type == 4) {
            rect(s, (int)b->x - 2, (int)b->y - 2, 5, 5, PIX_PACK(120, 86, 50));
            line(s, (int)b->x, (int)b->y, (int)(b->x - b->vx * 0.2f),
                 (int)(b->y - b->vy * 0.2f) - 4, PIX_PACK(236, 230, 214));
        } else if (b->type == 1) {
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
    draw_archie_2d(g);
    draw_explosions(g);
    draw_player_damage(g);
    {
        int pitch = 2 + (int)(g->player_vy / 45.0f);
        draw_plane(g, (int)g->player_x, (int)g->player_y, 0, pitch);
        if (mission_armed(g))
            draw_side_gun(g, (int)g->player_x, (int)g->player_y, 0, pitch,
                          sim_now(g) - g->last_shot < 60u);
        draw_fabric_holes(g, (int)g->player_x, (int)g->player_y);
    }
    draw_squall_2d(g);
    if (g->shake > 0.0f) {
        uint32_t now = sim_now(g);
        shift_frame(s, (int)(sinf((float)now * 0.093f) * g->shake * 6.0f),
                    (int)(cosf((float)now * 0.077f) * g->shake * 5.0f));
    }
    if (g->blast_flash > 0.0f)
        blend_rect(s, 0, 0, SCREEN_W, SCREEN_H, 255, 226, 170,
                   (uint8_t)(g->blast_flash * 150.0f));
    snprintf(buf, sizeof(buf), "SCORE %05d", g->score);
    draw_field_sign(g, 8, 8, 112, buf);
    if (is_perspective_mode(campaign_missions[g->chapter][g->mission].mode))
        snprintf(buf, sizeof(buf), "AIR KILLS %d/%d", g->kills,
                 current_mission_goal(g));
    else if (g->leg != LEG_OUT)
        snprintf(buf, sizeof(buf), g->leg == LEG_HOME ? "HOMEWARD" : "LAND");
    else if (g->chapter == 0)
        snprintf(buf, sizeof(buf), "TASKS %d/%d", g->objectives,
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
        snprintf(buf, sizeof(buf), personal_arms(g) ? "CARBINE" : "UNARMED");
    draw_field_sign(g, 264, 8, 108, buf);
    if (g->bags > 0 || g->chapter == 0)
        snprintf(buf, sizeof(buf), "BAGS %d", g->bags);
    else
        snprintf(buf, sizeof(buf), "BOMBS %02d", g->bombs_remaining);
    draw_field_sign(g, 264, 39, 108, buf);
    snprintf(buf, sizeof(buf), "FUEL %02d", (int)g->fuel);
    draw_field_sign(g, 526, 8, 106, buf);
    snprintf(buf, sizeof(buf), "AIRFRAME %03d", g->health > 0 ? g->health : 0);
    draw_field_sign(g, 502, 39, 130, buf);
    if (g->notice_until && sim_now(g) < g->notice_until)
        draw_field_sign(g, 200, 140, 240, g->notice);
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
    {
        char memo[64];
        snprintf(memo, sizeof(memo), "%s SEAGOON WAS EXCEEDINGLY LUCKY.",
                 rank_short[g->rank & 3]);
        typewriter_text(g, 31, 73, memo, 7, 11, ink);
    }
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
    {
        static const char *const carved[4] = {"2ND LT", "LT", "CAPT", "MAJOR"};
        const char *rank = carved[g->rank & 3];
        typewriter_text(g, 320 - typewriter_width(rank, 11) / 2, 217, rank, 7, 11,
                        inscription);
    }
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
    } else if (g->raider_through) {
        snprintf(out, size, g->special == SPECIAL_AIRSHIP ?
            "AN ENEMY AIRSHIP CROSSED THE COAST LAST NIGHT AND DROPPED BOMBS. "
            "OUR AIRMEN WERE UNABLE TO OVERTAKE IT." :
            "ENEMY AEROPLANES REACHED LONDON IN DAYLIGHT. CASUALTIES ARE "
            "REPORTED IN THE EAST END.");
    } else if (g->obs_count) {
        /* The 1914 cockpit patrols: the observer's work, as much of it as
         * was done, in the order the paper would put it. */
        static const char *const work[4] = {
            "COLUMNS ON THE MARCH REPORTED", "A MESSAGE DROPPED AT H.Q.",
            "A BATTERY PHOTOGRAPHED", "OUR GUNS RANGED ON THE ENEMY"
        };
        const char *done[4];
        int k, n = 0;
        for (k = 0; k < 4; ++k) {
            int j;
            for (j = 0; j < g->obs_count; ++j)
                if (g->obs[j].kind == k && g->obs[j].done) done[n++] = work[k];
        }
        if (n == 0)
            snprintf(out, size, "OUR AIRMEN WATCHED THE ROADS YESTERDAY, BUT "
                     "MIST HID THE ENEMY'S MOVEMENTS.");
        else if (n == 1)
            snprintf(out, size, "RECONNAISSANCE YESTERDAY: %s.", done[0]);
        else if (n == 2)
            snprintf(out, size, "RECONNAISSANCE YESTERDAY: %s AND %s.",
                     done[0], done[1]);
        else if (n == 3)
            snprintf(out, size, "RECONNAISSANCE YESTERDAY: %s, %s AND %s.",
                     done[0], done[1], done[2]);
        else
            snprintf(out, size, "RECONNAISSANCE YESTERDAY: %s, %s, %s AND %s.",
                     done[0], done[1], done[2], done[3]);
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
        snprintf(out, size, wingman_hit(g) >= 0 ?
                 "IN AIR FIGHTING YESTERDAY %d HOSTILE MACHINES WERE BROUGHT "
                 "DOWN. ONE OF OURS WAS DAMAGED BUT REACHED OUR LINES." :
                 "IN AIR FIGHTING YESTERDAY %d HOSTILE MACHINES "
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
    {
        /* A torn-off column of the late edition pinned to the board:
         * white newsprint, the masthead in blackletter, set in columns. */
        pix_t ink = PIX_PACK(30, 28, 26);
        char cased[200];
        rect(s, 410, 62, 196, 172, PIX_PACK(30, 22, 16));
        rect(s, 406, 58, 196, 172, PIX_PACK(232, 230, 220));
        blend_rect(s, 406, 58, 196, 172, 200, 196, 180, 30);
        circle(s, 504, 64, 3, PIX_PACK(150, 40, 30));
        font_text(g, FONT_NEWS_DECK, 416, 80, "LATE EDITION", PIX_PACK(128, 36, 28));
        font_text(g, FONT_NEWS_ITALIC, 416, 96, "The Western Courier", ink);
        rect(s, 414, 101, 180, 1, ink);
        rect(s, 414, 103, 180, 1, ink);
        news_case(words, cased, (int)sizeof(cased));
        news_column(g, FONT_NEWS_SMALL, 416, 116, 176, 12, 222, cased, ink);
    }
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
         g->success ? "DUTY CARRIED OUT" : "DUTY NOT CARRIED OUT",
         12, 18, red);
    if (g->special == SPECIAL_TRUCE)
        snprintf(buf, sizeof(buf), "PUDDING DELIVERED           %d/1",
                 g->pudding_delivered ? 1 : 0);
    else if (g->obs_count)
        snprintf(buf, sizeof(buf), "OBSERVER'S TASKS            %d/%d",
                 g->objectives, current_mission_goal(g));
    else if (is_perspective_mode(campaign_missions[g->chapter][g->mission].mode))
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
    snprintf(buf, sizeof(buf), "AIRFRAME REMAINING          %d",
             g->health > 0 ? g->health : 0);
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
    } else if (g->mission_save_state == -1) {
        snprintf(buf, sizeof(buf), "SAVE FAILED - ENTER WILL RETRY");
    } else {
        snprintf(buf, sizeof(buf), "SAVE FAILED - ENTER TO CARRY ON");
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
    SDL_AudioSpec want;
    /* Recordings are kept as mono 11 kHz: the mixer sums them to one
     * channel anyway, and mono halves their memory. */
    init_audio_tables();
    /* The march, the mess's song and the pipes are loaded when a scene
     * plays them (ensure_music), not all at start-up. */
    memset(&want, 0, sizeof(want));
    want.freq = 11025;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 512;
    want.callback = audio_callback;
    want.userdata = &g->audio;
    g->audio.noise = 0x19171917u;
    g->audio.record_track = RECORD_NONE;
    g->audio.music_gain = (float)g->music_volume / 10.0f;
    g->audio.effects_gain = (float)g->effects_volume / 10.0f;
    g->audio.device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
    if (!g->audio.device) {
        fprintf(stderr, "Canvas Skies audio disabled: %s\n", SDL_GetError());
        return 0;
    }
    SDL_PauseAudioDevice(g->audio.device, 0);
    return 1;
}

/* The three recordings the mixer plays by mood: loaded when a scene's
 * mood wants one, let go when it does not (unless kept in memory). */
static void ensure_music(Game *g, int mood)
{
    struct {
        uint8_t **data;
        uint32_t *len, *pos;
        const char *path;
        int mood;
    } track[3];
    int k;
    track[0].data = &g->audio.music_data; track[0].len = &g->audio.music_len;
    track[0].pos = &g->audio.music_pos; track[0].path = CANVAS_MARCH_WAV;
    track[0].mood = MUSIC_MARCH;
    track[1].data = &g->audio.mess_data; track[1].len = &g->audio.mess_len;
    track[1].pos = &g->audio.mess_pos; track[1].path = CANVAS_MESS_WAV;
    track[1].mood = MUSIC_MESS;
    track[2].data = &g->audio.piper_data; track[2].len = &g->audio.piper_len;
    track[2].pos = &g->audio.piper_pos; track[2].path = CANVAS_PIPER_WAV;
    track[2].mood = MUSIC_PIPER;
    if (!g->audio.device) return;
    for (k = 0; k < 3; ++k) {
        uint8_t *old = NULL;
        if (mood != track[k].mood) {
            if (!*track[k].data || g->cache_in_ram) continue;
            SDL_LockAudioDevice(g->audio.device);
            old = *track[k].data;
            *track[k].data = NULL;
            *track[k].len = 0;
            SDL_UnlockAudioDevice(g->audio.device);
            SDL_free(old);
        } else if (!*track[k].data) {
            uint32_t len = 0;
            uint8_t *data = load_mono_wav(track[k].path, &len);
            if (!data) {
                fprintf(stderr, "Recording unavailable; using synth: %s\n",
                        SDL_GetError());
                continue;
            }
            SDL_LockAudioDevice(g->audio.device);
            *track[k].data = data;
            *track[k].len = len;
            *track[k].pos = 0;
            SDL_UnlockAudioDevice(g->audio.device);
        }
    }
}

/* ---- Art on demand ------------------------------------------------------
 *
 * Every painting belongs to the scenes that show it.  On entering a scene
 * the groups it needs are loaded; unless the settings keep everything in
 * memory, whatever else is held is let go first, so a machine with 32 MB
 * holds only the art of the scene in front of it. */
typedef enum {
    AG_TITLE = 1u << 0, AG_BOARD = 1u << 1, AG_JOURNAL = 1u << 2,
    AG_DEBRIEF = 1u << 3, AG_NEWS = 1u << 4, AG_CINEMA = 1u << 5,
    AG_MESS = 1u << 6, AG_MEDALS = 1u << 7, AG_CARDS = 1u << 8,
    AG_OFFICE = 1u << 9, AG_LOSS = 1u << 10, AG_CHAPTER = 1u << 11,
    AG_SIDE = 1u << 12, AG_SKY = 1u << 13, AG_TOPDOWN = 1u << 14,
    AG_TARGETS = 1u << 15, AG_EXPLODE = 1u << 16, AG_COCKPIT = 1u << 17,
    AG_GROUND = 1u << 18, AG_GUNNER = 1u << 19, AG_LEWIS = 1u << 20,
    AG_CARBINE = 1u << 21, AG_RIFLEMEN = 1u << 22, AG_REMEMBER = 1u << 23
} AssetGroup;

enum { ASSET_PLAIN, ASSET_LOOSE_KEY, ASSET_EXACT_KEY, ASSET_INDEX_KEY };

typedef struct {
    size_t offset;          /* of the SDL_Surface pointer in Game */
    const char *path;
    unsigned group;
    unsigned char convert;  /* to the screen's format */
    unsigned char key;
} AssetSpec;

#define ASSET(field, path, group, convert, key) \
    {offsetof(Game, field), path, group, convert, key}
static const AssetSpec asset_specs[] = {
    ASSET(title, CANVAS_TITLE_BMP, AG_TITLE, 0, ASSET_PLAIN),
    ASSET(title_banner, CANVAS_TITLE_BANNER_BMP, AG_TITLE, 0, ASSET_LOOSE_KEY),
    ASSET(mission_board, CANVAS_BOARD_BMP, AG_BOARD, 0, ASSET_PLAIN),
    ASSET(fatal_memorial, CANVAS_FATAL_MEMORIAL_BMP, AG_DEBRIEF, 1, ASSET_PLAIN),
    ASSET(field_hospital, CANVAS_FIELD_HOSPITAL_BMP, AG_DEBRIEF, 1, ASSET_PLAIN),
    ASSET(journal, CANVAS_JOURNAL_BMP, AG_JOURNAL, 0, ASSET_PLAIN),
    ASSET(sky, CANVAS_SKY_BMP, AG_SKY, 1, ASSET_PLAIN),
    ASSET(battlefield, CANVAS_BATTLEFIELD_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(foreground_panorama, CANVAS_FOREGROUND_BMP, AG_SIDE, 0, ASSET_LOOSE_KEY),
    ASSET(midground[0], CANVAS_MID_FARM_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(midground[1], CANVAS_MID_CHURCH_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(midground[2], CANVAS_MID_POPLARS_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(midground[3], CANVAS_MID_SMOKE_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(midground[4], CANVAS_MID_TELEGRAPH_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(midground[5], CANVAS_MID_WALL_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(near_props[0], CANVAS_NEAR_CRATER_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(near_props[1], CANVAS_NEAR_TRENCH_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(near_props[2], CANVAS_NEAR_TREE_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(near_props[3], CANVAS_NEAR_WIRE_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(near_props[4], CANVAS_NEAR_ROAD_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(near_props[5], CANVAS_NEAR_MOUND_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(machine_gun_sprite, CANVAS_MACHINE_GUN_BMP, AG_SIDE, 0, ASSET_LOOSE_KEY),
    ASSET(aa_gun_sprite, CANVAS_AA_GUN_BMP, AG_SIDE, 0, ASSET_LOOSE_KEY),
    ASSET(side_civilians, CANVAS_SIDE_CIVILIANS_BMP, AG_SIDE, 1, ASSET_LOOSE_KEY),
    ASSET(riflemen_sheet, CANVAS_RIFLEMEN_BMP, AG_RIFLEMEN, 1, ASSET_LOOSE_KEY),
    ASSET(topdown_battlefield, CANVAS_TOPDOWN_BATTLEFIELD_BMP, AG_TOPDOWN, 1, ASSET_PLAIN),
    ASSET(topdown_player, CANVAS_TOPDOWN_PLAYER_BMP, AG_TOPDOWN, 1, ASSET_LOOSE_KEY),
    ASSET(dogfight_enemy, CANVAS_DOGFIGHT_ENEMY_BMP, AG_TOPDOWN, 1, ASSET_LOOSE_KEY),
    ASSET(topdown_machines[0], CANVAS_TOPDOWN_TWOSEAT_BMP, AG_TOPDOWN, 1, ASSET_LOOSE_KEY),
    ASSET(topdown_machines[1], CANVAS_TOPDOWN_LATE_BMP, AG_TOPDOWN, 1, ASSET_LOOSE_KEY),
    ASSET(topdown_targets, CANVAS_TOPDOWN_TARGETS_BMP, AG_TARGETS, 1, ASSET_LOOSE_KEY),
    ASSET(dogfight_background, CANVAS_DOGFIGHT_BACKGROUND_BMP, AG_CHAPTER, 1, ASSET_PLAIN),
    ASSET(explosion_atlas, CANVAS_EXPLOSION_ATLAS_BMP, AG_EXPLODE, 1, ASSET_LOOSE_KEY),
    ASSET(topdown_explosion_atlas, CANVAS_TOPDOWN_EXPLOSION_ATLAS_BMP, AG_EXPLODE, 1, ASSET_LOOSE_KEY),
    ASSET(pilot_cockpit, CANVAS_PILOT_COCKPIT_BMP, AG_COCKPIT, 1, ASSET_LOOSE_KEY),
    ASSET(pilot_cockpit_unarmed, CANVAS_PILOT_COCKPIT_UNARMED_BMP, AG_COCKPIT, 1, ASSET_LOOSE_KEY),
    ASSET(pilot_cockpit_single, CANVAS_PILOT_COCKPIT_SINGLE_BMP, AG_COCKPIT, 1, ASSET_LOOSE_KEY),
    ASSET(cockpit_revolver, CANVAS_COCKPIT_REVOLVER_BMP, AG_COCKPIT, 1, ASSET_LOOSE_KEY),
    ASSET(gunner_cockpit, CANVAS_GUNNER_COCKPIT_BMP, AG_GUNNER, 1, ASSET_LOOSE_KEY),
    ASSET(gunner_ring, CANVAS_GUNNER_RING_BMP, AG_GUNNER, 1, ASSET_LOOSE_KEY),
    ASSET(gunner_airframe, CANVAS_GUNNER_AIRFRAME_BMP, AG_GUNNER, 1, ASSET_LOOSE_KEY),
    ASSET(gunner_moving_guns, CANVAS_GUNNER_MOVING_BMP, AG_LEWIS, 1, ASSET_LOOSE_KEY),
    /* The carbine stays in its 256 colours, keyed by its last entry. */
    ASSET(rear_carbine, CANVAS_REAR_CARBINE_BMP, AG_CARBINE, 0, ASSET_INDEX_KEY),
    ASSET(newspaper_table, CANVAS_NEWSPAPER_TABLE_BMP, AG_NEWS, 1, ASSET_PLAIN),
    ASSET(cinema_hut, CANVAS_CINEMA_HUT_BMP, AG_CINEMA, 1, ASSET_PLAIN),
    /* The ribbons are purple silk, which the loose key would eat; the
     * painter leaves hard edges, so the exact colour is enough. */
    ASSET(medals, CANVAS_MEDALS_BMP, AG_MEDALS, 1, ASSET_EXACT_KEY),
    ASSET(mess, CANVAS_MESS_BMP, AG_MESS, 1, ASSET_PLAIN),
    ASSET(mess_rowdy, CANVAS_MESS_ROWDY_BMP, AG_MESS, 1, ASSET_PLAIN),
    ASSET(mess_wake, CANVAS_MESS_WAKE_BMP, AG_MESS, 1, ASSET_PLAIN),
    ASSET(machine_cards, CANVAS_MACHINE_CARDS_BMP, AG_CARDS, 1, ASSET_EXACT_KEY),
    ASSET(office_paper, CANVAS_OFFICE_PAPER_BMP, AG_OFFICE, 1, ASSET_PLAIN),
    ASSET(office_stamp, CANVAS_OFFICE_STAMP_BMP, AG_OFFICE, 1, ASSET_EXACT_KEY),
    ASSET(loss_card, CANVAS_LOSS_CARD_BMP, AG_LOSS, 1, ASSET_PLAIN),
#ifdef REMEMBER_LAYERS
    ASSET(remember_layer[0], CANVAS_REMEMBER_SKY_BMP, AG_REMEMBER, 1, ASSET_PLAIN),
    ASSET(remember_layer[1], CANVAS_REMEMBER_FAR_BMP, AG_REMEMBER, 1, ASSET_LOOSE_KEY),
    ASSET(remember_layer[2], CANVAS_REMEMBER_MID_BMP, AG_REMEMBER, 1, ASSET_LOOSE_KEY),
    ASSET(remember_layer[3], CANVAS_REMEMBER_NEAR_BMP, AG_REMEMBER, 1, ASSET_LOOSE_KEY),
#else
    ASSET(remember_field, CANVAS_REMEMBER_FIELD_BMP, AG_REMEMBER, 1, ASSET_PLAIN),
#endif
#ifdef REMEMBER_LIFE_SPRITES
    ASSET(remember_life, CANVAS_REMEMBER_LIFE_BMP, AG_REMEMBER, 1, ASSET_LOOSE_KEY),
#endif
    ASSET(remember_poppies, CANVAS_REMEMBER_POPPIES_BMP, AG_REMEMBER, 1,
          ASSET_LOOSE_KEY)
};
#define ASSET_COUNT ((int)(sizeof(asset_specs) / sizeof(asset_specs[0])))

static SDL_Surface **asset_slot(Game *g, const AssetSpec *a)
{
    return (SDL_Surface **)(void *)((char *)g + a->offset);
}

static void free_asset_groups(Game *g, unsigned drop)
{
    int i;
    drop &= g->assets_loaded;
    if (!drop) return;
    for (i = 0; i < ASSET_COUNT; ++i) {
        SDL_Surface **slot = asset_slot(g, &asset_specs[i]);
        if ((asset_specs[i].group & drop) && *slot) {
            SDL_FreeSurface(*slot);
            *slot = NULL;
        }
    }
    if (drop & AG_COCKPIT) {
        SDL_Surface **made[] = {&g->cockpit_composite,
                                &g->cockpit_composite_unarmed,
                                &g->cockpit_composite_single};
        for (i = 0; i < 3; ++i) if (*made[i]) {
            SDL_FreeSurface(*made[i]);
            *made[i] = NULL;
        }
    }
    if (drop & AG_GROUND) free_ground_mips(g);
    if (drop & AG_TITLE) {
        /* The backdrops composed from the title painting go with it. */
        if (g->title_backdrop) SDL_FreeSurface(g->title_backdrop);
        if (g->dawn_backdrop) SDL_FreeSurface(g->dawn_backdrop);
        g->title_backdrop = g->dawn_backdrop = NULL;
    }
    g->assets_loaded &= ~drop;
}

static void load_asset_groups(Game *g, unsigned want)
{
    unsigned fresh = want & ~g->assets_loaded;
    int i;
    if (!fresh) return;
    for (i = 0; i < ASSET_COUNT; ++i) {
        const AssetSpec *a = &asset_specs[i];
        SDL_Surface **slot = asset_slot(g, a);
        if (!(a->group & fresh) || *slot) continue;
        *slot = SDL_LoadBMP(a->path);
        if (!*slot) {
            fprintf(stderr, "Art unavailable (%s): %s\n", a->path,
                    SDL_GetError());
            continue;
        }
        if (a->convert) convert_to_screen(g, slot);
        if (a->key == ASSET_LOOSE_KEY)
            prepare_magenta_key(*slot);
        else if (a->key == ASSET_EXACT_KEY)
            SDL_SetColorKey(*slot, SDL_TRUE,
                            SDL_MapRGB((*slot)->format, 255, 0, 255));
        else if (a->key == ASSET_INDEX_KEY &&
                 (*slot)->format->BitsPerPixel == 8)
            SDL_SetColorKey(*slot, SDL_TRUE, 255);
    }
    if (fresh & AG_SKY) prepare_sky(g);
    if (fresh & AG_TOPDOWN) blend_vertical_seam(g->topdown_battlefield);
    if (fresh & AG_COCKPIT) build_cockpit_composite(g);
    if (fresh & AG_GROUND) {
        /* Built from the top-down painting, which need not stay. */
        SDL_Surface *ground = g->topdown_battlefield;
        if (!ground) {
            ground = SDL_LoadBMP(CANVAS_TOPDOWN_BATTLEFIELD_BMP);
            convert_to_screen(g, &ground);
            blend_vertical_seam(ground);
        }
        build_ground_mips(g, ground);
        if (ground != g->topdown_battlefield) SDL_FreeSurface(ground);
    }
    g->assets_loaded |= fresh;
}

static unsigned asset_groups_for_scene(const Game *g, Scene scene)
{
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    switch (scene) {
    case SCENE_TITLE: case SCENE_MENU: case SCENE_DAWN: case SCENE_SETTINGS:
        return AG_TITLE;
    case SCENE_STORY:
        return AG_JOURNAL | AG_LOSS | AG_CARDS | AG_NEWS | AG_TITLE |
               AG_CHAPTER | AG_TARGETS;
    case SCENE_BRIEFING: case SCENE_EPILOGUE: return AG_BOARD;
    case SCENE_DEBRIEF: return AG_BOARD | AG_DEBRIEF;
    case SCENE_AWARD: return AG_JOURNAL | AG_MEDALS;
    case SCENE_SCRAPBOOK: return AG_NEWS;
    case SCENE_MESS: return AG_MESS;
    case SCENE_CONDUCT: return AG_MEDALS | AG_OFFICE;
    case SCENE_COURT_MARTIAL: return AG_JOURNAL;
    case SCENE_CINEMA: return AG_CINEMA;
    case SCENE_FILM: return 0;
    case SCENE_REMEMBER: return AG_REMEMBER;
    case SCENE_SORTIE:
        if (mode == MISSION_SIDE)
            return AG_SIDE | AG_SKY | AG_EXPLODE | AG_RIFLEMEN;
        if (mode == MISSION_TOP_DOWN)
            return AG_TOPDOWN | AG_TARGETS | AG_EXPLODE | AG_RIFLEMEN;
        if (mode == MISSION_DOGFIGHT_3D)
            return AG_COCKPIT | AG_GROUND | AG_SKY | AG_EXPLODE;
        return AG_GUNNER | AG_CHAPTER | AG_EXPLODE |
               (mission_armed(g) ? AG_LEWIS :
                personal_arms(g) ? AG_CARBINE : 0u);
    }
    return 0;
}

static void ensure_scene_assets(Game *g, Scene scene)
{
    unsigned want = asset_groups_for_scene(g, scene);
    if (!g->cache_in_ram) {
        free_asset_groups(g, g->assets_loaded & ~want);
        if (scene != SCENE_CINEMA) newsreel_free(&g->newsreel);
    }
    load_asset_groups(g, want);
}

/* Free fast memory, from exec.library: the settings choose their default
 * from it, and the settings page shows it. */
#if defined(__amigaos__)
extern void *SysBase;   /* exec.library, opened by the startup code */
#endif
static unsigned long exec_avail_mem(unsigned long requirements)
{
#if defined(__amigaos__)
    register unsigned long d0 __asm__("d0");
    register unsigned long d1 __asm__("d1") = requirements;
    register void *a6 __asm__("a6") = SysBase;
    __asm__ volatile ("jsr -216(%%a6)"
                      : "=r"(d0), "+r"(d1)
                      : "r"(a6)
                      : "a0", "a1", "cc", "memory");
    return d0;
#else
    (void)requirements;
    return 0;
#endif
}
#define MEMF_ANY_BITS 0ul
#define MEMF_FAST_BITS 4ul

/* ---- The officer's preferences --------------------------------------------
 *
 * Kept apart from the campaign in CanvasSkies.prefs, so an old save is
 * never touched: a line naming the file, then "name value" pairs. */
static void load_prefs(Game *g)
{
    FILE *f = fopen(CANVAS_PREFS_PATH, "r");
    char name[32];
    int version, value;
    if (!f) return;
    if (fscanf(f, "CANVAS_SKIES_PREFS %d", &version) == 1) {
        while (fscanf(f, "%31s %d", name, &value) == 2) {
            if (value < 0) value = 0;
            if (strcmp(name, "cache") == 0) g->cache_in_ram = value != 0;
            else if (strcmp(name, "music") == 0)
                g->music_volume = value > 10 ? 10 : value;
            else if (strcmp(name, "effects") == 0)
                g->effects_volume = value > 10 ? 10 : value;
        }
    }
    fclose(f);
}

static void save_prefs(const Game *g)
{
    FILE *f = fopen(CANVAS_PREFS_PATH, "w");
    if (!f) return;
    fprintf(f, "CANVAS_SKIES_PREFS 1\ncache %d\nmusic %d\neffects %d\n",
            g->cache_in_ram, g->music_volume, g->effects_volume);
    fclose(f);
}

static void apply_volumes(Game *g)
{
    if (!g->audio.device) return;
    SDL_LockAudioDevice(g->audio.device);
    g->audio.music_gain = (float)g->music_volume / 10.0f;
    g->audio.effects_gain = (float)g->effects_volume / 10.0f;
    SDL_UnlockAudioDevice(g->audio.device);
}

#define SETTINGS_ROWS 5

static void draw_settings_slider(Game *g, int x, int y, int value, int selected)
{
    SDL_Surface *s = g->screen;
    pix_t ink = PIX_PACK(36, 30, 26);
    int k;
    rect(s, x, y + 7, 200, 2, ink);
    for (k = 0; k <= 10; ++k)
        rect(s, x + k * 20, y + (k % 5 == 0 ? 2 : 4), 1, k % 5 == 0 ? 12 : 8,
             ink);
    /* A brass slide on the rule. */
    rect(s, x + value * 20 - 5, y, 11, 16, PIX_PACK(24, 18, 14));
    rect(s, x + value * 20 - 4, y + 1, 9, 14,
         selected ? PIX_PACK(214, 168, 80) : PIX_PACK(160, 124, 62));
}

static void draw_settings(Game *g)
{
    static const char *const names[SETTINGS_ROWS] = {
        "KEEP THE ART IN MEMORY", "MUSIC", "EFFECTS",
        "FULL SCREEN  (F9)", "RETURN TO THE OFFICERS' MENU"
    };
    SDL_Surface *s = g->screen;
    pix_t ink = PIX_PACK(36, 30, 26), red = PIX_PACK(140, 34, 26);
    pix_t faint = PIX_PACK(112, 92, 66);
    char buf[80];
    unsigned long now_free = exec_avail_mem(MEMF_FAST_BITS);
    int i, w;
    draw_title(g);
    blend_rect(s, 0, 0, SCREEN_W, SCREEN_H, 5, 7, 10, 150);
    /* A sheet of buff foolscap on the adjutant's desk. */
    rect(s, 106, 34, 436, 336, PIX_PACK(14, 10, 8));
    rect(s, 100, 28, 436, 336, PIX_PACK(222, 210, 178));
    blend_rect(s, 100, 28, 436, 336, 180, 150, 90, 40);
    for (i = 0; i < 9; ++i)                     /* foxing on the old paper */
        blend_circle(s, 120 + (int)(scene_hash((uint32_t)i * 77u) % 400u),
                     44 + (int)(scene_hash((uint32_t)i * 131u) % 300u),
                     6 + i % 5, 150, 112, 60, 40);
    rect(s, 114, 42, 408, 1, PIX_PACK(150, 120, 80));
    rect(s, 114, 350, 408, 1, PIX_PACK(150, 120, 80));
    w = display_width("THE OFFICER'S PREFERENCES", 23);
    display_text(g, 318 - w / 2, 50, "THE OFFICER'S PREFERENCES", 16, 23, ink);
    w = typewriter_width("UP / DOWN  CHOOSE    LEFT / RIGHT  ALTER    "
                         "ESCAPE  RETURN", 11);
    typewriter_text(g, 318 - w / 2, 80,
                    "UP / DOWN  CHOOSE    LEFT / RIGHT  ALTER    "
                    "ESCAPE  RETURN", 7, 11, red);
    rect(s, 130, 96, 376, 1, ink);
    for (i = 0; i < SETTINGS_ROWS; ++i) {
        int y = 112 + i * 40;
        int selected = i == g->settings_row;
        if (selected) blend_rect(s, 116, y - 6, 404, 32, 150, 110, 50, 60);
        display_text(g, 130, y, names[i], 12, 18, selected ? red : ink);
        if (i == 0) {
            typewriter_text(g, 400, y + 4, g->cache_in_ram ? "ON" : "OFF",
                            7, 11, ink);
        } else if (i == 1 || i == 2) {
            int v = i == 1 ? g->music_volume : g->effects_volume;
            draw_settings_slider(g, 290, y + 2, v, selected);
            snprintf(buf, sizeof(buf), "%d", v);
            typewriter_text(g, 502, y + 4, buf, 7, 11, ink);
        } else if (i == 3) {
            typewriter_text(g, 400, y + 4, g->fullscreen ? "ON" : "OFF",
                            7, 11, ink);
        }
    }
    typewriter_text(g, 130, 300, g->cache_in_ram ?
                    "ART AND RECORDS ARE KEPT ONCE USED: FOR LARGE MEMORIES" :
                    "EACH SCENE LOADS ITS OWN ART AND LETS IT GO AFTER",
                    7, 11, faint);
    if (now_free) {
        unsigned long used = g->fast_at_start > now_free ?
                             g->fast_at_start - now_free : 0;
        snprintf(buf, sizeof(buf),
                 "FAST MEMORY FREE %lu.%lu MB    THE GAME HOLDS %lu.%lu MB",
                 now_free >> 20, ((now_free >> 10) & 1023) * 10 / 1024,
                 used >> 20, ((used >> 10) & 1023) * 10 / 1024);
        typewriter_text(g, 130, 322, buf, 7, 11, faint);
    }
}

static void handle_settings_key(Game *g, SDL_Keycode key)
{
    int step = 0;
    switch (key) {
    case SDLK_UP:
        g->settings_row = (g->settings_row + SETTINGS_ROWS - 1) % SETTINGS_ROWS;
        break;
    case SDLK_DOWN:
        g->settings_row = (g->settings_row + 1) % SETTINGS_ROWS;
        break;
    case SDLK_LEFT: step = -1; break;
    case SDLK_RIGHT: step = 1; break;
    case SDLK_RETURN: case SDLK_KP_ENTER: case SDLK_SPACE:
        step = g->settings_row >= 3 || g->settings_row == 0 ? 1 : 0;
        if (g->settings_row == 4) {
            set_scene(g, g->settings_from);
            return;
        }
        break;
    case SDLK_ESCAPE:
        set_scene(g, g->settings_from);
        return;
    case SDLK_F9:
        toggle_fullscreen(g);
        break;
    default:
        return;
    }
    if (step) {
        switch (g->settings_row) {
        case 0:
            g->cache_in_ram = !g->cache_in_ram;
            /* Turned off, what this page does not show goes at once. */
            ensure_scene_assets(g, SCENE_SETTINGS);
            ensure_music(g, music_mood_for_scene(g, SCENE_SETTINGS));
            break;
        case 1:
            g->music_volume += step;
            if (g->music_volume < 0) g->music_volume = 0;
            if (g->music_volume > 10) g->music_volume = 10;
            apply_volumes(g);   /* the march below is the test */
            break;
        case 2:
            g->effects_volume += step;
            if (g->effects_volume < 0) g->effects_volume = 0;
            if (g->effects_volume > 10) g->effects_volume = 10;
            apply_volumes(g);
            g->audio.crack = 2600;  /* a rifle shot to judge it by */
            break;
        case 3:
            toggle_fullscreen(g);
            break;
        }
        save_prefs(g);
    }
    g->redraw_static = 1;
}

static int game_init(Game *g)
{
    memset(g, 0, sizeof(*g));
    g->running = 1;
    g->lives = 3;
    g->morale = 70;     /* a cheerful mess until something sours it */
    /* A fit flight for the test entries; a campaign sets its own. */
    g->wingman_health[0] = g->wingman_health[1] = g->wingman_health[2] = 100;
    /* With room to spare the art stays once loaded; otherwise each scene
     * holds only its own.  The officer's preferences may say otherwise. */
    g->fast_at_start = exec_avail_mem(MEMF_FAST_BITS);
    g->cache_in_ram = g->fast_at_start >= 96ul * 1024ul * 1024ul;
    g->music_volume = 10;
    g->effects_volume = 10;
    load_prefs(g);
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
    /* The haze of the far fields comes from the top-down painting; it is
     * measured now, and the painting loaded again when a scene shows it. */
    measure_ground_tint(g);
    /* An 8-bit coverage map: it is read directly, never converted. */
    g->fonts = SDL_LoadBMP(CANVAS_FONTS_BMP);
    if (g->fonts && g->fonts->format->BytesPerPixel != 1) {
        SDL_FreeSurface(g->fonts);
        g->fonts = NULL;
    }
    if (!g->fonts)
        fprintf(stderr, "The font atlas is unavailable\n");
    g->player_sprites = SDL_LoadBMP(CANVAS_PLAYER_SPRITES_BMP);
    g->enemy_sprites = SDL_LoadBMP(CANVAS_ENEMY_SPRITES_BMP);
    {
        static const char *const side[4] = {
            CANVAS_PLAYER_TWOSEAT_BMP, CANVAS_PLAYER_LATE_BMP,
            CANVAS_ENEMY_MONO_BMP, CANVAS_ENEMY_LATE_BMP
        };
        int k;
        for (k = 0; k < 4; ++k) g->machine_sprites[k] = SDL_LoadBMP(side[k]);
    }
    if (!g->player_sprites || !g->enemy_sprites)
        fprintf(stderr, "Aircraft sprite atlas unavailable; using vector fallback\n");
    {
        /* The aircraft atlases are read pixel by pixel every frame.
         * Holding them in the screen's format lets the renderer read
         * them directly instead of decoding 24-bit BMP pixels through SDL. */
        SDL_Surface **atlases[] = {
            &g->player_sprites, &g->enemy_sprites,
            &g->machine_sprites[0], &g->machine_sprites[1],
            &g->machine_sprites[2], &g->machine_sprites[3]
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
    prepare_sky(g);
    open_joystick(g);
    init_audio(g);
    set_scene(g, SCENE_TITLE);
    return 1;
}

/* ---- Conduct ----------------------------------------------------------- */
static const char *offence_words(int kind)
{
    return kind == CIV_AMBULANCE ? "FIRED ON A MOTOR AMBULANCE" :
           kind == CIV_OWN_LINE ? "BOMBED OUR OWN FRONT LINE" :
                                  "FIRED ON CIVILIANS AND THEIR PROPERTY";
}

/* An office plate: aged paper, a border, a crest of crown and laurel drawn
 * in ink, and a red rubber stamp. */
static void draw_office_paper(Game *g, const char *heading, const char *sub,
                              const char *stamp)
{
    SDL_Surface *s = g->screen;
    pix_t ink = PIX_PACK(36, 30, 26), red = PIX_PACK(140, 34, 26);
    int k, w;
    if (g->office_paper) {
        /* Foolscap on the adjutant's desk (make_office_paper.py). */
        SDL_BlitSurface(g->office_paper, NULL, s, NULL);
    } else {
        rect(s, 0, 0, SCREEN_W, SCREEN_H, PIX_PACK(30, 22, 16));
        rect(s, 96, 30, 460, 344, PIX_PACK(14, 10, 8));
        rect(s, 90, 24, 460, 344, PIX_PACK(222, 210, 178));
        blend_rect(s, 90, 24, 460, 344, 180, 150, 90, 40);
        for (k = 0; k < 9; ++k)                 /* foxing on the old paper */
            blend_circle(s, 110 + (int)(scene_hash((uint32_t)k * 77u) % 420u),
                         40 + (int)(scene_hash((uint32_t)k * 131u) % 310u),
                         6 + k % 5, 150, 112, 60, 40);
        rect(s, 104, 38, 432, 1, PIX_PACK(150, 120, 80));
        rect(s, 104, 353, 432, 1, PIX_PACK(150, 120, 80));
    }
    /* The crest, printed in black. */
    if (g->medals) {
        SDL_Rect src = {5 * 72, 8, 72, 64}, dst = {284, 28, 72, 64};
        SDL_BlitSurface(g->medals, &src, s, &dst);
    }
    w = display_width(heading, 23);
    display_text(g, 320 - w / 2, 80, heading, 16, 23, ink);
    w = typewriter_width(sub, 11);
    typewriter_text(g, 320 - w / 2, 108, sub, 7, 11, red);
    rect(s, 120, 126, 400, 1, ink);
    if (stamp && g->office_stamp) {
        /* Struck a little crooked: the frame inked unevenly, the words,
         * then the specks where the ink failed to take. */
        int sw;
        SDL_Rect src, dst;
        w = typewriter_width(stamp, 15);
        sw = w + 30 > 300 ? 300 : w + 30;
        /* The frame's left part to the width of the words, then its
         * right-hand end, from the 300-wide stamp. */
        src.x = 0; src.y = 0; src.w = sw - 10; src.h = 40;
        dst.x = 366; dst.y = 312; dst.w = src.w; dst.h = 40;
        SDL_BlitSurface(g->office_stamp, &src, s, &dst);
        src.x = 290; src.w = 10; dst.x = 366 + sw - 10;
        SDL_BlitSurface(g->office_stamp, &src, s, &dst);
        typewriter_text(g, 381, 324, stamp, 10, 15, red);
        src.x = 300; src.w = sw - 10; dst.x = 366;
        SDL_BlitSurface(g->office_stamp, &src, s, &dst);
        src.x = 590; src.w = 10; dst.x = 366 + sw - 10;
        SDL_BlitSurface(g->office_stamp, &src, s, &dst);
    } else if (stamp) {
        w = typewriter_width(stamp, 15);
        rect(s, 372, 318, w + 20, 30, red);
        rect(s, 375, 321, w + 14, 24, PIX_PACK(222, 204, 170));
        blend_rect(s, 375, 321, w + 14, 24, 160, 40, 30, 50);
        typewriter_text(g, 382, 325, stamp, 10, 15, red);
    }
}

static void draw_conduct(Game *g)
{
    char buf[80];
    pix_t ink = PIX_PACK(36, 30, 26), red = PIX_PACK(140, 34, 26);
    int second = g->offences >= 2;
    draw_office_paper(g, "MEMORANDUM",
                      "FROM THE OFFICER COMMANDING THE SQUADRON",
                      "ON THE RECORD");
    snprintf(buf, sizeof(buf), "TO:    %s N. SEAGOON", rank_short[g->rank & 3]);
    typewriter_text(g, 124, 140, buf, 7, 11, ink);
    snprintf(buf, sizeof(buf), "DATE:  %s", mission_date(g));
    typewriter_text(g, 124, 156, buf, 7, 11, ink);
    snprintf(buf, sizeof(buf), "YOU ARE REPORTED TO HAVE %s.",
             offence_words(g->sortie_offence_kind));
    wrapped_text_limit(g, 124, 182, buf, 56, 16, 1000, 1, ink);
    typewriter_text(g, 124, 216, "THE MARKINGS WERE PLAIN AND THE ORDERS WERE PLAIN.",
                    7, 11, ink);
    if (!second) {
        typewriter_text(g, 124, 244, "A REPRIMAND IS ENTERED ON YOUR RECORD.",
                        7, 11, red);
        typewriter_text(g, 124, 262, "NO RECOMMENDATION FOR AN AWARD WILL BE MADE.",
                        7, 11, red);
    } else if (g->rank_penalty > 0 && !g->grounded_next) {
        typewriter_text(g, 124, 244, "A SECOND OFFENCE. YOU REVERT ONE RANK.",
                        7, 11, red);
        typewriter_text(g, 124, 262, "A THIRD WILL BE A COURT MARTIAL.", 7, 11,
                        red);
    } else {
        typewriter_text(g, 124, 244, "A SECOND OFFENCE. YOU ARE GROUNDED", 7, 11,
                        red);
        typewriter_text(g, 124, 262, "FOR THE NEXT SORTIE. A THIRD WILL BE A COURT",
                        7, 11, red);
        typewriter_text(g, 124, 278, "MARTIAL.", 7, 11, red);
    }
    typewriter_text(g, 124, 300, "MAJOR, COMMANDING", 7, 11, ink);
    typewriter_text(g, 320 - typewriter_width("ENTER", 11) / 2, 378, "ENTER",
                    7, 11, PIX_PACK(200, 180, 140));
}

static void draw_court_martial(Game *g)
{
    SDL_Surface *s = g->screen;
    pix_t ink = PIX_PACK(36, 30, 26), red = PIX_PACK(140, 34, 26);
    char buf[80];
    if (g->court_martial_stage == 0) {
        snprintf(buf, sizeof(buf), "HELD IN THE FIELD, %s", mission_date(g));
        draw_office_paper(g, "GENERAL COURT MARTIAL", buf, "CASHIERED");
        snprintf(buf, sizeof(buf), "ACCUSED:  %s N. SEAGOON, %s",
                 rank_short[g->rank & 3], corps_short(g));
        typewriter_text(g, 124, 140, buf, 7, 11, ink);
        typewriter_text(g, 124, 164, "CHARGE:   CONDUCT TO THE PREJUDICE OF GOOD", 7,
                        11, ink);
        typewriter_text(g, 124, 180, "          ORDER AND MILITARY DISCIPLINE, IN",
                        7, 11, ink);
        typewriter_text(g, 124, 196, "          THAT, HAVING BEEN WARNED, HE FIRED",
                        7, 11, ink);
        typewriter_text(g, 124, 212, "          A THIRD TIME ON WHAT HE WAS BOUND",
                        7, 11, ink);
        typewriter_text(g, 124, 228, "          TO SPARE.", 7, 11, ink);
        typewriter_text(g, 124, 252, "PLEA:     NOT GUILTY.", 7, 11, ink);
        typewriter_text(g, 124, 270, "FINDING:  GUILTY.", 7, 11, red);
        typewriter_text(g, 124, 288, "SENTENCE: TO BE CASHIERED.", 7, 11, red);
        typewriter_text(g, 124, 304, "CONFIRMED BY THE COMMANDER-IN-CHIEF.", 7, 11,
                        ink);
    } else if (g->court_martial_stage == 1) {
        /* The last page of the diary. */
        rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 34, 24, 17));
        if (g->journal) SDL_BlitSurface(g->journal, NULL, s, NULL);
        draw_journal_rules(g);
        journal_text(g, 110, journal_line_y(JOURNAL_LEFT, 0), "The Last Page",
                     red);
        journal_paragraph(g, 110, journal_line_y(JOURNAL_LEFT, 4),
                          "They took my wings this morning. Tom would not "
                          "look at me.", 24, ink);
        journal_paragraph(g, 338, journal_line_y(JOURNAL_RIGHT, 0),
                          "I was warned, and I did it again. There is no one "
                          "to blame but the man who writes this.", 25, ink);
        journal_paragraph(g, 338, journal_line_y(JOURNAL_RIGHT, 10),
                          "I go home tomorrow on the leave boat, a civilian.",
                          25, ink);
    } else {
        rect(s, 0, 0, SCREEN_W, SCREEN_H, PIX_PACK(30, 22, 16));
        rect(s, 156, 90, 330, 200, PIX_PACK(16, 12, 10));
        rect(s, 150, 84, 330, 200, PIX_PACK(222, 210, 178));
        font_text(g, FONT_NEWS_ITALIC, 166, 108, "The Western Courier", ink);
        rect(s, 164, 114, 302, 1, ink);
        font_text(g, FONT_NEWS_HEAD, 315 - font_width(FONT_NEWS_HEAD,
                  "AIRMAN CASHIERED.") / 2, 146, "AIRMAN CASHIERED.", red);
        font_text(g, FONT_NEWS_BODY, 166, 176,
                  "A GENERAL COURT MARTIAL IN FRANCE", ink);
        font_text(g, FONT_NEWS_BODY, 166, 194,
                  "HAS SENTENCED AN OFFICER OF THE", ink);
        font_text(g, FONT_NEWS_BODY, 166, 212,
                  "FLYING SERVICE TO BE CASHIERED.", ink);
        font_text(g, FONT_NEWS_BODY, 166, 230,
                  "THE COURT SAT IN PRIVATE.", ink);
    }
    typewriter_text(g, 320 - typewriter_width("ENTER", 11) / 2, 378, "ENTER",
                    7, 11, PIX_PACK(200, 180, 140));
}

/* ---- The picture house ---------------------------------------------------
 *
 * On a chapter's first morning the squadron goes to the Y.M.C.A. hut
 * behind the lines, where the week's reels are shown: real film of the
 * period (see CANVAS_SKIES_SOURCES.md), two reels matched to the chapter,
 * each after its title card, with the pianist in the pit.  The reels are
 * read one at a time from newsreels.bin (make_cinema_reels.py): a key
 * frame of 4-bit greys, then the 8x8 blocks that change, 8 frames a
 * second, projected half as large again in sepia, with the flicker and
 * scratches of a well-worn print. */
#define REEL_W 192
#define REEL_H 120
#define REEL_FPS_MS 125u
#define REEL_TITLE_MS 1600u
#define CINEMA_SCREEN_X 176
#define CINEMA_SCREEN_Y 70

static int cinema_reels_present(void)
{
    FILE *f = fopen(CANVAS_NEWSREELS_BIN, "rb");
    if (!f) return 0;
    fclose(f);
    return 1;
}

static void newsreel_free(Newsreel *r)
{
    free(r->data);
    r->data = NULL;
    r->len = 0;
}

/* Reads reel 'index' of the file and shows its key frame. */
static int newsreel_load(Newsreel *r, int index)
{
    FILE *f = fopen(CANVAS_NEWSREELS_BIN, "rb");
    uint8_t head[6], entry[48];
    uint32_t offset, len;
    int count, k;
    newsreel_free(r);
    if (!f) return 0;
    if (fread(head, 1, 6, f) != 6 || memcmp(head, "CSNR", 4) != 0) {
        fclose(f);
        return 0;
    }
    count = (head[4] << 8) | head[5];
    if (index < 0 || index >= count ||
        fseek(f, 6L + (long)index * 48L, SEEK_SET) != 0 ||
        fread(entry, 1, 48, f) != 48) {
        fclose(f);
        return 0;
    }
    offset = ((uint32_t)entry[0] << 24) | ((uint32_t)entry[1] << 16) |
             ((uint32_t)entry[2] << 8) | entry[3];
    len = ((uint32_t)entry[4] << 24) | ((uint32_t)entry[5] << 16) |
          ((uint32_t)entry[6] << 8) | entry[7];
    memcpy(r->title, entry + 8, 40);
    r->title[40] = '\0';
    r->data = (uint8_t *)malloc(len);
    if (!r->data || len < 2 + REEL_W * REEL_H / 2 ||
        fseek(f, (long)offset, SEEK_SET) != 0 ||
        fread(r->data, 1, len, f) != len) {
        fclose(f);
        newsreel_free(r);
        return 0;
    }
    fclose(f);
    r->len = len;
    r->frames = (r->data[0] << 8) | r->data[1];
    for (k = 0; k < REEL_W * REEL_H / 2; ++k) {
        r->pix[k * 2] = r->data[2 + k] >> 4;
        r->pix[k * 2 + 1] = r->data[2 + k] & 15;
    }
    r->cursor = 2 + REEL_W * REEL_H / 2;
    r->shown = 1;
    r->index = index;
    r->started = SDL_GetTicks();
    return 1;
}

/* Applies the changed blocks of each frame due by now. */
static void newsreel_advance(Newsreel *r, uint32_t now)
{
    uint32_t due;
    if (!r->data || now < r->started + REEL_TITLE_MS) return;
    due = (now - r->started - REEL_TITLE_MS) / REEL_FPS_MS + 1;
    while ((uint32_t)r->shown < due && r->shown < r->frames &&
           r->cursor + 45 <= r->len) {
        const uint8_t *bits = r->data + r->cursor;
        int k;
        r->cursor += 45;
        for (k = 0; k < (REEL_W / 8) * (REEL_H / 8); ++k) {
            int bx, by, y, x;
            const uint8_t *b;
            if (!(bits[k >> 3] & (0x80 >> (k & 7)))) continue;
            if (r->cursor + 32 > r->len) return;
            b = r->data + r->cursor;
            r->cursor += 32;
            bx = (k % (REEL_W / 8)) * 8;
            by = (k / (REEL_W / 8)) * 8;
            for (y = 0; y < 8; ++y)
                for (x = 0; x < 8; x += 2) {
                    uint8_t v = b[y * 4 + x / 2];
                    r->pix[(by + y) * REEL_W + bx + x] = v >> 4;
                    r->pix[(by + y) * REEL_W + bx + x + 1] = v & 15;
                }
        }
        ++r->shown;
    }
}

static int newsreel_done(const Newsreel *r, uint32_t now)
{
    return !r->data || (r->shown >= r->frames &&
           now >= r->started + REEL_TITLE_MS +
                  (uint32_t)r->frames * REEL_FPS_MS + 400u);
}

#define CINEMA_REELS_PER_CHAPTER 2
static void cinema_start(Game *g)
{
    g->cinema_reel_slot = 0;
    if (!newsreel_load(&g->newsreel, g->chapter * CINEMA_REELS_PER_CHAPTER))
        newsreel_free(&g->newsreel);
}

static void update_cinema(Game *g)
{
    uint32_t now = SDL_GetTicks();
    newsreel_advance(&g->newsreel, now);
    if (newsreel_done(&g->newsreel, now) &&
        g->cinema_reel_slot + 1 < CINEMA_REELS_PER_CHAPTER && g->newsreel.data) {
        ++g->cinema_reel_slot;
        newsreel_load(&g->newsreel, g->chapter * CINEMA_REELS_PER_CHAPTER +
                                    g->cinema_reel_slot);
    }
}

static void draw_cinema(Game *g)
{
    SDL_Surface *s = g->screen;
    Newsreel *r = &g->newsreel;
    uint32_t now = SDL_GetTicks();
    int x, y;
    if (g->cinema_hut) SDL_BlitSurface(g->cinema_hut, NULL, s, NULL);
    else rect(s, 0, 0, SCREEN_W, SCREEN_H, PIX_PACK(20, 16, 12));
    if (!r->data || now < r->started + REEL_TITLE_MS ||
        newsreel_done(r, now)) {
        /* The title card, white lettering on black, or the end of the
         * programme. */
        const char *title = r->data && !newsreel_done(r, now) ? r->title :
                            "THAT IS ALL FOR TONIGHT";
        rect(s, CINEMA_SCREEN_X, CINEMA_SCREEN_Y, REEL_W * 3 / 2,
             REEL_H * 3 / 2, PIX_PACK(14, 12, 10));
        rect(s, CINEMA_SCREEN_X + 10, CINEMA_SCREEN_Y + 10, REEL_W * 3 / 2 - 20,
             1, PIX_PACK(200, 190, 170));
        rect(s, CINEMA_SCREEN_X + 10, CINEMA_SCREEN_Y + REEL_H * 3 / 2 - 11,
             REEL_W * 3 / 2 - 20, 1, PIX_PACK(200, 190, 170));
        /* A long title drops to the smaller face to stay on the screen. */
        FontId face = font_width(FONT_SUBHEAD, title) > REEL_W * 3 / 2 - 24 ?
                      FONT_LABEL : FONT_SUBHEAD;
        font_text(g, face, 320 - font_width(face, title) / 2,
                  CINEMA_SCREEN_Y + 96, title, PIX_PACK(226, 218, 200));
    } else {
        /* The frame, half as large again, in sepia, the lamp flickering. */
        pix_t lut[16];
        int flicker = (int)(scene_hash(now / 90u) % 18u);
        for (x = 0; x < 16; ++x) {
            /* The screen is the brightest thing in the hall: the 16 grey
             * steps run from a lifted black to full white on an eased
             * curve, so shadows keep their detail. */
            int v = 14 + (int)(241.0f * powf((float)x / 15.0f, 0.8f)) - flicker;
            if (v < 0) v = 0;
            if (v > 255) v = 255;
            lut[x] = PIX_PACK(v * 236 / 240, v * 214 / 240, v * 172 / 240);
        }
        if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
        for (y = 0; y < REEL_H * 3 / 2; ++y) {
            pix_t *row = PIX_ROW(s, CINEMA_SCREEN_Y + y) + CINEMA_SCREEN_X;
            const uint8_t *src = r->pix + (y * 2 / 3) * REEL_W;
            for (x = 0; x < REEL_W * 3 / 2; ++x)
                row[x] = lut[src[x * 2 / 3]];
        }
        if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
        for (x = 0; x < 2; ++x) {               /* scratches on the print */
            uint32_t h = scene_hash(now / 125u * 7u + (uint32_t)x);
            if (h % 5u == 0u)
                rect(s, CINEMA_SCREEN_X + (int)(h % (REEL_W * 3 / 2)),
                     CINEMA_SCREEN_Y, 1, REEL_H * 3 / 2,
                     PIX_PACK(232, 224, 200));
        }
    }
    typewriter_text(g, 320 - typewriter_width("ENTER", 11) / 2, 384, "ENTER",
                    7, 11, PIX_PACK(226, 206, 166));
}

/* ---- The aerodrome films -------------------------------------------------
 *
 * Before each sortie a take-off, after it a landing, shown full screen on
 * old film with their own sound (make_airfield_films.py).  One film is
 * held at a time, and Enter or fire skips it. */
enum {
    FILM_DAWN, FILM_FLIGHT_OF_THREE, FILM_MUD, FILM_1914_OBSERVER,
    FILM_DUSK, FILM_SHOT_UP, FILM_DEAD_STICK, FILM_RAIN
};

static uint32_t be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | p[3];
}

static void film_free(Game *g)
{
    AirfieldFilm *f = &g->film;
    if (g->audio.device) {
        SDL_LockAudioDevice(g->audio.device);
        g->audio.film_sound = NULL;
        g->audio.film_len = g->audio.film_pos = 0;
        SDL_UnlockAudioDevice(g->audio.device);
    }
    free(f->data);
    free(f->pix);
    free(f->sound);
    memset(f, 0, sizeof(*f));
}

static int film_load(Game *g, int index)
{
    AirfieldFilm *f = &g->film;
    FILE *file = fopen(CANVAS_AIRFIELD_FILMS_BIN, "rb");
    uint8_t head[16], entry[56];
    uint32_t pic_at, pic_len, snd_at, snd_len;
    int k, count;
    film_free(g);
    if (!file) return 0;
    if (fread(head, 1, 16, file) != 16 || memcmp(head, "CSAF", 4) != 0) {
        fclose(file);
        return 0;
    }
    f->w = (head[6] << 8) | head[7];
    f->h = (head[8] << 8) | head[9];
    f->fps = (head[10] << 8) | head[11];
    count = (head[14] << 8) | head[15];
    if (index < 0 || index >= count || f->w <= 0 || f->h <= 0 ||
        f->w % 8 || f->h % 8 || f->fps <= 0 ||
        fseek(file, 16L + (long)index * 56L, SEEK_SET) != 0 ||
        fread(entry, 1, 56, file) != 56) {
        fclose(file);
        return 0;
    }
    pic_at = be32(entry); pic_len = be32(entry + 4);
    snd_at = be32(entry + 8); snd_len = be32(entry + 12);
    f->data = (uint8_t *)malloc(pic_len);
    f->pix = (uint8_t *)malloc((size_t)f->w * f->h);
    f->sound = snd_len ? (int16_t *)malloc(snd_len) : NULL;
    if (!f->data || !f->pix || (snd_len && !f->sound) ||
        pic_len < 2u + (uint32_t)(f->w * f->h / 2) ||
        fseek(file, (long)pic_at, SEEK_SET) != 0 ||
        fread(f->data, 1, pic_len, file) != pic_len ||
        (snd_len && (fseek(file, (long)snd_at, SEEK_SET) != 0 ||
                     fread(f->sound, 1, snd_len, file) != snd_len))) {
        fclose(file);
        film_free(g);
        return 0;
    }
    fclose(file);
    f->len = pic_len;
    f->sound_len = snd_len / 2;
    f->frames = (f->data[0] << 8) | f->data[1];
    for (k = 0; k < f->w * f->h / 2; ++k) {
        f->pix[k * 2] = f->data[2 + k] >> 4;
        f->pix[k * 2 + 1] = f->data[2 + k] & 15;
    }
    f->cursor = 2u + (uint32_t)(f->w * f->h / 2);
    f->shown = 1;
    f->index = index;
    f->started = SDL_GetTicks();
    if (g->audio.device && f->sound) {
        SDL_LockAudioDevice(g->audio.device);
        g->audio.film_sound = f->sound;
        g->audio.film_len = f->sound_len;
        g->audio.film_pos = 0;
        SDL_UnlockAudioDevice(g->audio.device);
    }
    return 1;
}

/* Applies the changed blocks of each frame due by now. */
static void film_advance(AirfieldFilm *f, uint32_t now)
{
    int bw = f->w / 8, blocks = (f->w / 8) * (f->h / 8);
    uint32_t map = (uint32_t)(blocks + 7) / 8;
    uint32_t due;
    if (!f->data) return;
    due = (now - f->started) * (uint32_t)f->fps / 1000u + 1u;
    while ((uint32_t)f->shown < due && f->shown < f->frames &&
           f->cursor + map <= f->len) {
        const uint8_t *bits = f->data + f->cursor;
        int k;
        f->cursor += map;
        for (k = 0; k < blocks; ++k) {
            int bx, by, y, x;
            const uint8_t *b;
            if (!(bits[k >> 3] & (0x80 >> (k & 7)))) continue;
            if (f->cursor + 32 > f->len) return;
            b = f->data + f->cursor;
            f->cursor += 32;
            bx = (k % bw) * 8;
            by = (k / bw) * 8;
            for (y = 0; y < 8; ++y)
                for (x = 0; x < 8; x += 2) {
                    uint8_t v = b[y * 4 + x / 2];
                    f->pix[(by + y) * f->w + bx + x] = v >> 4;
                    f->pix[(by + y) * f->w + bx + x + 1] = v & 15;
                }
        }
        ++f->shown;
    }
}

static int film_done(const AirfieldFilm *f, uint32_t now)
{
    return !f->data || (f->shown >= f->frames &&
           now >= f->started + (uint32_t)f->frames * 1000u /
                               (uint32_t)f->fps + 300u);
}

/* The take-off the day calls for. */
static int takeoff_film_for(const Game *g)
{
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    uint32_t h = scene_hash((uint32_t)(g->chapter * 16 + g->mission) +
                            0x5eedu);
    if (g->chapter == 0) return FILM_1914_OBSERVER;
    if (mode == MISSION_DOGFIGHT_3D) return FILM_FLIGHT_OF_THREE;
    return h % 3u == 0u ? FILM_MUD : FILM_DAWN;
}

/* The landing the sortie earned: shot about if the airframe is, a dead
 * stick if the tank ran dry, otherwise dusk or rain as the day goes. */
static int landing_film_for(const Game *g)
{
    uint32_t h = scene_hash((uint32_t)(g->chapter * 16 + g->mission) +
                            0x1a0du);
    if (g->fuel <= 0.0f) return FILM_DEAD_STICK;
    if (g->health < 50) return FILM_SHOT_UP;
    return h % 3u == 0u ? FILM_RAIN : FILM_DUSK;
}

static int film_start_for(Game *g, Scene from, Scene to)
{
    int index;
    if (g->qa_hold || g->qa_run || from == SCENE_FILM) return 0;
    if (from == SCENE_DAWN && to == SCENE_SORTIE)
        index = takeoff_film_for(g);
    else if (from == SCENE_SORTIE && to == SCENE_DEBRIEF &&
             g->health > 0 && !g->fatal_sortie && !g->flight_crashed &&
             g->tail_integrity > 0)
        index = landing_film_for(g);
    else
        return 0;
    g->film_next = to;
    set_scene(g, SCENE_FILM);
    if (!film_load(g, index)) {
        set_scene(g, to);
        return 1;
    }
    return 1;
}

static void film_end(Game *g)
{
    Scene next = g->film_next;
    film_free(g);
    set_scene(g, next);
}

static void update_film(Game *g)
{
    uint32_t now = SDL_GetTicks();
    film_advance(&g->film, now);
    if (film_done(&g->film, now)) film_end(g);
}

static void draw_film(Game *g)
{
    SDL_Surface *s = g->screen;
    AirfieldFilm *f = &g->film;
    uint32_t now = SDL_GetTicks();
    pix_t lut[16];
    int x, y, flicker = (int)(scene_hash(now / 70u) % 14u);
    if (!f->data || f->w * 2 != SCREEN_W || f->h * 2 != SCREEN_H) {
        rect(s, 0, 0, SCREEN_W, SCREEN_H, PIX_PACK(10, 8, 6));
        return;
    }
    for (x = 0; x < 16; ++x) {
        /* The picture-house lamp's curve, in the same sepia. */
        int v = 14 + (int)(241.0f * powf((float)x / 15.0f, 0.8f)) - flicker;
        if (v < 0) v = 0;
        if (v > 255) v = 255;
        lut[x] = PIX_PACK(v * 236 / 240, v * 214 / 240, v * 172 / 240);
    }
    if (SDL_MUSTLOCK(s)) SDL_LockSurface(s);
    for (y = 0; y < f->h; ++y) {
        pix_t *row = PIX_ROW(s, y * 2);
        const uint8_t *src = f->pix + y * f->w;
        for (x = 0; x < f->w; ++x)
            row[x * 2] = row[x * 2 + 1] = lut[src[x]];
        memcpy(PIX_ROW(s, y * 2 + 1), row, sizeof(pix_t) * SCREEN_W);
    }
    if (SDL_MUSTLOCK(s)) SDL_UnlockSurface(s);
}

/* ---- We remember ----------------------------------------------------------
 *
 * The very end: the old front in the summer of 1919, the trenches grassed
 * over and the poppies grown up through the wire, the camera drifting
 * slowly across it with nothing but the wind.  Over it, WE REMEMBER and
 * the roll: names given by people who have given to the Royal British
 * Legion, of someone of theirs who served, read from WeRemember.txt
 * beside the program so that the roll can grow without a new program.
 * One line a name; blank lines and lines starting with # are passed over;
 * UTF-8 or Latin-1, with accents folded to the plain letters the period
 * faces carry. */
#define REMEMBER_MAX_NAMES 4000
#define REMEMBER_NAME_LEN 72
#define REMEMBER_HEADING_MS 2500u      /* the heading fades in over this */
#define REMEMBER_SCROLL_AT 7000u       /* and the roll begins to rise */
#define REMEMBER_LINE 21               /* pixels from one name to the next */
#define REMEMBER_RISE 13               /* pixels a second */
#define REMEMBER_BAND_TOP 262          /* the shaded band for the roll */
#define REMEMBER_REST_Y 352            /* where the closing line rests */
#define REMEMBER_PAN_MS 150000u        /* to drift from one end to the other */
#define REMEMBER_POPPY_COUNT 34

#ifdef REMEMBER_LIFE_SPRITES
/* What moves over the field: each crosses the screen in period_ms, at
 * depth/32 of the near layer's pace, bobbing 'bobs' times a crossing. */
typedef struct {
    int sprite, y, bob, bobs, depth;
    uint32_t period_ms, frame_ms;
} RememberLife;
static const RememberLife remember_life[] = {
    {0, 288, 9, 5, 26, 38000u, 90u},           /* a tortoiseshell */
    {1, 306, 7, 7, 30, 47000u, 80u},           /* a cabbage white */
    {2, 274, 6, 6, 28, 33000u, 70u},           /* a common blue */
    {3, 138, 4, 3, 2, 120000u, 220u},          /* the lark, high up */
    {4, 230, 18, 2, 16, 26000u, 400u},         /* thistledown */
    {4, 190, 22, 2, 14, 31000u, 400u},
    {4, 255, 14, 3, 18, 22000u, 400u}
};
#define REMEMBER_LIFE_COUNT ((int)(sizeof(remember_life) / sizeof(remember_life[0])))
#endif

static const char *const remember_closing[2] = {
    "PLEASE CONSIDER A GIFT TO THE ROYAL BRITISH LEGION.",
    "BRITISHLEGION.ORG.UK/GET-INVOLVED/DONATE"
};

/* A Latin-1 letter to the plain one the faces have. */
static char fold_latin1(unsigned c)
{
    static const char *const from =
        "\xc0\xc1\xc2\xc3\xc4\xc5\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd1"
        "\xd2\xd3\xd4\xd5\xd6\xd8\xd9\xda\xdb\xdc\xdd"
        "\xe0\xe1\xe2\xe3\xe4\xe5\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf1"
        "\xf2\xf3\xf4\xf5\xf6\xf8\xf9\xfa\xfb\xfc\xfd\xff";
    static const char *const to =
        "AAAAAACEEEEIIIIN" "OOOOOOUUUUY"
        "aaaaaaceeeeiiiin" "oooooouuuuyy";
    const char *at;
    if (c < 0x80) return (char)c;
    at = c ? strchr(from, (int)c) : NULL;
    return at ? to[at - from] : '\0';
}

static void remember_load_names(Game *g)
{
    FILE *f = fopen(CANVAS_WE_REMEMBER_TXT, "rb");
    char line[256];
    if (!f) return;
    g->remember_names = (char **)calloc(REMEMBER_MAX_NAMES, sizeof(char *));
    if (!g->remember_names) {
        fclose(f);
        return;
    }
    while (g->remember_count < REMEMBER_MAX_NAMES &&
           fgets(line, sizeof(line), f)) {
        char name[REMEMBER_NAME_LEN + 1];
        const unsigned char *p = (const unsigned char *)line;
        int n = 0;
        if (p[0] == 0xef && p[1] == 0xbb && p[2] == 0xbf) p += 3;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == '#' || *p == '\r' || *p == '\n' || !*p) continue;
        while (*p && *p != '\r' && *p != '\n' && n < REMEMBER_NAME_LEN) {
            unsigned c = *p++;
            char out;
            /* Two-byte UTF-8 for the Latin-1 range; anything wider is
             * dropped, as the faces could not show it. */
            if (c >= 0xc0 && c < 0xe0 && (*p & 0xc0) == 0x80) {
                c = ((c & 0x1f) << 6) | (*p++ & 0x3f);
            } else if (c >= 0xe0) {
                while ((*p & 0xc0) == 0x80) ++p;
                continue;
            }
            out = fold_latin1(c);
            if (out >= 'a' && out <= 'z') out = (char)(out - 'a' + 'A');
            if (out >= 32 && out <= 126) name[n++] = out;
        }
        while (n > 0 && name[n - 1] == ' ') --n;
        if (!n) continue;
        name[n] = '\0';
        g->remember_names[g->remember_count] = (char *)malloc((size_t)n + 1);
        if (!g->remember_names[g->remember_count]) break;
        memcpy(g->remember_names[g->remember_count], name, (size_t)n + 1);
        ++g->remember_count;
    }
    fclose(f);
}

static void remember_start(Game *g)
{
    uint32_t len = 0;
    uint8_t *wind;
    g->scene_started = SDL_GetTicks();
    if (!g->remember_names) remember_load_names(g);
    if (!g->audio.device || g->audio.wind) return;
    wind = load_mono_wav(CANVAS_REMEMBER_WIND_WAV, &len);
    if (!wind) return;
    SDL_LockAudioDevice(g->audio.device);
    g->audio.wind = (const int16_t *)(void *)wind;
    g->audio.wind_len = len / 2;
    g->audio.wind_pos = 0;
    SDL_UnlockAudioDevice(g->audio.device);
}

static void remember_free(Game *g)
{
    int i;
    if (g->audio.device && g->audio.wind) {
        const int16_t *wind;
        SDL_LockAudioDevice(g->audio.device);
        wind = g->audio.wind;
        g->audio.wind = NULL;
        g->audio.wind_len = g->audio.wind_pos = 0;
        SDL_UnlockAudioDevice(g->audio.device);
        SDL_free((void *)wind);
    }
    for (i = 0; i < g->remember_count; ++i) free(g->remember_names[i]);
    free(g->remember_names);
    g->remember_names = NULL;
    g->remember_count = 0;
}

/* Lettering centred over the field, cream with a soft dark edge all
 * round so it reads over sky and poppies alike.  Fading, it thins into
 * the field rather than darkening. */
static void remember_line(Game *g, FontId face, int y, const char *text,
                          int fade)
{
    static const signed char edge[8][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {1, 1}, {-1, 1}, {1, -1}, {2, 2}
    };
    int w = font_width(face, text), x = 320 - w / 2, k;
    if (y < -40 || y > SCREEN_H + 40 || fade <= 0) return;
    if (face == FONT_TITLE) {
        if (fade >= 12)
            for (k = 0; k < (fade >= 24 ? 8 : 4); ++k)
                font_text(g, face, x + edge[k][0], y + edge[k][1], text,
                          PIX_PACK(14, 16, 12));
    } else {
        /* The names: a soft dark strip behind each, feathered at its ends
         * so it reads as shade rather than a box, and a gentle shadow. */
        int a = 70 * fade / 32, half = w / 2 + 18, top = y - 14;
        for (k = 0; k < 4; ++k)
            blend_rect(g->screen, 320 - half + k * 5, top + (k > 1),
                       2 * (half - k * 5), 19 - 2 * (k > 1), 10, 10, 8,
                       (uint8_t)(a / 4));
        if (fade >= 16)
            font_text(g, face, x + 1, y + 1, text, PIX_PACK(20, 20, 16));
    }
    font_text(g, face, x, y, text,
              PIX_PACK(60 + 168 * fade / 32, 58 + 160 * fade / 32,
                       50 + 140 * fade / 32));
}

/* Names come up through the foot of the band and go out at its head. */
static int remember_band_fade(int y)
{
    if (y > SCREEN_H - 4 || y < REMEMBER_BAND_TOP - 4) return 0;
    if (y > SCREEN_H - 30) return (SCREEN_H - 4 - y) * 32 / 26;
    if (y < REMEMBER_BAND_TOP + 22) return (y - REMEMBER_BAND_TOP + 4) * 32 / 26;
    return 32;
}

static void draw_remember(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t now = SDL_GetTicks(), t = now - g->scene_started;
    int pan = 0, i, y, fade, last;
    float drift;
    /* The field, drifting slowly to the right and resting at its end. */
    drift = t >= REMEMBER_PAN_MS ? 1.0f : (float)t / (float)REMEMBER_PAN_MS;
    drift = drift * drift * (3.0f - 2.0f * drift);
#ifdef REMEMBER_LAYERS
    if (g->remember_layer[0]) {
        /* Sky, distance, middle ground and foreground, each a strip that
         * slides at its own pace: the near poppies pass, the far church
         * hardly moves. */
        for (i = 0; i < REMEMBER_LAYERS; ++i) {
            SDL_Surface *layer = g->remember_layer[i];
            SDL_Rect src, dst;
            if (!layer) continue;
            src.x = (int)((float)(layer->w - SCREEN_W) * drift);
            src.y = 0; src.w = SCREEN_W; src.h = layer->h;
            dst.x = 0; dst.y = remember_layer_info[i][1];
            dst.w = SCREEN_W; dst.h = layer->h;
            SDL_BlitSurface(layer, &src, s, &dst);
            if (i == REMEMBER_LAYERS - 1) pan = src.x;
        }
    } else
#endif
    if (g->remember_field) {
        SDL_Rect src, dst;
        pan = (int)((float)(g->remember_field->w - SCREEN_W) * drift);
        src.x = pan; src.y = 0; src.w = SCREEN_W; src.h = SCREEN_H;
        dst.x = 0; dst.y = 0; dst.w = SCREEN_W; dst.h = SCREEN_H;
        SDL_BlitSurface(g->remember_field, &src, s, &dst);
    } else {
        gradient(s, 120, 140, 160, 90, 104, 70);
    }
    /* Poppies and grass along the foot, nearer than the painting and so
     * passing a little faster, each swaying as the gusts come through. */
    if (g->remember_poppies) {
        float gust = 0.5f + 0.5f * sinf((float)now * 0.0011f) *
                            sinf((float)now * 0.00037f + 1.3f);
        for (i = 0; i < REMEMBER_POPPY_COUNT; ++i) {
            uint32_t h = scene_hash((uint32_t)i * 2654435761u + 77u);
            int variant = (int)(h % REMEMBER_POPPY_VARIANTS);
            int span = SCREEN_W + 120;
            int x = (int)((h >> 8) % (uint32_t)span) - pan % span;
            /* The frames lean one way, stand, and lean the other:
             * played 0 1 2 3 2 1 for the to-and-fro. */
            static const unsigned char sway[6] = {0, 1, 2, 3, 2, 1};
            int phase = sway[(now / (170u + (h >> 20) % 90u) + (h >> 12)) %
                             6u];
            const short *f;
            SDL_Rect src, dst;
            while (x < -60) x += span;
            if (gust < 0.35f) phase = 1 + (phase & 1);   /* barely stirring */
            f = remember_poppy_frames[variant][phase % REMEMBER_SWAY_FRAMES];
            src.x = f[0]; src.y = f[1]; src.w = f[2]; src.h = f[3];
            dst.x = x;
            dst.y = SCREEN_H - f[3] - (int)((h >> 4) % 18u);
            dst.w = f[2]; dst.h = f[3];
            SDL_BlitSurface(g->remember_poppies, &src, s, &dst);
        }
    }
#ifdef REMEMBER_LIFE_SPRITES
    /* A butterfly or two over the poppies, a lark high up, thistledown
     * drifting on the wind. */
    if (g->remember_life)
        for (i = 0; i < REMEMBER_LIFE_COUNT; ++i) {
            const RememberLife *l = &remember_life[i];
            uint32_t h = scene_hash((uint32_t)i * 40503u + 9u);
            float u = (float)((t + h % 20000u) % l->period_ms) /
                      (float)l->period_ms;
            int x = (int)(u * (SCREEN_W + 80)) - 40 - pan * l->depth / 32;
            int y = l->y + (int)(sinf(u * 6.283f * l->bobs + (float)i) *
                                 (float)l->bob);
            const short *f = remember_life_frames[l->sprite]
                [(now / l->frame_ms + h) %
                 (uint32_t)remember_life_frame_count[l->sprite]];
            SDL_Rect src, dst;
            while (x < -40) x += SCREEN_W + 80;
            src.x = f[0]; src.y = f[1]; src.w = f[2]; src.h = f[3];
            dst.x = x; dst.y = y; dst.w = f[2]; dst.h = f[3];
            SDL_BlitSurface(g->remember_life, &src, s, &dst);
        }
#endif
    fade = t >= REMEMBER_HEADING_MS ? 32 :
           (int)(t * 32u / REMEMBER_HEADING_MS);
    remember_line(g, FONT_TITLE, 74, "WE REMEMBER", fade);
    if (t < REMEMBER_SCROLL_AT) return;
    /* The roll rises through a gently shaded band low in the picture,
     * fading in at its foot and out at its head, so the field above
     * stays open.  When the closing lines reach the band's middle it
     * stops, and the field is left to the wind. */
    for (i = 0; i < 8; ++i)
        blend_rect(s, 0, REMEMBER_BAND_TOP - 24 + i * 3, SCREEN_W, 3,
                   8, 9, 8, (uint8_t)(6 + i * 7));
    blend_rect(s, 0, REMEMBER_BAND_TOP, SCREEN_W,
               SCREEN_H - REMEMBER_BAND_TOP, 8, 9, 8, 48);
    last = (g->remember_count ? g->remember_count * REMEMBER_LINE + 90 : 0) +
           REMEMBER_LINE;
    y = SCREEN_H + 6 -
        (int)((t - REMEMBER_SCROLL_AT) * (uint32_t)REMEMBER_RISE / 1000u);
    if (y + last < REMEMBER_REST_Y) y = REMEMBER_REST_Y - last;
    for (i = 0; i < g->remember_count; ++i)
        remember_line(g, FONT_LABEL, y + i * REMEMBER_LINE,
                      g->remember_names[i],
                      remember_band_fade(y + i * REMEMBER_LINE));
    y += last - REMEMBER_LINE;
    for (i = 0; i < 2; ++i)
        remember_line(g, i ? FONT_TYPE_SMALL : FONT_LABEL, y + i * 20,
                      remember_closing[i], remember_band_fade(y + i * 20));
}

static void advance_after_debrief(Game *g)
{
    if (!g->success && g->fatal_sortie && g->lives <= 0) {
        g->menu_selection = 0;
        set_scene(g, SCENE_TITLE);
    }
    else if (!g->success) set_scene(g, SCENE_BRIEFING);
    /* Grounded: the squadron flies the next sortie without him, and
     * career_advance strikes it off just as save_mission_result did. */
    else if (career_advance(&g->chapter, &g->mission, &g->campaign_complete,
                            &g->grounded_next, CHAPTER_COUNT,
                            MISSIONS_PER_CHAPTER) == CAREER_WAR_OVER)
        set_scene(g, SCENE_EPILOGUE);
    else {
        apply_story_wingmen(g);
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
    if (g->scene == SCENE_SETTINGS) {
        handle_settings_key(g, ev->key.keysym.sym);
        return;
    }
    if (g->scene == SCENE_REMEMBER) {
        /* Once the heading has been read, Escape or Enter leaves it. */
        SDL_Keycode k = ev->key.keysym.sym;
        if ((k == SDLK_ESCAPE || k == SDLK_RETURN || k == SDLK_KP_ENTER) &&
            SDL_GetTicks() - g->scene_started > REMEMBER_HEADING_MS + 1500u) {
            g->last_action_key = SDL_GetTicks();
            set_scene(g, SCENE_TITLE);
        }
        return;
    }
    if (g->scene == SCENE_FILM) {
        /* Enter, the fire key or Escape cuts to the next scene. */
        SDL_Keycode k = ev->key.keysym.sym;
        if (k == SDLK_RETURN || k == SDLK_KP_ENTER || k == SDLK_SPACE ||
            k == SDLK_ESCAPE) {
            g->last_action_key = SDL_GetTicks();
            film_end(g);
        }
        return;
    }
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
        else if (g->scene == SCENE_SORTIE &&
                 (g->flight_doomed || g->flight_crashed || g->health <= 0 ||
                  g->wreck_started)) {
            /* Going down is not escaped by leaving: the sortie ends as the
             * loss it is, and the debrief saves it. */
            finish_alternate_sortie(g);
        }
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
            int has_continue = g->has_save && g->saved_lives > 0 &&
                       !g->saved_conduct[4];
            if (g->menu_selection == campaign_menu_count(g) - 1) {
                g->confirm_overwrite = 0;
                g->settings_row = 0;
                g->settings_from = SCENE_MENU;
                set_scene(g, SCENE_SETTINGS);
                break;
            }
            if (g->menu_selection == campaign_menu_count(g) - 2) {
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
                apply_story_wingmen(g);
                qa_report_flight(g, "CONTINUE");
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
            set_scene(g, SCENE_DAWN);
        } else if (g->scene == SCENE_DAWN) {
            if (SDL_GetTicks() - g->scene_started < 1200u) break;
            reset_sortie(g);
            set_scene(g, SCENE_SORTIE);
        } else if (g->scene == SCENE_DEBRIEF) {
            /* A failed save is retried once; if that fails too, the
             * next Enter carries on with the progress held in memory
             * rather than leaving the pilot stuck at this board. */
            if (g->mission_save_state == -1) {
                g->mission_save_state = 0;
                if (!save_mission_result(g)) {
                    g->mission_save_state = -2;
                    g->redraw_static = 1;
                    break;
                }
            }
            if (g->conduct_pending) {
                g->conduct_pending = 0;
                set_scene(g, g->offences >= 3 ? SCENE_COURT_MARTIAL :
                                                 SCENE_CONDUCT);
                break;
            }
            /* The evening Thyne is lost has its own card; no party. */
            if (g->success && !g->mess_done &&
                !(g->chapter == 4 && g->mission == 8)) {
                g->mess_done = 1;
                set_scene(g, SCENE_MESS);
                break;
            }
            if (g->pending_award || g->pending_rank) {
                set_scene(g, SCENE_AWARD);
                break;
            }
            advance_after_debrief(g);
        } else if (g->scene == SCENE_CONDUCT) {
            if (SDL_GetTicks() - g->scene_started < 900u) break;
            if (g->success && !g->mess_done) {
                g->mess_done = 1;
                set_scene(g, SCENE_MESS);
                break;
            }
            advance_after_debrief(g);
        } else if (g->scene == SCENE_CINEMA) {
            if (SDL_GetTicks() - g->scene_started < 700u) break;
            /* Out into the morning: on to the paper or the diary. */
            newsreel_free(&g->newsreel);
            set_scene(g, SCENE_STORY);
            g->story_stage = news_index_for(g->chapter, g->mission) >= 0 ? 3 : 2;
            g->reveal_started = SDL_GetTicks();
        } else if (g->scene == SCENE_COURT_MARTIAL) {
            if (SDL_GetTicks() - g->scene_started < 900u) break;
            if (++g->court_martial_stage < 3) {
                g->scene_started = SDL_GetTicks();
                g->redraw_static = 1;
                break;
            }
            g->menu_selection = 0;
            set_scene(g, SCENE_TITLE);
        } else if (g->scene == SCENE_MESS) {
            if (SDL_GetTicks() - g->reveal_started < MESS_LINES * 1100u + 600u) {
                g->reveal_started = SDL_GetTicks() - 60000u;
                break;
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
            } else if (g->story_stage != 2 && g->mission == 0 &&
                       (g->story_stage == 4 ||
                        (g->story_stage == 0 && !new_machines_today(g))) &&
                       g->cinema_seen != g->chapter + 1 &&
                       cinema_reels_present()) {
                g->cinema_seen = g->chapter + 1;
                set_scene(g, SCENE_CINEMA);
            } else if (g->story_stage != 2) {
                if (g->story_stage == 0 && new_machines_today(g))
                    g->story_stage = 4;
                else if (g->story_stage <= 0 || g->story_stage == 4) {
                    if (thyne_lost_tonight(g)) g->story_stage = 1;
                    else if (news_index_for(g->chapter, g->mission) >= 0)
                        g->story_stage = 3;
                    else g->story_stage = 2;
                } else if (g->story_stage == 0 && thyne_lost_tonight(g))
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
        } else if (g->scene == SCENE_EPILOGUE) set_scene(g, SCENE_REMEMBER);
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
    /* Only the test entries use their own save file; any other argument
     * starts the game normally on the player's campaign. */
    /* QA: RUN flies the sortie to its end as a campaign sortie would,
     * keeping the pilot alive, and logs it; FLY hands the controls to the
     * QA autopilot.  Both may follow any sortie entry below. */
    int qa_run_arg = 0, qa_fly_arg = 0;
    {
        int k, n = 1;
        for (k = 1; k < argc; ++k) {
            if (k > 1 && strcmp(argv[k], "RUN") == 0) qa_run_arg = 1;
            else if (k > 1 && strcmp(argv[k], "FLY") == 0) qa_fly_arg = 1;
            else if (k > 1 && strcmp(argv[k], "TIME") == 0) qa_frame_time = 1;
            else if (k > 1 && strcmp(argv[k], "SLOWFRAMES") == 0) qa_slow_frames = 1;
            else argv[n++] = argv[k];
        }
        argc = n;
    }
    if (argc > 1) {
        static const char *const qa_modes[] = {
            "AIRSHIP", "AWARD", "CHAPTER_SUCCESS", "CINEMA", "COURTMARTIAL",
            "DAWN", "DEATH", "LOSS",
            "DEBRIEF",
            "DEBRIEF_SUCCESS", "DIARY", "DOGFIGHT", "FINAL_SUCCESS", "GOTHA",
            "GOALS", "GUNNER", "HOSPITAL", "MACHINES", "MESS", "MISSION", "NEWS",
            "REPRIMAND", "SIDE", "TOPDOWN",
            "TOPMIX",
            "TRACERS",
            "TRUCE",
            "UNARMED",
            "UNARMED_GUNNER",
            /* The flight's health and a grounding, carried by the save. */
            "GROUNDED", "QASAVE", "WINGMEN"
        };
        int q;
        for (q = 0; q < (int)(sizeof(qa_modes) / sizeof(qa_modes[0])); ++q)
            if (strcmp(argv[1], qa_modes[q]) == 0) use_qa_save_paths();
    }
    if (!game_init(&g)) return 1;
    /* QA entry points keep the normal title/campaign flow untouched while
     * allowing each renderer to be launched directly on real Amiga hardware. */
    if (argc > 1 && strcmp(argv[1], "SIDE") == 0) {
        g.chapter = 2;
        g.mission = 4;
        /* SIDE n: the first side-on sortie of chapter n instead. */
        if (argc > 2 && argv[2][0] >= '0' && argv[2][0] <= '7') {
            int m;
            g.chapter = argv[2][0] - '0';
            /* SIDE n VILLAGE: the first of them with a village to spare. */
            for (m = 0; m < MISSIONS_PER_CHAPTER; ++m) {
                g.mission = m;
                if (campaign_missions[g.chapter][m].mode == MISSION_SIDE &&
                    campaign_missions[g.chapter][m].special == SPECIAL_NONE &&
                    (argc < 4 || side_village_today(&g)))
                    break;
            }
            g.qa_hold = argc > 3;
            qa_still = argc > 4 && strcmp(argv[4], "STILL") == 0;
        }
        reset_sortie(&g);
        set_scene(&g, SCENE_SORTIE);
        if (qa_still) {
            int i;
            for (i = 0; i < MAX_TARGETS; ++i) {
                Target *t = &g.targets[i];
                if (!t->active) continue;
                t->world_x = t->civilian == SC_FARM ? g.scroll + 150.0f :
                             t->civilian == SC_CHURCH ? g.scroll + 330.0f :
                             t->civilian == SC_CART ? g.scroll + 520.0f :
                             g.scroll + 5000.0f;
            }
        }
    } else if (argc > 1 && (strcmp(argv[1], "TRUCE") == 0 ||
                            strcmp(argv[1], "AIRSHIP") == 0 ||
                            strcmp(argv[1], "GOTHA") == 0)) {
        /* The three special sorties of the war-news chapters. */
        g.chapter = argv[1][0] == 'T' ? 0 : argv[1][0] == 'A' ? 2 : 3;
        g.mission = argv[1][0] == 'T' ? 11 : 10;
        reset_sortie(&g);
        g.qa_hold = argv[1][0] != 'T';
        set_scene(&g, SCENE_SORTIE);
    } else if (argc > 2 && strcmp(argv[1], "TOPMIX") == 0) {
        /* TOPMIX n: the first sortie flown with top-down mix n. */
        int want = atoi(argv[2]), c, m, found = 0;
        for (c = 0; c < CHAPTER_COUNT && !found; ++c)
            for (m = 0; m < MISSIONS_PER_CHAPTER && !found; ++m)
                if (topdown_mix_for(c, m) == want) {
                    g.chapter = c;
                    g.mission = m;
                    found = 1;
                }
        reset_sortie(&g);
        g.qa_hold = 1;
        g.qa_autopilot = argc > 3;   /* TOPMIX n FLY: the autopilot flies it */
        set_scene(&g, SCENE_SORTIE);
    } else if (argc > 1 && strcmp(argv[1], "GOALS") == 0) {
        /* QA: every flat sortie's order against what it offers.  Each is
         * laid out as for flying; an order above the targets on offer,
         * or an armed sortie with fewer than the order and two bombs, is
         * a failure, and the game exits with 1. */
        int c, m, bad = 0;
        for (c = 0; c < CHAPTER_COUNT; ++c)
            for (m = 0; m < MISSIONS_PER_CHAPTER; ++m) {
                MissionMode mode = campaign_missions[c][m].mode;
                int goal, offered, ok;
                if ((mode != MISSION_SIDE && mode != MISSION_TOP_DOWN) ||
                    campaign_missions[c][m].special != SPECIAL_NONE) continue;
                g.chapter = c;
                g.mission = m;
                reset_sortie(&g);
                goal = current_mission_goal(&g);
                offered = targets_on_offer(&g);
                ok = goal <= offered &&
                     (!mission_armed(&g) || g.task_total > 0 ||
                      g.bombs_remaining >= goal + 2);
                printf("C%d M%02d %-7s order %2d offered %2d bombs %2d bags %d %s\n",
                       c + 1, m + 1, mode == MISSION_SIDE ? "SIDE" : "TOPDOWN",
                       goal, offered, g.bombs_remaining, g.bags,
                       ok ? "ok" : "TOO HIGH");
                if (!ok) bad = 1;
            }
        return bad;
    } else if (argc > 3 && strcmp(argv[1], "MISSION") == 0) {
        /* MISSION c m: chapter c, mission m of the campaign, from 1. */
        g.chapter = (atoi(argv[2]) + CHAPTER_COUNT - 1) % CHAPTER_COUNT;
        g.mission = (atoi(argv[3]) + MISSIONS_PER_CHAPTER - 1) %
                    MISSIONS_PER_CHAPTER;
        reset_sortie(&g);
        set_scene(&g, SCENE_SORTIE);
    } else if (argc > 1 && (strcmp(argv[1], "REPRIMAND") == 0 ||
                            strcmp(argv[1], "COURTMARTIAL") == 0)) {
        /* An ambulance hit on a 1918 convoy sortie: the first offence, or
         * the third. */
        g.chapter = 5;
        g.mission = 0;
        reset_sortie(&g);
        g.offences = argv[1][0] == 'C' ? 2 : 0;
        g.sortie_offences = 1;
        g.sortie_offence_kind = CIV_AMBULANCE;
        g.success = 1;
        count_sortie_in_record(&g);
        g.career_counted = 1;
        g.conduct_pending = 0;
        set_scene(&g, g.offences >= 3 ? SCENE_COURT_MARTIAL : SCENE_CONDUCT);
    } else if (argc > 2 && strcmp(argv[1], "AWARD") == 0) {
        /* AWARD n: decoration n pinned on in the colonel's office. */
        g.chapter = 3;
        g.mission = 0;
        g.victories = 9;
        g.pending_award = 1 << (atoi(argv[2]) % 5);
        set_scene(&g, SCENE_AWARD);
    } else if (argc > 1 && strcmp(argv[1], "DAWN") == 0) {
        /* Dawn at the hangars before a 1916 sortie. */
        g.chapter = 2;
        g.mission = 1;
        set_scene(&g, SCENE_DAWN);
    } else if (argc > 1 && strcmp(argv[1], "LOSS") == 0) {
        /* The evening Gridpipe Thyne does not come back. */
        g.chapter = 4;
        g.mission = 9;
        set_scene(&g, SCENE_STORY);
        g.story_stage = 1;
    } else if (argc > 1 && strcmp(argv[1], "MESS") == 0) {
        /* The mess after a good day: seven on the board, two of them new.
         * MESS QUIET is an evening with nothing brought down; MESS WAKE
         * the evening Eccles goes to the sick bay. */
        g.chapter = 2;
        g.mission = 3;
        g.victories = 7;
        g.kills = 2;
        g.success = 1;
        if (argc > 2 && strcmp(argv[2], "QUIET") == 0) g.kills = 0;
        if (argc > 2 && strcmp(argv[2], "WAKE") == 0)
            g.wingman_hit_today[0] = 1;
        set_scene(&g, SCENE_MESS);
    } else if (argc > 1 && strcmp(argv[1], "REMEMBER") == 0) {
        /* The roll at the very end. */
        g.chapter = CHAPTER_COUNT - 1;
        set_scene(&g, SCENE_REMEMBER);
    } else if (argc > 2 && strcmp(argv[1], "FILM") == 0) {
        /* One aerodrome film, 0..7, then the title. */
        g.film_next = SCENE_TITLE;
        set_scene(&g, SCENE_FILM);
        if (!film_load(&g, atoi(argv[2]))) set_scene(&g, SCENE_TITLE);
    } else if (argc > 1 && strcmp(argv[1], "SETTINGS") == 0) {
        /* The preferences page; SETTINGS ALL first loads every scene's
         * art, as a long campaign with the art kept in memory would. */
        g.settings_from = SCENE_TITLE;
        set_scene(&g, SCENE_SETTINGS);
        if (argc > 2 && strcmp(argv[2], "ALL") == 0)
            load_asset_groups(&g, ~0u);
    } else if (argc > 2 && strcmp(argv[1], "CINEMA") == 0) {
        /* CINEMA n: chapter n's evening at the picture house. */
        g.chapter = atoi(argv[2]) % CHAPTER_COUNT;
        g.mission = 0;
        set_scene(&g, SCENE_CINEMA);
    } else if (argc > 2 && strcmp(argv[1], "MACHINES") == 0) {
        /* MACHINES n: chapter n's first morning, with its new machines. */
        g.chapter = atoi(argv[2]) % CHAPTER_COUNT;
        g.mission = 0;
        mesh_chapter = g.chapter;
        set_scene(&g, SCENE_STORY);
        g.story_stage = 4;
    } else if (argc > 1 && strcmp(argv[1], "DIARY") == 0) {
        g.qa_diary_all = 1;
        set_scene(&g, SCENE_STORY);
    } else if (argc > 1 && strcmp(argv[1], "NEWS") == 0) {
        /* The front page before the first day of the Somme. */
        g.chapter = 2;
        g.mission = 4;
        set_scene(&g, SCENE_STORY);
    } else if (argc > 1 && (strcmp(argv[1], "TOPDOWN") == 0 ||
                            strcmp(argv[1], "TRACERS") == 0)) {
        g.chapter = 2;
        g.mission = 1;
        reset_sortie(&g);
        g.qa_tracers = argv[1][1] == 'R';
        g.qa_hold = g.qa_tracers;
        set_scene(&g, SCENE_SORTIE);
    } else if (argc > 1 && (strcmp(argv[1], "UNARMED") == 0 ||
                            strcmp(argv[1], "UNARMED_GUNNER") == 0)) {
        /* 1914 from the cockpit or the rear seat: no gun aboard. */
        g.chapter = 0;
        g.mission = strcmp(argv[1], "UNARMED") == 0 ? 5 : 2;
        reset_sortie(&g);
        g.qa_hold = 1;
        set_scene(&g, SCENE_SORTIE);
    } else if (argc > 1 && strcmp(argv[1], "DOGFIGHT") == 0) {
        /* DOGFIGHT n TIME: TIME is taken with RUN and FLY above. */
        /* Use a durable late-war squadron for visual QA so the direct test
         * remains playable instead of completing before the window appears. */
        g.chapter = 6;
        g.mission = 5;
        /* DOGFIGHT n: the first cockpit sortie of chapter n instead, to
         * check that year's machine and guns. */
        if (argc > 2 && argv[2][0] >= '0' && argv[2][0] <= '7') {
            int m;
            g.chapter = argv[2][0] - '0';
            for (m = 0; m < MISSIONS_PER_CHAPTER; ++m)
                if (campaign_missions[g.chapter][m].mode == MISSION_DOGFIGHT_3D &&
                    campaign_missions[g.chapter][m].special == SPECIAL_NONE) {
                    g.mission = m;
                    break;
                }
        }
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
        g.flight_doomed_started = sim_now(&g);
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
        g.flight_doomed_started = sim_now(&g);
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
    } else if (argc > 1 && strcmp(argv[1], "WINGMEN") == 0) {
        /* WINGMEN e t b: Eccles, Gridpipe Thyne and Bluebottle at the
         * given health (1..100, 100 if left out) on the morning of
         * "The Interrupter", the 1915 cockpit patrol.  The QA save is written
         * at once and the briefing's roster shows the flight; Enter flies
         * the sortie as the campaign would, and the debrief saves what
         * the fight left them with.  QASAVE then continues from it. */
        int k;
        g.chapter = 1;
        g.mission = 5;
        for (k = 0; k < 3; ++k) {
            int hp = argc > 2 + k ? atoi(argv[2 + k]) : 100;
            g.wingman_health[k] = hp < 1 ? 1 : hp > 100 ? 100 : hp;
        }
        apply_story_wingmen(&g);
        save_campaign(&g);
        qa_report_flight(&g, "WINGMEN");
        set_scene(&g, SCENE_BRIEFING);
    } else if (argc > 1 && strcmp(argv[1], "GROUNDED") == 0) {
        /* A second offence with no rank to lose, on "Fog on the Somme"
         * (1918, mission 1): the debrief saves, the memorandum grounds
         * him, and the war must go on at "Westward" (mission 3) with
         * "Burn the Stores" flown without him, whether the evening is
         * seen through or the game is quit and QASAVE continued. */
        g.chapter = 5;
        g.mission = 0;
        reset_sortie(&g);
        g.offences = 1;
        g.sortie_offences = 1;
        g.sortie_offence_kind = CIV_AMBULANCE;
        g.success = 1;
        g.kills = 0;
        set_scene(&g, SCENE_DEBRIEF);
    }
    /* QASAVE: the ordinary title and officers' menu, on the QA save, so
     * Continue can be tried on what the entries above wrote. */
    if (g.scene == SCENE_SORTIE && qa_run_arg) {
        g.qa_hold = 0;
        g.qa_run = 1;
    }
    if (g.scene == SCENE_SORTIE && qa_fly_arg) g.qa_autopilot = 1;
    previous = SDL_GetTicks();
    while (g.running) {
        if (g.window_restore_at && SDL_GetTicks() >= g.window_restore_at) {
            g.window_restore_at = 0;
            refresh_desktop();
            SDL_ShowWindow(g.window);
            SDL_RaiseWindow(g.window);
            g.window_surface = SDL_GetWindowSurface(g.window);
            g.redraw_static = 1;
        }
        SDL_Event ev;
        uint32_t now;
        float dt;
        const uint8_t *keys;
        while (SDL_PollEvent(&ev)) handle_event(&g, &ev);
        now = SDL_GetTicks();
        dt = (float)(now - previous) / 1000.0f;
        previous = now;
        if (dt > SIM_MAX_STEP) dt = SIM_MAX_STEP;
        {
            static uint8_t merged[SDL_NUM_SCANCODES];
            memcpy(merged, SDL_GetKeyboardState(NULL), sizeof(merged));
            merge_joystick_keys(&g, merged);
            keys = merged;
        }
        /* The dawn scene takes off by itself if nobody presses Enter. */
        if (g.scene == SCENE_DAWN && SDL_GetTicks() - g.scene_started > 11000u) {
            reset_sortie(&g);
            set_scene(&g, SCENE_SORTIE);
        }
        if (g.scene == SCENE_SORTIE) update_sortie(&g, dt, keys);
        /* The officers' menu is static: it is redrawn only when the selection
         * or prompt changes (redraw_static), not every frame. */
        if (g.scene == SCENE_TITLE ||
            g.scene == SCENE_SORTIE || g.scene == SCENE_DEBRIEF ||
            g.scene == SCENE_STORY || g.scene == SCENE_AWARD ||
            g.scene == SCENE_DAWN || g.scene == SCENE_MESS ||
            g.scene == SCENE_CONDUCT || g.scene == SCENE_COURT_MARTIAL ||
            g.scene == SCENE_CINEMA || g.scene == SCENE_FILM ||
            g.scene == SCENE_REMEMBER ||
            g.redraw_static) {
            uint32_t drawn_from = SDL_GetTicks();
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
            case SCENE_DAWN: draw_dawn(&g); break;
            case SCENE_MESS: draw_mess(&g); break;
            case SCENE_CONDUCT: draw_conduct(&g); break;
            case SCENE_COURT_MARTIAL: draw_court_martial(&g); break;
            case SCENE_CINEMA: update_cinema(&g); draw_cinema(&g); break;
            case SCENE_SETTINGS: draw_settings(&g); break;
            case SCENE_REMEMBER: draw_remember(&g); break;
            case SCENE_FILM:
                update_film(&g);
                if (g.scene == SCENE_FILM) draw_film(&g);
                break;
            }
            if (qa_frame_time && g.scene == SCENE_SORTIE) {
                /* The mean over the last 64 frames, in tenths of a ms. */
                static uint32_t sum, n, shown;
                char t[32];
                sum += SDL_GetTicks() - drawn_from;
                if (++n == 64) { shown = sum * 10u / 64u; sum = n = 0; }
                snprintf(t, sizeof(t), "DRAW %u.%u MS", (unsigned)(shown / 10u),
                         (unsigned)(shown % 10u));
                rect(g.screen, 470, 6, 164, 18, PIX_PACK(0, 0, 0));
                typewriter_text(&g, 476, 9, t, 7, 11, PIX_PACK(255, 255, 255));
            }
            if (SDL_MUSTLOCK(g.screen)) SDL_UnlockSurface(g.screen);
            present_frame(&g);
            g.redraw_static = 0;
        }
        SDL_Delay(8);
        if (qa_slow_frames && g.scene == SCENE_SORTIE) SDL_Delay(117);
    }
    if (g.audio.device) SDL_CloseAudioDevice(g.audio.device);
    free_asset_groups(&g, ~0u);
    if (g.title_backdrop) SDL_FreeSurface(g.title_backdrop);
    if (g.dawn_backdrop) SDL_FreeSurface(g.dawn_backdrop);
    if (g.fonts) SDL_FreeSurface(g.fonts);
    if (g.player_sprites) SDL_FreeSurface(g.player_sprites);
    if (g.enemy_sprites) SDL_FreeSurface(g.enemy_sprites);
    {
        int k;
        for (k = 0; k < 4; ++k)
            if (g.machine_sprites[k]) SDL_FreeSurface(g.machine_sprites[k]);
    }
    if (g.audio.music_data) SDL_free(g.audio.music_data);
    if (g.audio.mess_data) SDL_free(g.audio.mess_data);
    if (g.audio.piper_data) SDL_free(g.audio.piper_data);
    if (g.audio.record_data) SDL_free(g.audio.record_data);
    newsreel_free(&g.newsreel);
    film_free(&g);
    remember_free(&g);
    if (g.joystick) SDL_JoystickClose(g.joystick);
    if (g.screen) SDL_FreeSurface(g.screen);
    if (g.fullscreen) {
        SDL_SetWindowFullscreen(g.window, 0);
        refresh_desktop();
    }
    SDL_DestroyWindow(g.window);
    SDL_Quit();
    return 0;
}
