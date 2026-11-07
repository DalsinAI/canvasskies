#!/usr/bin/env python3
"""Paint the old front in the summer of 1919, for the roll call at the end.

  remember-field.bmp     a wide panorama, 1600x400, to drift across behind
                         the names: the trenches grassed over and sinking
                         into the ground, rusted pickets and loops of wire
                         half lost in the grass, shell holes standing full of
                         water and reeds, a broken tree or two, and wild
                         poppies over all of it, thickest close by, under a
                         high soft sky.  The upper part of the sky is kept
                         deep and quiet so that pale lettering reads on it.
  remember-poppies.bmp   poppies and tufts of grass for the bottom edge,
                         each in four frames swaying in the wind, on
                         magenta (255, 0, 255).
  canvas_remember.h      where each frame lies in that sheet, and the quiet
                         band of the panorama.

The ground is a height field seen in perspective from a man's eye: the
trench lines and the craters are hollows in it, softened by a year of
grass, and are lit by the afternoon sun from their slopes.  Flowers,
grass, pickets, wire, reeds and trees stand on it and are painted from
the far distance forward.  Everything is drawn here; nothing is copied.
The poppies are wild flowers seen as they grow, cupped and crumpled and
nodding on their stems, not any badge or emblem.

  python3 make_remember.py [--preview OUTDIR]

--preview also writes OUTDIR/remember-preview.webp: ten seconds of the
slow pan with the poppies swaying along the bottom.
"""

import argparse
import math
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFilter

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"
HEADER = ROOT / "src" / "modern" / "canvas_remember.h"

W, H = 1600, 400
S = 2                              # painted at twice the size, then reduced
HY = 212                           # the horizon, at 1x
FOCAL = 450.0                      # at 1x
EYE = 3.0                          # metres up, on a low rise
QUIET = (12, 118)                  # the calm band for lettering, at 1x
SUN = np.array([-0.62, 0.62, -0.48])
SUN = SUN / np.linalg.norm(SUN)
KEY = (255, 0, 255)


def fbm(n, seed, base=4, beta=2.0):
    """Noise with a falling spectrum, made in the frequency domain so that
    it wraps without a seam, 0..1."""
    rng = np.random.default_rng(seed)
    f = np.fft.fftfreq(n) * n
    r = np.sqrt(f[:, None] ** 2 + f[None, :] ** 2)
    amp = np.where(r >= base * 0.5, np.maximum(r, 1) ** (-beta / 2 - 0.5), 0.0)
    spec = (rng.standard_normal((n, n)) + 1j * rng.standard_normal((n, n))) * amp
    out = np.real(np.fft.ifft2(spec)).astype(np.float32)
    return (out - out.min()) / (out.max() - out.min())


TEX = fbm(512, 3, base=3)
TEX2 = fbm(512, 8, base=10, beta=1.6)
CLOUD = fbm(512, 21, base=2, beta=3.4)


def tex(t, u, v):
    """Bilinear sample of a wrapping texture."""
    h, w = t.shape
    u0 = np.floor(u)
    v0 = np.floor(v)
    fu, fv = u - u0, v - v0
    u0 = u0.astype(np.int64) % w
    v0 = v0.astype(np.int64) % h
    u1, v1 = (u0 + 1) % w, (v0 + 1) % h
    return (t[v0, u0] * (1 - fu) + t[v0, u1] * fu) * (1 - fv) + \
        (t[v1, u0] * (1 - fu) + t[v1, u1] * fu) * fv


# -- the lie of the land ------------------------------------------------------------

