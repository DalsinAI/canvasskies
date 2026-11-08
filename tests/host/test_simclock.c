/* Host test: the sortie clock keeps step with the flying, whatever the
 * frame rate, and the cooldowns read from it hold. */
#include <stdio.h>
#include "../../src/modern/canvas_ticks.h"

static int failures;
#define CHECK(cond) do { if (!(cond)) { \
    printf("FAIL line %d: %s\n", __LINE__, #cond); ++failures; } } while (0)

/* Advances a fresh clock by 'frames' steps of 'dt' seconds. */
static uint32_t run(uint32_t start, int frames, float dt)
{
    uint32_t ms = start;
    float carry = 0.0f;
    int i;
    for (i = 0; i < frames; ++i) sim_advance(&ms, &carry, dt);
    return ms;
}

/* How many times a gun with a 130 ms cooldown fires in 'frames' frames. */
static int shots(int frames, float dt)
{
    uint32_t ms = 10000u, last = 0u;
    float carry = 0.0f;
    int i, n = 0;
    for (i = 0; i < frames; ++i) {
        sim_advance(&ms, &carry, dt);
        if (ticks_past(ms, last, 129u)) { last = ms; ++n; }
    }
    return n;
}

int main(void)
{
    /* Sixty steps of a sixtieth make a second, without the fraction lost. */
    CHECK(run(0u, 60, 1.0f / 60.0f) >= 999u && run(0u, 60, 1.0f / 60.0f) <= 1001u);
    /* Ten minutes at 50 frames a second: no drift worth a frame. */
    CHECK(run(0u, 30000, 0.02f) >= 599990u && run(0u, 30000, 0.02f) <= 600010u);
    /* A long stall moves the clock only as far as the world: one clamped
     * step, 50 ms, however long the frame took. */
    CHECK(run(0u, 1, 2.5f) == 50u);
    CHECK(run(0u, 1, -1.0f) == 0u);
    /* At eight frames a second the clock runs at the clamped step, so
     * eight such frames are 400 ms of sortie, not a second. */
    CHECK(run(0u, 8, 0.125f) == 400u);
    /* The same stretch of sortie, flown fast or slow, fires about the same
     * number of rounds: 20 s at 50 fps against 20 s of 50 ms steps. */
    {
        int fast = shots(1000, 0.02f), slow = shots(400, 0.125f);
        CHECK(fast >= 130 && fast <= 155);
        CHECK(slow >= 130 && slow <= 155);
        CHECK(fast - slow <= 25 && slow - fast <= 25);
    }
    /* Across the 32-bit wrap the clock and the cooldown still agree. */
    {
        uint32_t ms = 0xffffff00u, at = ms;
        float carry = 0.0f;
        int i;
        for (i = 0; i < 20; ++i) sim_advance(&ms, &carry, 0.02f);
        CHECK(ms == 0x00000090u);
        CHECK(ticks_past(ms, at, 399u));
        CHECK(!ticks_past(ms, at, 400u));
    }
    if (!failures) printf("test_simclock: all passed\n");
    return failures != 0;
}
