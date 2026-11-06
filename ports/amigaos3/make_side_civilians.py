#!/usr/bin/env python3
"""Paint the civilian places of the side-on view, whole and wrecked.

A farmhouse, a village church and a refugee cart, seen from the side with
the low sun behind the viewer's left shoulder, in the palette of the
painted side-on landscape.  Writes assets/canvas-skies/side-civilians.bmp
and src/modern/canvas_side_civilians.h (each sprite's rectangle).
"""

import math
import sys
from pathlib import Path

from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parent))
from paint import Canvas, KEY  # noqa: E402
from scene3d import Scene, rot_z  # noqa: E402
import numpy as np  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
ATLAS = ROOT / "assets" / "canvas-skies" / "side-civilians.bmp"
HEADER = ROOT / "src" / "modern" / "canvas_side_civilians.h"
SUN = (-0.5, -0.9)

WALL = (166, 150, 122)
WALL_DARK = (120, 106, 86)
TILE = (150, 76, 52)
SLATE = (90, 92, 98)
TIMBER = (100, 72, 48)
CHAR = (40, 34, 30)
ASH = (88, 80, 72)
WINDOW = (52, 46, 40)
HORSE = (96, 64, 40)
CANVAS = (176, 160, 122)


def C(w, h):
    return Canvas(w, h, light=SUN)


def stonework(c, x0, y0, x1, y1, base, seed):
    """Coursed rubble stone: blocks of varying length and tone, each lit
    along its top edge, set in darker mortar."""
    rng = np.random.default_rng(seed)
    c.part([(x0, y0), (x1, y0), (x1, y1), (x0, y1)],
           tuple(int(v * 0.62) for v in base), gloss=0.0, rim=0.0, round_=0.3)
    y = y0 + 0.4
    row = 0
    while y < y1 - 0.5:
        h = rng.uniform(3.2, 4.4)
        x = x0 + (rng.uniform(1.5, 3.5) if row % 2 else 0.4)
        while x < x1 - 0.5:
            w = rng.uniform(4.0, 8.5)
            tone = rng.normal(1.0, 0.07)
            col = tuple(int(min(255, v * tone)) for v in base)
            c.part([(x, y), (min(x1 - 0.4, x + w - 0.7), y),
                    (min(x1 - 0.4, x + w - 0.7), min(y1, y + h - 0.7)),
                    (x, min(y1, y + h - 0.7))], col, gloss=0.25, grain=3,
                   rim=0.35, round_=0.5)
            x += w
        y += h
        row += 1


def shutters(c, x, y, w, h):
    """A window in its stone frame, with green wooden shutters folded back."""
    c.part([(x - 1, y - 1), (x + w + 1, y - 1), (x + w + 1, y + h + 1),
            (x - 1, y + h + 1)], (186, 176, 152), gloss=0.3, round_=0.3)
    c.part([(x, y), (x + w, y), (x + w, y + h), (x, y + h)], WINDOW,
           gloss=0.5, round_=0.3)
    c.line((x + w / 2, y), (x + w / 2, y + h), (150, 140, 120), 0.4)
    c.line((x, y + h / 2), (x + w, y + h / 2), (150, 140, 120), 0.4)
    for sx in (x - 4, x + w + 0.5):
        c.part([(sx, y), (sx + 3.5, y), (sx + 3.5, y + h), (sx, y + h)],
               (62, 84, 64), gloss=0.25, round_=0.3)


def farmhouse(wreck):
    c = C(96, 64)
    stonework(c, 6, 30, 70, 62, WALL, 1)
    stonework(c, 70, 38, 92, 62, WALL_DARK, 2)          # the barn end
    if wreck:
        c.part([(6, 30), (20, 22), (34, 30), (52, 26), (70, 30), (70, 34),
                (6, 34)], CHAR, gloss=0.0, grain=12, round_=1.2)
        for x in (14, 30, 46, 60):
            c.part([(x, 40), (x + 6, 40), (x + 6, 50), (x, 50)], CHAR,
                   round_=0.3)
        for k in range(14):
            x = 4 + k * 6.5
            c.part([(x, 58 - (k % 3) * 2), (x + 5, 58), (x + 4, 63), (x, 63)],
                   [ASH, WALL, TILE][k % 3], round_=0.4)
        return c.image()
    c.part([(2, 31), (38, 6), (74, 31)], TILE, gloss=0.25, round_=0.8)
    for k in range(1, 9):                                # rows of tiles
        y = 6 + k * 3
        half = (y - 6) * 36 / 25
        c.line((38 - half, y), (38 + half, y), (124, 60, 42), 0.5)
    c.part([(68, 38), (94, 38), (82, 26)], SLATE, gloss=0.2, round_=0.6)
    c.part([(50, 6), (55, 6), (55, 18), (50, 18)], (120, 82, 64), round_=0.3)
    for x in (16, 32, 48, 61):
        shutters(c, x, 40, 6, 10)
    c.part([(74, 46), (88, 46), (88, 62), (74, 62)], TIMBER, round_=0.4)
    for k in range(4):                                   # the barn door's boards
        c.line((75 + k * 3.5, 46.5), (75 + k * 3.5, 61.5), (70, 50, 34), 0.5)
    c.part([(2, 31), (74, 31), (74, 33), (2, 33)], (60, 40, 30), gloss=0,
           rim=0, round_=0.2)                           # the eaves' shadow
    return c.image()


