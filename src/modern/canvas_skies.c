/*
 * Canvas Skies: 1917
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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef CANVAS_TITLE_BMP
#define CANVAS_TITLE_BMP "assets/canvas-skies/title.bmp"
#endif
#ifndef CANVAS_DISPLAY_FONT_BMP
#define CANVAS_DISPLAY_FONT_BMP "assets/canvas-skies/font-display.bmp"
#endif
#ifndef CANVAS_TYPEWRITER_FONT_BMP
#define CANVAS_TYPEWRITER_FONT_BMP "assets/canvas-skies/font-typewriter.bmp"
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

typedef enum {
    SCENE_TITLE,
    SCENE_BRIEFING,
    SCENE_SORTIE,
    SCENE_DEBRIEF
} Scene;

typedef struct {
    float x, y, vx, vy;
    float life;
    int enemy;
    int type;
    int active;
} Bullet;

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
} AudioState;

typedef struct {
    SDL_Window *window;
    SDL_Surface *screen;
    SDL_Surface *title;
    SDL_Surface *font_display;
    SDL_Surface *font_typewriter;
    SDL_Surface *player_sprites;
    SDL_Surface *enemy_sprites;
    AudioState audio;
    Scene scene;
    int running;
    int fullscreen;
    int redraw_static;
    int success;
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
    Enemy enemies[MAX_ENEMIES];
    Target targets[MAX_TARGETS];
} Game;

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

/* Pre-rasterised Liberation faces: serif display and field typewriter. */
static void period_text(SDL_Surface *dst, SDL_Surface *atlas,
                        int cell_w, int cell_h, int x, int y,
                        const char *str, int draw_w, int draw_h,
                        uint32_t color)
{
    if (!atlas) {
        text(dst, x, y, str, draw_h >= 20 ? 3 : 1, color);
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
            for (py = 0; py < draw_h; ++py) {
                int sy = sy0 + py * cell_h / draw_h;
                for (px = 0; px < draw_w; ++px) {
                    int sx = sx0 + px * cell_w / draw_w;
                    uint8_t r, g, b;
                    SDL_GetRGB(surface_pixel(atlas, sx, sy), atlas->format,
                               &r, &g, &b);
                    if ((int)r + (int)g + (int)b > 150)
                        pixel(dst, x + px, y + py, color);
                }
            }
        }
        x += draw_w;
    }
    if (SDL_MUSTLOCK(atlas)) SDL_UnlockSurface(atlas);
}

static void display_text(Game *g, int x, int y, const char *str,
                         int w, int h, uint32_t color)
{
    period_text(g->screen, g->font_display, 40, 40, x, y, str, w, h, color);
}

