#!/usr/bin/env python3
"""make_unarmed_cockpit.py IN.bmp OUT.bmp: the pilot's cockpit without its twin guns and ring sight.
Above a rounded decking hump the guns become the clear colour (the sky shows through); the hump
is doped fabric over formers, lit like the rest of the painting: warm on its crest, dark on its
flanks, with a centre seam.  The leather coaming below (y >= 308) stays."""
import sys
import numpy as np
from PIL import Image

src = np.asarray(Image.open(sys.argv[1]).convert("RGB")).astype(np.float32)
out = src.copy()
KEY = np.array([255, 0, 255], np.float32)
x0, x1, coaming = 214, 427, 308
cx, half = 320.0, (x1 - x0) / 2.0
rng = np.random.default_rng(1914)
lit = np.array([196, 150, 104], np.float32)     # fabric in the low sun
shadow = np.array([86, 62, 46], np.float32)     # the same fabric on the flank
for x in range(x0, x1):
    u = (x - cx) / half                         # -1 .. 1 across the decking
    top = int(round(272 + 30 * u * u))
    for y in range(170, coaming):
        if y < top:
            out[y, x] = KEY
            continue
        down = (y - top) / max(1, coaming - top)    # 0 at the crest, 1 at the coaming
        t = np.clip(1.0 - 0.75 * abs(u) ** 1.4 - 0.35 * down, 0, 1)
        c = shadow + (lit - shadow) * t
        c *= 0.96 + 0.08 * rng.random()             # grain of the doped linen
        if abs(x - cx) < 1.0: c *= 0.72             # the centre seam

        out[y, x] = c
    if 0 <= top < coaming:
        out[top, x] = np.minimum(out[top, x] * 1.18 + 14, 255)   # the crest catches the light
Image.fromarray(out.clip(0, 255).astype(np.uint8)).save(sys.argv[2])