def church(wreck):
    c = C(100, 96)
    stonework(c, 4, 52, 70, 94, WALL, 3)
    stonework(c, 70, 26, 94, 94, (156, 142, 116), 4)    # the tower
    if wreck:
        c.part([(4, 52), (18, 40), (32, 54), (50, 46), (70, 52), (70, 58),
                (4, 58)], CHAR, gloss=0.0, grain=12, round_=1.2)
        c.part([(70, 26), (78, 18), (86, 30), (94, 22), (94, 32), (70, 32)],
               CHAR, round_=1.0)
        for k in range(16):
            x = 2 + k * 6
            c.part([(x, 90 - (k % 3) * 2), (x + 5, 90), (x + 4, 95), (x, 95)],
                   [ASH, WALL, SLATE][k % 3], round_=0.4)
        return c.image()
    c.part([(0, 53), (37, 30), (74, 53)], SLATE, gloss=0.25, round_=0.8)
    c.part([(68, 27), (82, 2), (96, 27)], SLATE, gloss=0.3, round_=0.6)  # spire
    c.part([(80, 0), (84, 0), (84, 8), (80, 8)], (60, 60, 60), round_=0.2)
    for x in (14, 34, 54):                               # lancet windows
        c.part([(x - 1.2, 61), (x + 7.2, 61), (x + 7.2, 81), (x - 1.2, 81)],
               (186, 176, 152), gloss=0.3, round_=1.6)
        c.part([(x, 62), (x + 6, 62), (x + 6, 80), (x, 80)], WINDOW, gloss=0.5,
               round_=1.2)
        for k in range(1, 4):
            c.line((x, 62 + k * 4.5), (x + 6, 62 + k * 4.5), (90, 96, 104), 0.35)
    c.part([(0, 53), (74, 53), (74, 55), (0, 55)], (60, 50, 44), gloss=0,
           rim=0, round_=0.2)                           # the eaves' shadow
    c.part([(77, 40), (87, 40), (87, 52), (77, 52)], (60, 58, 56), round_=2.5)
    c.part([(78, 76), (86, 76), (86, 94), (78, 94)], TIMBER, round_=2.0)
    return c.image()


