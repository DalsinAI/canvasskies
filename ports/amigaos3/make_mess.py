#!/usr/bin/env python3
"""Paint the squadron mess at night.

A timber hut lit by a hanging oil lamp: bunting along the ceiling, the
squadron board on the left wall (left blank: the game chalks the victories
on it, x 48..332, y 72..252), a long table with glasses and bottles, the
gramophone with its brass horn on a side table, a stove and a propeller
hung on the wall.  The lower part is left dark for the evening's lines.
Writes assets/canvas-skies/mess.bmp.
"""

import math
import sys
from pathlib import Path

import numpy as np
from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parent))
from paint import Canvas  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "canvas-skies" / "mess.bmp"
W, H = 640, 400
RNG = np.random.default_rng(1918)
BOARD = (40, 64, 340, 260)


def room():
    yy, xx = np.mgrid[0:H, 0:W].astype(np.float32)
    base = np.zeros((H, W, 3), np.float32)
    plank = ((xx // 26) % 2) * 6 + np.sin(yy * 0.07 + (xx // 26) * 2.3) * 4
    for c, v in enumerate((58, 40, 28)):
        base[..., c] = v + plank
    base[(xx % 26) < 1.5] *= 0.62
    floor = yy > 268
    base[floor] = np.array([44, 32, 22]) + (np.sin(xx[floor] * 0.04) * 3)[..., None]
    base += RNG.normal(0, 3.5, (H, W, 1))
    lamp = np.exp(-(((xx - 470) / 230) ** 2 + ((yy - 110) / 170) ** 2))
    base *= (0.32 + 1.05 * lamp)[..., None] * np.array([1.0, 0.86, 0.62])
    return Image.fromarray(np.clip(base, 0, 255).astype(np.uint8))


def furniture(c):
    wood, dark = (112, 72, 42), (60, 38, 24)
    x0, y0, x1, y1 = BOARD
    c.part([(x0, y0), (x1, y0), (x1, y1), (x0, y1)], (96, 62, 34), gloss=0.3,
           round_=1.5)                                   # the board's frame
    c.part([(x0 + 8, y0 + 8), (x1 - 8, y0 + 8), (x1 - 8, y1 - 8), (x0 + 8, y1 - 8)],
           (28, 40, 32), gloss=0.05, grain=6, rim=0.25, round_=0.5)
    for k in range(6):                                   # old chalk smudges
        x, y = RNG.uniform(x0 + 20, x1 - 30), RNG.uniform(y0 + 20, y1 - 20)
        c.part([(x, y), (x + 26, y + 6)], (52, 64, 56), shape="ellipse",
               gloss=0, rim=0, round_=2)
    # The long table along the floor, a cloth, glasses and bottles.
    c.part([(360, 236), (640, 230), (640, 262), (360, 266)], wood, gloss=0.35,
           round_=1.5)
    c.part([(380, 232), (620, 228), (620, 238), (380, 242)], (214, 206, 186),
           gloss=0.2, round_=1)
    for k, x in enumerate((396, 430, 470, 520, 566, 600)):
        if k % 2:
            c.part([(x - 4, 200), (x + 4, 200), (x + 5, 232), (x - 5, 232)],
                   (40, 70, 44), gloss=0.7, round_=1.5)  # a bottle
            c.part([(x - 2, 190), (x + 2, 200)], (40, 70, 44), round_=0.6)
        else:
            c.part([(x - 5, 216), (x + 5, 216), (x + 4, 232), (x - 4, 232)],
                   (200, 190, 150), gloss=0.8, grain=2, rim=0.2,
                   round_=1)                             # a glass of beer
    # The gramophone on its side table, the horn turned to the room.
    c.part([(548, 188), (606, 188), (606, 214), (548, 214)], dark, gloss=0.4,
           round_=1.5)
    c.part([(556, 180), (598, 188)], (30, 26, 22), shape="ellipse", round_=0.5)
    c.part([(574, 184), (612, 130), (636, 110), (636, 158), (590, 186)],
           (186, 140, 64), gloss=0.7, round_=2)          # brass horn
    c.part([(612, 112), (640, 156)], (120, 84, 34), shape="ellipse", round_=1)
    # A propeller hung on the wall above the table.
    c.part([(420, 92), (560, 72), (562, 80), (422, 100)], (126, 82, 44),
           gloss=0.5, round_=3)
    c.part([(482, 78), (500, 96)], (70, 50, 30), shape="ellipse", round_=0.6)
    # The stove at the right, its fire showing.
    c.part([(600, 250), (636, 250), (636, 300), (600, 300)], (40, 38, 36),
           gloss=0.45, round_=3)
    c.part([(608, 270), (628, 290)], (255, 140, 50), shape="ellipse", gloss=0,
           rim=0, round_=1)
    c.glow((618, 280), 30, (255, 140, 50), 0.5)


def lamp_and_bunting(c):
    c.line((470, 0), (470, 70), (40, 36, 30), 1.0)
    c.part([(458, 70), (482, 92)], (200, 168, 90), shape="ellipse", gloss=0.6,
           round_=1)
    c.glow((470, 84), 34, (255, 210, 130), 0.9)
    cols = [(176, 40, 34), (230, 224, 210), (40, 60, 128)]
    for k in range(17):
        x = k * 40
        sag = 8 * math.sin(math.pi * ((x % 320) / 320))
        c.part([(x, 12 + sag), (x + 30, 12 + sag), (x + 15, 36 + sag)],
               cols[k % 3], gloss=0.25, round_=0.4)
    c.line((0, 12), (640, 12), (90, 74, 52), 0.8)


def main():
    img = room()
    c = Canvas(W, H, light=(0.3, -1.0))
    furniture(c)
    lamp_and_bunting(c)
    layer = c.image()
    key = np.all(np.asarray(layer) == (255, 0, 255), axis=2)
    a = np.asarray(img).copy()
    a[~key] = np.asarray(layer)[~key]
    # Keep the foot of the room dark for the evening's lines of text.
    yy = np.arange(H)[:, None]
    a = (a * np.where(yy > 272, 0.55, 1.0)[..., None]).astype(np.uint8)
    Image.fromarray(a).save(OUT)


if __name__ == "__main__":
    main()
