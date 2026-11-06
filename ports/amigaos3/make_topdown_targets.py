#!/usr/bin/env python3
"""Paint the top-down mission targets, whole and wrecked, into one atlas.

Everything is seen from directly above with the sun low in the upper left,
in the muted palette of the painted battlefield.  Each sprite is painted at
four times size with the shared painter (paint.py) and filtered down; the
game draws its shadow from the same silhouette, offset to the lower right.

Writes assets/canvas-skies/topdown-targets.bmp and
src/modern/canvas_topdown_sprites.h (the rectangle of each sprite).
"""

import math
import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFont

sys.path.insert(0, str(Path(__file__).resolve().parent))
from paint import Canvas, KEY, rot  # noqa: E402
import trenches  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
ATLAS = ROOT / "assets" / "canvas-skies" / "topdown-targets.bmp"
HEADER = ROOT / "src" / "modern" / "canvas_topdown_sprites.h"
STENCIL = ROOT / "assets" / "fonts" / "stardosstencil" / "StardosStencil-Bold.ttf"
SUN = (-0.6, -0.8)
RNG = np.random.default_rng(1917)

# Palette, matched to the painted top-down ground.
MUD = (92, 78, 56)
EARTH = (104, 86, 60)
CHAR = (38, 32, 28)
ASH = (72, 66, 60)
TYRE = (34, 32, 30)
KHAKI_TILT = (112, 108, 74)
GERMAN_GREY = (88, 92, 82)
CAB = (70, 76, 58)
CREAM = (206, 198, 172)
IRON = (46, 46, 44)
RUST = (112, 62, 38)
TILE = (150, 74, 50)
SLATE = (96, 98, 102)
STONE = (148, 140, 122)
TIMBER = (106, 76, 50)
CANVAS = (178, 162, 124)
SANDBAG_OURS = (176, 158, 112)
SANDBAG_THEIRS = (110, 108, 96)
GRASS = (84, 92, 54)
HORSE = (96, 64, 40)


def C(w, h):
    return Canvas(w, h, light=SUN)


def lit(colour, d):
    """The colour lightened (or, for d < 0, darkened) by d."""
    return tuple(max(0, min(255, v + d)) for v in colour)


def ribs(c, x0, y0, x1, y1, n, colour, vertical=False, width=0.6):
    for k in range(1, n):
        t = k / n
        if vertical:
            x = x0 + (x1 - x0) * t
            c.line((x, y0), (x, y1), colour, width)
        else:
            y = y0 + (y1 - y0) * t
            c.line((x0, y), (x1, y), colour, width)


def debris(c, w, h, n, colours, size=2.5):
    for _ in range(n):
        x, y = RNG.uniform(2, w - 2), RNG.uniform(2, h - 2)
        s = RNG.uniform(0.6, 1.0) * size
        c.part([(x - s, y - s * 0.6), (x + s, y - s * 0.4), (x + s * 0.6, y + s),
                (x - s * 0.8, y + s * 0.7)], colours[RNG.integers(len(colours))],
               gloss=0.05, grain=6, round_=0.3)


def scorch(c, cx, cy, r):
    c.part([(cx - r, cy - r), (cx + r, cy + r)], CHAR, shape="ellipse",
           gloss=0.0, grain=10, rim=0.2, round_=r * 0.25)


def crater(c, cx, cy, r):
    c.part([(cx - r * 1.25, cy - r * 1.25), (cx + r * 1.25, cy + r * 1.25)],
           EARTH, shape="ellipse", gloss=0.12, grain=12, round_=r * 0.3)
    c.part([(cx - r, cy - r), (cx + r, cy + r)], MUD, shape="ellipse",
           gloss=0.0, grain=10, rim=0.7, round_=r * 0.2)
    c.part([(cx - r * 0.5, cy - r * 0.45), (cx + r * 0.5, cy + r * 0.55)],
           (60, 56, 46), shape="ellipse", gloss=0.0, grain=8, round_=1)


# ---- vehicles (front toward the bottom of the screen) -------------------
def lorry(tilt=KHAKI_TILT, wreck=False, label=None):
    c = C(22, 58)
    if wreck:
        scorch(c, 11, 30, 10)
    for y in (12, 22, 46):                       # wheels, twinned at the back
        for x in (2.5, 19.5):
            c.part([(x - 2.4, y - 4), (x + 2.4, y + 4)], TYRE,
                   shape="ellipse", gloss=0.25, round_=0.6)
            c.part([(x - 0.9, y - 1.2), (x + 0.9, y + 1.2)], (70, 66, 60),
                   shape="ellipse", gloss=0.4, round_=0.2)
    body = CHAR if wreck else tilt
    if not wreck:
        # The tilt sags between its hoops: alternate bands lit and shaded,
        # the ridge catching the light, ropes down the sides.
        c.part([(4, 2), (18, 2), (18, 40), (4, 40)], body, gloss=0.25,
               grain=5, round_=1.8)
        for k in range(6):
            y0 = 3 + k * 6.2
            c.part([(4.6, y0 + 3.2), (17.4, y0 + 3.2), (17.4, y0 + 6),
                    (4.6, y0 + 6)], lit(tilt, -16), gloss=0.0, grain=3,
                   rim=0.0, round_=1.2)
        c.part([(4.2, 2.4), (11, 2.4), (11, 39.6), (4.2, 39.6)], lit(tilt, 14),
               gloss=0.0, rim=0.0, grain=3, round_=1.2)   # the sunny side
        for y in (8, 20, 32):
            c.line((4, y), (2.6, y + 1.5), (60, 54, 40), 0.4)
            c.line((18, y), (19.4, y + 1.5), (60, 54, 40), 0.4)
        c.part([(4.5, 39), (17.5, 39), (17.5, 41), (4.5, 41)], TIMBER,
               round_=0.3)                                # tailboard
    else:
        c.part([(4, 2), (18, 2), (18, 40), (4, 40)], body, gloss=0.18,
               grain=8, round_=1.8)
        for y in (8, 16, 24, 32):                # bare, burnt hoops
            c.line((4.5, y), (17.5, y), RUST, 0.8)
        debris(c, 22, 58, 9, [ASH, CHAR, RUST])
    # Mudguards over the front wheels, the cab roof, the bonnet with its
    # louvres and the radiator.
    for x in (1.2, 16.8):
        c.part([(x, 42), (x + 4, 42), (x + 4, 50), (x, 50)],
               CHAR if wreck else lit(CAB, -12), gloss=0.3, round_=1.4)
    c.part([(5, 41), (17, 41), (17, 50), (5, 50)], CHAR if wreck else CAB,
           gloss=0.3, grain=3, round_=1.2)
    c.part([(6.5, 50), (15.5, 50), (15.2, 56.5), (6.8, 56.5)],
           (54, 52, 48) if not wreck else CHAR, gloss=0.45, round_=0.8)
    if not wreck:
        for k in range(3):
            c.line((7.4, 51.5 + k * 1.6), (9.6, 51.5 + k * 1.6), (34, 32, 30), 0.35)
            c.line((12.4, 51.5 + k * 1.6), (14.6, 51.5 + k * 1.6), (34, 32, 30), 0.35)
        c.line((11, 50), (11, 56.4), (90, 88, 82), 0.4)
        c.part([(7, 56), (15, 57.5)], (120, 118, 108), round_=0.2)  # radiator
    if label and not wreck:
        img = c.image()
        img = letter_tilt(img, label)
        return img
    return c.image()


