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
#ifndef CANVAS_BOARD_BMP
#define CANVAS_BOARD_BMP "assets/canvas-skies/mission-board-v2.bmp"
#endif
#ifndef CANVAS_JOURNAL_BMP
#define CANVAS_JOURNAL_BMP "assets/canvas-skies/journal.bmp"
#endif
#ifndef CANVAS_BATTLEFIELD_BMP
#define CANVAS_BATTLEFIELD_BMP "assets/canvas-skies/far-hills.bmp"
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

#define SCREEN_W 640
#define SCREEN_H 360
#define MAX_BULLETS 80
#define MAX_ENEMIES 10
#define MAX_TARGETS 12
#define MAX_EXPLOSIONS 24
#define MIDGROUND_ELEMENT_COUNT 6
#define NEAR_ELEMENT_COUNT 6

typedef enum {
    SCENE_TITLE,
    SCENE_STORY,
    SCENE_BRIEFING,
    SCENE_SORTIE,
    SCENE_DEBRIEF,
    SCENE_EPILOGUE
} Scene;

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
    float life;
    int enemy;
    int type;
    int active;
} Bullet;

typedef struct {
    float x, y, age, duration;
    int large;
    int active;
} Explosion;

typedef struct {
    float x, y, phase;
    int active, hp;
    uint32_t last_shot;
} Enemy;

typedef struct {
    float world_x;
    int active;
    int type;
    uint32_t last_shot;
} Target;

typedef struct {
    SDL_AudioDeviceID device;
    uint32_t sample_clock;
    uint32_t noise;
    uint8_t *music_data;
    uint32_t music_len;
    uint32_t music_pos;
    volatile int scene;
    volatile int shot;
    volatile int blast;
    volatile float engine_speed;
    volatile float engine_load;
    volatile float other_engine;
    volatile float other_pitch;
    volatile float vehicle_engine;
    float engine_phase;
    float other_phase;
    float vehicle_phase;
} AudioState;

typedef struct {
    SDL_Window *window;
    SDL_Surface *screen;
    SDL_Surface *title;
    SDL_Surface *mission_board;
    SDL_Surface *journal;
    SDL_Surface *battlefield;
    SDL_Surface *foreground_panorama;
    SDL_Surface *midground[MIDGROUND_ELEMENT_COUNT];
    SDL_Surface *near_props[NEAR_ELEMENT_COUNT];
    SDL_Surface *machine_gun_sprite;
    SDL_Surface *aa_gun_sprite;
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
    int success;
    int chapter;
    int mission;
    int wingman_health[3];
    uint32_t scene_started;
    float player_x;
    float player_y;
    float player_vy;
    float airspeed;
    float scroll;
    float fuel;
    int health;
    int score;
    int objectives;
    uint32_t last_shot;
    uint32_t last_bomb;
    uint32_t last_damage;
    Bullet bullets[MAX_BULLETS];
    Explosion explosions[MAX_EXPLOSIONS];
    Enemy enemies[MAX_ENEMIES];
    Target targets[MAX_TARGETS];
} Game;

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

