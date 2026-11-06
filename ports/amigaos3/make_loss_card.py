#!/usr/bin/env python3
"""Render the losses card on scene3d.py: the mess the night a pilot is
lost, his empty chair drawn up to the table by candlelight, his greatcoat
over its back and his pipe left where he put it down.  The left of the
room stays dark for the lines of text (x 50..360, y 140..320).  Writes
assets/canvas-skies/loss-card.bmp.
"""

import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, str(Path(__file__).resolve().parent))
from scene3d import SS, Scene, planks, rot_z  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"
W, H = 640, 400


def mask(pts):
    m = Image.new("L", (W * SS, H * SS), 0)
    ImageDraw.Draw(m).polygon([(x * SS, y * SS) for x, y in pts], fill=255)
    return np.asarray(m) > 127


def rect(x0, y0, x1, y1):
    return mask([(x0, y0), (x1, y0), (x1, y1), (x0, y1)])


def loss_card():
    sc = Scene(W, H, ambient=(0.02, 0.018, 0.016))
    candle = (500, 196)
    sc.point((candle[0], candle[1], 300), (1.5, 1.0, 0.5), 130)
    a, n, _ = planks(H * SS, W * SS, 81, base=(104, 76, 52), widths=(18, 30),
                     soot_top=0.5)
    sc.surface(np.ones((H * SS, W * SS), bool), 520, a, normals=n)
    floor, _, _ = planks(H * SS, W * SS, 82, base=(70, 50, 34), widths=(6, 9),
                         vertical=False, nails=False)
    sc.surface(rect(0, 300, W, H), 420, floor * 0.7, normal=(0, -0.9, -0.4))
    # The table, its edge toward us, the candle and his pipe on it.
    top, _, _ = planks(H * SS, W * SS, 83, base=(118, 80, 46), widths=(6, 8),
                       vertical=False, nails=False)
    sc.surface(rect(430, 214, 640, 224), 320, top, normal=(0, -0.95, -0.3),
               spec=0.3)
    sc.surface(rect(430, 224, 640, 244), 318, top * 0.6, spec=0.2)
    for x in (446, 620):
        sc.cylinder(x, 244, 320, 4, 322, (66, 44, 28), spec=0.2)
    sc.cylinder(candle[0], 200, 216, 3.2, 300, (232, 224, 204), spec=0.4)
    sc.ellipsoid((candle[0], 216, 300), (8, 2.2, 7), (150, 120, 70), spec=0.7)
    sc.glow((candle[0], candle[1]), 4, (255, 220, 140), 1.6)
    sc.glow((candle[0], candle[1] + 2), 22, (255, 170, 80), 0.45)
    sc.ellipsoid((556, 210, 300), (7, 4.5, 5), (64, 38, 22), spec=0.6,
                 shine=30)                                 # the pipe's bowl
    sc.cylinder(560, 211, 213, 1.2, 299, (30, 24, 20))
    for t in np.linspace(0, 1, 30):                        # its stem
        sc.ellipsoid((560 + t * 26, 212 + t * 1.5, 299), (1.2, 1.2, 1.2),
                     (34, 26, 20), spec=0.4)
    # The empty chair, drawn up to the table: legs, seat, the bowed back.
    wood = (86, 56, 34)
    for x, z in ((392, 330), (436, 330), (396, 300), (432, 300)):
        sc.cylinder(x, 252, 330, 2.6, z, wood, spec=0.3)
    sc.surface(rect(386, 246, 442, 254), 312, wood, normal=(0, -0.9, -0.4),
               spec=0.3)
    sc.surface(rect(386, 254, 442, 258), 310, np.array(wood) * 0.7)
    for x in (390, 438):
        sc.cylinder(x, 150, 252, 2.8, 300, wood, spec=0.35)
    sc.surface(rect(388, 148, 440, 160), 299, wood, normal=(0, -0.3, -1),
               spec=0.35)
    for k in range(4):
        sc.cylinder(398 + k * 10, 160, 246, 1.6, 301, wood, spec=0.3)
    # A greatcoat still hung over its back.
    sc.ellipsoid((414, 172, 296), (24, 18, 6), (70, 62, 44), rim=0.4,
                 R=rot_z(0.05))
    sc.ellipsoid((404, 206, 297), (11, 34, 5), (64, 56, 40), R=rot_z(0.08),
                 rim=0.4)
    img = sc.finish()
    a = np.asarray(img).astype(np.float32)
    # Keep the reading side of the room quiet.
    xx = np.arange(W)[None, :, None]
    a *= np.clip(0.35 + (xx - 300) / 200, 0.35, 1.0)
    return Image.fromarray(a.astype(np.uint8))


def main():
    loss_card().save(ASSETS / "loss-card.bmp")


if __name__ == "__main__":
    main()
