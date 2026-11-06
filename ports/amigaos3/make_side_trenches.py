#!/usr/bin/env python3
"""Cut fire trenches into the side-on foreground, seen end-on at ground
level: a sandbagged parapet on the enemy side and a lower parados
behind, the cut itself with its revetted walls, firestep and duckboards
going away into the dark, and a belt of wire on pickets out in front.

Rendered with scene3d.py, backlit by the evening sky like the rest of
the foreground, and set into foreground-panorama-base.bmp (the painted
ground as it was) to make assets/canvas-skies/foreground-panorama.bmp.
"""

import math
import sys
from pathlib import Path

import numpy as np
from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parent))
from scene3d import SS, Scene, fbm, rot_z  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"
BASE = ASSETS / "foreground-panorama-base.bmp"
OUT = ASSETS / "foreground-panorama.bmp"
KEY = (255, 0, 255)
SITES = (340, 880)             # flat stretches of ground, clear of stumps


def ground_line(img):
    a = np.asarray(img.convert("RGB")).astype(int)
    key = (a[..., 0] > 200) & (a[..., 1] < 60) & (a[..., 2] > 200)
    top = np.array([np.argmax(~key[:, x]) for x in range(a.shape[1])])
    return np.array([np.percentile(top[max(0, x - 30):x + 30], 80)
                     for x in range(len(top))])


def sandbags(sc, x0, x1, ground, rows, z, rng, tone):
    """Bags laid like bricks, each a lit pillow, in rows narrowing upward."""
    y = ground
    for r in range(rows):
        inset = r * 3.5
        x = x0 + inset + (r % 2) * 4
        while x < x1 - inset:
            w = rng.uniform(7.5, 9.5)
            col = np.array(tone) * rng.normal(1.0, 0.08)
            sc.ellipsoid((x + w / 2, y - 2.2, z + rng.normal(0, 1)),
                         (w / 2 + 0.4, 2.6, 4), col, R=rot_z(rng.normal(0, 0.06)),
                         rim=1.1, spec=0.05)
            x += w
        y -= 4.2


def trench(sc, cx, ground, rng):
    # Front is to the right (the enemy's side in the flight's direction).
    cut0, cut1 = cx - 13, cx + 13
    # Inside the cut: the far revetment going back into shadow, the
    # firestep along the front wall, duckboards on the floor.
    yy, xx = np.mgrid[0:sc.h, 0:sc.w].astype(np.float32) / SS
    inside = (xx > cut0) & (xx < cut1) & (yy > ground) & (yy < ground + 30)
    wattle = 0.6 + 0.4 * (np.sin(yy * 3.2 + np.sin(xx * 0.9) * 0.8) > 0)
    posts = (np.abs(((xx - cut0) % 9) - 4.5) < 0.8)
    alb = np.zeros((sc.h, sc.w, 3), np.float32)
    alb[:] = np.array([0.22, 0.16, 0.10])
    alb *= wattle[..., None]
    alb[posts] = np.array([0.30, 0.22, 0.14])
    sc.surface(inside, 60, alb, normal=(0, 0, -1))
    step = (xx > cx + 4) & (xx < cut1) & (yy > ground + 12) & (yy < ground + 15)
    sc.surface(step, 50, (90, 66, 42), normal=(0, -1, -0.3))
    duck = (xx > cut0 + 1) & (xx < cut1 - 1) & (yy > ground + 26) & \
        (yy < ground + 28) & (((xx * 1.4) % 1) < 0.6)
    sc.surface(duck, 50, (110, 84, 54), normal=(0, -1, -0.2))
    # Parapet on the enemy side, parados behind, both in sandbags over
    # a bank of spoil.
    sc.ellipsoid((cut1 + 22, ground + 4, 40), (26, 9, 10), (40, 32, 24),
                 rim=0.8)
    sandbags(sc, cut1 - 2, cut1 + 36, ground + 1, 5, 30, rng, (80, 70, 50))
    sc.ellipsoid((cut0 - 16, ground + 4, 40), (20, 6, 10), (40, 32, 24),
                 rim=0.8)
    sandbags(sc, cut0 - 30, cut0 + 2, ground + 1, 3, 30, rng, (70, 64, 50))
    # The wire: screw pickets and coils tangled between them.
    for k in range(9):
        px = cut1 + 46 + k * 15 + rng.normal(0, 2)
        lean = rng.normal(0, 0.12)
        top = ground - rng.uniform(13, 18)
        sc.ellipsoid((px + math.sin(lean) * (ground - top) / 2,
                      (ground + top) / 2, 20), (0.9, (ground - top) / 2, 0.9),
                     (60, 50, 40), R=rot_z(lean), rim=1.2)
    wire_pts = []
    for strand in range(4):
        x = cut1 + 40
        y = ground - 3 - strand * 3.5
        while x < cut1 + 190:
            wire_pts.append((x, y + math.sin(x * 0.7 + strand) * 1.6 +
                             rng.normal(0, 0.5)))
            x += 0.6
    for (x, y) in wire_pts:
        sc.ellipsoid((x, y, 18), (0.45, 0.45, 0.45), (50, 46, 42), rim=1.4)
    for _ in range(6):                                  # coils of concertina
        ccx = cut1 + 50 + rng.uniform(0, 130)
        r = rng.uniform(5, 8)
        for t in np.linspace(0, 6 * math.pi, 260):
            x = ccx + t * 0.9 + math.cos(t) * r * 0.35
            y = ground - r + math.sin(t) * r
            sc.ellipsoid((x, y, 16), (0.4, 0.4, 0.4), (54, 50, 46), rim=1.4)


def main():
    base = Image.open(BASE).convert("RGB")
    W, H = base.size
    g = ground_line(base)
    sc = Scene(W, H, ambient=(0.04, 0.035, 0.03))
    sc.direction((0.35, -0.75, 1), (1.3, 0.95, 0.75))    # the evening sky, behind
    sc.direction((-0.3, -0.6, -1), (0.12, 0.1, 0.1))     # a little fill
    rng = np.random.default_rng(1916)
    for cx in SITES:
        trench(sc, cx, float(g[cx]) + 1, rng)
    drawn = np.isfinite(sc.z)
    img = sc.finish(1.0)
    hit = np.asarray(Image.fromarray(drawn.astype(np.uint8) * 255).resize(
        (W, H), Image.BOX)) > 110
    out = np.asarray(base).copy()
    out[hit] = np.asarray(img)[hit]
    Image.fromarray(out).save(OUT)


if __name__ == "__main__":
    main()
