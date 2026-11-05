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
    for y in (12, 22, 46):                       # wheels
        for x in (2.5, 19.5):
            c.part([(x - 2.4, y - 4), (x + 2.4, y + 4)], TYRE,
                   shape="ellipse", gloss=0.1, round_=0.6)
    body = CHAR if wreck else tilt
    c.part([(4, 2), (18, 2), (18, 40), (4, 40)], body, gloss=0.18, round_=1.8)
    if not wreck:
        ribs(c, 4.5, 4, 17.5, 39, 6, tuple(max(0, v - 22) for v in tilt))
        c.line((11, 3), (11, 39), tuple(min(255, v + 18) for v in tilt), 0.6)
    else:
        for y in (8, 16, 24, 32):                # bare, burnt hoops
            c.line((4.5, y), (17.5, y), RUST, 0.8)
        debris(c, 22, 58, 9, [ASH, CHAR, RUST])
    c.part([(5, 41), (17, 41), (17, 52), (5, 52)], CHAR if wreck else CAB,
           gloss=0.2, round_=1.2)                # cab roof
    c.part([(6, 52), (16, 52), (15.5, 57), (6.5, 57)],
           (54, 52, 48) if not wreck else CHAR, gloss=0.25, round_=0.8)
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
    c.part([(4, 4), (22, 4), (22, 62), (4, 62)], CHAR if wreck else IRON,
           gloss=0.35, round_=2.5)                  # boiler and tender
    if not wreck:
        c.part([(7, 6), (19, 6), (19, 36), (7, 36)], (58, 58, 56), gloss=0.5,
               round_=4)                            # boiler top
        c.part([(9.5, 30), (16.5, 30), (16.5, 36), (9.5, 36)], (24, 22, 22),
               shape="ellipse", round_=0.6)         # chimney
        c.part([(10, 18), (16, 24)], (120, 96, 50), shape="ellipse",
               gloss=0.6, round_=0.5)               # brass dome
        c.part([(5, 40), (21, 40), (21, 50), (5, 50)], (40, 40, 38),
               gloss=0.2, round_=1)                 # cab roof
        c.part([(5, 52), (21, 52), (21, 72), (5, 72)], (52, 46, 40),
               gloss=0.15, round_=1.5)              # tender, coal
        debris(c, 26, 76, 0, [IRON])
    else:
        debris(c, 26, 76, 14, [ASH, CHAR, RUST, IRON], 3)
    return c.image()


def wagon(colour=(96, 70, 46), wreck=False):
    c = C(24, 58)
    if wreck:
        scorch(c, 12, 29, 12)
    c.part([(3, 3), (21, 3), (21, 55), (3, 55)], CHAR if wreck else colour,
           gloss=0.18, round_=1.4)
    if not wreck:
        ribs(c, 3.5, 4, 20.5, 54, 8, tuple(max(0, v - 20) for v in colour),
             width=0.5)
        c.line((12, 4), (12, 54), tuple(min(255, v + 22) for v in colour), 0.8)
    else:
        c.line((3.5, 4), (20.5, 54), RUST, 1.2)
        c.line((20.5, 4), (3.5, 54), RUST, 1.2)
        debris(c, 24, 58, 10, [ASH, CHAR, RUST])
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
    c.part([(x0, y0), (x1, y0), (x1, y1), (x0, y1)], colour, gloss=0.22,
           round_=1.0)
    if ridge_vertical:
        xm = (x0 + x1) / 2
        c.part([(x0, y0), (xm, y0), (xm, y1), (x0, y1)],
               tuple(min(255, v + 18) for v in colour), gloss=0.1, round_=0.6)
        c.line((xm, y0), (xm, y1), tuple(max(0, v - 40) for v in colour), 1.0)
        if tiles:
            ribs(c, x0 + 1, y0, x1 - 1, y1, int((y1 - y0) / 2.6),
                 tuple(max(0, v - 24) for v in colour), width=0.45)
    else:
        ym = (y0 + y1) / 2
        c.part([(x0, y0), (x1, y0), (x1, ym), (x0, ym)],
               tuple(min(255, v + 18) for v in colour), gloss=0.1, round_=0.6)
        c.line((x0, ym), (x1, ym), tuple(max(0, v - 40) for v in colour), 1.0)
        if tiles:
            ribs(c, x0, y0 + 1, x1, y1 - 1, int((x1 - x0) / 2.6),
                 tuple(max(0, v - 24) for v in colour), vertical=True,
                 width=0.45)


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
           round_=0.6)                               # platform
    c.line((14, 2), (14, 118), (196, 186, 160), 0.8)  # its edge
    if wreck:
        ruin(c, 16, 20, 38, 88, (150, 120, 100))
        crater(c, 26, 102, 9)
    else:
        roof(c, 16, 20, 38, 88, SLATE, ridge_vertical=True)
        c.part([(18, 92), (36, 92), (36, 112), (18, 112)], (96, 94, 88),
               gloss=0.15, round_=0.8)               # goods shed
        ribs(c, 18, 93, 36, 111, 5, (78, 76, 72))
        for y in (30, 58, 78):                       # chimneys
            c.part([(29, y), (34, y + 4)], (120, 72, 54), round_=0.5)
    return c.image()


