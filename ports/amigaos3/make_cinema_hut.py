#!/usr/bin/env python3
"""Paint the picture house: a Y.M.C.A. hut behind the lines at night.

Timber walls and roof trusses picked out by an oil lamp and a stove, a
canvas screen in a proscenium hung with bunting, an upright piano with
its pianist by a candle, smoke drifting through the projector's beam, and
rows of soldiers in caps, helmets and greatcoats, rim-lit by the screen.
Painted with the shared painter (paint.py) at four times size and
filtered down.  The screen is left dark for the reels
(x 176..464, y 70..250).  Writes assets/canvas-skies/cinema-hut.bmp.
"""

import math
import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFilter

sys.path.insert(0, str(Path(__file__).resolve().parent))
from paint import Canvas  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "canvas-skies" / "cinema-hut.bmp"
RNG = np.random.default_rng(1917)
W, H = 640, 400
SCREEN = (176, 70, 464, 250)
TIMBER, TIMBER_DARK = (92, 64, 40), (52, 36, 24)


def room():
    """Walls, floor and the roof trusses, lit warm from the stove and lamp
    and cool from the screen."""
    yy, xx = np.mgrid[0:H, 0:W].astype(np.float32)
    base = np.zeros((H, W, 3), np.float32)
    plank = ((xx // 22) % 2) * 6 + np.sin(yy * 0.08 + (xx // 22) * 3.1) * 4
    for c, v in enumerate((64, 44, 30)):
        base[..., c] = v + plank
    base[(xx % 22) < 1.5] *= 0.6                       # board joints
    roof = yy < 120
    base[roof] = base[roof] * 0.7
    base += RNG.normal(0, 4, (H, W, 1))
    stove = np.exp(-(((xx - 590) / 120) ** 2 + ((yy - 250) / 110) ** 2))
    lamp = np.exp(-(((xx - 70) / 140) ** 2 + ((yy - 120) / 120) ** 2))
    screen = np.exp(-(((xx - 320) / 260) ** 2 + ((yy - 170) / 200) ** 2))
    light = 0.28 + stove[..., None] * np.array([1.1, 0.7, 0.35]) + \
        lamp[..., None] * np.array([0.9, 0.7, 0.4]) + \
        screen[..., None] * np.array([0.55, 0.52, 0.48])
    return Image.fromarray(np.clip(base * light, 0, 255).astype(np.uint8))


def trusses(c):
    """King-post trusses across the roof, receding toward the screen."""
    for k, (y, s) in enumerate(((10, 1.0), (40, 0.75), (62, 0.56))):
        half = 340 * s
        tie_y = y + 60 * s
        apex = (320, y)
        left, right = (320 - half, tie_y), (320 + half, tie_y)
        w = 9 * s
        for a, b in ((left, apex), (apex, right), (left, right), (apex, (320, tie_y))):
            c.part([a, b, w], TIMBER, shape="capsule", gloss=0.25, grain=8,
                   round_=0.5)
        c.part([(320 - half * 0.5, tie_y), (320 - half * 0.25, y + 30 * s), 6 * s],
               TIMBER_DARK, shape="capsule", round_=0.4)
        c.part([(320 + half * 0.5, tie_y), (320 + half * 0.25, y + 30 * s), 6 * s],
               TIMBER_DARK, shape="capsule", round_=0.4)


def proscenium(c):
    x0, y0, x1, y1 = SCREEN
    c.part([(x0 - 30, y0 - 26), (x1 + 30, y0 - 26), (x1 + 30, y1 + 22),
            (x0 - 30, y1 + 22)], (88, 54, 34), gloss=0.2, round_=1.5)
    c.part([(x0 - 22, y0 - 18), (x1 + 22, y0 - 18), (x1 + 22, y1 + 14),
            (x0 - 22, y1 + 14)], (124, 78, 46), gloss=0.3, round_=1.0)
    # Curtains drawn back, in heavy red serge with folds.
    for side in (-1, 1):
        edge = x0 - 22 if side < 0 else x1 + 22
        for k in range(5):
            x = edge + side * (k * 6 - 26)
            c.part([(x, y0 - 18), (x + side * 7, y0 - 18), (x + side * 9, y1 + 14),
                    (x + side * 2, y1 + 14)], (128 - k * 8, 34, 30), gloss=0.35,
                   round_=1.0)
    # Bunting: Union flags and pennants strung across the top.
    cols = [(176, 40, 34), (230, 226, 214), (40, 60, 128)]
    for k in range(17):
        x = x0 - 20 + k * ((x1 - x0 + 40) / 16)
        sag = 8 * math.sin(math.pi * k / 16)
        c.part([(x - 8, y0 - 30 + sag), (x + 8, y0 - 30 + sag), (x, y0 - 16 + sag)],
               cols[k % 3], gloss=0.2, round_=0.4)
    c.line((x0 - 22, y0 - 30), (x1 + 22, y0 - 30), (60, 50, 40), 0.8)


def stove(c):
    c.part([(572, 210), (612, 210), (614, 300), (570, 300)], (40, 38, 36),
           gloss=0.45, round_=4)
    c.part([(584, 70), (600, 70), (600, 212), (584, 212)], (46, 44, 42),
           gloss=0.4, round_=2)                      # the flue
    c.part([(580, 260), (604, 286)], (255, 150, 60), shape="ellipse",
           gloss=0.0, rim=0.0, round_=1)              # the fire's glow
    c.glow((592, 272), 40, (255, 140, 50), 0.6)


def lamp(c):
    c.line((70, 40), (70, 96), (40, 36, 30), 1.0)
    c.part([(60, 96), (80, 116)], (200, 168, 90), shape="ellipse", gloss=0.6,
           round_=1)
    c.glow((70, 106), 26, (255, 214, 130), 0.85)


def piano(c):
    """An upright in the pit, the pianist on his stool, a candle on top."""
    c.part([(14, 262), (118, 262), (120, 352), (12, 352)], (58, 34, 22),
           gloss=0.4, grain=6, round_=2)
    c.part([(20, 270), (112, 270), (112, 300), (20, 300)], (78, 48, 30),
           gloss=0.3, round_=1.5)                    # the front panel
    c.part([(16, 304), (116, 304), (118, 314), (14, 314)], (226, 218, 196),
           gloss=0.2, round_=0.5)                    # the keys
    for k in range(17):
        c.line((20 + k * 5.8, 304), (20 + k * 5.8, 309), (20, 16, 14), 0.8)
    c.part([(40, 248), (46, 262)], (232, 224, 200), round_=0.5)   # candle
    c.glow((43, 244), 22, (255, 200, 120), 0.9)
    # The pianist, his back to us, in shirt sleeves and braces.
    c.part([(96, 300), (148, 300), (152, 352), (92, 352)], (70, 64, 52),
           gloss=0.2, round_=6)                      # shoulders and back
    c.part([(112, 268), (134, 292)], (180, 140, 108), shape="ellipse",
           gloss=0.3, round_=2)                      # head, lit by the candle
    c.part([(110, 262), (136, 278)], (40, 30, 24), shape="ellipse",
           round_=1)                                 # his hair


def audience(c):
    """Three rows of men seen from behind, rim-lit by the screen."""
    rng = np.random.default_rng(14)
    rim = (196, 186, 160)
    for row, (y, s) in enumerate(((298, 0.8), (330, 1.0), (366, 1.25))):
        x = 150 + rng.integers(-8, 8)
        while x < W + 20:
            r = 9 * s
            coat = [(48, 46, 34), (58, 52, 38), (44, 42, 40)][rng.integers(3)]
            c.part([(x - r * 2.3, y + r * 0.8), (x + r * 2.3, y + r * 0.8),
                    (x + r * 2.6, H + 10), (x - r * 2.6, H + 10)], coat,
                   gloss=0.1, grain=5, round_=r * 0.6)          # greatcoat
            c.part([(x - r * 0.75, y - r), (x + r * 0.75, y + r * 0.9)],
                   (40, 32, 26), shape="ellipse", round_=1)      # head
            kind = rng.integers(3)
            if kind == 0:                                       # service cap
                c.part([(x - r * 1.0, y - r * 1.25), (x + r * 1.0, y - r * 0.55)],
                       (70, 66, 46), shape="ellipse", gloss=0.2, round_=1)
            elif kind == 1:                                     # steel helmet
                c.part([(x - r * 1.5, y - r * 0.55), (x + r * 1.5, y - r * 0.2)],
                       (60, 62, 52), shape="ellipse", gloss=0.35, round_=1)
                c.part([(x - r * 0.9, y - r * 1.3), (x + r * 0.9, y - r * 0.3)],
                       (66, 68, 58), shape="ellipse", gloss=0.35, round_=1)
            c.line((x - r * 0.7, y - r * 1.15), (x + r * 0.7, y - r * 1.15), rim,
                   0.9)                                         # the rim light
            x += int(r * 4.4 + rng.integers(0, 10))


def smoke_and_beam(img):
    x0, y0, x1, y1 = SCREEN
    beam = Image.new("L", (W, H), 0)
    ImageDraw.Draw(beam).polygon([(330, 398), (x0, y0), (x1, y0), (x1, y1),
                                  (x0, y1)], fill=40)
    beam = beam.filter(ImageFilter.GaussianBlur(14))
    smoke = np.zeros((H, W), np.float32)
    yy, xx = np.mgrid[0:H, 0:W]
    for _ in range(14):
        cx, cy = RNG.uniform(200, 460), RNG.uniform(120, 330)
        rx, ry = RNG.uniform(40, 90), RNG.uniform(12, 26)
        smoke += np.exp(-(((xx - cx) / rx) ** 2 + ((yy - cy) / ry) ** 2)) * 0.6
    b = np.asarray(beam, np.float32) / 255.0 * (0.6 + 0.8 * np.clip(smoke, 0, 1))
    arr = np.asarray(img).astype(np.float32)
    arr += b[..., None] * np.array([230, 220, 196]) * 0.9
    return Image.fromarray(np.clip(arr, 0, 255).astype(np.uint8))


def main():
    img = room()
    c = Canvas(W, H, light=(0.0, -1.0))
    trusses(c)
    proscenium(c)
    stove(c)
    lamp(c)
    layer = c.image()
    key = np.all(np.asarray(layer) == (255, 0, 255), axis=2)
    arr = np.asarray(img).copy()
    arr[~key] = np.asarray(layer)[~key]
    img = Image.fromarray(arr)
    x0, y0, x1, y1 = SCREEN
    # The screen: unbleached canvas, its weave faintly showing, left dark.
    scr = np.full((y1 - y0, x1 - x0, 3), 22, np.float32)
    scr += RNG.normal(0, 1.5, scr.shape[:2])[..., None]
    img.paste(Image.fromarray(np.clip(scr, 0, 255).astype(np.uint8)), (x0, y0))
    img = smoke_and_beam(img)
    img.paste(Image.fromarray(np.clip(scr, 0, 255).astype(np.uint8)), (x0, y0))
    c2 = Canvas(W, H, light=(0.3, -1.0))
    piano(c2)
    audience(c2)
    layer = c2.image()
    key = np.all(np.asarray(layer) == (255, 0, 255), axis=2)
    arr = np.asarray(img).copy()
    arr[~key] = np.asarray(layer)[~key]
    Image.fromarray(arr).save(OUT)


if __name__ == "__main__":
    main()
