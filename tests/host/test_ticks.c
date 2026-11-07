/* Host test: guns held by a last_shot in the future do not fire early. */
#include <stdio.h>
#include "../../src/modern/canvas_ticks.h"

static int failures;
#define CHECK(cond) do { if (!(cond)) { \
    printf("FAIL line %d: %s\n", __LINE__, #cond); ++failures; } } while (0)

int main(void)
{
    uint32_t spawn = 10000u, grace = spawn + 650u;  /* last_shot set ahead */
    /* During the grace, with any fire delay, no shot. */
    CHECK(!ticks_past(spawn, grace, 0u));
    CHECK(!ticks_past(spawn + 649u, grace, 0u));
    CHECK(!ticks_past(spawn, grace, 900u));
    /* After the grace and the delay, a shot. */
    CHECK(!ticks_past(grace + 900u, grace, 900u));
    CHECK(ticks_past(grace + 901u, grace, 900u));
    /* Ordinary cooldown after a shot. */
    CHECK(!ticks_past(5000u, 4500u, 500u));
    CHECK(ticks_past(5001u, 4500u, 500u));
    /* Across the 32-bit wrap. */
    CHECK(!ticks_past(0x00000010u, 0xfffffff0u, 100u));
    CHECK(ticks_past(0x00000080u, 0xfffffff0u, 100u));
    CHECK(!ticks_past(0xfffffff0u, 0x00000100u, 0u));
    if (!failures) printf("test_ticks: all passed\n");
    return failures != 0;
}