class Land:
    """The ground as heights over (x across, z away from us), in metres."""

    def __init__(self, seed=1919):
        rng = np.random.default_rng(seed)
        # Trench lines running across the view, zig-zagging by their bays.
        self.trenches = [(10.5, 1.6, 0.0), (17.5, 2.0, 1.3), (28.0, 2.6, 2.1),
                         (46.0, 3.2, 0.7), (80.0, 4.5, 2.9), (130.0, 6.0, 1.7)]
        # Shell holes, large and small; the deeper ones hold water.
        self.craters = []
        for _ in range(70):
            z = float(np.exp(rng.uniform(math.log(8.0), math.log(150))))
            x = rng.uniform(-1.0, 1.0) * z * (W / 2) / FOCAL * 1.05
            r = rng.uniform(1.0, 3.6) * (1 + z / 120)
            self.craters.append((x, z, r, rng.random() < 0.22))

    def height(self, x, z):
        h = 0.45 * (tex(TEX, x * 0.6 + 50, z * 0.6) - 0.5)          # rolling
        h += 0.08 * (tex(TEX2, x * 6, z * 6) - 0.5)                  # tussocks
        for z0, wid, ph in self.trenches:
            d = z - self._zig(x, z0, ph)
            h -= 0.85 * np.exp(-(d / (0.6 * wid)) ** 2)             # the fill-in
            h += 0.35 * np.exp(-((d + 1.4 * wid) / (0.8 * wid)) ** 2)  # parapet
            h += 0.12 * np.exp(-((d - 1.3 * wid) / (0.8 * wid)) ** 2)  # parados
        for cx, cz, r, wet in self.craters:
            d2 = ((x - cx) ** 2 + (z - cz) ** 2) / (r * r)
            h -= (0.55 if wet else 0.35) * r * 0.35 * np.exp(-d2 * 1.6)
            h += 0.10 * r * 0.35 * np.exp(-(np.sqrt(d2) - 1.1) ** 2 * 8)
        return h

    def _zig(self, x, z0, ph):
        """The line of a trench: fire bays and traverses, squared but soft,
        wandering as the ground took it."""
        bays = np.tanh(2.5 * np.sin(x / 6.0 + ph))
        return z0 * (1 + 0.10 * bays) + 1.5 * np.sin(x / 31 + ph)

    def trench_lines(self, x, z):
        """1 along the bottom of each old trench, where the grass is rank."""
        out = np.zeros_like(x)
        for z0, wid, ph in self.trenches:
            out = np.maximum(out, np.exp(-((z - self._zig(x, z0, ph)) / (0.45 * wid)) ** 2))
        return out

    def parapets(self, x, z):
        out = np.zeros_like(x)
        for z0, wid, ph in self.trenches:
            d = z - self._zig(x, z0, ph)
            out = np.maximum(out, np.exp(-((d + 1.4 * wid) / (0.6 * wid)) ** 2))
        return out

    def water(self, x, z):
        """1 inside a flooded shell hole, with its ragged edge."""
        out = np.zeros_like(x)
        for cx, cz, r, wet in self.craters:
            if not wet:
                continue
            d = np.sqrt(((x - cx) / (r * 0.62)) ** 2 + ((z - cz) / (r * 0.55)) ** 2)
            edge = 1 + 0.18 * (tex(TEX2, x * 9 + cx, z * 9) - 0.5)
            out = np.maximum(out, np.clip((edge - d) * 5 - 0.6, 0, 1))
        return out

    def poppies(self, x, z):
        """How thick the poppies grow: in drifts, thickest near, and on the
        turned soil of the old parapets."""
        d = np.clip((tex(TEX, x * 0.9 + 9, z * 1.1 + 3) - 0.40) * 3.0, 0, 1)
        d = d * (0.30 + 0.70 * np.clip(1 - (z - 4) / 60, 0, 1))
        d = d + 0.5 * self.parapets(x, z)
        d = d * (1 - self.trench_lines(x, z)) ** 2
        return np.clip(d, 0, 1)


# -- the sky ----------------------------------------------------------------------------

def sky(h, w):
    """High summer: deep and calm above, paling and warming to the
    horizon, with thin high cloud and a few soft heaps low down."""
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    t = np.clip(yy / (HY * S), 0, 1)
    top = np.array([62, 90, 132], np.float32)
    mid = np.array([120, 150, 184], np.float32)
    low = np.array([214, 216, 208], np.float32)
    k = t ** 1.6
    col = top * (1 - k[..., None]) + mid * k[..., None]
    k2 = np.clip((t - 0.55) / 0.45, 0, 1) ** 1.5
    col = col * (1 - k2[..., None]) + low * k2[..., None]
    # Thin high cloud: streaks, faint in the quiet band.
    u = xx / (w / 512.0) * 0.6
    v = yy / (h / 512.0) * 0.25
    wisp = tex(TEX, u * 1.5 + 30, v * 3.0 + 7)
    wisp = np.clip((wisp - 0.55) * 2.5, 0, 1) * (0.10 + 0.35 * np.clip((t - 0.45) / 0.3, 0, 1))
    col = col * (1 - wisp[..., None]) + np.array([214, 216, 210]) * wisp[..., None]
    # Fair-weather heaps in the lower sky, each a cluster of soft lobes
    # with a flat base, lit from the left and above; smaller and lower
    # towards the horizon, none in the band kept for the lettering.
    rng = np.random.default_rng(7)
    dens = np.zeros((h, w), np.float32)
    light = np.zeros((h, w), np.float32)
    x = rng.uniform(0, 140) * S
    while x < w:
        base_y = rng.uniform(150, 196) * S
        size = (1.0 - (base_y / S - 150) / 70) * rng.uniform(22, 42) * S
        for k in range(rng.integers(10, 22)):
            cx = x + rng.normal(0, size * 1.1)
            r = size * rng.uniform(0.25, 0.7)
            cy = base_y - r * rng.uniform(0.3, 1.1)
            y0, y1 = int(max(0, cy - r * 1.6)), int(min(h, base_y + 2))
            x0, x1 = int(max(0, cx - r * 1.6)), int(min(w, cx + r * 1.6))
            if x0 >= x1 or y0 >= y1:
                continue
            gy, gx = np.mgrid[y0:y1, x0:x1].astype(np.float32)
            d2 = ((gx - cx) ** 2 + (gy - cy) ** 2) / (r * r)
            bump = np.exp(-d2 * 1.8) * (gy < base_y)
            dens[y0:y1, x0:x1] += bump
            # Light from up and to the left: lobes brighter on that side.
            lit = np.clip(0.75 - 0.35 * ((gx - cx) - (gy - cy)) / r, 0.35, 1.15)
            light[y0:y1, x0:x1] += bump * lit
        x += size * rng.uniform(3.0, 6.0)
    edge = 1 + 0.9 * (tex(TEX2, xx / 2.2, yy / 1.6) - 0.5) + 0.5 * (tex(TEX, xx / 4.0, yy / 3.0) - 0.5)
    cover = np.clip((dens * edge - 0.30) * 1.1, 0, 1) ** 1.3
    shade = np.clip(light / np.maximum(dens, 1e-3), 0.35, 1.15)
    ccol = np.array([244, 242, 234]) * (0.70 + 0.30 * shade)[..., None]
    belly = np.clip((yy / S - 160) / 40, 0, 1) * 0.10
    ccol = ccol * (1 - belly[..., None])
    col = col * (1 - cover[..., None] * 0.88) + ccol * cover[..., None] * 0.88
    # A veil of high cloud, combed out by the wind, below the quiet band.
    veil = tex(CLOUD, xx / 9.0 + 50, yy / 1.2)
    veil = np.clip((veil - 0.5) * 3, 0, 1) * np.clip((t - 0.55) / 0.15, 0, 1) * \
        np.clip((0.9 - t) / 0.1, 0, 1) * 0.30
    col = col * (1 - veil[..., None]) + np.array([228, 230, 226]) * veil[..., None]
    return col