def cart(wreck):
    """A farm cart piled with what a family could carry, the horse between
    the shafts and the family walking beside it (scene3d.py), lit from the
    sky behind the viewer's left shoulder."""
    if wreck:
        c = C(80, 44)
        for k in range(10):
            x = 4 + k * 7
            c.part([(x, 36 - (k % 2) * 3), (x + 6, 37), (x + 5, 43), (x, 43)],
                   [TIMBER, CHAR, CANVAS][k % 3], round_=0.4)
        c.part([(20, 30), (34, 44)], CHAR, shape="ellipse", round_=0.6)
        return c.image()
    sc = Scene(80, 44, ambient=(0.28, 0.27, 0.27))
    sc.direction((-0.6, -0.7, -0.5), (1.15, 1.05, 0.92))
    sc.direction((0.5, -0.2, 0.6), (0.35, 0.32, 0.3))   # the sky behind
    horse = (98, 64, 40)
    # The horse: barrel, chest, neck, head, and four legs, one lifted.
    sc.ellipsoid((64, 20, 40), (10, 6, 5), horse, spec=0.3, rim=0.4)
    sc.ellipsoid((72, 18, 40), (5, 6.5, 5), horse, spec=0.3, rim=0.4)
    sc.ellipsoid((75, 12, 40), (2.6, 6, 3), horse, R=rot_z(0.5), spec=0.3,
                 rim=0.4)
    sc.ellipsoid((78, 7.5, 40), (2.4, 4.2, 2.2), horse, R=rot_z(1.05),
                 spec=0.3, rim=0.4)
    sc.ellipsoid((73.5, 9, 39), (1.2, 4.5, 1.4), (40, 28, 20), R=rot_z(0.5))
    for x, z, lift in ((57, 43, 0), (59, 37, 0), (69, 43, 3), (71, 37, 0)):
        sc.ellipsoid((x, 35 - lift / 2, z), (1.4, 8 - lift / 2, 1.4), horse,
                     R=rot_z(0.25 if lift else 0), rim=0.3)
    sc.ellipsoid((54, 18, 40), (1.2, 4, 1.2), (60, 40, 26), R=rot_z(1.3))  # tail
    # The cart: its bed, the load under a roped sheet, the wheel, shafts.
    sc.ellipsoid((26, 23, 44), (21, 4.5, 8), TIMBER, spec=0.2, rim=0.3)
    sc.ellipsoid((22, 13, 44), (17, 8.5, 7), CANVAS, spec=0.1, rim=0.4)
    sc.ellipsoid((36, 10, 42), (5, 5, 5), (110, 70, 50), rim=0.4)   # a trunk
    sc.ellipsoid((12, 9, 42), (4, 3.5, 4), (80, 90, 110), rim=0.4)  # a bundle
    sc.ellipsoid((25, 33, 38), (9.5, 9.5, 1.4), (72, 54, 38), spec=0.2)
    sc.ellipsoid((25, 33, 37), (2.2, 2.2, 1.4), (50, 38, 28), spec=0.3)
    for k in range(8):                                  # the spokes
        a = k * math.pi / 4
        for t in np.linspace(0.25, 0.9, 6):
            sc.ellipsoid((25 + math.cos(a) * 9 * t, 33 + math.sin(a) * 9 * t,
                          37.5), (0.7, 0.7, 0.7), (44, 32, 24))
    sc.ellipsoid((50, 21, 40), (8, 0.9, 0.9), TIMBER)   # the shafts
    # The family: the mother in a shawl, a child, the old man.
    for x, h, coat, head in ((44, 12, (80, 60, 70), (200, 168, 138)),
                             (49, 8, (110, 80, 60), (206, 172, 140)),
                             (4, 13, (60, 60, 64), (190, 160, 130))):
        y = 43 - h * 0.92
        sc.ellipsoid((x, y - h * 0.35, 30), (3.8, h * 0.5, 3.0), coat,
                     rim=0.5)                           # the body, shoulders
        sc.ellipsoid((x + 0.4, y + h * 0.45, 31), (3.0, h * 0.5, 2.6),
                     np.array(coat) * 0.8, rim=0.4)     # the skirt or legs
        sc.ellipsoid((x + 3.4, y - h * 0.2, 29), (1.2, h * 0.4, 1.2), coat,
                     R=rot_z(-0.25), rim=0.5)           # an arm swinging
        sc.ellipsoid((x, y - h * 0.9 - 1.6, 30), (1.9, 2.2, 1.9), head, rim=0.5)
        sc.ellipsoid((x - 0.6, y - h * 0.9 - 2.8, 30), (2.2, 1.4, 2.2),
                     np.array(coat) * 0.8, rim=0.5)     # a shawl or a cap
    drawn = np.isfinite(sc.z)
    img = np.asarray(sc.finish()).copy()
    hit = np.asarray(Image.fromarray(drawn.astype(np.uint8) * 255).resize(
        (80, 44), Image.BOX)) > 110
    img[~hit] = KEY
    return Image.fromarray(img)


SPRITES = [("SC_FARM", farmhouse(False)), ("SC_FARM_WRECK", farmhouse(True)),
           ("SC_CHURCH", church(False)), ("SC_CHURCH_WRECK", church(True)),
           ("SC_CART", cart(False)), ("SC_CART_WRECK", cart(True))]


def main():
    x, places, h = 0, [], max(img.height for _, img in SPRITES)
    for name, img in SPRITES:
        places.append((name, x, 0, img.width, img.height))
        x += img.width + 1
    atlas = Image.new("RGB", (x, h), KEY)
    for (name, px, py, w, hh), (_, img) in zip(places, SPRITES):
        atlas.paste(img, (px, py))
    atlas.save(ATLAS)
    out = ["/* Generated by ports/amigaos3/make_side_civilians.py: do not edit. */",
           "#ifndef CANVAS_SIDE_CIVILIANS_H", "#define CANVAS_SIDE_CIVILIANS_H",
           "", "typedef enum {"]
    out += [f"    {name}," for name, *_ in places]
    out += ["    SC_SPRITE_COUNT", "} SideCivilianSprite;", "",
            "static const short side_civilian_rect[SC_SPRITE_COUNT][4] = {"]
    out += [f"    {{{px}, {py}, {w}, {hh}}}, /* {name} */"
            for name, px, py, w, hh in places]
    out += ["};", "", "#endif", ""]
    HEADER.write_text("\n".join(out))


if __name__ == "__main__":
    main()
