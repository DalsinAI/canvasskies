#!/usr/bin/env python3
"""Paint the paper of the new-machines cards.

  cell 0  268x250  "From the aircraft park": a buff record card
  cell 1  268x250  "Intelligence summary": a grey-blue form
  cell 2  600x70   the diary slip the line beneath is written on

Paper with fibre, foxing and a deckled, slightly worn edge; on the cards a
mount for the photograph of the machine (x 12..256, y 32..156) with black
corner tabs, a ruled line under the heading and, on the record card, a
paper clip at the top.  Magenta is clear.  Writes
assets/canvas-skies/machine-cards.bmp.
"""

import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFilter

sys.path.insert(0, str(Path(__file__).resolve().parent))
from paint import Canvas, KEY  # noqa: E402
from scene3d import fbm  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "canvas-skies" / "machine-cards.bmp"


def paper(w, h, colour, seed, foxing=1.0):
    rng = np.random.default_rng(seed)
    a = np.zeros((h, w, 3), np.float32)
    a[:] = np.array(colour, np.float32)
    fibre = fbm(h, w, seed, 5, 40)
    blotch = fbm(h, w, seed + 1, 4, 3)
    a *= (0.93 + 0.1 * fibre)[..., None]
    a *= (1 - 0.12 * foxing * np.clip((blotch - 0.5) * 3, 0, 1))[..., None]
    for _ in range(int(14 * foxing)):                # foxing spots
        x, y, r = rng.uniform(0, w), rng.uniform(0, h), rng.uniform(1, 3.5)
        yy, xx = np.ogrid[0:h, 0:w]
        d = np.hypot(xx - x, yy - y) / r
        spot = 0.14 * np.exp(-d * d)[..., None]
        a *= 1 - spot * np.array([1.0, 1.15, 1.6])
    # Edges darkened by handling, and a deckled outline.
    yy, xx = np.mgrid[0:h, 0:w]
    edge = np.minimum(np.minimum(xx, w - 1 - xx), np.minimum(yy, h - 1 - yy))
    a *= (0.8 + 0.2 * np.clip(edge / 10, 0, 1))[..., None]
    deckle = (fbm(h, w, seed + 2, 3, 60) - 0.5) * 3
    inside = edge + deckle > 1.2
    out = np.where(inside[..., None], a, np.array(KEY, np.float32))
    return out


def mount(a, x0, y0, x1, y1):
    """The photograph's mount: a shade darker, a sunk edge, corner tabs."""
    a[y0:y1, x0:x1] *= 0.93
    a[y0:y0 + 1, x0:x1] *= 0.7
    a[y0:y1, x0:x0 + 1] *= 0.7
    a[y1 - 1:y1, x0:x1] *= 1.08
    a[y0:y1, x1 - 1:x1] *= 1.08
    tab = 12
    for cx, cy, sx, sy in ((x0, y0, 1, 1), (x1, y0, -1, 1), (x0, y1, 1, -1),
                           (x1, y1, -1, -1)):
        img = Image.new("L", (a.shape[1], a.shape[0]), 0)
        ImageDraw.Draw(img).polygon([(cx - sx * 2, cy - sy * 2),
                                     (cx + sx * tab, cy - sy * 2),
                                     (cx - sx * 2, cy + sy * tab)], fill=255)
        m = np.asarray(img) > 0
        a[m] = np.array([28, 24, 22])
        hl = np.asarray(img.filter(ImageFilter.FIND_EDGES)) > 0
        a[hl & m] = np.array([70, 64, 58])


def clip(a, x, y):
    """A paper clip over the top edge, wire lit along its curve."""
    c = Canvas(a.shape[1], a.shape[0], light=(-0.5, -0.9))
    pts = [(x, y + 30), (x, y), (x + 8, y), (x + 8, y + 24), (x + 3, y + 24),
           (x + 3, y + 6)]
    for p0, p1 in zip(pts[:-1], pts[1:]):
        c.part([p0, p1, 2.2], (150, 152, 158), shape="capsule", gloss=0.8,
               round_=0.3)
    im = np.asarray(c.image()).astype(np.float32)
    k = np.all(im == KEY, axis=2)
    sh = np.zeros_like(k)
    sh[2:, 2:] = ~k[:-2, :-2]
    paperish = ~np.all(a == KEY, axis=2)
    a[sh & k & paperish] *= 0.7
    a[~k] = im[~k]


def main():
    card = paper(268, 250, (222, 210, 178), 1)
    mount(card, 12, 32, 256, 156)
    card[26:27, 10:258] = np.array([60, 48, 40])     # the rule under the head
    clip(card, 230, 2)
    form = paper(268, 250, (198, 204, 196), 2, foxing=0.6)
    mount(form, 12, 32, 256, 156)
    form[26:27, 10:258] = np.array([60, 52, 48])
    for y in range(176, 240, 18):                    # the form's ruled lines
        form[y:y + 1, 10:258] *= 0.82
    slip = paper(600, 70, (222, 210, 178), 3)
    sheet = np.full((250, 268 * 2 + 600 + 2, 3), KEY, np.float32)
    sheet[:, :268] = card
    sheet[:, 269:537] = form
    sheet[:70, 538:1138] = slip
    Image.fromarray(np.clip(sheet, 0, 255).astype(np.uint8)).save(OUT)


if __name__ == "__main__":
    main()
