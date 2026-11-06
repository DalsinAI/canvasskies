#!/usr/bin/env python3
"""The office papers (the memorandum and the court martial's finding).

  office-paper.bmp   640x400: a sheet of aged foolscap on the adjutant's
                     desk, under the lamp: oak boards with a leather
                     blotter, the paper folded once across (its crease
                     and the shadow along it), punched at the left edge,
                     a brass paper clip at the top, the printed form
                     number in the corner, foxing and handling marks.
                     The paper is x 90..550, y 24..368, as before.
  office-stamp.bmp   two cells, 300x40: the rubber stamp's frame in
                     uneven red ink, and over it a mask of paper-coloured
                     specks where the ink failed to take (magenta clear).
"""

import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFont

sys.path.insert(0, str(Path(__file__).resolve().parent))
from paint import Canvas, KEY  # noqa: E402
from scene3d import SS, Scene, fbm, planks  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"
FONTS = ROOT / "assets" / "fonts"
W, H = 640, 400
PX0, PY0, PX1, PY1 = 90, 24, 550, 368
PAPER = (226, 214, 184)
RED = (150, 36, 28)


def desk():
    sc = Scene(W, H, ambient=(0.08, 0.06, 0.05))
    sc.point((470, -40, -120), (2.4, 1.8, 1.1), 420)
    a, n, _ = planks(H * SS, W * SS, 101, base=(96, 60, 34), widths=(46, 70),
                     vertical=False, nails=False)
    sc.surface(np.ones((H * SS, W * SS), bool), 500, a, normals=n, spec=0.3,
               shine=24)
    leather = 0.8 + 0.2 * fbm(H * SS, W * SS, 102, 5, 30)
    m = np.zeros((H * SS, W * SS), bool)
    m[8 * SS:392 * SS, 60 * SS:580 * SS] = True
    sc.surface(m, 495, np.stack([0.20 * leather, 0.24 * leather,
                                 0.16 * leather], -1), spec=0.15)
    return np.asarray(sc.finish()).astype(np.float32)


def sheet(img):
    w, h = PX1 - PX0, PY1 - PY0
    a = np.zeros((h, w, 3), np.float32)
    a[:] = np.array(PAPER, np.float32)
    a *= (0.92 + 0.1 * fbm(h, w, 103, 5, 50))[..., None]
    a *= (1 - 0.1 * np.clip((fbm(h, w, 104, 4, 3) - 0.5) * 3, 0, 1))[..., None]
    rng = np.random.default_rng(7)
    yy, xx = np.mgrid[0:h, 0:w]
    for _ in range(16):                               # foxing
        x, y, r = rng.uniform(0, w), rng.uniform(0, h), rng.uniform(1.5, 5)
        spot = 0.16 * np.exp(-((xx - x) ** 2 + (yy - y) ** 2) / (r * r))
        a *= 1 - spot[..., None] * np.array([0.9, 1.1, 1.6])
    edge = np.minimum(np.minimum(xx, w - 1 - xx), np.minimum(yy, h - 1 - yy))
    a *= (0.84 + 0.16 * np.clip(edge / 14, 0, 1))[..., None]
    # Folded once across: the upper half tilts to the lamp, the lower away,
    # and the crease itself a fine dark line with a light one beside it.
    fy = h // 2 + 6
    a[:fy] *= 1.03
    a[fy:] *= (0.94 + 0.06 * np.clip((yy[fy:] - fy) / 40, 0, 1))[..., None]
    a[fy - 1] *= 0.72
    a[fy] *= 1.08
    # Punched holes at the left, the desk showing through.
    for y in (h // 2 - 70, h // 2 + 70):
        hole = (xx - 12) ** 2 + (yy - y) ** 2 < 16
        a[hole] = img[PY0:PY1, PX0:PX1][hole] * 0.5
    # Printed form number and the rules top and foot.
    a[14, 14:w - 14] = np.array([150, 120, 80])
    a[h - 15, 14:w - 14] = np.array([150, 120, 80])
    t = Image.new("RGB", (w, h))
    t.paste(Image.fromarray(np.clip(a, 0, 255).astype(np.uint8)))
    d = ImageDraw.Draw(t)
    f = ImageFont.truetype(str(FONTS / "oldstandardtt/OldStandard-Regular.ttf"), 9)
    d.text((16, 2), "Army Form B. 122.", font=f, fill=(90, 74, 56))
    d.text((w - 118, h - 13), "Wt. 1914/3000.  H. & S.", font=f,
           fill=(110, 92, 70))
    a = np.asarray(t).astype(np.float32)
    # The sheet's shadow on the blotter, then the sheet.
    img[PY0 + 5:PY1 + 6, PX0 + 6:PX1 + 7] *= 0.45
    img[PY0:PY1, PX0:PX1] = a


def paper_clip(img):
    c = Canvas(W, H, light=(-0.5, -0.9))
    x, y = 482, 14
    pts = [(x, y + 34), (x, y), (x + 9, y), (x + 9, y + 28), (x + 3, y + 28),
           (x + 3, y + 7)]
    for p0, p1 in zip(pts[:-1], pts[1:]):
        c.part([p0, p1, 2.4], (176, 150, 92), shape="capsule", gloss=0.9,
               round_=0.3)                        # brass wire
    im = np.asarray(c.image()).astype(np.float32)
    k = np.all(im == KEY, axis=2)
    sh = np.zeros_like(k)
    sh[3:, 3:] = ~k[:-3, :-3]
    img[sh & k] *= 0.6
    img[~k] = im[~k]


def stamp():
    """The stamp's frame, inked unevenly, and the specks where it failed."""
    w, h = 300, 40
    ink = fbm(h, w, 105, 5, 20)
    frame = np.zeros((h, w), bool)
    frame[1:5, 1:w - 1] = frame[h - 5:h - 1, 1:w - 1] = True
    frame[1:h - 1, 1:5] = frame[1:h - 1, w - 5:w - 1] = True
    frame[7:9, 7:w - 7] = frame[h - 9:h - 7, 7:w - 7] = True
    frame[7:h - 7, 7:9] = frame[7:h - 7, w - 9:w - 7] = True
    take = ink > 0.22
    a = np.full((h, w, 3), KEY, np.float32)
    shade = (0.75 + 0.5 * ink)[..., None]
    a[frame & take] = (np.array(RED, np.float32) * shade)[frame & take]
    b = np.full((h, w, 3), KEY, np.float32)
    rng = np.random.default_rng(9)
    specks = (fbm(h, w, 106, 4, 40) > 0.68) | (rng.random((h, w)) > 0.95)
    b[specks] = np.array(PAPER, np.float32) * 0.96
    return np.concatenate([a, b], 1)


def main():
    img = desk()
    sheet(img)
    paper_clip(img)
    Image.fromarray(np.clip(img, 0, 255).astype(np.uint8)).save(
        ASSETS / "office-paper.bmp")
    Image.fromarray(np.clip(stamp(), 0, 255).astype(np.uint8)).save(
        ASSETS / "office-stamp.bmp")


if __name__ == "__main__":
    main()