def letter_tilt(img, label):
    """AMBULANCE along the cover, read from the side of the road."""
    font = ImageFont.truetype(str(STENCIL), 10)
    w = int(font.getlength(label)) + 2
    t = Image.new("RGBA", (w, 12), (0, 0, 0, 0))
    ImageDraw.Draw(t).text((1, 0), label, font=font, fill=(40, 34, 30, 255))
    t = t.rotate(90, expand=True)
    if t.height > 36:
        t = t.resize((t.width, 36), Image.LANCZOS)
    x = (img.width - t.width) // 2
    img.paste(t, (x, 4), t)
    # A dark band each side frames the lettering.
    d = ImageDraw.Draw(img)
    d.line((x - 2, 4, x - 2, 39), fill=(150, 140, 116))
    d.line((x + t.width + 1, 4, x + t.width + 1, 39), fill=(150, 140, 116))
    return img


def locomotive(wreck=False):
    c = C(26, 76)
    if wreck:
        scorch(c, 13, 40, 14)
        c.part([(4, 4), (22, 4), (22, 62), (4, 62)], CHAR, gloss=0.35,
               round_=2.5)
        debris(c, 26, 76, 14, [ASH, CHAR, RUST, IRON], 3)
        return c.image()
    c.part([(3, 3), (23, 3), (23, 72), (3, 72)], (34, 34, 32), gloss=0.2,
           round_=1.2)                                # running plate
    c.part([(3, 2), (23, 2), (23, 4.5), (3, 4.5)], (140, 40, 30), gloss=0.3,
           round_=0.3)                                # buffer beam
    for x in (5, 21):
        c.part([(x - 1.6, 0), (x + 1.6, 2.6)], (90, 90, 88), shape="ellipse",
               round_=0.2)                            # buffers
    # The boiler: a long cylinder lit along its upper-left flank, the dark
    # smokebox at the front with its chimney, the dome and safety valves.
    c.part([(6, 5), (20, 5), (20, 40), (6, 40)], (40, 44, 40), gloss=0.7,
           grain=2, round_=6)
    c.part([(7, 6), (11, 6), (11, 39), (7, 39)], (84, 92, 84), gloss=0.0,
           rim=0.0, round_=1.4)                       # the sheen on the boiler
    for y in (16, 26, 36):
        c.line((6.4, y), (19.6, y), (120, 104, 60), 0.5)  # brass bands
    c.part([(6, 5), (20, 5), (20, 12), (6, 12)], (24, 24, 24), gloss=0.4,
           round_=3)                                  # smokebox
    c.part([(10, 6.5), (16, 12.5)], (14, 14, 14), shape="ellipse", gloss=0.5,
           round_=0.6)                                # chimney
    c.part([(10.4, 19), (15.6, 25)], (150, 120, 56), shape="ellipse",
           gloss=0.8, round_=0.5)                     # brass dome
    c.part([(11.6, 31), (14.4, 34)], (150, 120, 56), shape="ellipse",
           gloss=0.8, round_=0.3)                     # safety valves
    # The cab roof with its vent, and the tender heaped with coal.
    c.part([(4, 40), (22, 40), (22, 51), (4, 51)], (46, 48, 44), gloss=0.35,
           round_=1.2)
    c.part([(10, 43), (16, 47)], (34, 34, 32), round_=0.4)
    c.part([(4, 53), (22, 53), (22, 73), (4, 73)], (40, 40, 38), gloss=0.3,
           round_=1.4)
    c.part([(6, 55), (20, 71)], (22, 22, 22), shape="ellipse", gloss=0.1,
           grain=12, round_=2)                        # the coal
    rng = np.random.default_rng(3)
    for _ in range(18):
        x, y = rng.uniform(7, 19), rng.uniform(56, 70)
        c.part([(x - 0.8, y - 0.8), (x + 0.8, y + 0.8)], (70, 70, 72),
               shape="ellipse", gloss=0.6, round_=0.1)
    return c.image()


