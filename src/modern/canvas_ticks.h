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

#endif