static uint32_t rgb(SDL_Surface *s, uint8_t r, uint8_t g, uint8_t b)
{
    return SDL_MapRGB(s->format, r, g, b);
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

static void circle(SDL_Surface *s, int cx, int cy, int radius, uint32_t c)
{
    int x, y;
    for (y = -radius; y <= radius; ++y)
        for (x = -radius; x <= radius; ++x)
            if (x * x + y * y <= radius * radius)
                pixel(s, cx + x, cy + y, c);
}

static void blend_rect(SDL_Surface *s, int x, int y, int w, int h,
                       uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
{
    int px, py;
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
            SDL_GetRGB(p, s->format, &or_, &og, &ob);
            or_ = (uint8_t)((or_ * (255 - alpha) + r * alpha) / 255);
            og = (uint8_t)((og * (255 - alpha) + g * alpha) / 255);
            ob = (uint8_t)((ob * (255 - alpha) + b * alpha) / 255);
            row[px] = rgb(s, or_, og, ob);
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
                    SDL_GetRGB(surface_pixel(atlas, sx, sy), atlas->format,
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

static float note_hz(int semitone)
{
    return 220.0f * powf(2.0f, (float)semitone / 12.0f);
}

static float triangle_wave(float p)
{
    float v = p - floorf(p);
    return 1.0f - 4.0f * fabsf(v - 0.5f);
}

static void audio_callback(void *userdata, uint8_t *stream, int len)
{
    AudioState *a = (AudioState *)userdata;
    int16_t *out = (int16_t *)stream;
    int frames = len / (int)(sizeof(int16_t) * 2);
    static const int chords[8][3] = {
        {0,3,7}, {-2,3,7}, {-4,0,5}, {-5,-2,3},
        {0,3,7}, {2,5,9}, {-4,0,5}, {-2,3,7}
    };
    int i;
    for (i = 0; i < frames; ++i) {
        float t = (float)a->sample_clock / 11025.0f;
        int beat = (int)(t * 2.2f);
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
            sample = triangle_wave(a->engine_phase) * (0.17f + speed * 0.09f);
            sample += sinf(2.0f * (float)M_PI * a->engine_phase * 0.5f) * 0.11f;
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
        } else if (a->music_data && a->music_len >= 4) {
            int16_t *music = (int16_t *)(a->music_data + a->music_pos);
            float gain = a->scene == SCENE_SORTIE ? 0.64f : 0.48f;
            sample = ((float)music[0] + (float)music[1]) / 65536.0f * gain;
            a->music_pos += 4;
            if (a->music_pos + 4 > a->music_len) a->music_pos = 0;
        } else {
            for (v = 0; v < 3; ++v) {
                float f = note_hz(chords[bar][v]);
                pad += triangle_wave(t * f) * 0.085f;
            }
            bass = triangle_wave(t * note_hz(chords[bar][0] - 12)) * 0.16f;
            melody = sinf(2.0f * (float)M_PI * t *
                          note_hz(chords[bar][(beat / 2) % 3] + 12)) * 0.09f;
            if ((beat & 1) == 0 && fmodf(t * 2.2f, 1.0f) < 0.09f)
                drums = ((float)((a->noise >> 16) & 0xffff) / 32768.0f - 1.0f) * 0.08f;
            if (a->scene == SCENE_TITLE)
                sample = pad + bass * 0.45f + melody * 0.35f;
            else if (a->scene == SCENE_STORY || a->scene == SCENE_BRIEFING ||
                     a->scene == SCENE_EPILOGUE)
                sample = pad * 0.75f + bass * 0.35f;
            else
                sample = pad * 0.65f + bass + melody + drums;
        }

        /* A two-step field-drum line makes the gramophone recording read as
         * a military march. Every hit starts and ends at zero amplitude: the
         * old global-phase bass waveform produced a small click per beat. */
        if (a->scene != SCENE_SORTIE) {
            float march = t * 2.18f;
            float phase = march - floorf(march);
            int march_beat = (int)march;
            float noise_sample = (float)((a->noise >> 16) & 0xffff) / 32768.0f - 1.0f;
            if (phase < 0.15f) {
                float u = phase / 0.15f;
                float env = sinf((float)M_PI * u) * expf(-1.8f * u);
                if (march_beat & 1) {
                    /* Broad snare brush, not a one-sample click. */
                    sample += noise_sample * env * 0.14f;
                } else {
                    float seconds = phase / 2.18f;
                    float hz = 78.0f - 28.0f * u;
                    sample += sinf(2.0f * (float)M_PI * seconds * hz)
                            * env * 0.19f;
                }
            }
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
            sample += sinf(2.0f * (float)M_PI * t * 49.0f) * env * 0.36f;
            sample += roar * env * 0.31f;
            --a->blast;
        }
        if (sample > 1.0f) sample = 1.0f;
        if (sample < -1.0f) sample = -1.0f;
        out[i * 2] = (int16_t)(sample * 18000.0f);
        out[i * 2 + 1] = out[i * 2];
        ++a->sample_clock;
    }
}

static void set_scene(Game *g, Scene scene)
{
    g->scene = scene;
    g->scene_started = SDL_GetTicks();
    g->redraw_static = 1;
    if (g->audio.device) {
        SDL_LockAudioDevice(g->audio.device);
        g->audio.scene = scene;
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
            SDL_GetRGB(surface_pixel(atlas, sx, sy), atlas->format,
                       &r, &green, &b);
            if (r > 180 && b > 180 && green < 130) continue;
            pixel(dst, tx, ty, rgb(dst, r, green, b));
        }
    }
    if (SDL_MUSTLOCK(atlas)) SDL_UnlockSurface(atlas);
}

static int terrain_y(float world_x)
{
    return 318 + (int)(8.0f * sinf(world_x * 0.007f)
                     + 4.0f * sinf(world_x * 0.019f));
}

static int current_mission_goal(const Game *g)
{
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    if (mode == MISSION_DOGFIGHT_3D) {
        int goal = 1 + g->chapter / 2 + g->mission / 6;
        if (goal > chapters[g->chapter].enemy_count)
            goal = chapters[g->chapter].enemy_count;
        return goal;
    }
    return chapters[g->chapter].goal;
}

static void reset_sortie(Game *g)
{
    int i;
    const Chapter *chapter = &chapters[g->chapter];
    MissionMode mode = campaign_missions[g->chapter][g->mission].mode;
    g->player_x = 150.0f;
    g->player_y = 225.0f;
    g->player_vy = 0.0f;
    g->airspeed = 98.0f;
    g->scroll = 0.0f;
    g->fuel = 100.0f;
    g->health = 100;
    g->score = 0;
    g->objectives = 0;
    g->last_shot = 0;
    g->last_bomb = 0;
    g->last_damage = 0;
    memset(g->bullets, 0, sizeof(g->bullets));
    memset(g->explosions, 0, sizeof(g->explosions));
    g->audio.engine_speed = g->airspeed;
    g->audio.engine_load = 0.0f;
    g->audio.other_engine = 0.0f;
    g->audio.other_pitch = 0.0f;
    g->audio.vehicle_engine = 0.0f;
    for (i = 0; i < MAX_ENEMIES; ++i) {
        g->enemies[i].x = 760.0f + i * 620.0f;
        g->enemies[i].y = 90.0f + (float)((i * 47) % 120);
        g->enemies[i].phase = i * 0.8f;
        g->enemies[i].hp = 1 + g->chapter / 3;
        g->enemies[i].active = mode == MISSION_DOGFIGHT_3D &&
                               i < chapter->enemy_count;
        g->enemies[i].last_shot = 0;
    }
    for (i = 0; i < MAX_TARGETS; ++i) {
        g->targets[i].world_x = 560.0f + i * 480.0f;
        g->targets[i].active = mode != MISSION_DOGFIGHT_3D &&
                               i < chapter->target_count;
        g->targets[i].type = g->chapter == 0 ? 0 : i & 1;
        g->targets[i].last_shot = 0;
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
            b->enemy = enemy; b->type = type; b->life = life;
            b->active = 1;
            return b;
        }
    }
    return NULL;
}

static void new_explosion(Game *g, float x, float y, int large)
{
    int i;
    for (i = 0; i < MAX_EXPLOSIONS; ++i) {
        Explosion *e = &g->explosions[i];
        if (!e->active) {
            e->x = x; e->y = y; e->age = 0.0f;
            e->duration = large ? 0.72f : 0.42f;
            e->large = large; e->active = 1;
            return;
        }
    }
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

static void fire(Game *g)
{
    uint32_t now = SDL_GetTicks();
    if (now - g->last_shot < 130) return;
    if (new_bullet(g, g->player_x + 34.0f, g->player_y - 1.0f,
                   390.0f, g->player_vy * 0.12f, 0, 0, 2.0f)) {
        g->last_shot = now;
        g->audio.shot = 1700;
    }
}

static void drop_bomb(Game *g)
{
    uint32_t now = SDL_GetTicks();
    if (now - g->last_bomb < 550) return;
    if (new_bullet(g, g->player_x, g->player_y + 16.0f,
                   42.0f, 55.0f, 0, 1, 4.0f))
        g->last_bomb = now;
}

static void update_sortie(Game *g, float dt, const uint8_t *keys)
{
    int i, j;
    uint32_t now = SDL_GetTicks();
    int ground;
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
    if (chapters[g->chapter].armed) {
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
        b->x += b->vx * dt;
        b->y += b->vy * dt;
        b->life -= dt;
        if (b->active && b->enemy && b->type == 2) {
            float dx = b->x - g->player_x;
            float dy = b->y - g->player_y;
            float distance = sqrtf(dx * dx + dy * dy);
            if (distance < 52.0f || b->life < 0.65f) {
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
        uint32_t interval = g->targets[i].type ? 1850u : 1020u;
        if (sx > 20.0f && sx < SCREEN_W - 10.0f &&
            now - g->targets[i].last_shot > interval + (uint32_t)(i * 61 % 330)) {
            aimed_fire(g, sx, (float)ty, g->targets[i].type ? 155.0f : 235.0f,
                       g->targets[i].type ? 2 : 0);
            g->targets[i].last_shot = now;
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
                new_explosion(g, sx, (float)ty, 1);
                g->audio.blast = 7000;
            }
        }
    }
    if (g->scroll > chapters[g->chapter].distance ||
        g->fuel <= 0.0f || g->health <= 0) {
        if (campaign_missions[g->chapter][g->mission].mode == MISSION_DOGFIGHT_3D)
            g->success = g->score / 250 >= current_mission_goal(g) &&
                         g->health > 0 && g->fuel > 0;
        else
            g->success = g->objectives >= current_mission_goal(g) &&
                         g->health > 0 && g->fuel > 0;
        set_scene(g, SCENE_DEBRIEF);
    }
}

static void draw_title(Game *g)
{
    SDL_Surface *s = g->screen;
    const char *title = "CANVAS SKIES";
    uint32_t now = SDL_GetTicks();
    int i;
    if (g->title) SDL_BlitScaled(g->title, NULL, s, NULL);
    else rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 20, 31, 48));
    blend_rect(s, 0, 0, 640, 112, 5, 12, 24, 112);
    blend_rect(s, 0, 300, 640, 60, 5, 10, 18, 150);
    /* A canvas squadron pennant whose cloth and lettering share the same
     * travelling wind wave. The campaign dates belong inside the story. */
    for (i = 0; i < 506; ++i) {
        float phase = (float)i * 0.031f + (float)now * 0.0042f;
        int wave = (int)(4.0f * sinf(phase) + 2.0f * sinf(phase * 0.43f));
        int shade = (int)(8.0f * sinf(phase + 0.8f));
        uint32_t cloth = rgb(s, (uint8_t)(202 + shade),
                            (uint8_t)(181 + shade),
                            (uint8_t)(132 + shade / 2));
        rect(s, 18 + i, 17 + wave, 1, 63, cloth);
        pixel(s, 18 + i, 17 + wave, rgb(s, 75, 55, 34));
        pixel(s, 18 + i, 79 + wave, rgb(s, 75, 55, 34));
    }
    for (i = 0; title[i]; ++i) {
        char glyph[2] = {title[i], '\0'};
        int gx = 52 + i * 36;
        float phase = (float)(gx - 18) * 0.031f + (float)now * 0.0042f;
        int wave = (int)(4.0f * sinf(phase) + 2.0f * sinf(phase * 0.43f));
        display_text(g, gx, 27 + wave, glyph, 36, 42, rgb(s, 46, 34, 24));
    }
    typewriter_text(g, 40, 316, "ENTER  BEGIN CAMPAIGN", 10, 15,
                    rgb(s, 244, 222, 165));
    typewriter_text(g, 470, 340, "ORIGINAL PROTOTYPE", 7, 11,
                    rgb(s, 190, 198, 201));
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
    if (g->journal) SDL_BlitScaled(g->journal, NULL, s, NULL);
    else gradient(s, 39, 27, 18, 88, 56, 31);
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
    if (g->mission_board) SDL_BlitScaled(g->mission_board, NULL, s, NULL);
    else gradient(s, 39, 27, 18, 88, 56, 31);
    if (mission->mode == MISSION_TOP_DOWN) {
        order0 = "FOLLOW THE MARKED MAP GRID.";
        order1 = "OBSERVE OR BOMB THE ASSIGNED SECTOR.";
        order2 = "RETURN ALONE WITH YOUR REPORT.";
    } else if (mission->mode == MISSION_DOGFIGHT_3D) {
        order0 = "RENDEZVOUS WITH THE ALLIED FLIGHT.";
        order1 = "ENGAGE THE ENEMY SQUADRON.";
        order2 = "BRING YOUR WINGMEN HOME.";
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
    typewriter_text(g, 73, 205, "ARROW KEYS  MANOEUVRE", 7, 11, ink);
    typewriter_text(g, 73, 221, "X / Z       THROTTLE", 7, 11, ink);
    if (chapter->armed) {
        typewriter_text(g, 73, 237, "SPACE       MACHINE GUN", 7, 11, ink);
        typewriter_text(g, 73, 253, "B           DROP BOMB", 7, 11, ink);
    } else {
        typewriter_text(g, 73, 237, "NO ARMAMENT FITTED", 7, 11, red);
        typewriter_text(g, 73, 253, "OBSERVE AND RETURN", 7, 11, ink);
    }
    rect(s, 68, 304, 194, 25, rgb(s, 184, 139, 63));
    rect(s, 70, 306, 190, 21, rgb(s, 57, 39, 27));
    circle(s, 76, 311, 2, rgb(s, 184, 139, 63));
    circle(s, 253, 311, 2, rgb(s, 184, 139, 63));
    typewriter_text(g, 83, 311, "ENTER  FLY THE MISSION", 7, 11,
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
    int flash = SDL_GetTicks() - target->last_shot < 90;
    if (sprite) {
        SDL_Rect dst = {sx - sprite->w / 2, ty + 12 - sprite->h, 0, 0};
        SDL_BlitSurface(sprite, NULL, s, &dst);
        if (flash) {
            if (target->type)
                circle(s, sx + 17, ty - 52, 4, rgb(s, 255, 177, 54));
            else
                circle(s, sx + 37, ty - 31, 3, rgb(s, 255, 196, 72));
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

static void draw_sky_detail(Game *g)
{
    SDL_Surface *s = g->screen;
    int i;
    int drift = (int)(g->scroll * 0.11f);
    for (i = 0; i < 5; ++i) {
        int x = ((i * 173 - drift) % 820 + 820) % 820 - 80;
        int y = 58 + (i * 37) % 75;
        uint32_t cloud = rgb(s, 190, 189, 173);
        circle(s, x, y, 12, cloud);
        circle(s, x + 14, y - 5, 16, cloud);
        circle(s, x + 31, y + 1, 12, cloud);
        rect(s, x, y, 32, 13, cloud);
    }
}

static void draw_distant_villages(Game *g)
{
    SDL_Surface *s = g->screen;
    int i;
    int drift = (int)(g->scroll * 0.32f);
    for (i = -1; i < 6; ++i) {
        int x = i * 150 - (drift % 150);
        int ground = 238 + (int)(12.0f * sinf((x + drift) * 0.012f));
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

static void draw_explosions(Game *g)
{
    SDL_Surface *s = g->screen;
    int i;
    for (i = 0; i < MAX_EXPLOSIONS; ++i) if (g->explosions[i].active) {
        Explosion *e = &g->explosions[i];
        float p = e->age / e->duration;
        int radius = (int)((e->large ? 27.0f : 15.0f) * p) + 3;
        int x = (int)e->x, y = (int)e->y;
        if (p < 0.48f) {
            circle(s, x, y, radius, rgb(s, 239, 116, 35));
            circle(s, x, y, radius / 2, rgb(s, 255, 221, 108));
        } else {
            circle(s, x - radius / 3, y, radius / 2, rgb(s, 69, 64, 58));
            circle(s, x + radius / 3, y - radius / 4, radius / 2, rgb(s, 88, 82, 73));
            circle(s, x, y - radius / 2, radius / 2, rgb(s, 106, 99, 88));
        }
    }
}

static void draw_player_damage(Game *g)
{
    SDL_Surface *s = g->screen;
    int i;
    uint32_t now = SDL_GetTicks();
    if (g->health >= 80) return;
    for (i = 0; i < 6; ++i) {
        int wobble = (int)(sinf((now * 0.006f) + i * 1.7f) * 5.0f);
        int radius = 4 + i * 2;
        uint8_t grey = (uint8_t)(64 + i * 13);
        circle(s, (int)g->player_x - 31 - i * 10,
               (int)g->player_y + wobble, radius, rgb(s, grey, grey, grey - 5));
    }
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
    uint32_t wood = rgb(s, 54, 38, 27);
    uint32_t brass = rgb(s, 180, 137, 63);
    rect(s, x + 2, y + 3, w, 25, rgb(s, 25, 25, 24));
    rect(s, x, y, w, 25, brass);
    rect(s, x + 2, y + 2, w - 4, 21, wood);
    circle(s, x + 6, y + 6, 2, brass);
    circle(s, x + w - 7, y + 6, 2, brass);
    typewriter_text(g, x + 10, y + 6, label, 7, 11, rgb(s, 239, 215, 155));
}

static void draw_sortie(Game *g)
{
    SDL_Surface *s = g->screen;
    int x, i;
    uint32_t far, near, earth, wire;
    char buf[64];
    if (g->battlefield) {
        SDL_Rect source;
        int limit = g->battlefield->w - SCREEN_W;
        source.x = (int)(g->scroll * 0.035f);
        if (source.x < 0) source.x = 0;
        if (source.x > limit) source.x = limit;
        source.y = 0;
        source.w = SCREEN_W;
        source.h = SCREEN_H;
        SDL_BlitSurface(g->battlefield, &source, s, NULL);
    } else {
        gradient(s, 46, 80, 117, 190, 168, 122);
        draw_sky_detail(g);
    }
    far = rgb(s, 87, 105, 99);
    near = rgb(s, 72, 59, 46);
    earth = rgb(s, 43, 37, 31);
    wire = rgb(s, 43, 39, 34);
    draw_element_layer(g, g->midground, MIDGROUND_ELEMENT_COUNT,
                       0.16f, 285, 285, 0x4d494431u);
    if (!g->battlefield) {
        for (x = 0; x < SCREEN_W; ++x) {
            int fy = 228 + (int)(18 * sinf((x + g->scroll * 0.18f) * 0.012f));
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
        int position = (int)(g->scroll * 0.52f) % g->foreground_panorama->w;
        int first = g->foreground_panorama->w - position;
        if (first > SCREEN_W) first = SCREEN_W;
        /* Cool atmospheric separation behind the darker near-ground layer. */
        blend_rect(s, 0, 235, SCREEN_W, 90, 70, 83, 91, 34);
        source.x = position;
        source.y = 0;
        source.w = first;
        source.h = SCREEN_H;
        dest.x = 0;
        dest.y = 0;
        SDL_BlitSurface(g->foreground_panorama, &source, s, &dest);
        if (first < SCREEN_W) {
            source.x = 0;
            source.w = SCREEN_W - first;
            dest.x = first;
            SDL_BlitSurface(g->foreground_panorama, &source, s, &dest);
        }
    }
    draw_element_layer(g, g->near_props, NEAR_ELEMENT_COUNT,
                       0.52f, 235, 350, 0x4e454152u);
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
        if (sx > -50 && sx < SCREEN_W + 50)
            draw_plane(g, sx, sy, 1, 2 + (int)(sinf(g->scroll * 0.02f + i) * 1.2f));
    }
    for (i = 0; i < MAX_BULLETS; ++i) if (g->bullets[i].active) {
        Bullet *b = &g->bullets[i];
        if (b->type == 1) {
            circle(s, (int)b->x, (int)b->y, 3, rgb(s, 45, 37, 31));
            line(s, (int)b->x - 5, (int)b->y - 5, (int)b->x, (int)b->y,
                 rgb(s, 184, 153, 104));
        } else if (b->type == 2) {
            circle(s, (int)b->x, (int)b->y, 4, rgb(s, 38, 34, 31));
            circle(s, (int)b->x, (int)b->y, 2, rgb(s, 236, 119, 45));
        } else {
            uint32_t tracer = b->enemy ? rgb(s, 255, 113, 57) : rgb(s, 255, 228, 144);
            int length = b->enemy ? 5 : 8;
            rect(s, (int)b->x, (int)b->y, length, 2, tracer);
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
    if (campaign_missions[g->chapter][g->mission].mode == MISSION_DOGFIGHT_3D)
        snprintf(buf, sizeof(buf), "AIR KILLS %d/%d", g->score / 250,
                 current_mission_goal(g));
    else if (current_mission_goal(g) > 0)
        snprintf(buf, sizeof(buf), "TARGETS %d/%d", g->objectives,
                 current_mission_goal(g));
    else
        snprintf(buf, sizeof(buf), "DUTY  PATROL");
    draw_field_sign(g, 8, 39, 112, buf);
    snprintf(buf, sizeof(buf), "SPEED %03d", (int)g->airspeed);
    draw_field_sign(g, 264, 8, 108, buf);
    snprintf(buf, sizeof(buf), "FUEL %02d", (int)g->fuel);
    draw_field_sign(g, 526, 8, 106, buf);
    snprintf(buf, sizeof(buf), "AIRFRAME %03d", g->health > 0 ? g->health : 0);
    draw_field_sign(g, 502, 39, 130, buf);
}

static void draw_debrief(Game *g)
{
    SDL_Surface *s = g->screen;
    char buf[64];
    uint32_t ink = rgb(s, 47, 37, 27);
    uint32_t red = rgb(s, 125, 39, 31);
    if (g->mission_board) SDL_BlitScaled(g->mission_board, NULL, s, NULL);
    else gradient(s, 39, 27, 18, 88, 56, 31);
    display_text(g, 69, 48,
         g->success ? "MISSION ACCOMPLISHED" : "MISSION INCOMPLETE",
         12, 18, red);
    if (campaign_missions[g->chapter][g->mission].mode == MISSION_DOGFIGHT_3D)
        snprintf(buf, sizeof(buf), "ENEMY AIRCRAFT              %d/%d",
                 g->score / 250, current_mission_goal(g));
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
         g->success ? "THE MESS WILL HEAR." : "WE FLY AGAIN AT DAWN.",
         12, 18, red);
    typewriter_text(g, 74, 308,
                    g->success ? "ENTER  CONTINUE THE CAMPAIGN" :
                                 "ENTER  FLY THE MISSION AGAIN",
                    7, 11, ink);
}

static void draw_epilogue(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t ink = rgb(s, 47, 37, 27);
    uint32_t red = rgb(s, 125, 39, 31);
    if (g->mission_board) SDL_BlitScaled(g->mission_board, NULL, s, NULL);
    else gradient(s, 39, 27, 18, 88, 56, 31);
    display_text(g, 72, 48, "11:00 HOURS", 16, 23, red);
    display_text(g, 72, 82, "ARMISTICE", 22, 28, ink);
    typewriter_text(g, 73, 137, "THE GUNS FALL SILENT.", 7, 11, ink);
    typewriter_text(g, 73, 158, "FOUR YEARS OF SKY AND MUD ARE OVER.", 7, 11, ink);
    typewriter_text(g, 73, 179, "THE EMPTY CHAIRS REMAIN.", 7, 11, ink);
    typewriter_text(g, 73, 221, "TOM RINGS THE FIELD BELL.", 7, 11, red);
    typewriter_text(g, 73, 242, "YOU TURN FOR HOME.", 7, 11, red);
    typewriter_text(g, 73, 308, "ENTER  RETURN TO TITLE", 7, 11, ink);
}

static int init_audio(Game *g)
{
    SDL_AudioSpec want, wav_spec;
    uint8_t *wav_data = NULL;
    uint32_t wav_len = 0;
    SDL_AudioCVT cvt;
    if (SDL_LoadWAV(CANVAS_MARCH_WAV, &wav_spec, &wav_data, &wav_len)) {
        int cvt_result = SDL_BuildAudioCVT(&cvt, wav_spec.format,
                                           wav_spec.channels, wav_spec.freq,
                                           AUDIO_S16SYS, 2, 11025);
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
    g->screen = SDL_GetWindowSurface(g->window);
    if (!g->screen || g->screen->format->BytesPerPixel != 4) {
        fprintf(stderr, "Canvas Skies needs a 32-bit SDL window surface\n");
        return 0;
    }
    g->title = SDL_LoadBMP(CANVAS_TITLE_BMP);
    if (!g->title)
        fprintf(stderr, "Title art unavailable (%s): %s\n", CANVAS_TITLE_BMP, SDL_GetError());
    g->mission_board = SDL_LoadBMP(CANVAS_BOARD_BMP);
    if (!g->mission_board)
        fprintf(stderr, "Mission board unavailable (%s): %s\n",
                CANVAS_BOARD_BMP, SDL_GetError());
    g->journal = SDL_LoadBMP(CANVAS_JOURNAL_BMP);
    if (!g->journal)
        fprintf(stderr, "Journal art unavailable (%s): %s\n",
                CANVAS_JOURNAL_BMP, SDL_GetError());
    g->battlefield = SDL_LoadBMP(CANVAS_BATTLEFIELD_BMP);
    if (g->battlefield) {
        SDL_Surface *converted = SDL_ConvertSurface(g->battlefield,
                                                    g->screen->format, 0);
        if (converted) {
            SDL_FreeSurface(g->battlefield);
            g->battlefield = converted;
        }
    } else {
        fprintf(stderr, "Battlefield panorama unavailable (%s): %s\n",
                CANVAS_BATTLEFIELD_BMP, SDL_GetError());
    }
    g->foreground_panorama = SDL_LoadBMP(CANVAS_FOREGROUND_BMP);
    if (g->foreground_panorama)
        SDL_SetColorKey(g->foreground_panorama, SDL_TRUE,
                        SDL_MapRGB(g->foreground_panorama->format, 255, 0, 255));
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
                SDL_SetColorKey(g->midground[i], SDL_TRUE,
                    SDL_MapRGB(g->midground[i]->format, 255, 0, 255));
            else
                fprintf(stderr, "Midground element unavailable (%s): %s\n",
                        mid_paths[i], SDL_GetError());
        }
        for (i = 0; i < NEAR_ELEMENT_COUNT; ++i) {
            g->near_props[i] = SDL_LoadBMP(near_paths[i]);
            if (g->near_props[i])
                SDL_SetColorKey(g->near_props[i], SDL_TRUE,
                    SDL_MapRGB(g->near_props[i]->format, 255, 0, 255));
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
            SDL_SetColorKey(sprites[i], SDL_TRUE,
                            SDL_MapRGB(sprites[i]->format, 255, 0, 255));
    }
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
    init_audio(g);
    set_scene(g, SCENE_TITLE);
    return 1;
}

static void handle_event(Game *g, SDL_Event *ev)
{
    if (ev->type == SDL_QUIT) g->running = 0;
    if (ev->type == SDL_WINDOWEVENT &&
        (ev->window.event == SDL_WINDOWEVENT_EXPOSED ||
         ev->window.event == SDL_WINDOWEVENT_SIZE_CHANGED))
        g->redraw_static = 1;
    if (ev->type != SDL_KEYDOWN || ev->key.repeat) return;
    switch (ev->key.keysym.sym) {
    case SDLK_F10:
        g->fullscreen = !g->fullscreen;
        SDL_SetWindowFullscreen(g->window,
            g->fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
        g->screen = SDL_GetWindowSurface(g->window);
        g->redraw_static = 1;
        break;
    case SDLK_ESCAPE:
        if (g->scene == SCENE_TITLE) g->running = 0;
        else set_scene(g, SCENE_TITLE);
        break;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        if (g->scene == SCENE_TITLE) {
            int pilot;
            g->chapter = 0;
            g->mission = 0;
            for (pilot = 0; pilot < 3; ++pilot) g->wingman_health[pilot] = 100;
            set_scene(g, SCENE_STORY);
        }
        else if (g->scene == SCENE_STORY) set_scene(g, SCENE_BRIEFING);
        else if (g->scene == SCENE_BRIEFING) {
            reset_sortie(g);
            set_scene(g, SCENE_SORTIE);
        } else if (g->scene == SCENE_DEBRIEF) {
            if (!g->success) set_scene(g, SCENE_BRIEFING);
            else if (g->mission + 1 < MISSIONS_PER_CHAPTER) {
                ++g->mission;
                set_scene(g, SCENE_STORY);
            } else if (g->chapter + 1 >= CHAPTER_COUNT) {
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
    (void)argc; (void)argv;
    if (!game_init(&g)) return 1;
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
        if (g.scene == SCENE_TITLE || g.scene == SCENE_SORTIE ||
            g.redraw_static) {
            if (SDL_MUSTLOCK(g.screen)) SDL_LockSurface(g.screen);
            switch (g.scene) {
            case SCENE_TITLE: draw_title(&g); break;
            case SCENE_STORY: draw_story(&g); break;
            case SCENE_BRIEFING: draw_briefing(&g); break;
            case SCENE_SORTIE: draw_sortie(&g); break;
            case SCENE_DEBRIEF: draw_debrief(&g); break;
            case SCENE_EPILOGUE: draw_epilogue(&g); break;
            }
            if (SDL_MUSTLOCK(g.screen)) SDL_UnlockSurface(g.screen);
            SDL_UpdateWindowSurface(g.window);
            g.redraw_static = 0;
        }
        SDL_Delay(8);
    }
    if (g.audio.device) SDL_CloseAudioDevice(g.audio.device);
    if (g.title) SDL_FreeSurface(g.title);
    if (g.mission_board) SDL_FreeSurface(g.mission_board);
    if (g.journal) SDL_FreeSurface(g.journal);
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
    SDL_DestroyWindow(g.window);
    SDL_Quit();
    return 0;
}