static void typewriter_text(Game *g, int x, int y, const char *str,
                            int w, int h, uint32_t color)
{
    period_text(g->screen, g->font_typewriter, 16, 20, x, y, str, w, h, color);
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
        if (a->music_data && a->music_len >= 4) {
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
            else if (a->scene == SCENE_BRIEFING)
                sample = pad * 0.75f + bass * 0.35f;
            else
                sample = pad * 0.65f + bass + melody + drums;
        }
        if (a->shot > 0) {
            sample += ((a->shot & 4) ? 0.28f : -0.28f);
            --a->shot;
        }
        if (a->blast > 0) {
            sample += ((float)((a->noise >> 16) & 0xffff) / 32768.0f - 1.0f)
                      * ((float)a->blast / 5000.0f) * 0.65f;
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
    return 284 + (int)(16.0f * sinf(world_x * 0.007f)
                     + 9.0f * sinf(world_x * 0.019f));
}

static void reset_sortie(Game *g)
{
    int i;
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
    for (i = 0; i < MAX_ENEMIES; ++i) {
        g->enemies[i].x = 760.0f + i * 620.0f;
        g->enemies[i].y = 90.0f + (float)((i * 47) % 120);
        g->enemies[i].phase = i * 0.8f;
        g->enemies[i].hp = 2;
        g->enemies[i].active = 1;
        g->enemies[i].last_shot = 0;
    }
    for (i = 0; i < MAX_TARGETS; ++i) {
        g->targets[i].world_x = 560.0f + i * 480.0f;
        g->targets[i].active = 1;
        g->targets[i].type = i & 1; /* machine gun, then anti-aircraft gun */
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

static void aimed_fire(Game *g, float x, float y, float speed, int type)
{
    float dx = g->player_x - x;
    float dy = g->player_y - y;
    float length = sqrtf(dx * dx + dy * dy);
    if (length < 1.0f) length = 1.0f;
    if (new_bullet(g, x, y, dx / length * speed, dy / length * speed,
                   1, type, type == 2 ? 2.8f : 3.5f))
        g->audio.shot = type == 2 ? 90 : 140;
}

static void fire(Game *g)
{
    uint32_t now = SDL_GetTicks();
    if (now - g->last_shot < 130) return;
    if (new_bullet(g, g->player_x + 34.0f, g->player_y - 1.0f,
                   390.0f, g->player_vy * 0.12f, 0, 0, 2.0f)) {
        g->last_shot = now;
        g->audio.shot = 180;
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
    if (keys[SDL_SCANCODE_X] || keys[SDL_SCANCODE_RIGHT]) g->airspeed += 42.0f * dt;
    if (keys[SDL_SCANCODE_Z] || keys[SDL_SCANCODE_LEFT]) g->airspeed -= 42.0f * dt;
    if (g->airspeed < 62.0f) g->airspeed = 62.0f;
    if (g->airspeed > 142.0f) g->airspeed = 142.0f;
    if (keys[SDL_SCANCODE_A]) g->player_x -= 125.0f * dt;
    if (keys[SDL_SCANCODE_D]) g->player_x += 125.0f * dt;
    if (g->player_x < 72.0f) g->player_x = 72.0f;
    if (g->player_x > 430.0f) g->player_x = 430.0f;
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_COMMA])
        g->player_vy -= 245.0f * dt;
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_SLASH])
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
    if (keys[SDL_SCANCODE_SPACE]) fire(g);
    if (keys[SDL_SCANCODE_B]) drop_bomb(g);

    for (i = 0; i < MAX_BULLETS; ++i) if (g->bullets[i].active) {
        Bullet *b = &g->bullets[i];
        if (b->type == 1) b->vy += 105.0f * dt;
        b->x += b->vx * dt;
        b->y += b->vy * dt;
        b->life -= dt;
        if (b->life <= 0.0f || b->x < -24.0f || b->x > SCREEN_W + 24.0f ||
            b->y < 24.0f || b->y > SCREEN_H + 24.0f)
            b->active = 0;
        if (b->active && b->enemy && fabsf(b->x - g->player_x) < 25.0f &&
            fabsf(b->y - g->player_y) < 14.0f && now - g->last_damage > 260u) {
            g->health -= b->type == 2 ? 8 : 3;
            g->last_damage = now;
            b->active = 0;
            g->audio.blast = b->type == 2 ? 2200 : 700;
        }
    }

    for (i = 0; i < MAX_ENEMIES; ++i) if (g->enemies[i].active) {
        float sx = g->enemies[i].x - g->scroll;
        float sy = g->enemies[i].y + sinf(g->scroll * 0.01f + g->enemies[i].phase) * 30.0f;
        if (sx < -80) g->enemies[i].active = 0;
        if (sx > g->player_x + 65.0f && sx < SCREEN_W + 20.0f &&
            now - g->enemies[i].last_shot > (uint32_t)(1250 + i * 73 % 520)) {
            aimed_fire(g, sx - 30.0f, sy, 225.0f, 0);
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
                    g->audio.blast = 4200;
                }
            }
        }
        if (fabsf(sx - g->player_x) < 36.0f && fabsf(sy - g->player_y) < 18.0f) {
            g->health -= 25;
            g->enemies[i].active = 0;
            g->audio.blast = 3600;
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
                g->audio.blast = 5000;
            }
        }
    }
    if (g->scroll > 6500.0f || g->fuel <= 0.0f || g->health <= 0) {
        g->success = g->objectives >= 4 && g->health > 0 && g->fuel > 0;
        set_scene(g, SCENE_DEBRIEF);
    }
}

