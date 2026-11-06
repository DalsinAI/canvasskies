#!/usr/bin/env python3
"""Paint the decorations and the office crest.

Five medals on their ribbons, 72x112 each, in the order of the game's
decorations table: the bronze oak leaf of a Mention in Despatches; the
Military Cross, a silver cross with the royal cipher on white-purple-white
watered silk; the Distinguished Flying Cross, a silver cross flory with
wings and propeller on violet-and-white diagonal stripes; the
Distinguished Service Order, a white enamelled cross in gold on red silk
edged blue; and the Military Cross with its silver bar.  Then a crown and
laurel crest, 64x56, for the office papers.  Metal is shaded from the low
sun with a bright rim and a darker hollow.

Writes assets/canvas-skies/medals.bmp (6 cells, 72x112, magenta clear).
"""

import math
import sys
from pathlib import Path

import numpy as np
from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parent))
from paint import Canvas, KEY, rot  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "canvas-skies" / "medals.bmp"
SILVER, SILVER_D = (196, 198, 204), (110, 112, 120)
GOLD, BRONZE = (206, 164, 74), (150, 98, 46)
ENAMEL = (236, 234, 226)
PURPLE, WHITE = (104, 44, 116), (236, 232, 220)
RED, BLUE = (168, 30, 34), (36, 52, 124)


def C():
    return Canvas(72, 112, light=(-0.5, -0.9))


def ribbon(c, bands, diagonal=False):
    """Watered silk with its folded top and a pin bar."""
    x0, x1, y0, y1 = 14, 58, 4, 50
    if diagonal:
        for k in range(-10, 14):
            col = bands[k % 2]
            x = x0 + k * 5
            c.part([(x, y0), (x + 5, y0), (x + 5 + 46, y1), (x + 46, y1)], col,
                   gloss=0.1, grain=3, rim=0.0, round_=0)
    else:
        w = (x1 - x0) / len(bands)
        for k, col in enumerate(bands):
            c.part([(x0 + k * w, y0), (x0 + (k + 1) * w, y0),
                    (x0 + (k + 1) * w, y1), (x0 + k * w, y1)], col, gloss=0.12,
                   grain=3, rim=0.05, round_=0)
    for y in range(y0 + 3, y1, 4):                       # the watering
        c.line((x0, y), (x1, y + 1.5), (255, 255, 255), 0.25)
    c.part([(x0 - 1, y0 - 3), (x1 + 1, y0 - 3), (x1 + 1, y0 + 1),
            (x0 - 1, y0 + 1)], (120, 120, 126), gloss=0.5, round_=0.5)  # pin bar
    c.part([(31, 48), (41, 48), (40, 58), (32, 58)], SILVER_D, gloss=0.4,
           round_=1)                                     # the suspender


def cross_pattee(c, cx, cy, r, col, arms_w=0.42):
    for a in range(4):
        ang = a * math.pi / 2
        pts = [(cx, cy), (cx + r * arms_w * 0.4, cy - r * 0.2),
               (cx + r * arms_w, cy - r), (cx - r * arms_w, cy - r),
               (cx - r * arms_w * 0.4, cy - r * 0.2)]
        c.part(rot(pts, ang, (cx, cy)), col, gloss=0.55, grain=2, rim=0.5,
               round_=0.4)


def leaf(c):
    """The bronze oak leaf of a Mention in Despatches, pinned on its own."""
    cx, cy = 36, 56
    pts = []
    for k in range(13):
        t = k / 12
        y = cy - 34 + t * 64
        w = 13 * math.sin(math.pi * t) * (1 + 0.25 * math.sin(t * 5 * math.pi))
        pts.append((cx + w, y))
    pts += [(cx - (x - cx), y) for x, y in reversed(pts)]
    c.part(pts, BRONZE, gloss=0.6, grain=3, rim=0.5, round_=0.6)
    c.line((cx, cy - 32), (cx, cy + 32), (96, 60, 26), 1.0)
    for k in range(4):
        y = cy - 20 + k * 12
        c.line((cx, y), (cx + 9, y - 6), (110, 70, 30), 0.6)
        c.line((cx, y), (cx - 9, y - 6), (110, 70, 30), 0.6)


