#!/usr/bin/env python3
"""Re-cut the explosion atlases at 160 pixels a cell from their painted
sources, so the large bursts are scaled up far less in play.

Each cell keeps the placing of the 96-pixel atlas it replaces: the
painting in a source cell is fitted to the same box, scaled by 160/96,
so ground bursts still stand on their anchor.  Transparent pixels are
magenta, as before.

  assets/canvas-skies/explosion-atlas.bmp          (from -source.png)
  assets/canvas-skies/topdown-explosion-atlas.bmp  (from -source.png)
"""

from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"
OLD, NEW = 96, 160
KEY = (255, 0, 255)


def bbox(mask):
    ys, xs = np.nonzero(mask)
    if not len(xs):
        return None
    return xs.min(), ys.min(), xs.max() + 1, ys.max() + 1


def recut(name, old):
    src = Image.open(ASSETS / f"{name}-source.png").convert("RGBA")
    sw, sh = src.width / 4, src.height / 4
    old = np.asarray(old.convert("RGB"))
    out = Image.new("RGBA", (NEW * 4, NEW * 4), (0, 0, 0, 0))
    for f in range(16):
        cx, cy = f % 4, f // 4
        cell = old[cy * OLD:(cy + 1) * OLD, cx * OLD:(cx + 1) * OLD]
        target = bbox(~np.all(cell == KEY, axis=2))
        piece = src.crop((round(cx * sw), round(cy * sh),
                          round((cx + 1) * sw), round((cy + 1) * sh)))
        have = bbox(np.asarray(piece)[..., 3] > 24)
        if target is None or have is None:
            continue
        piece = piece.crop(have)
        k = NEW / OLD
        x0, y0, x1, y1 = (round(v * k) for v in target)
        piece = piece.resize((max(1, x1 - x0), max(1, y1 - y0)), Image.LANCZOS)
        out.alpha_composite(piece, (cx * NEW + x0, cy * NEW + y0))
    a = np.asarray(out)
    rgb = a[..., :3].copy()
    rgb[a[..., 3] < 128] = KEY
    return Image.fromarray(rgb)


def main():
    for name in ("explosion-atlas", "topdown-explosion-atlas"):
        path = ASSETS / f"{name}.bmp"
        recut(name, Image.open(path)).save(path)


if __name__ == "__main__":
    main()