# -- the ground, pixel by pixel ----------------------------------------------------

GRASS = np.array([88, 104, 58], np.float32)
DRY = np.array([150, 142, 98], np.float32)
LUSH = np.array([60, 84, 46], np.float32)
CHALK = np.array([176, 170, 150], np.float32)
HAZE = np.array([176, 184, 182], np.float32)
RED = np.array([186, 30, 22], np.float32)


def ground(land, h, w, rng):
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float64)
    below = yy > HY * S + 0.5
    dy = np.maximum(yy - HY * S, 0.5)
    z = EYE * FOCAL * S / dy
    x = (xx - w / 2) * z / (FOCAL * S)
    g = below
    X, Z = x[g], z[g]
    e = 0.04 + Z * 0.004
    hh = land.height(X, Z)
    hx = (land.height(X + e, Z) - hh) / e
    hz = (land.height(X, Z + e) - hh) / e
    n = np.stack([-hx, np.ones_like(hx), -hz], -1)
    n /= np.linalg.norm(n, axis=-1, keepdims=True)
    lam = np.clip(n @ SUN, 0, 1)
    # The grass: summer green with drier patches, lusher in the hollows.
    dry = np.clip((tex(TEX, X * 0.35 + 70, Z * 0.35) - 0.5) * 2.2, 0, 1)
    col = GRASS * (1 - dry[:, None] * 0.6) + DRY * (dry[:, None] * 0.6)
    hollow = np.clip(-hh * 1.6, 0, 1)
    col = col * (1 - hollow[:, None]) + LUSH * hollow[:, None]
    tr = land.trench_lines(X, Z) * (0.85 + 0.3 * tex(TEX2, X * 2, Z * 2))
    col = col * (1 - 0.75 * tr[:, None]) + LUSH * 0.62 * (0.75 * tr[:, None])
    par = land.parapets(X, Z) * (0.7 + 0.5 * tex(TEX2, X * 1.5 + 7, Z * 2))
    col = col * (1 - 0.5 * par[:, None]) + DRY * 1.15 * (0.5 * par[:, None])
    # A little chalk still showing on the old parapets.
    chalk = np.clip((hh - 0.12) * 5, 0, 1) * np.clip((tex(TEX2, X * 3, Z * 3) - 0.62) * 5, 0, 1)
    col = col * (1 - chalk[:, None] * 0.5) + CHALK * chalk[:, None] * 0.5
    # Blades and seed heads as fine texture, coarse near and fine far.
    fine = tex(TEX2, X * 30, Z * 30)
    col = col * (0.85 + 0.30 * fine)[:, None]
    shade = 0.30 + 0.95 * lam
    col = col * shade[:, None]
    # Standing water in the shell holes, showing the sky.
    wet = land.water(X, Z)
    if wet.any():
        skyc = np.array([104, 124, 140], np.float32) * (0.75 + 0.35 * tex(TEX2, X * 2, Z * 10))[:, None]
        rim = np.clip(wet * 2.2, 0, 1) - wet
        col = col * (1 - rim[:, None] * 0.8) + np.array([70, 62, 44]) * rim[:, None] * 0.8
        col = col * (1 - wet[:, None]) + skyc * wet[:, None]
    # The far poppies: a red that thickens and thins with the drifts.
    pop = land.poppies(X, Z)
    far = np.clip((Z - 30) / 30, 0, 1)
    speck = rng.random(len(X)) < pop * 0.22 * far
    col = np.where(speck[:, None], col * 0.25 + RED * 0.75, col)
    tint = pop * np.clip((Z - 40) / 40, 0, 1) * 0.30
    col = col * (1 - tint[:, None]) + (RED * 0.8 + col * 0.2) * tint[:, None]
    col = np.where((wet > 0.5)[:, None], col, col)
    # Haze into the distance.
    f = 1 - np.exp(-Z / 260.0)
    col = col * (1 - f[:, None]) + HAZE * f[:, None]
    out = np.zeros((h, w, 3), np.float32)
    out[g] = col
    zz = np.full((h, w), np.inf)
    zz[g] = Z
    return out, below, zz