static void draw_title(Game *g)
{
    SDL_Surface *s = g->screen;
    if (g->title) SDL_BlitScaled(g->title, NULL, s, NULL);
    else rect(s, 0, 0, SCREEN_W, SCREEN_H, rgb(s, 20, 31, 48));
    blend_rect(s, 0, 0, 640, 120, 5, 12, 24, 145);
    blend_rect(s, 0, 300, 640, 60, 5, 10, 18, 150);
    display_text(g, 34, 20, "CANVAS SKIES", 42, 44, rgb(s, 238, 203, 126));
    display_text(g, 40, 68, "1917", 25, 28, rgb(s, 224, 228, 222));
    typewriter_text(g, 40, 316, "ENTER  BEGIN CAMPAIGN", 10, 15,
                    rgb(s, 244, 222, 165));
    typewriter_text(g, 470, 340, "ORIGINAL PROTOTYPE", 8, 11,
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

static void draw_briefing(Game *g)
{
    SDL_Surface *s = g->screen;
    uint32_t cream, ink, red, blue;
    gradient(s, 17, 28, 43, 37, 48, 58);
    cream = rgb(s, 226, 216, 185);
    ink = rgb(s, 34, 33, 29);
    red = rgb(s, 139, 42, 33);
    blue = rgb(s, 55, 83, 103);
    rect(s, 32, 24, 576, 312, cream);
    rect(s, 44, 36, 552, 4, red);
    display_text(g, 52, 50, "SQUADRON BRIEFING", 24, 27, ink);
    typewriter_text(g, 52, 88, "17 OCTOBER 1917  0540 HOURS", 9, 13, red);
    typewriter_text(g, 52, 116, "MISSION: BREAK THE MORNING BARRAGE", 9, 13, ink);
    typewriter_text(g, 52, 138, "CROSS THE LINES AT LOW ALTITUDE.", 9, 13, ink);
    typewriter_text(g, 52, 154, "DESTROY FOUR GUN EMPLACEMENTS.", 9, 13, ink);
    typewriter_text(g, 52, 170, "RETURN WITH FUEL AND AIRFRAME INTACT.", 9, 13, ink);
    rect(s, 366, 104, 202, 142, rgb(s, 202, 195, 166));
    line(s, 380, 212, 550, 126, blue);
    line(s, 380, 218, 550, 132, blue);
    circle(s, 398, 203, 7, red);
    circle(s, 536, 140, 7, red);
    typewriter_text(g, 378, 226, "AERODROME", 8, 11, ink);
    typewriter_text(g, 494, 112, "TARGETS", 8, 11, ink);
    display_text(g, 52, 220, "CONTROLS", 18, 21, red);
    typewriter_text(g, 52, 250, "UP DOWN  CLIMB DIVE", 9, 13, ink);
    typewriter_text(g, 52, 266, "A D      MOVE IN FORMATION", 9, 13, ink);
    typewriter_text(g, 52, 282, "X Z      THROTTLE", 9, 13, ink);
    typewriter_text(g, 52, 298, "SPACE FIRE   B BOMB", 9, 13, ink);
    typewriter_text(g, 52, 318, "ENTER  FLY THE MISSION", 9, 13, red);
}

static void draw_ground_gun(Game *g, int sx, int ty, Target *target)
{
    SDL_Surface *s = g->screen;
    uint32_t iron = rgb(s, 45, 46, 39);
    uint32_t olive = rgb(s, 72, 75, 57);
    uint32_t sand = rgb(s, 119, 96, 66);
    int flash = SDL_GetTicks() - target->last_shot < 90;
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
    int base = ((int)g->scroll / 180) * 180;
    for (i = -1; i < 6; ++i) {
        int wx = base + i * 180;
        int sx = wx - (int)g->scroll;
        int gy = terrain_y((float)wx);
        if (((wx / 180) & 1) == 0) {
            rect(s, sx - 15, gy - 22, 30, 22, rgb(s, 91, 68, 49));
            rect(s, sx - 18, gy - 26, 36, 6, rgb(s, 57, 48, 42));
            rect(s, sx - 4, gy - 13, 8, 13, rgb(s, 40, 35, 31));
        } else {
            rect(s, sx - 2, gy - 19, 4, 19, rgb(s, 57, 43, 31));
            circle(s, sx, gy - 25, 12, rgb(s, 45, 69, 43));
            circle(s, sx - 8, gy - 20, 8, rgb(s, 53, 78, 47));
        }
    }
}

static void draw_sortie(Game *g)
{
    SDL_Surface *s = g->screen;
    int x, i;
    uint32_t far, near, earth, wire;
    char buf[64];
    gradient(s, 46, 80, 117, 190, 168, 122);
    far = rgb(s, 87, 105, 99);
    near = rgb(s, 66, 75, 61);
    earth = rgb(s, 80, 61, 43);
    wire = rgb(s, 43, 39, 34);
    for (x = 0; x < SCREEN_W; ++x) {
        int fy = 228 + (int)(18 * sinf((x + g->scroll * 0.18f) * 0.012f));
        rect(s, x, fy, 1, SCREEN_H - fy, far);
    }
    for (x = 0; x < SCREEN_W; ++x) {
        float wx = g->scroll + x;
        int gy = terrain_y(wx);
        rect(s, x, gy, 1, SCREEN_H - gy, near);
        rect(s, x, gy + 9, 1, SCREEN_H - gy - 9, earth);
    }
    for (x = 40; x < SCREEN_W; x += 90) {
        line(s, x, 276, x, 308, wire);
        line(s, x + 2, 276, x + 2, 308, wire);
        line(s, x, 282, x + 90, 293, wire);
    }
    draw_scenery(g);
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
    {
        int pitch = 2 + (int)(g->player_vy / 45.0f);
        draw_plane(g, (int)g->player_x, (int)g->player_y, 0, pitch);
    }
    blend_rect(s, 0, 0, 640, 34, 7, 13, 21, 175);
    snprintf(buf, sizeof(buf), "SCORE %05d", g->score);
    typewriter_text(g, 12, 9, buf, 8, 12, rgb(s, 237, 221, 171));
    snprintf(buf, sizeof(buf), "GUNS %d OF 4  SPD %03d", g->objectives, (int)g->airspeed);
    typewriter_text(g, 236, 9, buf, 8, 12, rgb(s, 237, 221, 171));
    snprintf(buf, sizeof(buf), "FUEL %02d  AIR %03d", (int)g->fuel, g->health);
    typewriter_text(g, 492, 9, buf, 7, 12, rgb(s, 237, 221, 171));
}

static void draw_debrief(Game *g)
{
    SDL_Surface *s = g->screen;
    char buf[64];
    gradient(s, 20, 28, 39, 71, 65, 53);
    blend_rect(s, 56, 34, 528, 292, 224, 215, 186, 230);
    display_text(g, 82, 56,
         g->success ? "MISSION ACCOMPLISHED" : "MISSION INCOMPLETE",
         22, 26, rgb(s, 80, 36, 27));
    snprintf(buf, sizeof(buf), "GUN EMPLACEMENTS DESTROYED  %d", g->objectives);
    typewriter_text(g, 86, 122, buf, 9, 13, rgb(s, 35, 34, 29));
    snprintf(buf, sizeof(buf), "AIRCRAFT SCORE              %d", g->score);
    typewriter_text(g, 86, 144, buf, 9, 13, rgb(s, 35, 34, 29));
    snprintf(buf, sizeof(buf), "FUEL REMAINING              %d", (int)g->fuel);
    typewriter_text(g, 86, 166, buf, 9, 13, rgb(s, 35, 34, 29));
    display_text(g, 86, 214,
         g->success ? "THE MESS WILL HEAR OF THIS." : "WE FLY AGAIN AT DAWN.",
         16, 20, rgb(s, 80, 36, 27));
    typewriter_text(g, 86, 286, "ENTER  RETURN TO OPERATIONS", 9, 13,
                    rgb(s, 35, 34, 29));
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
    g->window = SDL_CreateWindow("Canvas Skies: 1917",
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
    g->font_display = SDL_LoadBMP(CANVAS_DISPLAY_FONT_BMP);
    g->font_typewriter = SDL_LoadBMP(CANVAS_TYPEWRITER_FONT_BMP);
    if (!g->font_display || !g->font_typewriter)
        fprintf(stderr, "Period font atlas unavailable; using fallback font\n");
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
        if (g->scene == SCENE_TITLE) set_scene(g, SCENE_BRIEFING);
        else if (g->scene == SCENE_BRIEFING) {
            reset_sortie(g);
            set_scene(g, SCENE_SORTIE);
        } else if (g->scene == SCENE_DEBRIEF) set_scene(g, SCENE_TITLE);
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
        if (g.scene == SCENE_SORTIE || g.redraw_static) {
            if (SDL_MUSTLOCK(g.screen)) SDL_LockSurface(g.screen);
            switch (g.scene) {
            case SCENE_TITLE: draw_title(&g); break;
            case SCENE_BRIEFING: draw_briefing(&g); break;
            case SCENE_SORTIE: draw_sortie(&g); break;
            case SCENE_DEBRIEF: draw_debrief(&g); break;
            }
            if (SDL_MUSTLOCK(g.screen)) SDL_UnlockSurface(g.screen);
            SDL_UpdateWindowSurface(g.window);
            g.redraw_static = 0;
        }
        SDL_Delay(8);
    }
    if (g.audio.device) SDL_CloseAudioDevice(g.audio.device);
    if (g.title) SDL_FreeSurface(g.title);
    if (g.font_display) SDL_FreeSurface(g.font_display);
    if (g.font_typewriter) SDL_FreeSurface(g.font_typewriter);
    if (g.player_sprites) SDL_FreeSurface(g.player_sprites);
    if (g.enemy_sprites) SDL_FreeSurface(g.enemy_sprites);
    if (g.audio.music_data) SDL_free(g.audio.music_data);
    SDL_DestroyWindow(g.window);
    SDL_Quit();
    return 0;
}