def wagon(colour=(96, 70, 46), wreck=False):
    c = C(24, 58)
    if wreck:
        scorch(c, 12, 29, 12)
        c.part([(3, 3), (21, 3), (21, 55), (3, 55)], CHAR, gloss=0.18,
               round_=1.4)
        c.line((3.5, 4), (20.5, 54), RUST, 1.2)
        c.line((20.5, 4), (3.5, 54), RUST, 1.2)
        debris(c, 24, 58, 10, [ASH, CHAR, RUST])
        return c.image()
    for y in (1, 57):                              # couplings
        c.line((12, y - 1), (12, y + 1), IRON, 1.0)
    c.part([(3, 3), (21, 3), (21, 55), (3, 55)], colour, gloss=0.25, grain=6,
           round_=1.0)
    c.part([(3.4, 3.4), (8, 3.4), (8, 54.6), (3.4, 54.6)], lit(colour, 16),
           gloss=0.0, rim=0.0, grain=3, round_=0.6)   # the sunny side
    # A load under a roped tarpaulin, or open and full of crates.
    if sum(colour) > 260:
        c.part([(5, 6), (19, 6), (19, 52), (5, 52)], (70, 76, 56), gloss=0.35,
               grain=4, round_=2.5)
        for y in (14, 24, 34, 44):
            c.line((5, y), (19, y + 0.5), (44, 46, 36), 0.5)
    else:
        for i in range(2):
            for j in range(5):
                x, y = 5.5 + i * 7, 6 + j * 9.4
                c.part([(x, y), (x + 6, y), (x + 6, y + 8.2), (x, y + 8.2)],
                       (150, 116, 72), gloss=0.3, round_=0.5)
                c.line((x + 0.5, y + 4.1), (x + 5.5, y + 4.1), (110, 82, 50), 0.4)
    for x, y in ((3, 3), (21, 3), (3, 55), (21, 55)):   # iron corner plates
        c.part([(x - 1.1, y - 1.1), (x + 1.1, y + 1.1)], IRON, round_=0.2)
    return c.image()


def derailed():
    """The locomotive on its side across the bank, burning."""
    c = C(70, 60)
    crater(c, 22, 18, 12)
    pts = rot([(10, 26), (62, 26), (62, 42), (10, 42)], -0.5, (36, 34))
    c.part(pts, IRON, gloss=0.3, round_=2.5)
    pts = rot([(14, 30), (58, 30), (58, 38), (14, 38)], -0.5, (36, 34))
    c.part(pts, (60, 58, 54), gloss=0.5, round_=3)
    for k in range(4):
        x = 18 + k * 11
        p = rot([(x, 42)], -0.5, (36, 34))[0]
        c.part([(p[0] - 3, p[1] - 3), (p[0] + 3, p[1] + 3)], RUST,
               shape="ellipse", round_=0.5)
    debris(c, 70, 60, 18, [ASH, CHAR, RUST, IRON], 3)
    return c.image()


