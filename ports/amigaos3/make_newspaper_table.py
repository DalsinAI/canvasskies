#!/usr/bin/env python3
"""Paint the morning paper's setting: the mess table under the lamp, a
newsvendor's bundle of papers tied with crossed, knotted string, and the
day's copy lying on top of it, off-white newsprint with its fold and
foxing.  The game sets the type on the top copy at run time.

The halftone block on the front page is made from our own painted
battlefield, dithered as the period presses screened their photographs.

Writes assets/canvas-skies/newspaper-table.bmp (640x400).
Top copy: x 44..596, y 8..392.  Halftone block: x 266..467, y 140..238.
"""

import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFilter

sys.path.insert(0, str(Path(__file__).resolve().parent))
from paint import Canvas  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"
RNG = np.random.default_rng(1918)
TOP = (44, 8, 596, 392)
PHOTO = (266, 140, 467, 238)


def table():
    """Dark oak planks, the lamplight falling from the upper right."""
    w, h = 640, 400
    yy, xx = np.mgrid[0:h, 0:w]
    base = np.zeros((h, w, 3), np.float32)
    plank = (yy // 58) % 2
    grain = np.sin(xx * 0.045 + np.sin(yy * 0.11 + xx * 0.004) * 2.4) * 8
    for c, v in enumerate((70, 46, 30)):
        base[..., c] = v + plank * 6 + grain
    base += RNG.normal(0, 4, (h, w, 1))
    for y in range(58, h, 58):                       # plank joints
        base[y - 1:y + 1] *= 0.55
    light = np.exp(-(((xx - 560) / 420.0) ** 2 + ((yy - 40) / 360.0) ** 2))
    base *= (0.45 + 0.75 * light)[..., None]
    return Image.fromarray(np.clip(base, 0, 255).astype(np.uint8))


def bundle(img):
    """The stack beneath, a little larger than the top copy and offset to
    the lower right, its edges showing as layers of greyer newsprint."""
    d = ImageDraw.Draw(img, "RGBA")
    x0, y0, x1, y1 = TOP
    d.rectangle((x0 + 12, y0 + 14, x1 + 22, y1 + 8), fill=(10, 8, 6, 140))
    for k in range(7, 0, -1):                        # sheets, deepest first
        g = 196 - k * 9
        off = k * 2
        d.rectangle((x0 + 4 + off, y0 + 4 + off, x1 + 6 + off, y1 + 2 + off),
                    fill=(g, g - 4, g - 14))
        d.line((x0 + 4 + off, y1 + 2 + off, x1 + 6 + off, y1 + 2 + off),
               fill=(g - 40, g - 44, g - 50))
    return img


def string(img):
    """Coarse string tied twice round the bundle: it shows where the stack
    reaches beyond the top copy, crossing to a knot with loose ends."""
    c = Canvas(640, 400, light=(-0.4, -0.9))
    twine = (186, 160, 112)
    x0, y0, x1, y1 = TOP
    mid_x, mid_y = 340, 214
    c.line((x1 - 2, mid_y + 2), (x1 + 24, mid_y + 4), twine, 3.2)  # right
    c.line((mid_x + 2, y1 - 2), (mid_x + 6, y1 + 16), twine, 3.2)  # bottom
    c.line((mid_x - 2, y0 + 2), (mid_x + 1, y0 - 6), twine, 2.4)   # top
    # The knot at the lower right corner, ends trailing on the table.
    c.part([(x1 + 10, y1 - 2), (x1 + 22, y1 + 8)], (150, 124, 84),
           shape="ellipse", gloss=0.25, round_=1.0)
    c.line((x1 + 18, y1 + 6), (x1 + 36, y1 + 4), twine, 2.0)
    c.line((x1 + 14, y1 + 7), (x1 + 28, y1 + 18), twine, 2.0)
    c.line((x1 + 14, y1 + 2), (x1 + 20, mid_y + 4), twine, 2.2)
    c.line((x1 + 14, y1 + 2), (mid_x + 6, y1 + 12), twine, 2.2)
    layer = c.image()
    mask = Image.eval(layer.convert("RGB"), lambda v: v)
    key = np.all(np.asarray(layer) == (255, 0, 255), axis=2)
    out = np.asarray(img).copy()
    lay = np.asarray(layer)
    out[~key] = lay[~key]
    del mask
    return Image.fromarray(out)


def top_copy(img):
    """Off-white newsprint, greyed a little, the fold crease down the middle
    and a few spots of foxing; then the halftone block."""
    x0, y0, x1, y1 = TOP
    w, h = x1 - x0, y1 - y0
    yy, xx = np.mgrid[0:h, 0:w]
    paper = np.full((h, w, 3), (232, 230, 220), np.float32)
    paper -= RNG.normal(0, 3.5, (h, w, 1))
    paper *= (0.93 + 0.07 * np.exp(-((xx - w * 0.62) / (w * 0.7)) ** 2))[..., None]
    crease = np.exp(-((xx - w / 2) / 2.2) ** 2)
    paper -= crease[..., None] * 26
    paper += np.exp(-((xx - w / 2 - 4) / 3.0) ** 2)[..., None] * 8
    for _ in range(14):                              # foxing
        cx, cy = RNG.uniform(0, w), RNG.uniform(0, h)
        r = RNG.uniform(2, 7)
        spot = np.exp(-(((xx - cx) ** 2 + (yy - cy) ** 2) / (r * r)))
        paper -= spot[..., None] * np.array([18, 26, 40]) * RNG.uniform(0.4, 1)
    edge = np.minimum(np.minimum(xx, w - 1 - xx), np.minimum(yy, h - 1 - yy))
    paper *= (0.9 + 0.1 * np.clip(edge / 6.0, 0, 1))[..., None]
    img.paste(Image.fromarray(np.clip(paper, 0, 255).astype(np.uint8)),
              (x0, y0))
    # The halftone: our own battlefield, screened in a regular dot pattern.
    px0, py0, px1, py1 = PHOTO
    src = Image.open(ASSETS / "title.bmp").convert("L")
    src = src.crop((10, 150, 10 + 360, 150 + 176)).resize((px1 - px0, py1 - py0))
    a = np.asarray(src.filter(ImageFilter.GaussianBlur(0.6)), np.float32) / 255.0
    a = np.clip((a - a.min()) / (a.max() - a.min() + 1e-6), 0, 1)
    a = 0.06 + 0.9 * a ** 0.8                      # the press's contrast
    bayer = np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9],
                      [15, 7, 13, 5]]) / 16.0
    th = np.tile(bayer, ((py1 - py0) // 4 + 1, (px1 - px0) // 4 + 1))
    dots = a > th[:py1 - py0, :px1 - px0]
    ink, white = np.array([46, 44, 42]), np.array([226, 224, 214])
    halftone = np.where(dots[..., None], white, ink).astype(np.uint8)
    img.paste(Image.fromarray(halftone), (px0, py0))
    d = ImageDraw.Draw(img)
    d.rectangle((px0 - 1, py0 - 1, px1, py1), outline=(40, 38, 36))
    return img


def main():
    img = table()
    img = bundle(img)
    img = top_copy(img)
    img = string(img)
    img.save(ASSETS / "newspaper-table.bmp")


if __name__ == "__main__":
    main()
