#!/usr/bin/env python3
"""Render the squadron mess at night.

Built and lit with the small renderer in scene3d.py: a hut of rough-sawn
boards lit by one hanging oil lamp, bunting strung under the eaves, the
squadron board on the left wall (its slate left clean: the game chalks
the victories on it, x 48..332, y 72..252), a long table with its cloth,
bottles and glasses of beer catching the lamp, the gramophone with its
brass horn, a propeller hung on the wall and the stove glowing in the
corner.  The foot of the room is left dark for the evening's lines.
Rendered at twice the size, filtered down and dithered to RGB565.
Writes assets/canvas-skies/mess.bmp.
"""

import math
import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, str(Path(__file__).resolve().parent))
from scene3d import SS, Scene, fbm, normalize, planks, rot_z  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "canvas-skies" / "mess.bmp"
W, H = 640, 400
BOARD = (40, 64, 340, 260)
LAMP = (420, 74)


def mask(pts):
    m = Image.new("L", (W * SS, H * SS), 0)
    ImageDraw.Draw(m).polygon([(x * SS, y * SS) for x, y in pts], fill=255)
    return np.asarray(m) > 127


def rect(x0, y0, x1, y1):
    return mask([(x0, y0), (x1, y0), (x1, y1), (x0, y1)])


def room(sc):
    a, n, _ = planks(H * SS, W * SS, 71, base=(116, 84, 56), widths=(18, 32),
                     soot_top=0.5)
    sc.surface(np.ones((H * SS, W * SS), bool), 520, a, normals=n)
    floor, _, _ = planks(H * SS, W * SS, 72, base=(74, 54, 36), widths=(6, 9),
                         vertical=False, nails=False)
    sc.surface(rect(0, 268, W, H), 470, floor * 0.7, normal=(0, -0.9, -0.4))
    # The eaves beam and the bunting strung beneath it.
    beam, _, _ = planks(H * SS, W * SS, 73, base=(100, 70, 44),
                        widths=(400, 401), vertical=False)
    sc.surface(rect(0, 0, W, 9), 500, beam, normal=(0, 0.5, -1))
    cols = [(176, 40, 34), (226, 220, 204), (40, 60, 128)]
    cloth = 0.85 + 0.15 * fbm(H * SS, W * SS, 74, 3, 60)
    for k in range(17):
        x = k * 40
        sag = 8 * math.sin(math.pi * ((x % 320) / 320))
        m = mask([(x, 11 + sag), (x + 30, 11 + sag), (x + 15, 36 + sag)])
        alb = np.array(cols[k % 3], np.float32)[None, None, :] / 255.0 * \
            cloth[..., None]
        sc.surface(m, 495 - k * 0.01, alb, normal=(0.1 * math.sin(k), 0.3, -1))
    sc.surface(mask([(0, 10), (640, 10), (640, 12), (0, 12)]), 494,
               (60, 50, 38))


def board(sc):
    x0, y0, x1, y1 = BOARD
    wood, _, _ = planks(H * SS, W * SS, 75, base=(120, 80, 46),
                        widths=(400, 401), vertical=False)
    yy, xx = np.mgrid[0:H * SS, 0:W * SS].astype(np.float32) / SS
    n = np.zeros((H * SS, W * SS, 3), np.float32)
    n[..., 2] = -1
    n[..., 0] += np.where(xx < x0 + 4, -0.7, np.where(xx > x1 - 4, 0.7, 0))
    n[..., 1] += np.where(yy < y0 + 4, -0.7, np.where(yy > y1 - 4, 0.7, 0))
    sc.surface(rect(x0, y0, x1, y1) & ~rect(x0 + 8, y0 + 8, x1 - 8, y1 - 8),
               505, wood, normals=normalize(n), spec=0.25)
    slate = 0.85 + 0.15 * fbm(H * SS, W * SS, 76, 5, 8)
    smudge = np.clip((fbm(H * SS, W * SS, 77, 4, 4) - 0.55) * 3, 0, 1)
    alb = np.stack([0.11 + 0.08 * smudge, 0.15 + 0.08 * smudge,
                    0.12 + 0.07 * smudge], -1) * slate[..., None]
    sc.surface(rect(x0 + 8, y0 + 8, x1 - 8, y1 - 8), 506, alb, spec=0.05)
    sc.surface(rect(x0 + 10, y1 - 6, x1 - 10, y1 - 2), 503, wood,
               normal=(0, -0.8, -0.6))                    # the chalk ledge
    for k in range(3):
        sc.cylinder(x0 + 40 + k * 12, y1 - 8, y1 - 6, 1.2, 501,
                    (230, 226, 210))