# -- things that stand up out of the grass ------------------------------------------

def project(x, z, y=0.0):
    """World (x, height y, z) to the 2x canvas."""
    sx = W * S / 2 + x * FOCAL * S / z
    sy = HY * S + (EYE - y) * FOCAL * S / z
    return sx, sy


def hazed(c, z):
    f = 1 - math.exp(-z / 260.0)
    return tuple(int(v * (1 - f) + hz * f) for v, hz in zip(c, HAZE))


def poppy(d, x, y, r, rng, z, open_=1.0):
    """One wild poppy head seen from the side or a little above: four
    crumpled petals in a cup, darker in their hearts, the cup tilted."""
    tilt = rng.normal(0, 0.35)
    base = np.array([186, 32, 24]) * rng.uniform(0.85, 1.12)
    dark = tuple(int(v) for v in hazed(base * 0.55, z))
    lit = tuple(int(v) for v in hazed(np.minimum(base * 1.25, 255), z))
    mid = tuple(int(v) for v in hazed(base, z))
    if open_ < 0.5:                  # a bud, nodding
        d.ellipse((x - r * 0.45, y - r * 0.7, x + r * 0.45, y + r * 0.5), fill=hazed((70, 96, 52), z))
        return
    # Back petals, then the front ones, each a ragged ellipse.
    for k, (ox, oy, sx, sy, c) in enumerate(((-0.45, -0.35, 0.7, 0.75, dark),
                                              (0.45, -0.35, 0.7, 0.75, dark),
                                              (-0.35, 0.05, 0.75, 0.7, mid),
                                              (0.35, 0.05, 0.75, 0.7, lit))):
        ca, sa = math.cos(tilt), math.sin(tilt)
        px = x + (ox * ca - oy * sa) * r
        py = y + (ox * sa + oy * ca) * r
        d.ellipse((px - sx * r, py - sy * r * 0.9, px + sx * r, py + sy * r), fill=c)
    if r > 2.2 and rng.random() < 0.7:
        d.ellipse((x - r * 0.22, y - r * 0.55, x + r * 0.22, y - r * 0.25), fill=hazed((26, 22, 20), z))


def blade(d, x, y, length, lean, col, width=1):
    """A blade or a stalk of grass, curving as it leans."""
    pts = []
    for k in range(5):
        t = k / 4
        pts.append((x + lean * length * t * t, y - length * t))
    d.line(pts, fill=col, width=width)


