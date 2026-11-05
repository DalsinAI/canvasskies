#!/usr/bin/env python3
"""Rebuild journal.bmp from the painting with its ruled lines taken out.

The painted rules ran slightly uphill and bowed toward the gutter, so the
writing never sat on them.  The game now rules the pages itself, dead level,
and this script lifts the painted rules off the paper first: inside each page
every pixel is replaced by the median of a short vertical run, which removes
thin horizontal lines and keeps the stains and the foxing.
"""

from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"

# Paper inside each page of the 640x360 picture, clear of the curled
# corners, the gutter stitching and the binding.
PAGES = ((96, 64, 308, 306), (326, 60, 552, 306))
RUN = 9   # rows in the vertical median, comfortably wider than a rule


def main() -> None:
    picture = Image.open(ASSETS / "journal-source.png").convert("RGB")
    picture = picture.resize((640, 360), Image.LANCZOS)
    pixels = np.asarray(picture).astype(np.int16)
    out = pixels.copy()
    half = RUN // 2
    for x0, y0, x1, y1 in PAGES:
        rows = np.stack([pixels[y0 + d - half:y1 + d - half, x0:x1]
                         for d in range(RUN)])
        median = np.median(rows, axis=0)
        page = out[y0:y1, x0:x1]
        # Rules are darker than the paper round them: only lighten.
        lighter = median.sum(axis=2) > page.sum(axis=2)
        page[lighter] = median[lighter]
    Image.fromarray(out.astype(np.uint8)).save(ASSETS / "journal.bmp")


if __name__ == "__main__":
    main()