def hangar(wreck=False):
    """Two Bessonneau canvas hangars side by side."""
    c = C(96, 62)
    for x0 in (2, 50):
        if wreck:
            c.part([(x0, 6), (x0 + 44, 6), (x0 + 44, 56), (x0, 56)], CHAR,
                   gloss=0.0, grain=12, round_=1.5)
            for k in range(1, 6):                    # bare burnt frames
                x = x0 + k * 7.3
                c.line((x, 7), (x, 55), (90, 60, 40), 1.1)
            debris(c, 96, 62, 6, [ASH, CANVAS, CHAR])
        else:
            c.part([(x0, 6), (x0 + 44, 6), (x0 + 44, 56), (x0, 56)], CANVAS,
                   gloss=0.2, round_=2.5)
            c.part([(x0, 6), (x0 + 22, 6), (x0 + 22, 56), (x0, 56)],
                   (194, 180, 142), gloss=0.12, round_=2)
            for k in range(1, 6):                    # the canvas folds
                x = x0 + k * 7.3
                c.line((x, 7), (x, 55), (150, 134, 100), 0.9)
            c.line((x0 + 22, 6), (x0 + 22, 56), (126, 112, 84), 1.2)
            c.part([(x0 + 4, 56), (x0 + 40, 56), (x0 + 40, 60), (x0 + 4, 60)],
                   (70, 62, 48), round_=0.6)          # the doorway curtain
    return c.image()


def tents(wreck=False):
    c = C(56, 56)
    spots = [(10, 10), (30, 8), (46, 16), (14, 32), (34, 30), (24, 48)]
    for x, y in spots:
        if wreck:
            scorch(c, x, y, 7)
            debris(c, 56, 56, 2, [ASH, CANVAS])
        else:
            c.part([(x - 7, y - 7), (x + 7, y + 7)], (204, 198, 176),
                   shape="ellipse", gloss=0.25, round_=0.8)
            for a in range(0, 360, 60):
                r = math.radians(a)
                c.line((x, y), (x + 6.5 * math.cos(r), y + 6.5 * math.sin(r)),
                       (168, 160, 138), 0.5)
            c.part([(x - 1.2, y - 1.2), (x + 1.2, y + 1.2)], TIMBER,
                   shape="ellipse", round_=0.2)
    return c.image()


def dump(wreck=False):
    """An ammunition dump: shell boxes under tarpaulins in a sandbag ring."""
    c = C(72, 72)
    if wreck:
        crater(c, 36, 36, 22)
        debris(c, 72, 72, 22, [ASH, CHAR, TIMBER, SANDBAG_THEIRS], 3)
        return c.image()
    c.part([(4, 4), (68, 68)], SANDBAG_THEIRS, shape="ellipse", gloss=0.2,
           round_=2)
    c.part([(9, 9), (63, 63)], MUD, shape="ellipse", gloss=0.0, round_=2)
    for i in range(3):
        for j in range(2):
            x, y = 16 + i * 14, 20 + j * 18
            c.part([(x, y), (x + 11, y), (x + 11, y + 13), (x, y + 13)],
                   (84, 92, 64), gloss=0.2, round_=1.6)   # tarpaulin
            c.line((x + 1, y + 6.5), (x + 10, y + 6.5), (64, 70, 48), 0.6)
    for k in range(5):
        x = 20 + k * 7
        c.part([(x, 54), (x + 5, 54), (x + 5, 58), (x, 58)], TIMBER,
               round_=0.4)                             # loose shell boxes
    return c.image()


def depot(wreck=False):
    c = C(64, 42)
    if wreck:
        ruin(c, 2, 2, 62, 38, (110, 104, 92))
        return c.image()
    c.part([(2, 2), (62, 2), (62, 38), (2, 38)], (92, 94, 90), gloss=0.25,
           round_=0.8)
    ribs(c, 3, 3, 61, 37, 12, (112, 114, 108), vertical=True, width=0.7)
    c.line((2, 20), (62, 20), (70, 72, 70), 1.0)
    return c.image()