def picket(d, x, z, rng):
    """A corkscrew picket, rusted, leaning, its foot lost in the grass."""
    hgt = rng.uniform(0.7, 1.25)
    lean = rng.normal(0, 0.18)
    x0, y0 = project(x, z, 0.0)
    x1, y1 = project(x + lean * hgt, z, hgt)
    w_ = max(2, int(0.06 * FOCAL * S / z))
    rust = hazed((78 + rng.integers(-10, 10), 46, 30), z)
    d.line((x0, y0, x1, y1), fill=rust, width=w_)
    # The eyes of the corkscrew.
    for t in (0.35, 0.6, 0.85):
        ex = x0 + (x1 - x0) * t
        ey = y0 + (y1 - y0) * t
        rr = w_ * 1.6
        d.ellipse((ex - rr, ey - rr, ex + rr, ey + rr), outline=rust, width=max(1, w_ // 2))
    return (x + lean * hgt * 0.7, hgt * 0.7)


def wire(d, a, b, z, rng):
    """Barbed wire sagging between two pickets, in loose loops."""
    (xa, ya), (xb, yb) = a, b
    col = hazed((58, 42, 32), z)
    pts = []
    n = 40
    loops = rng.integers(2, 5)
    for k in range(n + 1):
        t = k / n
        x = xa + (xb - xa) * t
        y = ya + (yb - ya) * t - 0.35 * math.sin(math.pi * t) * (0.6 + 0.4 * rng.random()) + 0.25
        x += 0.12 * math.sin(t * loops * 2 * math.pi)
        y += 0.10 * math.cos(t * loops * 2 * math.pi)
        pts.append(project(x, z + 0.3 * math.sin(t * 7), max(0.0, y)))
    d.line(pts, fill=col, width=max(2, int(0.018 * FOCAL * S / z)))
    for k in range(2, n, 3):                    # the barbs
        px, py = pts[k]
        s = max(1, int(0.02 * FOCAL * S / z))
        d.line((px - s, py - s, px + s, py + s), fill=col, width=1)


def broken_tree(d, x, z, rng, height):
    """A tree shattered by the shelling: a bare trunk with a splintered top
    and a stub or two of limb, a few new shoots of leaf this summer."""
    bark = hazed((74, 68, 60), z)
    dark = hazed((46, 42, 38), z)
    lean = rng.normal(0, 0.08)
    top = (x + lean * height, height)
    s = FOCAL * S / z
    wb, wt = 0.45 * s, 0.22 * s
    bx, by = project(x, z, 0)
    tx, ty = project(*top, z) if False else project(top[0], z, top[1])
    poly = [(bx - wb, by), (tx - wt, ty), (tx - wt * 0.3, ty - 0.5 * s),
            (tx + wt * 0.2, ty - 0.15 * s), (tx + wt * 0.6, ty - 0.8 * s),
            (tx + wt, ty), (bx + wb, by)]
    d.polygon(poly, fill=bark)
    d.line([(bx + wb * 0.5, by), (tx + wt * 0.5, ty)], fill=dark, width=max(1, int(wt * 0.4)))
    for k in range(rng.integers(2, 4)):
        t = rng.uniform(0.45, 0.85)
        sx_ = bx + (tx - bx) * t
        sy_ = by + (ty - by) * t
        side = rng.choice([-1, 1])
        ln = rng.uniform(0.8, 1.8) * s
        ex, ey = sx_ + side * ln, sy_ - ln * rng.uniform(0.4, 0.9)
        d.line((sx_, sy_, ex, ey), fill=bark, width=max(1, int(wt * 0.5)))
        if rng.random() < 0.6:                 # new leaf at the stub
            for j in range(14):
                lx = ex + rng.normal(0, 0.2 * s)
                ly = ey + rng.normal(0, 0.15 * s)
                rr = 0.06 * s
                d.ellipse((lx - rr, ly - rr, lx + rr, ly + rr),
                          fill=hazed((66 + rng.integers(0, 20), 84 + rng.integers(0, 20), 48), z))


def reeds(d, cx, cz, r, rng):
    """Reeds and rushes round the edge of a flooded shell hole."""
    for k in range(int(40 + r * 20)):
        a = rng.uniform(0, 2 * math.pi)
        x = cx + math.cos(a) * r * 0.62 * rng.uniform(0.85, 1.1)
        z = cz + math.sin(a) * r * 0.55 * rng.uniform(0.85, 1.1)
        if z < 6.5:
            continue
        x0, y0 = project(x, z, 0)
        ln = rng.uniform(0.4, 0.9) * FOCAL * S / z
        col = hazed((96 + rng.integers(0, 40), 112 + rng.integers(0, 30), 60), z)
        blade(d, x0, y0, ln, rng.normal(0, 0.15), col, width=max(1, int(0.012 * FOCAL * S / z)))
        if rng.random() < 0.12:                 # a bulrush head
            d.ellipse((x0 - 1.5, y0 - ln - 4, x0 + 1.5, y0 - ln + 2), fill=hazed((70, 48, 30), z))


def stand_up(img, land, rng):
    """Everything upright, painted from the far distance forward."""
    d = ImageDraw.Draw(img)
    items = []
    half = (W / 2) / FOCAL * 1.08
    # Distant ruins of trees on the skyline, and a few nearer.
    for x, z, h_ in ((-60, 150, 9), (-48, 158, 7), (90, 140, 11), (210, 200, 8),
                     (-150, 210, 10), (-200, 175, 6), (12, 52, 6.5), (-20, 31, 5.0)):
        items.append((z, "tree", (x, h_)))
    # Pickets and wire, in broken runs just beyond the trenches.
    for z0, wid, ph in land.trenches[:4]:
        zr = z0 - 2.2 * wid
        x = -half * zr
        while x < half * zr:
            run = rng.integers(2, 5)
            if rng.random() < 0.5:
                pts = [(x + k * rng.uniform(2.2, 3.4), zr + rng.normal(0, 0.4)) for k in range(run)]
                items.append((zr, "wire", pts))
                x += run * 3.0
            x += rng.uniform(4, 14)
    items.append((9.0, "wire", [(-13.5, 9.0), (-10.8, 9.3), (-8.1, 8.9)]))
    items.append((9.6, "wire", [(9.5, 9.8), (12.4, 9.5)]))
    for cx, cz, r, wet in land.craters:
        if wet:
            items.append((cz - r * 0.3, "reeds", (cx, cz, r)))
    # Flowers and grass, nearer and nearer.
    for z in np.arange(60.0, EYE * FOCAL / (H - HY) - 0.6, -0.06):
        width = half * z * 2
        area = width * 0.06
        n_pop = rng.poisson(area * (16.0 if z < 12 else 8.0 if z < 25 else 3.0))
        xs = rng.uniform(-half * z, half * z, n_pop)
        dens = land.poppies(xs, np.full(n_pop, z))
        for x in xs[rng.random(n_pop) < dens]:
            items.append((z + rng.uniform(0, 0.08), "poppy", (x,)))
        if z < 18:
            n_gr = rng.poisson(area * (16 if z < 5 else 10 if z < 9 else 4))
            xs = rng.uniform(-half * z, half * z, n_gr)
            bare = land.trench_lines(xs, np.full(n_gr, z))
            for x in xs[rng.random(n_gr) > bare * 0.8]:
                items.append((z + rng.uniform(0, 0.08), "grass", (x,)))
    items.sort(key=lambda it: -it[0])
    for z, kind, a in items:
        s = FOCAL * S / z
        if kind == "tree":
            broken_tree(d, a[0], z, rng, a[1])
        elif kind == "wire":
            tops = [picket(d, x, zz, rng) for x, zz in a]
            for (xa, ya), (xb, yb) in zip(tops[:-1], tops[1:]):
                wire(d, (xa, ya), (xb, yb), z, rng)
        elif kind == "reeds":
            reeds(d, *a, rng)
        elif kind == "poppy":
            x = a[0]
            hgt = rng.uniform(0.25, 0.6)
            sway = rng.normal(0, 0.06)
            x0, y0 = project(x, z, 0)
            x1, y1 = project(x + sway, z, hgt)
            if z < 25:
                d.line((x0, y0, x1, y1), fill=hazed((76, 98, 52), z), width=max(1, int(0.006 * s)))
                if rng.random() < 0.3:          # a leaf, deeply cut
                    lx, ly = project(x + 0.05, z, hgt * 0.25)
                    d.line((x0, y0 - (y0 - y1) * 0.1, lx + 0.06 * s, ly), fill=hazed((70, 96, 50), z),
                           width=max(1, int(0.01 * s)))
            r = max(0.8, 0.046 * s * rng.uniform(0.8, 1.2))
            poppy(d, x1, y1, r, rng, z, open_=1.0 if rng.random() > 0.12 else 0.0)
        elif kind == "grass":
            x = a[0]
            x0, y0 = project(x, z, 0)
            for j in range(rng.integers(2, 6)):
                ln = rng.uniform(0.06, 0.26) * s
                g = rng.uniform(0.75, 1.25)
                col = (int(96 * g), int(112 * g), int(60 * g)) if rng.random() > 0.3 else \
                    (int(156 * g), int(146 * g), int(96 * g))
                blade(d, x0 + rng.normal(0, 0.03 * s), y0, ln, rng.normal(0.1, 0.35), hazed(col, z),
                      width=max(1, int(0.006 * s)))


def grade(img, keep_key=False):
    """A gentle muting of the colour, so the summer is remembered rather
    than shouted; the key colour is left alone."""
    a = np.asarray(img, np.float32)
    lum = (a @ np.array([0.299, 0.587, 0.114]))[..., None]
    out = a * 0.84 + lum * 0.16
    out = out * np.array([1.0, 0.99, 0.96])
    if keep_key:
        key = (a == KEY).all(-1)
        out[key] = KEY
    return Image.fromarray(np.clip(out + 0.5, 0, 255).astype(np.uint8))


def panorama():
    rng = np.random.default_rng(1919)
    land = Land()
    h, w = H * S, W * S
    col = sky(h, w)
    g, below, _ = ground(land, h, w, rng)
    col = np.where(below[..., None], g, col)
    # The far skyline: a long low rise with a copse or two, blue with haze.
    xx = np.arange(w)
    ridge = HY * S - (6 + 10 * tex(TEX, xx / w * 60.0, np.full(w, 11.0)) +
                      6 * np.clip((tex(TEX2, xx / w * 220.0, np.full(w, 3.0)) - 0.55) * 4, 0, 1))
    yy = np.arange(h)[:, None]
    rid = (yy > ridge[None, :]) & (yy <= HY * S + 1)
    copse = np.clip((tex(TEX, xx / w * 140.0, np.full(w, 77.0)) - 0.6) * 5, 0, 1)
    rcol = np.array([142, 156, 150]) * (1 - 0.12 * copse)[None, :, None]
    col = np.where(rid[..., None], rcol + 0 * col, col)
    img = Image.fromarray(np.clip(col, 0, 255).astype(np.uint8))
    stand_up(img, land, rng)
    img = img.filter(ImageFilter.GaussianBlur(0.6))
    return grade(img.resize((W, H), Image.LANCZOS))


# -- the sway sheet ----------------------------------------------------------------------

VARIANTS = [
    # name, frame w, h, kind
    ("tall poppy", 34, 40, "poppy_tall"),
    ("poppy pair", 34, 32, "poppy_pair"),
    ("poppy and bud", 30, 28, "poppy_bud"),
    ("small poppy", 22, 20, "poppy_small"),
    ("grass tuft", 32, 36, "tuft_tall"),
    ("low tuft", 26, 18, "tuft_low"),
]
SWAY = (-1.0, -0.35, 0.35, 1.0)        # the four frames; play 0 1 2 3 2 1


def sway_frame(kind, w, h, lean, seed):
    """One frame of a plant bending in the wind, drawn at four times the
    size with soft edges, then reduced; the edge is settled onto the
    dark green of the grass so that no fringe shows round the key."""
    U = 4
    im = Image.new("RGBA", (w * U, h * U), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    rng = np.random.default_rng(seed)
    foot = (w * U / 2, h * U - 1)

    def bend(px, height, t):
        """A point on a stalk of 'height' (px) at fraction t, bent by lean."""
        return (px + lean * height * 0.35 * t * t, foot[1] - height * t)

    def stalk(px, height, col, width):
        pts = [bend(px, height, k / 8) for k in range(9)]
        d.line(pts, fill=col, width=width)
        return pts[-1]

    def flower(x, y, r):
        dark = (120, 18, 14, 255)
        mid = (178, 30, 22, 255)
        lit = (214, 52, 36, 255)
        tilt = lean * 0.4
        for ox, oy, c in ((-0.45, -0.3, dark), (0.45, -0.3, dark), (-0.32, 0.08, mid), (0.36, 0.08, lit)):
            ca, sa = math.cos(tilt), math.sin(tilt)
            px = x + (ox * ca - oy * sa) * r
            py = y + (ox * sa + oy * ca) * r
            d.ellipse((px - 0.72 * r, py - 0.62 * r, px + 0.72 * r, py + 0.7 * r), fill=c)
        d.ellipse((x - 0.2 * r, y - 0.5 * r, x + 0.2 * r, y - 0.22 * r), fill=(28, 24, 20, 255))

    green = (74, 98, 50, 255)
    pale = (150, 142, 94, 255)
    if kind.startswith("poppy"):
        heads = {"poppy_tall": [(0.0, 0.92, 0.17)],
                 "poppy_pair": [(-0.13, 0.88, 0.16), (0.15, 0.66, 0.15)],
                 "poppy_bud": [(-0.1, 0.85, 0.18), (0.22, 0.7, -1)],
                 "poppy_small": [(0.0, 0.85, 0.22)]}[kind]
        # Grass round the feet first.
        for k in range(5):
            px = foot[0] + rng.normal(0, w * U * 0.15)
            stalk(px, h * U * rng.uniform(0.15, 0.35), green if k % 2 else pale, U)
        for ox, ht, r in heads:
            top = stalk(foot[0] + ox * w * U, h * U * ht * (0.86 if r < 0 else 0.84), green, max(2, U // 2 + 1))
            if r < 0:                       # a bud, nodding on its crook
                bx, by = top
                d.ellipse((bx - 0.07 * w * U, by - 0.02 * h * U, bx + 0.07 * w * U, by + 0.09 * h * U), fill=green)
            else:
                flower(top[0], top[1], r * w * U * 0.85)
    else:
        n = 14 if kind == "tuft_tall" else 10
        for k in range(n):
            px = foot[0] + float(np.clip(rng.normal(0, w * U * 0.10), -w * U * 0.2, w * U * 0.2))
            ht = h * U * rng.uniform(0.45, 0.90)
            col = green if rng.random() > 0.35 else pale
            spread = float(np.clip(rng.normal(0, 0.4), -0.6, 0.6))
            pts = []
            for j in range(9):
                t = j / 8
                pts.append((px + (lean * 0.35 + spread * 0.3) * ht * t * t, foot[1] - ht * t))
            d.line(pts, fill=col, width=U)
            if kind == "tuft_tall" and rng.random() < 0.3:      # a seed head
                x, y = pts[-1]
                d.ellipse((x - U, y - 3 * U, x + U, y + U), fill=pale)
    small = im.resize((w, h), Image.LANCZOS)
    a = np.asarray(small, np.float32)
    rgb = a[..., :3]
    alpha = a[..., 3:] / 255.0
    behind = np.array([58, 74, 40], np.float32)
    # Undo the premultiplication against nothing, settle on the grass.
    with np.errstate(invalid="ignore", divide="ignore"):
        rgb = np.where(alpha > 0, rgb, 0)
    out = rgb * np.minimum(alpha * 1.6, 1) + behind * (1 - np.minimum(alpha * 1.6, 1))
    keep = alpha[..., 0] > 0.4
    res = np.zeros((h, w, 3), np.uint8)
    res[:] = KEY
    res[keep] = np.clip(out[keep], 0, 255).astype(np.uint8)
    return Image.fromarray(res)


def sway_sheet():
    width = max(w for _, w, _, _ in VARIANTS) * 4 + 3
    height = sum(h for _, _, h, _ in VARIANTS) + len(VARIANTS) - 1
    sheet = Image.new("RGB", (width, height), KEY)
    rects = []
    y = 0
    for v, (name, w, h, kind) in enumerate(VARIANTS):
        row = []
        for f, lean in enumerate(SWAY):
            x = f * (w + 1)
            sheet.paste(sway_frame(kind, w, h, lean * 0.7, seed=v * 7 + 1), (x, y))
            row.append((x, y, w, h))
        rects.append((name, row))
        y += h + 1
    return grade(sheet, keep_key=True), rects


def header(rects):
    lines = ["/* Generated by ports/amigaos3/make_remember.py: do not edit. */",
             "#ifndef CANVAS_REMEMBER_H", "#define CANVAS_REMEMBER_H", "",
             f"#define REMEMBER_POPPY_VARIANTS {len(rects)}   /* kinds of poppy or tuft */",
             f"#define REMEMBER_SWAY_FRAMES {len(SWAY)}",
             "/* per variant, per sway frame: x, y, w, h in remember-poppies.bmp; the",
             " * foot of each frame is its bottom edge.  The frames lean from one",
             " * side to the other: play them 0 1 2 3 2 1 for a to-and-fro. */",
             "static const short remember_poppy_frames[REMEMBER_POPPY_VARIANTS][REMEMBER_SWAY_FRAMES][4] = {"]
    for name, row in rects:
        lines.append("    {" + ", ".join("{%d, %d, %d, %d}" % r for r in row) + "}, /* " + name + " */")
    lines += ["};",
              f"#define REMEMBER_FIELD_QUIET_TOP {QUIET[0]}    /* the calm band for lettering */",
              f"#define REMEMBER_FIELD_QUIET_BOTTOM {QUIET[1]}", "", "#endif", ""]
    HEADER.write_text("\n".join(lines))


# -- the preview ---------------------------------------------------------------------------

def preview(field, sheet, rects, out):
    """Ten seconds of the drift across the field with the poppies along
    the bottom edge swaying, each on its own beat."""
    fps = 20
    frames = []
    rng = np.random.default_rng(5)
    plants = []
    x = -6
    while x < 640:
        v = int(rng.integers(len(rects)))
        plants.append((x, v, rng.uniform(0, 6), rng.uniform(0.8, 1.3)))
        x += int(rng.integers(8, 22))
    order = (0, 1, 2, 3, 2, 1)
    for i in range(10 * fps):
        t = i / fps
        off = int(round(t * 24))                 # 24 pixels a second
        im = field.crop((off, 0, off + 640, 400)).copy()
        gust = 0.5 + 0.5 * math.sin(t * 0.9) * math.sin(t * 0.37 + 1)
        for px, v, ph, rate in plants:
            k = order[int((t * (2.0 + 2.5 * gust) * rate + ph) * 1) % 6]
            fx, fy, fw, fh = rects[v][1][k]
            spr = sheet.crop((fx, fy, fx + fw, fy + fh))
            mask = Image.fromarray(((np.asarray(spr) != KEY).any(-1) * 255).astype(np.uint8))
            im.paste(spr, (px, 400 - fh), mask)
        frames.append(im)
    frames[0].save(out / "remember-preview.webp", save_all=True, append_images=frames[1:],
                   duration=1000 // fps, loop=0, quality=80, method=4)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--preview")
    args = ap.parse_args()
    field = panorama()
    field.save(ASSETS / "remember-field.bmp")
    sheet, rects = sway_sheet()
    sheet.save(ASSETS / "remember-poppies.bmp")
    header(rects)
    if args.preview:
        out = Path(args.preview)
        out.mkdir(parents=True, exist_ok=True)
        field.save(out / "remember-field.png")
        sheet.resize((sheet.width * 4, sheet.height * 4), Image.NEAREST).save(out / "remember-poppies-x4.png")
        preview(field, sheet, rects, out)


if __name__ == "__main__":
    main()
