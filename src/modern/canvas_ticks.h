#ifndef CANVAS_TICKS_H
#define CANVAS_TICKS_H

#include <stdint.h>

/* Whether more than 'delay' ms have passed since 'at'.  A gun's last_shot
 * may lie in the future, to hold a newcomer's fire for a moment; an
 * unsigned now - at would then wrap to a huge number and let it fire at
 * once, so the difference is taken as signed.  It also holds across the
 * millisecond counter's own wrap after 49 days. */
static inline int ticks_past(uint32_t now, uint32_t at, uint32_t delay)
{
    return (int32_t)(now - at) > (int32_t)delay;
}

/* The sortie clock: advanced by the same clamped step the flying uses,
 * in whole milliseconds, carrying the fraction over so that sixty steps
 * of a sixtieth of a second make one second exactly, not 960 ms.  The
 * step is clamped as the main loop clamps it, so a long stall moves the
 * clock no further than it moves the world. */
#define SIM_MAX_STEP 0.05f
static inline void sim_advance(uint32_t *ms, float *carry, float dt)
{
    float whole;
    if (dt < 0.0f) dt = 0.0f;
    if (dt > SIM_MAX_STEP) dt = SIM_MAX_STEP;
    *carry += dt * 1000.0f;
    whole = (float)(uint32_t)*carry;
    *ms += (uint32_t)whole;
    *carry -= whole;
}

#endif