def mc(c, bar=False):
    ribbon(c, [WHITE, PURPLE, WHITE])
    if bar:
        c.part([(14, 30), (58, 30), (58, 37), (14, 37)], SILVER, gloss=0.6,
               round_=1)
        c.part([(30, 31), (42, 36)], (240, 240, 244), shape="ellipse", round_=0.3)
    cross_pattee(c, 36, 80, 22, SILVER)
    for a in range(4):                                   # crowns on the arms
        x, y = rot([(36, 60)], a * math.pi / 2, (36, 80))[0]
        c.part([(x - 3, y - 2), (x + 3, y + 2)], SILVER_D, shape="ellipse",
               round_=0.2)
    c.part([(31, 75), (41, 85)], SILVER_D, shape="ellipse", gloss=0.5,
           round_=0.3)                                   # the cipher
    c.line((33, 78), (39, 82), (230, 230, 236), 0.6)


def dfc(c):
    ribbon(c, [PURPLE, WHITE], diagonal=True)
    cross_pattee(c, 36, 80, 22, SILVER, arms_w=0.32)
    for side in (-1, 1):                                 # the wings
        c.part([(36, 74), (36 + side * 24, 66), (36 + side * 26, 72),
                (36 + side * 8, 80)], (220, 222, 228), gloss=0.6, round_=0.6)
    c.part([(33, 76), (39, 84)], (180, 182, 188), shape="ellipse", gloss=0.6,
           round_=0.3)
    c.line((36, 68), (36, 92), (130, 132, 140), 0.8)     # the propeller


def dso(c):
    ribbon(c, [BLUE, RED, RED, RED, BLUE])
    cross_pattee(c, 36, 80, 21, GOLD, arms_w=0.5)
    cross_pattee(c, 36, 80, 17, ENAMEL, arms_w=0.44)
    c.part([(29, 73), (43, 87)], (40, 110, 50), shape="ellipse", gloss=0.4,
           round_=0.4)                                   # the laurel
    c.part([(32, 76), (40, 84)], RED, shape="ellipse", gloss=0.5, round_=0.3)
    c.part([(34, 76), (38, 79)], GOLD, round_=0.2)       # the crown


def crest():
    """A crown over two sprays of laurel, printed in black."""
    c = Canvas(72, 112, light=(-0.5, -0.9))
    ink = (40, 34, 30)
    for side in (-1, 1):
        for k in range(8):
            t = k / 7
            a = math.pi * (0.55 + 0.38 * t)
            x = 36 + side * math.cos(a) * -22
            y = 44 + math.sin(a) * 20
            ang = side * (a + math.pi / 2)
            leaf_pts = rot([(x - 4.5, y), (x, y - 2), (x + 4.5, y), (x, y + 2)],
                           ang, (x, y))
            c.part(leaf_pts, ink, gloss=0.05, grain=2, rim=0.2, round_=0.6)
        c.line((36 + side * 3, 64), (36 + side * 20, 46), ink, 0.8)  # the stems
    c.part([(25, 30), (47, 30), (47, 35), (25, 35)], ink, round_=0.6)
    for x in (25, 30.5, 36, 41.5, 47):
        c.part([(x - 2.4, 30), (x + 2.4, 30), (x, 21)], ink, round_=0.4)
        c.part([(x - 1.4, 18.5), (x + 1.4, 21.3)], ink, shape="ellipse",
               round_=0.2)
    c.line((36, 11), (36, 18), ink, 1.3)
    c.line((33, 14), (39, 14), ink, 1.3)
    return c.image()


def main():
    cells = []
    for paint in (leaf, mc, dfc, dso, lambda c: mc(c, bar=True)):
        c = C()
        paint(c)
        im = c.image()
        if paint is dfc:                     # trim the stripes to the ribbon
            a = np.asarray(im).copy()
            a[:52, :13] = KEY
            a[:52, 59:] = KEY
            im = Image.fromarray(a)
        cells.append(im)
    cells.append(crest())
    sheet = Image.new("RGB", (72 * len(cells), 112), KEY)
    for i, im in enumerate(cells):
        sheet.paste(im, (i * 72, 0))
    sheet.save(OUT)


if __name__ == "__main__":
    main()