def table(sc):
    top, _, _ = planks(H * SS, W * SS, 78, base=(122, 82, 46), widths=(6, 8),
                       vertical=False)
    sc.surface(rect(356, 232, 560, 240), 380, top, normal=(0, -0.95, -0.3),
               spec=0.3)
    sc.surface(rect(356, 240, 560, 264), 378, top * 0.55, spec=0.2)
    cloth = 0.88 + 0.12 * fbm(H * SS, W * SS, 79, 3, 40)
    sc.surface(rect(368, 229, 548, 241), 370,
               np.stack([0.84 * cloth, 0.81 * cloth, 0.73 * cloth], -1),
               normal=(0, -0.7, -0.7), spec=0.1)
    for x in (380, 540):
        sc.cylinder(x, 264, 300, 3.5, 380, (70, 46, 28), spec=0.2)
    # Bottles, dark green glass with the lamp in them, and beer in glasses.
    for k, x in enumerate((386, 410, 434, 458, 480)):
        if k % 2:
            sc.cylinder(x, 206, 232, 4.6, 360, (36, 64, 40), spec=1.2,
                        shine=40)
            sc.ellipsoid((x, 206, 360), (4.6, 3.2, 4.6), (36, 64, 40),
                         spec=1.0, shine=40)
            sc.cylinder(x, 192, 205, 1.6, 359, (36, 64, 40), spec=1.0,
                        shine=40)
            sc.cylinder(x, 190, 193, 1.9, 358, (150, 120, 70))
        else:
            sc.cylinder(x, 214, 232, 4.4, 358, (230, 170, 70), spec=0.9,
                        shine=36)
            sc.glow((x - 1, 224), 3, (255, 190, 90), 0.35)  # lamp in the beer
            sc.ellipsoid((x, 214, 358), (4.4, 1.8, 4.4), (236, 228, 206),
                         spec=0.3)
    # The gramophone: its case, the turntable and the brass horn.
    case, _, _ = planks(H * SS, W * SS, 80, base=(86, 52, 30),
                        widths=(400, 401), vertical=False)
    sc.surface(rect(492, 206, 540, 229), 356, case, spec=0.45, shine=24)
    sc.surface(rect(490, 202, 542, 206), 355, case, normal=(0, -0.9, -0.4),
               spec=0.4)
    sc.ellipsoid((516, 201, 354), (20, 2, 12), (28, 26, 24), spec=0.5)
    n = 48
    for k in range(n):
        t = k / (n - 1)
        cx, cy = 518 + 34 * t, 198 - 46 * t * t - 10 * t
        r = 2.2 + 17 * t ** 2.2
        sc.ellipsoid((cx, cy, 350 - 8 * t), (r * 0.55, r, r), (188, 140, 62),
                     R=rot_z(-0.5 - 0.6 * t), spec=1.0, shine=30)
    sc.ellipsoid((553, 150, 340), (5, 13, 12), (70, 48, 22),
                 R=rot_z(-1.1))                           # into the bell


def propeller(sc):
    def lam(q, n):
        return 0.82 + 0.18 * (np.sin(q[..., 1] * 14) > 0)
    # High on the wall, well clear of the gramophone's horn below it.
    for side in (-1, 1):
        sc.ellipsoid((552 + side * 31, 72 - side * 4, 500), (5.5, 34, 3),
                     (128, 84, 44), R=rot_z(math.pi / 2 - 0.13), spec=0.6,
                     shine=24, texture=lam)
    sc.ellipsoid((552, 72, 497), (7, 7, 5), (60, 44, 30), spec=0.5)


def stove(sc):
    sc.cylinder(604, 206, 298, 20, 430, (46, 44, 42), spec=0.4, shine=14,
                texture=lambda u, v: 0.85 + 0.15 * (np.sin(v * 50) > 0.6))
    sc.ellipsoid((604, 206, 430), (20, 5, 12), (56, 54, 50), spec=0.4)
    sc.cylinder(604, 12, 206, 6, 432, (52, 50, 48), spec=0.3)
    sc.surface(rect(594, 254, 614, 280), 408, (255, 150, 60))
    sc.glow((604, 267), 12, (255, 140, 50), 1.0)


def lamp(sc):
    sc.surface(rect(LAMP[0] - 0.6, 0, LAMP[0] + 0.6, LAMP[1] - 10), 400,
               (40, 36, 30))
    sc.ellipsoid((LAMP[0], LAMP[1] - 10, 398), (8, 3, 8), (150, 120, 60),
                 spec=0.8)
    sc.ellipsoid((LAMP[0], LAMP[1] + 1, 398), (7, 12, 7), (250, 220, 150),
                 spec=1.0)                                # the glass chimney
    sc.ellipsoid((LAMP[0], LAMP[1] + 14, 398), (10, 5, 10), (176, 136, 64),
                 spec=0.9, shine=28)                      # the brass font
    sc.glow(LAMP, 16, (255, 214, 140), 1.6)


def main():
    sc = Scene(W, H, ambient=(0.035, 0.03, 0.025))
    sc.point((LAMP[0], LAMP[1] + 4, 330), (1.9, 1.35, 0.75), 200)
    sc.point((604, 267, 400), (1.2, 0.55, 0.2), 80)
    room(sc)
    board(sc)
    propeller(sc)
    table(sc)
    stove(sc)
    lamp(sc)
    img = sc.finish()
    # Keep the foot of the room dark for the evening's lines of text.
    a = np.asarray(img).astype(np.float32)
    a[272:] *= 0.55
    Image.fromarray(a.astype(np.uint8)).save(OUT)


if __name__ == "__main__":
    main()