# ---- buildings -------------------------------------------------------------
def roof(c, x0, y0, x1, y1, colour, ridge_vertical=False, tiles=True):
    """A pitched roof: the face toward the sun lit, the other shaded, the
    courses of tiles or slates across each face and the ridge capped."""
    c.part([(x0, y0), (x1, y0), (x1, y1), (x0, y1)], lit(colour, -42),
           gloss=0.1, grain=6, round_=0.8)
    if ridge_vertical:
        xm = (x0 + x1) / 2
        c.part([(x0, y0), (xm, y0), (xm, y1), (x0, y1)], lit(colour, 40),
               gloss=0.05, grain=5, rim=0.0, round_=0.3)
        if tiles:
            n = int((y1 - y0) / 2.2)
            ribs(c, x0 + 0.5, y0, xm, y1, n, lit(colour, 18), width=0.4)
            ribs(c, xm, y0, x1 - 0.5, y1, n, lit(colour, -62), width=0.4)
        c.part([(xm - 0.9, y0), (xm + 0.9, y0), (xm + 0.9, y1), (xm - 0.9, y1)],
               lit(colour, -46), gloss=0.5, round_=0.2)   # ridge tiles
    else:
        ym = (y0 + y1) / 2
        c.part([(x0, y0), (x1, y0), (x1, ym), (x0, ym)], lit(colour, 40),
               gloss=0.05, grain=5, rim=0.0, round_=0.3)
        if tiles:
            n = int((x1 - x0) / 2.2)
            ribs(c, x0, y0 + 0.5, x1, ym, n, lit(colour, 18), vertical=True,
                 width=0.4)
            ribs(c, x0, ym, x1, y1 - 0.5, n, lit(colour, -62), vertical=True,
                 width=0.4)
        c.part([(x0, ym - 0.9), (x1, ym - 0.9), (x1, ym + 0.9), (x0, ym + 0.9)],
               lit(colour, -46), gloss=0.5, round_=0.2)
    # Weathering: moss and soot in patches.
    for _ in range(int((x1 - x0) * (y1 - y0) / 300)):
        x, y = RNG.uniform(x0 + 2, x1 - 2), RNG.uniform(y0 + 2, y1 - 2)
        moss = tuple((a * 3 + b) // 4 for a, b in zip(colour, (80, 96, 56)))
        c.part([(x - 1.4, y - 0.8), (x + 1.4, y + 0.8)], moss,
               shape="ellipse", gloss=0.0, rim=0.0, grain=3, round_=0.6)


def chimney(c, x, y, w=4, h=4):
    c.part([(x + 1, y + 1), (x + w + 1, y + 1), (x + w + 5, y + h + 4),
            (x + 5, y + h + 4)], (24, 22, 20), gloss=0, rim=0,
           round_=0.4)                                # its shadow on the roof
    c.part([(x, y), (x + w, y), (x + w, y + h), (x, y + h)], (132, 76, 56),
           gloss=0.35, round_=0.4)
    c.part([(x + w / 2 - 0.9, y + h / 2 - 0.9), (x + w / 2 + 0.9, y + h / 2 + 0.9)],
           (24, 20, 18), shape="ellipse", round_=0.2)   # the pot


def ruin(c, x0, y0, x1, y1, wall=STONE):
    """Walls standing open to the sky, the roof fallen in and burnt."""
    c.part([(x0, y0), (x1, y0), (x1, y1), (x0, y1)], CHAR, gloss=0.0,
           grain=12, round_=0.8)
    for (a, b) in (((x0, y0), (x1, y0)), ((x1, y0), (x1, y1)),
                   ((x0, y1), (x1 * 0.6 + x0 * 0.4, y1)), ((x0, y0), (x0, y1))):
        c.line(a, b, wall, 2.0)
    debris(c, int(x1) + 2, int(y1) + 2, 16, [ASH, wall, TILE, CHAR], 2.6)


def station(wreck=False):
    c = C(40, 120)
    c.part([(1, 2), (14, 2), (14, 118), (1, 118)], (138, 128, 108), gloss=0.1,
           grain=6, round_=0.6)                       # platform
    for y in range(6, 118, 6):
        c.line((1.5, y), (13.5, y), (120, 110, 92), 0.35)   # flags
    c.line((14, 2), (14, 118), (206, 196, 168), 0.9)  # its white edge
    if wreck:
        ruin(c, 16, 20, 38, 88, (150, 120, 100))
        crater(c, 26, 102, 9)
        return c.image()
    for y in (12, 50, 96):                            # lamps
        c.part([(4, y), (7, y + 3)], (40, 40, 38), shape="ellipse", round_=0.2)
    # The platform canopy with its saw-tooth valance, then the building.
    c.part([(6, 26), (17, 26), (17, 84), (6, 84)], (110, 112, 108), gloss=0.3,
           round_=0.4)
    for y in range(27, 84, 2):
        c.line((6, y), (5, y + 1), (90, 92, 88), 0.5)
    roof(c, 16, 20, 38, 88, SLATE, ridge_vertical=True)
    for y in (28, 56, 80):
        chimney(c, 28.5, y, 4.5, 3.5)
    c.part([(18, 92), (36, 92), (36, 112), (18, 112)], (100, 98, 92),
           gloss=0.3, round_=0.6)                     # goods shed, corrugated
    ribs(c, 18, 93, 36, 111, 9, (80, 78, 74), vertical=True, width=0.5)
    c.line((24, 96), (24, 110), (120, 70, 40), 0.6)   # a rust run
    return c.image()


def hangar(wreck=False):
    """Two Bessonneau canvas hangars side by side.  Each roof is a barrel
    running front to back: light along the crown, falling away to dark at
    both eaves, with the frames as darker seams across it; guy ropes run
    out to pegs well clear of the canvas."""
    c = C(108, 66)
    for x0 in (8, 58):
        if wreck:
            c.part([(x0, 6), (x0 + 42, 6), (x0 + 42, 56), (x0, 56)], CHAR,
                   gloss=0.0, grain=12, round_=1.5)
            for k in range(1, 6):
                y = 6 + k * 8.3
                c.line((x0 + 1, y), (x0 + 41, y), (90, 60, 40), 1.1)
            debris(c, 108, 66, 6, [ASH, CANVAS, CHAR])
            continue
        for k in range(6):                            # guy ropes and pegs
            y = 9 + k * 9
            for sx, ex in ((x0 + 1, x0 - 6), (x0 + 41, x0 + 48)):
                c.line((sx, y), (ex, y + 2), (178, 160, 118), 0.9)
                c.part([(ex - 1, y + 1), (ex + 1, y + 3)], (40, 32, 22),
                       shape="ellipse", round_=0.2)
        for k in range(14):                           # the barrel
            t0, t1 = k / 14, (k + 1) / 14
            crown = 1 - abs((t0 + t1) - 1)            # 1 on the crown, 0 at eaves
            sun = 0.12 * (1 - (t0 + t1))              # the sun from the left
            col = lit(CANVAS, int(-70 + 110 * crown ** 0.8 + 80 * sun))
            c.part([(x0 + 42 * t0, 6), (x0 + 42 * t1 + 0.4, 6),
                    (x0 + 42 * t1 + 0.4, 56), (x0 + 42 * t0, 56)],
                   col, gloss=0.0, rim=0.0, grain=3, round_=0.2)
        for k in range(1, 6):                         # the frames, as seams
            y = 6 + k * 8.3
            c.line((x0 + 0.5, y), (x0 + 41.5, y), (98, 82, 56), 1.1)
            c.line((x0 + 8, y + 0.9), (x0 + 34, y + 0.9), (226, 214, 180), 0.4)
        c.line((x0, 6), (x0, 56), (70, 60, 44), 1.0)  # the eaves
        c.line((x0 + 42, 6), (x0 + 42, 56), (60, 52, 38), 1.0)
        c.part([(x0 + 4, 56), (x0 + 38, 56), (x0 + 38, 60), (x0 + 4, 60)],
               (70, 62, 48), round_=0.6)              # the doorway curtain
        c.part([(x0 + 16, 56), (x0 + 26, 56), (x0 + 26, 61), (x0 + 16, 61)],
               (34, 30, 24), round_=0.4)              # its opening
    return c.image()


def tents(wreck=False):
    """Bell tents: each cone lit on the sunny side, its door laced, guy
    lines pegged out round it."""
    c = C(56, 56)
    spots = [(10, 10), (30, 8), (46, 16), (14, 32), (34, 30), (24, 48)]
    for x, y in spots:
        if wreck:
            scorch(c, x, y, 7)
            debris(c, 56, 56, 2, [ASH, CANVAS])
            continue
        for a in range(0, 360, 45):
            r = math.radians(a + 20)
            c.line((x + 6.6 * math.cos(r), y + 6.6 * math.sin(r)),
                   (x + 8.8 * math.cos(r), y + 8.8 * math.sin(r)),
                   (120, 110, 90), 0.3)
        c.part([(x - 7, y - 7), (x + 7, y + 7)], (196, 190, 166),
               shape="ellipse", gloss=0.2, grain=4, round_=0.8)
        for k in range(12):                           # the cone's panels
            a0 = math.radians(k * 30)
            a1 = math.radians(k * 30 + 30)
            light = math.cos(a0 + math.radians(15) - math.atan2(-0.8, -0.6))
            col = lit((196, 190, 166), int(26 * light))
            c.part([(x, y), (x + 6.8 * math.cos(a0), y + 6.8 * math.sin(a0)),
                    (x + 6.8 * math.cos(a1), y + 6.8 * math.sin(a1))], col,
                   gloss=0.0, rim=0.0, round_=0.2)
        c.line((x, y), (x + 4.5, y + 5), (140, 130, 104), 0.5)  # the door
        c.part([(x - 1.2, y - 1.2), (x + 1.2, y + 1.2)], TIMBER,
               shape="ellipse", round_=0.2)
    return c.image()


def dump(wreck=False):
    """An ammunition dump: shells stacked in rows and boxes under roped
    tarpaulins, in a ring of sandbags."""
    c = C(72, 72)
    if wreck:
        crater(c, 36, 36, 22)
        debris(c, 72, 72, 22, [ASH, CHAR, TIMBER, SANDBAG_THEIRS], 3)
        return c.image()
    c.part([(4, 4), (68, 68)], SANDBAG_THEIRS, shape="ellipse", gloss=0.25,
           grain=5, round_=2)
    for k in range(40):                               # the bags
        a = math.radians(k * 9)
        for r in (29.5, 25.5):
            x, y = 36 + r * math.cos(a), 36 + r * math.sin(a)
            c.part([(x - 1.9, y - 1.3), (x + 1.9, y + 1.3)],
                   lit(SANDBAG_THEIRS, int(RNG.integers(-10, 14))),
                   shape="ellipse", gloss=0.3, round_=0.3)
    c.part([(10, 10), (62, 62)], MUD, shape="ellipse", gloss=0.0, grain=8,
           round_=2)
    c.part([(30, 62), (42, 62), (42, 70), (30, 70)], MUD, round_=0.5)  # gap
    for i in range(2):
        for j in range(2):
            x, y = 16 + i * 22, 16 + j * 20
            c.part([(x, y), (x + 15, y), (x + 15, y + 14), (x, y + 14)],
                   (84, 92, 64), gloss=0.35, grain=4, round_=2)   # tarpaulin
            c.line((x + 1, y + 7), (x + 14, y + 7), (60, 66, 46), 0.6)
            c.line((x + 7.5, y + 1), (x + 7.5, y + 13), (60, 66, 46), 0.6)
    for row in range(2):                              # shells, noses up
        for k in range(9):
            x, y = 17 + k * 4.4, 52 + row * 4.4
            c.part([(x - 1.8, y - 1.8), (x + 1.8, y + 1.8)], (176, 140, 70),
                   shape="ellipse", gloss=0.8, round_=0.2)
            c.part([(x - 0.7, y - 0.9), (x + 0.5, y + 0.3)], (226, 210, 160),
                   shape="ellipse", round_=0.1)
    return c.image()


def depot(wreck=False):
    """A corrugated-iron store with crates and barrels stacked outside."""
    c = C(64, 42)
    if wreck:
        ruin(c, 2, 2, 62, 38, (110, 104, 92))
        return c.image()
    c.part([(2, 2), (50, 2), (50, 38), (2, 38)], (92, 94, 90), gloss=0.3,
           round_=0.8)
    c.part([(2, 2), (50, 2), (50, 20), (2, 20)], (110, 112, 106), gloss=0.1,
           rim=0.0, round_=0.6)                       # the sunny pitch
    ribs(c, 3, 3, 49, 37, 18, (76, 78, 74), vertical=True, width=0.5)
    c.line((2, 20), (50, 20), (64, 66, 62), 1.0)
    for x in (10, 23, 37):                            # rust runs
        c.line((x, 3), (x + 0.5, 15), (120, 70, 44), 0.7)
    for i in range(2):                                # crates
        for j in range(3):
            x, y = 52.5 + i * 5.5, 4 + j * 6
            c.part([(x, y), (x + 5, y), (x + 5, y + 5), (x, y + 5)],
                   (150, 116, 72), gloss=0.35, round_=0.3)
    for j in range(4):                                # barrels
        x, y = 55 + (j % 2) * 5, 24 + (j // 2) * 6
        c.part([(x - 2.4, y - 2.4), (x + 2.4, y + 2.4)], (72, 70, 64),
               shape="ellipse", gloss=0.6, round_=0.2)
        c.part([(x - 0.8, y - 0.8), (x + 0.8, y + 0.8)], (40, 38, 36),
               shape="ellipse", round_=0.1)
    return c.image()


def farmhouse(wreck=False):
    """A Flemish farm: the tiled house and its lean-to, the yard with its
    midden and well, a haystack by the gable."""
    c = C(48, 42)
    if wreck:
        ruin(c, 4, 4, 44, 30, (160, 146, 120))
        crater(c, 36, 34, 6)
        return c.image()
    c.part([(2, 26), (30, 26), (30, 40), (2, 40)], (110, 98, 74), gloss=0.0,
           grain=8, round_=1.5)                       # the yard
    c.part([(11, 29), (19, 37)], (76, 60, 40), shape="ellipse", gloss=0.2,
           grain=6, round_=1)                         # the midden
    c.part([(21, 30), (26, 35)], STONE, shape="ellipse", gloss=0.3,
           round_=0.4)                                # the well
    c.part([(22.4, 31.4), (24.6, 33.6)], (30, 30, 30), shape="ellipse",
           round_=0.2)
    roof(c, 4, 4, 44, 26, TILE)
    c.part([(30, 26), (44, 26), (44, 38), (30, 38)], lit(TILE, -16),
           gloss=0.2, grain=5, round_=0.6)            # lean-to
    ribs(c, 30, 26, 44, 38, 6, lit(TILE, -36), vertical=True, width=0.4)
    chimney(c, 35, 6, 4, 4)
    c.part([(2.5, 28.5), (9.5, 38.5)], (176, 156, 96), shape="ellipse",
           gloss=0.3, grain=6, round_=1)              # haystack
    return c.image()


def church(wreck=False):
    """The church in its churchyard: nave and transepts under slate, the
    apse round at the east end, the tower's spire a lit and shaded
    pyramid, paths between the graves and a yew."""
    c = C(86, 116)
    c.part([(2, 2), (84, 2), (84, 114), (2, 114)], GRASS, gloss=0.05,
           grain=10, round_=1.5)
    for a, b in (((2, 2), (84, 2)), ((84, 2), (84, 114)), ((2, 114), (84, 114)),
                 ((2, 2), (2, 114))):
        c.line(a, b, STONE, 1.4)
    c.part([(40, 76), (44, 76), (44, 114), (40, 114)], (150, 138, 110),
           gloss=0.0, grain=6, round_=0.5)            # the path to the porch
    for k in range(18):                               # headstones and crosses
        x, y = 8 + (k % 9) * 8.6, 86 + (k // 9) * 13
        if abs(x - 42) < 5:
            continue
        c.part([(x + 1, y + 1), (x + 5, y + 4)], (40, 46, 30), gloss=0, rim=0,
               round_=0.3)
        if k % 3 == 1:
            c.part([(x + 1.4, y - 1), (x + 2.6, y - 1), (x + 2.6, y + 3.5),
                    (x + 1.4, y + 3.5)], (186, 180, 164), round_=0.2)
            c.part([(x, y), (x + 4, y), (x + 4, y + 1.2), (x, y + 1.2)],
                   (186, 180, 164), round_=0.2)
        else:
            c.part([(x, y), (x + 4, y), (x + 4, y + 2.5), (x, y + 2.5)],
                   (178, 172, 156), gloss=0.3, round_=0.3)
    for (x, y, r) in ((70, 66, 9), (12, 66, 7)):      # yews
        c.part([(x - r + 2, y - r + 2), (x + r + 3, y + r + 3)], (34, 40, 28),
               shape="ellipse", gloss=0, rim=0, round_=2)
        c.part([(x - r, y - r), (x + r, y + r)], (44, 62, 40), shape="ellipse",
               gloss=0.2, grain=8, round_=2)
    if wreck:
        ruin(c, 30, 10, 54, 76, STONE)
        ruin(c, 18, 30, 66, 44, STONE)
        crater(c, 20, 66, 8)
        return c.image()
    c.part([(31, 66), (53, 66), (53, 78), (31, 78)], SLATE, gloss=0.3,
           round_=8)                                  # the apse, rounded
    roof(c, 30, 14, 54, 72, SLATE, ridge_vertical=True)   # nave
    roof(c, 16, 32, 68, 44, SLATE)                    # transepts
    c.part([(38, 76), (46, 76), (46, 82), (38, 82)], STONE, gloss=0.3,
           round_=0.5)                                # the porch
    # The tower, and its spire as four lit and shaded faces.
    c.part([(32, 2), (52, 2), (52, 18), (32, 18)], STONE, gloss=0.3, grain=4,
           round_=0.6)
    cx, cy = 42, 10
    faces = (((33, 3), (51, 3), 30), ((51, 3), (51, 17), -28),
             ((51, 17), (33, 17), -42), ((33, 17), (33, 3), 16))
    for a, b, d in faces:
        c.part([a, b, (cx, cy)], lit((96, 98, 104), d), gloss=0.1,
               rim=0.0, round_=0.2)
    c.part([(cx - 1, cy - 1), (cx + 1, cy + 1)], (196, 170, 90),
           shape="ellipse", gloss=0.8, round_=0.2)    # the weathercock
    return c.image()


def cart(wreck=False):
    c = C(22, 46)
    if wreck:
        debris(c, 22, 46, 10, [TIMBER, CHAR, (150, 140, 110)], 2.6)
        return c.image()
    c.part([(3, 4), (19, 4), (19, 26), (3, 26)], TIMBER, gloss=0.2,
           round_=0.8)
    c.part([(5, 6), (17, 24)], (150, 140, 110), shape="ellipse", gloss=0.2,
           round_=2)                                  # the load under a sheet
    c.part([(0.5, 10), (3, 22)], (60, 44, 30), shape="ellipse", round_=0.4)
    c.part([(19, 10), (21.5, 22)], (60, 44, 30), shape="ellipse", round_=0.4)
    c.line((8, 26), (9, 32), TIMBER, 0.8)
    c.line((14, 26), (13, 32), TIMBER, 0.8)
    c.part([(7, 30), (15, 30), (14, 44), (8, 44)], HORSE, gloss=0.3,
           round_=2.5)                                # the horse
    c.part([(9, 40), (13, 46)], HORSE, shape="ellipse", round_=0.6)
    return c.image()


def refugees(wreck=False):
    """People on the road with bundles and a handcart."""
    c = C(40, 60)
    if wreck:
        debris(c, 40, 60, 12, [(150, 140, 110), TIMBER, (90, 80, 70)], 2.2)
        return c.image()
    rng = np.random.default_rng(5)
    coats = [(70, 70, 86), (110, 80, 60), (60, 60, 60), (140, 130, 110),
             (96, 40, 36)]
    for _ in range(11):
        x, y = rng.uniform(6, 34), rng.uniform(6, 54)
        col = coats[rng.integers(len(coats))]
        c.part([(x - 3.4, y - 2.6), (x + 3.4, y + 2.6)], col, shape="ellipse",
               gloss=0.25, grain=4, rim=0.3, round_=0.3)   # shoulders
        c.part([(x - 1.6, y - 1.8), (x + 1.6, y + 1.2)], (204, 172, 140),
               shape="ellipse", gloss=0.3, grain=3, rim=0.2,
               round_=0.1)                                 # head
        if rng.random() < 0.5:                             # a bundle
            c.part([(x + 2.2, y - 0.5), (x + 5.5, y + 3.2)], (170, 150, 112),
                   shape="ellipse", round_=0.2)
    c.part([(16, 28), (26, 28), (26, 40), (16, 40)], TIMBER, gloss=0.2,
           round_=0.6)                                # handcart
    c.part([(17, 29), (25, 39)], (164, 150, 120), shape="ellipse", round_=1)
    return c.image()


def mg_post(wreck=False, ours=False):
    c = C(38, 38)
    bag = SANDBAG_OURS if ours else SANDBAG_THEIRS
    if wreck:
        crater(c, 19, 19, 12)
        debris(c, 38, 38, 10, [bag, CHAR, IRON])
        return c.image()
    c.part([(3, 3), (35, 35)], bag, shape="ellipse", gloss=0.25, round_=1.5)
    for a in range(0, 360, 30):                      # the sandbags
        r = math.radians(a)
        c.line((19 + 11 * math.cos(r), 19 + 11 * math.sin(r)),
               (19 + 16 * math.cos(r), 19 + 16 * math.sin(r)),
               tuple(max(0, v - 30) for v in bag), 0.6)
    c.part([(9, 9), (29, 29)], MUD, shape="ellipse", round_=1)
    c.line((19, 20), (19, 6), IRON, 1.6)              # the gun
    c.part([(16, 18), (22, 24)], IRON, shape="ellipse", round_=0.4)
    c.part([(12, 22), (16, 26)], (90, 92, 84), shape="ellipse", round_=0.3)
    c.part([(22, 22), (26, 26)], (90, 92, 84), shape="ellipse", round_=0.3)
    return c.image()


def dugout(wreck=False):
    c = C(32, 32)
    if wreck:
        crater(c, 16, 16, 11)
        debris(c, 32, 32, 8, [TIMBER, CHAR])
        return c.image()
    c.part([(2, 4), (30, 4), (30, 28), (2, 28)], EARTH, gloss=0.15,
           round_=3)
    c.part([(8, 10), (24, 22)], (52, 44, 36), gloss=0.0, round_=1.5)
    ribs(c, 8, 10, 24, 22, 4, TIMBER, vertical=True, width=0.8)
    return c.image()


def aa_pit():
    """An anti-aircraft gun in its sandbagged pit, the barrel cocked high
    so from above it shows short, foreshortened toward the sky."""
    c = C(44, 44)
    c.part([(2, 2), (42, 42)], SANDBAG_THEIRS, shape="ellipse", gloss=0.25,
           grain=5, round_=1.5)
    for k in range(18):
        a = math.radians(k * 20)
        x, y = 22 + 17.5 * math.cos(a), 22 + 17.5 * math.sin(a)
        c.part([(x - 2.2, y - 1.5), (x + 2.2, y + 1.5)],
               lit(SANDBAG_THEIRS, int(RNG.integers(-14, 16))),
               shape="ellipse", gloss=0.35, round_=0.3)
    c.part([(8, 8), (36, 36)], MUD, shape="ellipse", gloss=0.0, grain=8,
           round_=1)
    for a in (0.6, 2.2, 3.8, 5.3):                    # the outriggers
        c.line((22, 22), (22 + 12 * math.cos(a), 22 + 12 * math.sin(a)),
               IRON, 1.6)
    c.part([(16, 16), (28, 28)], (62, 64, 58), shape="ellipse", gloss=0.6,
           round_=0.6)                                # the mounting
    c.part([(19, 6), (25, 6), (25, 22), (19, 22)], (52, 54, 50), gloss=0.6,
           round_=1.2)                                # the barrel, cocked up
    c.part([(20, 4), (24, 8)], (24, 24, 24), shape="ellipse", round_=0.3)
    for k in range(4):                                # ready shells
        c.part([(30 + k * 2.4, 32), (32 + k * 2.4, 34)], (176, 140, 70),
               shape="ellipse", gloss=0.8, round_=0.1)
    return c.image()


def convoy_lorry():
    """A grey lorry from the convoy, running across the screen."""
    return lorry(GERMAN_GREY).rotate(90, expand=True)


def aerodrome():
    """Their aerodrome: two sheds with a strip of trodden grass before
    them and two machines parked nose to tail."""
    c = C(110, 80)
    c.part([(2, 30), (108, 30), (108, 78), (2, 78)], (98, 108, 62),
           gloss=0.0, grain=8, round_=2)              # the field
    c.part([(4, 36), (106, 36), (106, 44), (4, 44)], (124, 114, 80),
           gloss=0.0, grain=6, round_=1)              # the apron
    for x0 in (8, 60):
        roof(c, x0, 4, x0 + 42, 32, (104, 84, 62))
        c.part([(x0 + 6, 32), (x0 + 36, 32), (x0 + 36, 35), (x0 + 6, 35)],
               (30, 26, 22), round_=0.4)              # the open doors
    for (x, y) in ((28, 60), (82, 64)):               # parked Albatros-types
        c.part([(x - 20, y - 4), (x + 20, y - 4), (x + 20, y + 3),
                (x - 20, y + 3)], (150, 136, 100), gloss=0.3, round_=1)
        c.part([(x - 2.5, y - 10), (x + 2.5, y - 10), (x + 1.6, y + 14),
                (x - 1.6, y + 14)], (140, 40, 34), gloss=0.4, round_=1)
        c.part([(x - 8, y + 11), (x + 8, y + 11), (x + 8, y + 15),
                (x - 8, y + 15)], (150, 136, 100), gloss=0.3, round_=0.6)
        for side in (-1, 1):
            cross(c, x + side * 15, y - 0.5, 3)
    return c.image()


def cross(c, x, y, r):
    c.part([(x - r, y - r), (x + r, y + r)], (230, 226, 214), round_=0.1)
    w = r * 0.42
    c.part([(x - w, y - r), (x + w, y - r), (x + w, y + r), (x - w, y + r)],
           (24, 22, 22), round_=0.05)
    c.part([(x - r, y - w), (x + r, y - w), (x + r, y + w), (x - r, y + w)],
           (24, 22, 22), round_=0.05)


def man(coat, cap):
    """A soldier seen from above: shoulders in his greatcoat, the cap or
    helmet over his head, an arm out."""
    c = C(12, 12)
    c.part([(1.5, 3.5), (10.5, 8.5)], coat, shape="ellipse", gloss=0.35,
           grain=3, round_=0.3)
    c.part([(8.5, 6), (11.5, 9.5)], coat, shape="ellipse", round_=0.2)
    c.part([(3.5, 3), (8.5, 8)], cap, shape="ellipse", gloss=0.5, round_=0.2)
    return c.image()


def football():
    c = C(6, 6)
    c.part([(1, 1), (5, 5)], (150, 110, 70), shape="ellipse", gloss=0.6,
           round_=0.1)
    return c.image()


# ---- ground tiles (each repeats vertically) --------------------------------
def seamless(img):
    """The painter lights every part from the low sun, so a tile comes out
    a shade lighter at its top than at its foot, and a strip of them shows
    a seam at every join.  Even each row out to the tile's own mean."""
    a = np.asarray(img).astype(np.float32)
    key = np.all(a == np.array(KEY, np.float32), axis=2)
    out = a.copy()
    whole = a[~key].mean(axis=0)
    for y in range(a.shape[0]):
        row = ~key[y]
        if row.sum() < 2:
            continue
        out[y, row] = a[y, row] * (whole / np.maximum(a[y, row].mean(axis=0), 1.0))
    out = np.clip(out, 0, 255).astype(np.uint8)
    out[key] = KEY
    # Never let a corrected pixel fall on the key itself.
    hit = np.all(out == np.array(KEY, np.uint8), axis=2) & ~key
    out[hit] = (254, 1, 250)
    return Image.fromarray(out)


def tile_road():
    c = C(44, 64)
    c.part([(2, -2), (42, -2), (42, 66), (2, 66)], (122, 106, 78), gloss=0.05,
           grain=12, rim=0.0, round_=0)
    for x in (12, 32):                               # ruts
        c.line((x, 0), (x, 64), (96, 82, 60), 2.2)
    c.line((2, 0), (2, 64), (84, 74, 54), 1.0)
    c.line((42, 0), (42, 64), (84, 74, 54), 1.0)
    return seamless(c.image())


def tile_rail():
    c = C(34, 64)
    c.part([(1, -2), (33, -2), (33, 66), (1, 66)], (98, 92, 82), gloss=0.0,
           grain=14, rim=0.0, round_=0)              # ballast
    for y in range(2, 64, 8):                        # sleepers
        c.part([(4, y), (30, y), (30, y + 3), (4, y + 3)], (80, 60, 42),
               gloss=0.1, round_=0.2)
    for x in (10, 24):                               # rails
        c.line((x, 0), (x, 64), (150, 148, 140), 1.3)
        c.line((x + 0.7, 0), (x + 0.7, 64), (70, 66, 60), 0.6)
    return seamless(c.image())


def tile_trench(ours=False, step=0):
    """A stretch of fire trench running up the screen, lit from the air
    (trenches.py); step 0 and 1 alternate, the second with a
    communication trench and heavier shelling."""
    return trenches.tile(ours, step, (31 if ours else 17) + step)


SPRITES = [
    ("TD_LORRY", lorry()), ("TD_LORRY_WRECK", lorry(wreck=True)),
    ("TD_LORRY_GREY", lorry(GERMAN_GREY)),
    ("TD_AMBULANCE", lorry(CREAM, label="AMBULANCE")),
    ("TD_AMBULANCE_WRECK", lorry(CREAM, wreck=True)),
    ("TD_LOCO", locomotive()), ("TD_LOCO_WRECK", derailed()),
    ("TD_WAGON", wagon()), ("TD_WAGON_GREY", wagon((92, 92, 86))),
    ("TD_WAGON_WRECK", wagon(wreck=True)),
    ("TD_STATION", station()), ("TD_STATION_WRECK", station(True)),
    ("TD_HANGAR", hangar()), ("TD_HANGAR_WRECK", hangar(True)),
    ("TD_TENTS", tents()), ("TD_TENTS_WRECK", tents(True)),
    ("TD_DUMP", dump()), ("TD_DUMP_WRECK", dump(True)),
    ("TD_DEPOT", depot()), ("TD_DEPOT_WRECK", depot(True)),
    ("TD_FARM", farmhouse()), ("TD_FARM_WRECK", farmhouse(True)),
    ("TD_CHURCH", church()), ("TD_CHURCH_WRECK", church(True)),
    ("TD_CART", cart()), ("TD_CART_WRECK", cart(True)),
    ("TD_REFUGEES", refugees()), ("TD_REFUGEES_WRECK", refugees(True)),
    ("TD_MG", mg_post()), ("TD_MG_WRECK", mg_post(True)),
    ("TD_MG_OURS", mg_post(ours=True)),
    ("TD_DUGOUT", dugout()), ("TD_DUGOUT_WRECK", dugout(True)),
    ("TD_AA_PIT", aa_pit()), ("TD_CONVOY", convoy_lorry()),
    ("TD_AERODROME", aerodrome()),
    ("TD_MAN_OURS", man((118, 104, 66), (96, 86, 54))),
    ("TD_MAN_THEIRS", man((100, 106, 98), (72, 76, 70))),
    ("TD_FOOTBALL", football()),
    ("TD_TILE_ROAD", tile_road()), ("TD_TILE_RAIL", tile_rail()),
    ("TD_TILE_TRENCH_A", tile_trench(False, 0)),
    ("TD_TILE_TRENCH_B", tile_trench(False, 1)),
    ("TD_TILE_OURS_A", tile_trench(True, 0)),
    ("TD_TILE_OURS_B", tile_trench(True, 1)),
]


def pop(img):
    """Push a target off the mud at 1x: a little more contrast, and its
    silhouette edged in a dark line."""
    a = np.asarray(img).astype(np.float32)
    key = np.all(a == KEY, axis=2)
    if key.all():
        return img
    mean = a[~key].mean(axis=0)
    a = np.where(key[..., None], a, np.clip((a - mean) * 1.25 + mean, 0, 255))
    solid = ~key
    inner = solid.copy()
    inner[1:, :] &= solid[:-1, :]
    inner[:-1, :] &= solid[1:, :]
    inner[:, 1:] &= solid[:, :-1]
    inner[:, :-1] &= solid[:, 1:]
    edge = solid & ~inner
    a[edge] *= 0.5
    out = a.astype(np.uint8)
    out[key] = KEY
    return Image.fromarray(out)


def main():
    global SPRITES
    SPRITES = [(n, img if n.startswith("TD_TILE") else pop(img))
               for n, img in SPRITES]
    # Shelf packing into a 512-wide atlas.
    width, x, y, shelf = 512, 0, 0, 0
    places = []
    for name, img in SPRITES:
        if x + img.width > width:
            x, y, shelf = 0, y + shelf + 1, 0
        places.append((name, x, y, img.width, img.height))
        x += img.width + 1
        shelf = max(shelf, img.height)
    atlas = Image.new("RGB", (width, y + shelf), KEY)
    for (name, x, y, w, h), (_, img) in zip(places, SPRITES):
        atlas.paste(img, (x, y))
    atlas.save(ATLAS)
    out = ["/* Generated by ports/amigaos3/make_topdown_targets.py: do not edit. */",
           "#ifndef CANVAS_TOPDOWN_SPRITES_H", "#define CANVAS_TOPDOWN_SPRITES_H",
           "", "typedef enum {"]
    out += [f"    {name}," for name, *_ in places]
    out += ["    TD_SPRITE_COUNT", "} TopdownSprite;", "",
            "static const short topdown_sprite_rect[TD_SPRITE_COUNT][4] = {"]
    out += [f"    {{{x}, {y}, {w}, {h}}}, /* {name} */" for name, x, y, w, h in places]
    out += ["};", "", "#endif", ""]
    HEADER.write_text("\n".join(out))


if __name__ == "__main__":
    main()
