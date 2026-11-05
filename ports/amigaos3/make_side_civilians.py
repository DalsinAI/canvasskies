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


def farmhouse(wreck):
    c = C(96, 64)
    c.part([(6, 30), (70, 30), (70, 62), (6, 62)], WALL, gloss=0.15, round_=0.8)
    c.part([(70, 38), (92, 38), (92, 62), (70, 62)], WALL_DARK, gloss=0.1,
           round_=0.6)                                   # the barn end
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
    for x in (14, 30, 46, 60):
        c.part([(x, 40), (x + 6, 40), (x + 6, 50), (x, 50)], WINDOW, gloss=0.3,
               round_=0.3)
        c.line((x + 3, 40), (x + 3, 50), (150, 140, 120), 0.4)
    c.part([(74, 46), (88, 46), (88, 62), (74, 62)], TIMBER, round_=0.4)
    return c.image()


def church(wreck):
    c = C(100, 96)
    c.part([(4, 52), (70, 52), (70, 94), (4, 94)], WALL, gloss=0.15, round_=0.8)
    c.part([(70, 26), (94, 26), (94, 94), (70, 94)], (156, 142, 116),
           gloss=0.2, round_=0.6)                        # the tower
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
        c.part([(x, 62), (x + 6, 62), (x + 6, 80), (x, 80)], WINDOW, gloss=0.3,
               round_=1.2)
    c.part([(77, 40), (87, 40), (87, 52), (77, 52)], (60, 58, 56), round_=2.5)
    c.part([(78, 76), (86, 76), (86, 94), (78, 94)], TIMBER, round_=2.0)
    return c.image()


def cart(wreck):
    c = C(80, 44)
    if wreck:
        for k in range(10):
            x = 4 + k * 7
            c.part([(x, 36 - (k % 2) * 3), (x + 6, 37), (x + 5, 43), (x, 43)],
                   [TIMBER, CHAR, CANVAS][k % 3], round_=0.4)
        c.part([(20, 30), (34, 44)], CHAR, shape="ellipse", round_=0.6)
        return c.image()
    c.part([(4, 14), (46, 14), (46, 32), (4, 32)], TIMBER, gloss=0.2,
           round_=0.8)                                   # the cart bed
    c.part([(6, 4), (44, 4), (44, 16), (6, 16)], CANVAS, gloss=0.2,
           round_=3.0)                                   # bundles under a sheet
    c.part([(16, 24), (34, 42)], (72, 54, 38), shape="ellipse", gloss=0.2,
           round_=0.6)                                   # the wheel
    c.line((25, 33), (25, 33), (40, 30, 24), 2.0)
    c.line((46, 22), (58, 24), TIMBER, 1.0)              # the shafts
    c.part([(54, 14), (74, 14), (74, 26), (54, 26)], HORSE, gloss=0.3,
           round_=3.0)                                   # the horse
    c.part([(70, 6), (78, 6), (78, 16), (70, 16)], HORSE, round_=2.0)
    for x in (56, 60, 68, 72):
        c.line((x, 26), (x, 40), (70, 46, 30), 1.2)
    for x, col in ((40, (70, 70, 86)), (48, (110, 80, 60)), (2, (96, 40, 36))):
        c.part([(x, 22), (x + 6, 22), (x + 6, 40), (x, 40)], col, gloss=0.2,
               round_=1.5)                               # people walking
        c.part([(x + 1, 15), (x + 5, 21)], (204, 172, 140), shape="ellipse",
               round_=0.3)
    return c.image()


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