def farmhouse(wreck=False):
    c = C(48, 42)
    if wreck:
        ruin(c, 4, 4, 44, 30, (160, 146, 120))
        crater(c, 36, 34, 6)
        return c.image()
    roof(c, 4, 4, 44, 26, TILE)
    c.part([(30, 26), (44, 26), (44, 38), (30, 38)], (140, 70, 48),
           gloss=0.2, round_=0.6)                     # lean-to
    c.part([(8, 30), (24, 40)], (110, 98, 70), shape="ellipse", round_=2)
    c.part([(35, 7), (39, 11)], (120, 80, 64), round_=0.3)  # chimney
    return c.image()


def church(wreck=False):
    """The church in its churchyard, with headstones and a yew."""
    c = C(86, 116)
    c.part([(2, 2), (84, 2), (84, 114), (2, 114)], GRASS, gloss=0.05,
           grain=10, round_=1.5)
    c.line((2, 2), (84, 2), STONE, 1.2)
    c.line((84, 2), (84, 114), STONE, 1.2)
    c.line((2, 114), (84, 114), STONE, 1.2)
    c.line((2, 2), (2, 114), STONE, 1.2)
    for k in range(14):                              # headstones
        x, y = 8 + (k % 7) * 10, 84 + (k // 7) * 12
        c.part([(x, y), (x + 4, y), (x + 4, y + 2.5), (x, y + 2.5)],
               (178, 172, 156), round_=0.3)
    c.part([(62, 60), (78, 76)], (44, 60, 40), shape="ellipse", gloss=0.1,
           round_=2)                                  # yew
    if wreck:
        ruin(c, 30, 10, 54, 76, STONE)
        ruin(c, 18, 30, 66, 44, STONE)
        crater(c, 20, 66, 8)
        return c.image()
    roof(c, 30, 14, 54, 76, SLATE, ridge_vertical=True)   # nave
    roof(c, 16, 32, 68, 44, SLATE)                    # transepts
    c.part([(32, 2), (52, 2), (52, 16), (32, 16)], STONE, gloss=0.3,
           round_=0.6)                                # tower
    c.part([(36, 5), (48, 13)], (90, 92, 96), round_=0.5)
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


# ---- ground tiles (each repeats vertically) --------------------------------
def tile_road():
    c = C(44, 64)
    c.part([(2, -2), (42, -2), (42, 66), (2, 66)], (122, 106, 78), gloss=0.05,
           grain=12, rim=0.0, round_=0)
    for x in (12, 32):                               # ruts
        c.line((x, 0), (x, 64), (96, 82, 60), 2.2)
    c.line((2, 0), (2, 64), (84, 74, 54), 1.0)
    c.line((42, 0), (42, 64), (84, 74, 54), 1.0)
    return c.image()


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
    return c.image()


def tile_trench(ours=False, step=0):
    """A fire trench running up the screen, zigzagged by its traverses.
    step 0 and 1 alternate so the zigzag continues from tile to tile."""
    c = C(72, 64)
    bag = SANDBAG_OURS if ours else SANDBAG_THEIRS
    off = 14 if step else -14
    # The trench floor: duckboards along a zigzag between two bays.
    pts_l = [(22 + off, -2), (22 + off, 22), (22 - off, 42), (22 - off, 66)]
    pts_r = [(x + 22, y) for x, y in pts_l]
    c.part([(36 + off - 18, -2), (36 + off + 6, -2), (36 + off + 6, 22),
            (36 - off + 6, 42), (36 - off + 6, 66), (36 - off - 18, 66),
            (36 - off - 18, 42), (36 + off - 18, 22)], (66, 54, 40),
           gloss=0.0, grain=10, rim=0.7, round_=0.8)
    for y in range(0, 64, 5):
        x = 36 + (off if y < 22 else -off if y > 42 else off * (1 - (y - 22) / 10))
        c.line((x - 14, y), (x + 2, y), (110, 86, 58), 0.6)
    # Parapet and parados of sandbags along both sides.
    for side in (-1, 1):
        for y in range(0, 64, 4):
            x = 36 + (off if y < 22 else -off if y > 42 else off * (1 - (y - 22) / 10))
            x = x - 6 + side * 13
            c.part([(x - 3, y), (x + 3, y), (x + 3, y + 3.4), (x - 3, y + 3.4)],
                   bag, gloss=0.2, grain=6, round_=0.5)
    del pts_l, pts_r
    return c.image()


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
    ("TD_TILE_ROAD", tile_road()), ("TD_TILE_RAIL", tile_rail()),
    ("TD_TILE_TRENCH_A", tile_trench(False, 0)),
    ("TD_TILE_TRENCH_B", tile_trench(False, 1)),
    ("TD_TILE_OURS_A", tile_trench(True, 0)),
    ("TD_TILE_OURS_B", tile_trench(True, 1)),
]


def main():
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
