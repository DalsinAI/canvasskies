#!/usr/bin/env python3
"""Paint the old front in the summer of 1919, for the roll call at the end.

A late-summer evening a year after the guns stopped.  The sun is low and
golden on the left; the old trench lines run across the field as soft
green banks with slumped parapets, their fire bays and traverses still
plain in the long shadows; the shell holes have grassed rims, and the
deep ones stand full of still water with reeds, holding the sky.  Rusted
corkscrew pickets lean among the grass with their wire sagging into it, a
duckboard rots by a trench, a stake still carries a rag of cloth, and in
the foreground a helmet and an old shell case lie grown over.  Poppies
come in drifts and swathes, dense at our feet and thinning to a red haze
far off, with cornflowers, ox-eye daisies and chamomile among them and
the grasses in seed.  Beyond: a line of shattered trees putting out new
leaf, poplars along a road, the broken tower of a village church, a
wayside cross on a rise, and the land rolling away into the haze under
layered cumulus and high cirrus.

The picture is made in four layers for the game to slide at different
speeds as it pans, and two small sprite sheets:

  remember-sky.bmp       the sky, opaque
  remember-far.bmp       the distant land, keyed magenta (255, 0, 255)
  remember-mid.bmp       the field, keyed
  remember-near.bmp      the flowers at our feet, keyed
  remember-poppies.bmp   single plants swaying, four frames each, keyed
  remember-life.bmp      butterflies, a lark and thistledown, keyed
  remember-field.bmp     the layers flattened into one, for the present
  canvas_remember.h      the layers' sizes and places, the frames of both
                         sprite sheets, and the quiet band for lettering

The ground is drawn as a landscape renderer draws it: each column of the
picture is walked from near to far over a height field, so hollows hide
behind their banks and a rise hides what lies beyond it.  The sun comes
from straight across the field, so a point is in shadow when anything to
its left along the same row stands higher than the sun's line, which a
running maximum finds at once.  Everything upright (flowers, grass,
pickets, trees, ruins) is painted as shaded shapes from the far distance
forward, lit on the side towards the sun and edged with its light, the
poppy petals glowing at their rims where it shines through them.  It is
all painted at three times the size, reduced, and dithered to the 16-bit
colours the game shows.  Nothing is copied from anywhere; the poppies are
wild flowers as they grow, nothing like any emblem.

  python3 make_remember.py [--preview OUTDIR]
"""

import argparse
import math
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFilter

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"
HEADER = ROOT / "src" / "modern" / "canvas_remember.h"

S = 3                              # painted at three times the size
H = 400
HY = 214                           # the horizon, at 1x
FOCAL = 450.0                      # at 1x
QUIET = (12, 118)                  # the calm band for lettering, at 1x
KEY = (255, 0, 255)
SUN_ELEV = math.radians(7.5)
TAN = math.tan(SUN_ELEV)
SUN_COL = np.array([1.00, 0.80, 0.56])
AMB_COL = np.array([0.40, 0.48, 0.64])
LIGHT2D = np.array([-0.93, -0.36])           # towards the sun, on the screen
LAYERS = {"sky": 1000, "far": 1300, "mid": 1600, "near": 2000}
SPEEDS = {"sky": 0.15, "far": 0.35, "mid": 0.6, "near": 1.0}


# -- noise -------------------------------------------------------------------------------

def spectral(n, seed, base=4, beta=2.0, aniso=1.0):
    """Noise with a falling spectrum, made in the frequency domain so that it
    wraps without a seam, 0..1.  aniso > 1 stretches it across."""
    rng = np.random.default_rng(seed)
    f = np.fft.fftfreq(n) * n
    r = np.sqrt((f[None, :] / aniso) ** 2 + (f[:, None]) ** 2)
    amp = np.where(r >= base * 0.5, np.maximum(r, 1) ** (-beta / 2 - 0.5), 0.0)
    spec = (rng.standard_normal((n, n)) + 1j * rng.standard_normal((n, n))) * amp
    out = np.real(np.fft.ifft2(spec)).astype(np.float32)
    return (out - out.min()) / (out.max() - out.min())


N1 = spectral(512, 3, base=3)
N2 = spectral(512, 8, base=10, beta=1.6)
N3 = spectral(512, 21, base=2, beta=3.2)
STREAK = spectral(512, 33, base=3, beta=2.6, aniso=6.0)


def tex(t, u, v):
    """Bilinear sample of a wrapping texture."""
    h, w = t.shape
    u = np.asarray(u, np.float64)
    v = np.asarray(v, np.float64)
    u0 = np.floor(u)
    v0 = np.floor(v)
    fu, fv = (u - u0).astype(np.float32), (v - v0).astype(np.float32)
    u0 = u0.astype(np.int64) % w
    v0 = v0.astype(np.int64) % h
    u1, v1 = (u0 + 1) % w, (v0 + 1) % h
    return (t[v0, u0] * (1 - fu) + t[v0, u1] * fu) * (1 - fv) + \
        (t[v1, u0] * (1 - fu) + t[v1, u1] * fu) * fv


def smoothstep(a, b, x):
    t = np.clip((x - a) / (b - a), 0, 1)
    return t * t * (3 - 2 * t)


def mix(a, b, t):
    t = np.asarray(t, np.float64)
    return np.asarray(a) * (1 - t[..., None]) + np.asarray(b) * t[..., None]


# -- the sky ------------------------------------------------------------------------------

SKY_STOPS = [(0, (38, 58, 94)), (60, (52, 76, 114)), (118, (88, 114, 150)),
             (160, (140, 152, 168)), (192, (198, 182, 160)), (214, (236, 204, 156))]


def sky_base(xf, y):
    """The sky's colour by height (1x), warmer and brighter towards the sun
    on the left; xf is the place across the scene, 0 at the sun's side."""
    ys = np.array([s[0] for s in SKY_STOPS], np.float64)
    cs = np.array([s[1] for s in SKY_STOPS], np.float64)
    yc = np.clip(y, 0, 214)
    col = np.stack([np.interp(yc, ys, cs[:, k]) for k in range(3)], -1)
    glow = np.exp(-((xf + 0.15) ** 2) / 0.25) * np.exp(-((214 - yc) / 70.0) ** 2)
    col = col + np.array([70, 44, 8]) * glow[..., None]
    halo = np.exp(-((xf + 0.1) ** 2) / 0.06) * np.exp(-((214 - yc) / 30.0) ** 2)
    return col + np.array([40, 30, 14]) * halo[..., None]


def cumulus(w, h, rng):
    """Layered cumulus, painted as a painter builds it: many rounded puffs,
    each lit on its sunward side and shaded beneath, heaped up from a flat
    grey base into sunlit crowns, the far ones small, low and pale in the
    haze.  Returns colour and cover at the canvas size."""
    col = np.zeros((h, w, 3), np.float32)
    cover = np.zeros((h, w), np.float32)
    L = np.array([-0.78, -0.42, 0.46])
    L /= np.linalg.norm(L)
    clouds = []
    x = rng.uniform(-60, 40)
    while x < w / S + 80:                             # a higher, thinner tier
        clouds.append((x, rng.uniform(150, 162), rng.uniform(60, 140), rng.uniform(6, 12), 0.55))
        x += rng.uniform(90, 220)
    x = rng.uniform(-60, 40)
    while x < w / S + 80:                             # the main heaps
        base = rng.uniform(172, 192)
        width = rng.uniform(30, 160)
        tall = rng.uniform(0.25, 0.55) * width * (1.15 - (base - 172) / 60)
        tall = min(tall, base - 128)
        clouds.append((x, base, width, tall, 1.0))
        x += width * rng.uniform(0.7, 1.6)
    x = rng.uniform(0, 40)
    while x < w / S:                                   # far, low and small
        clouds.append((x, rng.uniform(200, 207), rng.uniform(12, 30), rng.uniform(4, 8), 0.75))
        x += rng.uniform(30, 80)
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    fringe = tex(N2, xx / (S * 0.45), yy / (S * 0.45))
    for cx, base, width, tall, strength in clouds:
        puffs = []
        n = int(20 + width * 0.9)
        for k in range(n):
            u = rng.uniform(-1, 1)
            dome = (1 - u * u) ** 0.7
            top = base - tall * dome * rng.uniform(0.35, 1.0)
            r = rng.uniform(0.06, 0.18) * (width * 0.45 + tall * 0.8) * (0.55 + 0.6 * dome)
            px = cx + u * width * 0.5
            py = min(base - r * 0.55, top + r * 0.7)
            py = max(py, 123 + r)                     # crowns stay below the band
            puffs.append((rng.uniform(0, 1) + (py / base) * 0.0, px, py, r))
        puffs.sort(key=lambda q: -q[2])               # high and far first, low in front
        for _, px, py, r in puffs:
            x0, x1 = int(max(0, (px - r * 1.3) * S)), int(min(w, (px + r * 1.3) * S))
            y0, y1 = int(max(0, (py - r * 1.3) * S)), int(min(h, (base + 1) * S))
            if x0 >= x1 or y0 >= y1:
                continue
            gx = (xx[y0:y1, x0:x1] / S - px) / r
            gy = (yy[y0:y1, x0:x1] / S - py) / r
            d2 = gx * gx + gy * gy
            fr = fringe[y0:y1, x0:x1]
            a = np.clip((1.0 + 0.45 * (fr - 0.5) - np.sqrt(d2)) * 1.6, 0, 1) ** 1.2
            a *= smoothstep(base + 0.5, base - 2.5, yy[y0:y1, x0:x1] / S)     # flat base
            if not a.any():
                continue
            nz = np.sqrt(np.clip(1 - d2, 0, 1))
            lam = np.clip(gx * L[0] + gy * L[1] + nz * L[2], 0, 1)
            low = smoothstep(base - tall * 0.5, base, yy[y0:y1, x0:x1] / S)
            sun = np.array([255, 234, 200]) * (0.70 + 0.32 * lam[..., None])
            shade = mix([164, 160, 176], [118, 114, 132], low * 0.8)
            c = mix(shade, sun, smoothstep(0.05, 0.95, lam) * (1 - 0.5 * low))
            # Thin sunward edges catch the light.
            rim = np.clip((np.sqrt(d2) - 0.75) * 4, 0, 1) * np.clip(-gx * 0.9 - gy * 0.4, 0, 1)
            c = c + np.array([30, 22, 6]) * rim[..., None]
            # Where a puff thins at its edge it is lit through, not dark.
            thin = np.clip(1 - a, 0, 1) ** 0.7
            c = mix(c, [226, 220, 216], thin * 0.85)
            a = a * strength
            reg = (slice(y0, y1), slice(x0, x1))
            col[reg] = c * a[..., None] + col[reg] * (1 - a[..., None])
            cover[reg] = a + cover[reg] * (1 - a)
    # The puffs were laid over nothing: take the colour back out of the
    # coverage so the edges are not darkened twice.
    col = col / np.maximum(cover, 1e-4)[..., None]
    return col, cover


def cirrus(w, h, rng):
    """Mares' tails, combed out high up: thin curving strands, warm where
    the low sun catches them, all but gone in the band for the lettering."""
    im = Image.new("L", (w, h), 0)
    d = ImageDraw.Draw(im)
    for k in range(70):
        x = rng.uniform(-200, w / S + 50)
        y = rng.uniform(70, 150)
        ln = rng.uniform(40, 140)
        curl = rng.uniform(-0.25, 0.25)
        pts = []
        for j in range(16):
            t = j / 15
            pts.append(((x + ln * t) * S, (y + curl * ln * t * t + 6 * t) * S))
        a = int(rng.uniform(40, 110) * (0.25 if y < QUIET[1] else 1.0))
        d.line(pts, fill=a, width=int(rng.uniform(1, 3) * S))
        for j in range(4):                            # a hooked tail
            t = rng.uniform(0.2, 1)
            px_, py_ = pts[int(t * 15)]
            d.line([(px_, py_), (px_ - 6 * S, py_ + 4 * S)], fill=a // 2, width=S)
    im = im.filter(ImageFilter.GaussianBlur(2.2 * S))
    return np.asarray(im, np.float32) / 255 * 1.6


def sky_layer():
    w, h = LAYERS["sky"] * S, H * S
    rng = np.random.default_rng(1919)
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    xf = xx / w
    col = sky_base(xf, yy / S)
    ci = np.clip(cirrus(w, h, rng), 0, 0.6)
    ccol = mix([214, 210, 214], [255, 226, 186], np.exp(-xf * 2.5))
    col = col * (1 - ci[..., None]) + ccol * ci[..., None]
    cc, cover = cumulus(w, h, rng)
    # The low clouds sit in the haze.
    haze = smoothstep(170, 210, yy / S)[..., None]
    cc = cc * (1 - haze * 0.5) + sky_base(xf, yy / S) * haze * 0.5
    col = col * (1 - cover[..., None]) + cc * cover[..., None]
    alpha = np.ones((h, w), np.float32)
    return col, alpha


# -- the land, walked column by column ------------------------------------------------

class Ground:
    """A height field and its colours, rendered for one layer."""

    def __init__(self, width, eye, z0, z1, steps):
        self.W = width * S
        self.eye = eye
        self.f = FOCAL * S
        self.cx = self.W / 2
        self.z = z0 * (z1 / z0) ** (np.arange(steps) / (steps - 1))

    def render(self, height, colour, haze_len, top=0):
        """Walks every column from near to far.  height(x, z) and
        colour(x, z, h, nx, nz, shadow) work on (steps, columns) arrays."""
        z = self.z[:, None]
        cols = np.arange(self.W)[None, :] + 0.5
        x = (cols - self.cx) * z / self.f
        h = height(x, z)
        dx = z / self.f
        hx = np.gradient(h, axis=1) / dx
        hz = np.gradient(h, axis=0) / np.gradient(self.z)[:, None]
        # The sun along each row from the left: shadow where anything to the
        # left stands above the line of its rays.
        g = h + x * TAN
        run = np.maximum.accumulate(g, axis=1)
        shadow = np.clip((run - g) / (0.05 + 0.002 * z), 0, 1)
        col = colour(x, z, h, hx, hz, shadow)
        fog = 1 - np.exp(-z / haze_len)
        xf = cols / self.W
        hzc = haze_colour(xf, np.broadcast_to(self.z[:, None], x.shape))
        col = col * (1 - fog[..., None]) + hzc * fog[..., None]
        # Which step each pixel sees: the nearest whose top is at or above it.
        ys = HY * S + (self.eye - h) * self.f / z
        ymin = np.minimum.accumulate(ys, axis=0)
        img = np.zeros((H * S, self.W, 3), np.float32)
        alpha = np.zeros((H * S, self.W), np.float32)
        depth = np.full((H * S, self.W), np.inf, np.float32)
        rows = np.arange(top, H * S) + 0.5
        for c in range(self.W):
            yv = ymin[:, c]
            # yv falls as k rises; find for each row the first k with yv <= row.
            k = np.searchsorted(-yv, -rows, side="left")
            ok = k < len(yv)
            kk = np.minimum(k, len(yv) - 1)
            r = np.arange(top, H * S)
            img[r[ok], c] = col[kk[ok], c]
            alpha[r[ok], c] = 1
            depth[r[ok], c] = self.z[kk[ok]]
        self.x, self.h = x, h
        return img, alpha, depth


def haze_colour(xf, z):
    near = mix([214, 186, 150], [168, 172, 182], smoothstep(0.0, 0.9, xf))
    far = mix([206, 178, 148], [148, 154, 166], smoothstep(0.0, 0.9, xf))
    return mix(near, far, smoothstep(300, 3000, z))


def lit(alb, nx, ny, nz, shadow, sun=1.0):
    """Lambert from the low sun on the left, the cool sky everywhere else."""
    n = np.stack([nx, ny, nz], -1)
    n /= np.linalg.norm(n, axis=-1, keepdims=True)
    s = np.array([-math.cos(SUN_ELEV), math.sin(SUN_ELEV), 0.0])
    lam = np.clip(n @ s, 0, 1) * (1 - shadow)
    skyl = 0.55 + 0.45 * n[..., 1]
    return alb * (SUN_COL * lam[..., None] * 2.5 * sun + AMB_COL * skyl[..., None] * 1.35)


# -- the field: the mid layer's ground -----------------------------------------------------

class Field:
    def __init__(self, seed=1916):
        rng = np.random.default_rng(seed)
        # Fire trenches across the view: (distance, bay length, phase).
        self.lines = [(24.0, 15.0, 0.4), (33.0, 16.0, 2.0), (45.0, 18.0, 1.1),
                      (62.0, 21.0, 2.7), (86.0, 25.0, 0.2), (122.0, 30.0, 1.6)]
        self.craters = []
        for _ in range(40):
            z = float(np.exp(rng.uniform(math.log(20.0), math.log(150))))
            x = rng.uniform(-1, 1) * z * 1.85
            r = rng.uniform(1.2, 3.6) * (1 + z / 90)
            self.craters.append((x, z, r, False))
        # The big flooded ones, where they will be seen.
        for x, z, r in ((-46, 36, 4.6), (22, 49, 5.8), (88, 41, 4.2), (-118, 56, 6.0),
                        (150, 64, 5.2), (-6, 70, 4.5), (-170, 44, 4.0)):
            self.craters.append((x, z, r, True))

    def zig(self, x, z0, bay, ph):
        """Fire bays and traverses: a squared wave, softened by the slump."""
        sq = np.tanh(3.2 * np.sin(2 * np.pi * x / bay + ph))
        return z0 + (1.5 + 0.07 * z0) * sq + 2.0 * np.sin(x / 47 + ph * 2)

    def trench(self, x, z):
        """Depth below the ground of the trenches, and the banks either side."""
        h = np.zeros(np.broadcast_shapes(np.shape(x), np.shape(z)), np.float32)
        bottom = np.zeros_like(h)
        bank = np.zeros_like(h)
        for z0, bay, ph in self.lines:
            d = z - self.zig(x, z0, bay, ph)
            w = 1.1 + 0.022 * z0
            t = np.exp(-(d / w) ** 2)
            h += -1.0 * t
            par = np.exp(-((d + 1.7 * w) / (0.95 * w)) ** 2)
            pad = np.exp(-((d - 1.5 * w) / (0.85 * w)) ** 2)
            h += 0.38 * par + 0.22 * pad
            bottom = np.maximum(bottom, t)
            bank = np.maximum(bank, par)
        # A communication trench coming down towards us, wandering.
        cx = 7.0 + 2.5 * np.sin(z / 9.0) + 0.04 * z
        d = (x - cx)
        t = np.exp(-(d / 0.8) ** 2) * smoothstep(6, 9, z) * smoothstep(70, 55, z)
        h += -0.6 * t
        bottom = np.maximum(bottom, t)
        return h, bottom, bank

    def height(self, x, z):
        h = 0.40 * (tex(N1, x * 0.12 + 50, z * 0.12) - 0.5)
        h += 0.06 * (tex(N2, x * 5, z * 5) - 0.5)
        # The low rise at the back, over which the far land shows.
        h += (1.0 + 2.0 * tex(N1, x * 0.012 + 9, 3.0)) * smoothstep(90, 170, z)
        t, _, _ = self.trench(x, z)
        h = h + t
        self.water = np.zeros_like(h)
        for cx, cz, r, wet in self.craters:
            sel = (np.abs(z - cz) < 2.2 * r)[:, 0]
            if not sel.any():
                continue
            xs = x[sel]
            zs = np.broadcast_to(z, x.shape)[sel]
            d2 = ((xs - cx) ** 2 + (zs - cz) ** 2) / (r * r)
            rimh = 0.03 if wet else 0.08
            bowl = -0.30 * r * np.exp(-d2 * 1.7) + rimh * r * np.exp(-(np.sqrt(d2) - 1.05) ** 2 * 9)
            hs = h[sel] + bowl
            if wet:
                # Standing water fills the bowl to a little below the rim.
                ragged = 1 + 0.25 * (tex(N2, xs * 3 + cx, zs * 3) - 0.5)
                wmask = d2 < 0.62 * ragged
                level = h[sel] - 0.03 * r
                self.water[sel] = np.maximum(self.water[sel], wmask.astype(np.float32))
                hs = np.where(wmask, level, hs)
            h[sel] = hs
        return h

    def wet(self, x, z):
        """True over the open water of a flooded shell hole."""
        for cx, cz, r, wet in self.craters:
            if wet and ((x - cx) ** 2 + (z - cz) ** 2) / (r * r) < 0.66:
                return True
        return False

    def poppies(self, x, z):
        """How thick the poppies grow: in drifts and swathes, on the old
        turned soil of the parapets, thinner in the rank trench bottoms."""
        d = np.clip((tex(N1, x * 0.20 + 9, z * 0.32 + 3) - 0.46) * 3.0, 0, 1) ** 1.6
        d = d * (0.75 + 0.5 * tex(N2, x * 0.9, z * 0.9))
        _, bottom, bank = self.trench(x, z)
        return np.clip(d + 0.55 * bank - 0.8 * bottom, 0, 1)


def mid_ground(field, sky_rgb):
    g = Ground(LAYERS["mid"], 6.0, 13.0, 175.0, 1500)

    def colour(x, z, h, hx, hz, shadow):
        dry = np.clip((tex(N1, x * 0.12 + 70, z * 0.16) - 0.45) * 2.4, 0, 1)
        alb = mix([0.25, 0.32, 0.14], [0.50, 0.43, 0.24], dry * 0.7)
        _, bottom, bank = field.trench(x, z)
        alb = mix(alb, [0.11, 0.20, 0.07], np.clip(bottom * 1.3, 0, 1))   # rank grass
        chalk = bank * np.clip((tex(N2, x * 1.4, z * 1.4) - 0.35) * 2.5, 0, 1)
        alb = mix(alb, [0.62, 0.58, 0.46], chalk * 0.55)         # chalk spoil
        fine = tex(N2, x * 26, z * 26)
        alb = alb * (0.80 + 0.40 * fine)[..., None]
        # Wind going over the grass leaves it paler where it is bent.
        wave = np.sin(2 * np.pi * (x * 0.045 + z * 0.075) + 5 * tex(N1, x * 0.05, z * 0.05))
        sheen = smoothstep(0.55, 0.95, wave) * smoothstep(4, 12, z)
        alb = mix(alb, [0.60, 0.56, 0.36], sheen * 0.22)
        col = lit(alb, -hx, np.ones_like(hx), -hz, shadow) * 255
        # Poppy red thickening into a haze far off.
        pop = field.poppies(x, z)
        far = smoothstep(30, 90, z)
        col = mix(col, [178, 44, 30], pop * far * 0.55)
        # Still water: the sky turned over, a little darker.
        if field.water.any():
            yref = 2 * HY - (HY + (g.eye - h) * FOCAL / z)
            xfr = (np.arange(g.W)[None, :] / g.W) * 0.5
            refl = sky_rgb(np.broadcast_to(xfr, x.shape), np.clip(yref, 0, 214))
            ripple = 0.88 + 0.12 * tex(N2, x * 6, z * 30)
            wcol = refl * 0.70 * ripple[..., None]
            col = mix(col, wcol, field.water)
        return col

    img, alpha, depth = g.render(field.height, colour, 900.0, top=150 * S)
    return g, img, alpha, depth


def far_height(x, z):
    """Rolling land: a near swell, ridges beyond it, hills in the haze."""
    h = 4.0 * tex(N1, x / 90.0, z / 90.0)
    h += 22.0 * smoothstep(200, 450, z) * tex(N1, x / 260.0 + 40, z / 300.0)
    h += 50.0 * smoothstep(600, 1500, z) * tex(N1, x / 12.0 + 7, z / 40.0)
    h += 150.0 * smoothstep(1800, 3500, z) * smoothstep(0.3, 0.8, tex(N1, x / 14.0 + 70, 5.0))
    h += 300.0 * smoothstep(4000, 6500, z) * smoothstep(0.3, 0.85, tex(N1, x / 22.0 + 3, 211.0))
    return h


def far_land(sky_rgb):
    g = Ground(LAYERS["far"], 6.0, 150.0, 7000.0, 900)

    def height(x, z):
        return far_height(x, z)

    def colour(x, z, h, hx, hz, shadow):
        strip = np.floor((x * 0.7 + z * 0.4) / (60 + z * 0.05) + tex(N1, x / 300, z / 300) * 3).astype(int) % 4
        palette = np.array([[0.28, 0.36, 0.16], [0.46, 0.41, 0.22], [0.32, 0.38, 0.18],
                            [0.38, 0.33, 0.20]])
        alb = palette[strip]
        alb = alb * (0.85 + 0.3 * tex(N2, x / 20, z / 20))[..., None]
        col = lit(alb, -hx, np.ones_like(hx), -hz, shadow * 0.8) * 255
        pop = np.clip((tex(N1, x / 120 + 5, z / 120) - 0.55) * 3, 0, 1) * smoothstep(900, 300, z)
        col = mix(col, [176, 64, 44], pop * 0.45)
        return col

    img, alpha, depth = g.render(height, colour, 2600.0, top=120 * S)
    return g, img, alpha, depth


# -- painting shaded shapes ---------------------------------------------------------------

class Painter:
    """An RGBA canvas at S, onto which shaded shapes are laid far to near."""

    def __init__(self, w, h, rgb=None, alpha=None):
        self.w, self.h = w, h
        self.rgb = np.zeros((h, w, 3), np.float32) if rgb is None else rgb
        self.a = np.zeros((h, w), np.float32) if alpha is None else alpha

    def lay(self, x0, y0, rgb, a):
        """Lays a patch (straight colour, coverage a) over the canvas."""
        ph, pw = a.shape
        X0, Y0 = max(0, x0), max(0, y0)
        X1, Y1 = min(self.w, x0 + pw), min(self.h, y0 + ph)
        if X0 >= X1 or Y0 >= Y1:
            return
        a = a[Y0 - y0:Y1 - y0, X0 - x0:X1 - x0]
        rgb = rgb[Y0 - y0:Y1 - y0, X0 - x0:X1 - x0]
        r = (slice(Y0, Y1), slice(X0, X1))
        self.rgb[r] = rgb * a[..., None] + self.rgb[r] * (1 - a[..., None])
        self.a[r] = a + self.a[r] * (1 - a)

    def shape(self, draw, bbox, body, light=None, dark=None, rim=None, rim_w=0.0,
              alpha=1.0, grain=0.0, seed=0, shade_dir=LIGHT2D, glow=None):
        """A shape drawn by draw(ImageDraw, ox, oy) inside bbox, shaded from
        its dark side to its lit side along the light, with a rim of light
        on the edges that face the sun."""
        x0, y0, x1, y1 = (int(math.floor(bbox[0])) - 2, int(math.floor(bbox[1])) - 2,
                          int(math.ceil(bbox[2])) + 2, int(math.ceil(bbox[3])) + 2)
        if x1 <= 0 or y1 <= 0 or x0 >= self.w or y0 >= self.h:
            return
        pw, ph = x1 - x0, y1 - y0
        if pw * ph > 4_000_000:
            return
        m = Image.new("L", (pw, ph), 0)
        draw(ImageDraw.Draw(m), -x0, -y0)
        a = np.asarray(m, np.float32) / 255
        if not a.any():
            return
        yy, xx = np.mgrid[0:ph, 0:pw].astype(np.float32)
        tot = a.sum()
        cx, cy = (a * xx).sum() / tot, (a * yy).sum() / tot
        ext = max(2.0, math.sqrt(tot / math.pi))
        t = ((xx - cx) * shade_dir[0] + (yy - cy) * shade_dir[1]) / ext
        k = np.clip(0.5 + 0.5 * t, 0, 1)
        col = np.broadcast_to(np.asarray(body, np.float32), (ph, pw, 3)).copy()
        if light is not None:
            col = mix(col, light, np.clip(k * 1.4 - 0.4, 0, 1))
        if dark is not None:
            col = mix(col, dark, np.clip(0.6 - k * 1.2, 0, 1))
        if rim is not None and rim_w > 0:
            rad = max(1.0, ext * rim_w)
            b = np.asarray(m.filter(ImageFilter.GaussianBlur(rad)), np.float32) / 255
            gy, gx = np.gradient(b)
            nrm = np.sqrt(gx * gx + gy * gy) + 1e-6
            face = np.clip(-(gx * shade_dir[0] + gy * shade_dir[1]) / nrm, 0, 1)
            edge = np.clip((0.85 - b) / 0.6, 0, 1) * a
            col = mix(col, rim, edge * face)
        if glow is not None:
            col = col + np.asarray(glow) * 0
        if grain:
            rng = np.random.default_rng(seed)
            col = col * (1 + grain * (rng.random((ph, pw, 1)) - 0.5))
        self.lay(x0, y0, col, a * alpha)

    def poly(self, pts, body, **kw):
        xs = [p[0] for p in pts]
        ys = [p[1] for p in pts]
        self.shape(lambda d, ox, oy: d.polygon([(x + ox, y + oy) for x, y in pts], fill=255),
                   (min(xs), min(ys), max(xs), max(ys)), body, **kw)

    def ellipse(self, cx, cy, rx, ry, body, **kw):
        self.shape(lambda d, ox, oy: d.ellipse((cx - rx + ox, cy - ry + oy, cx + rx + ox, cy + ry + oy), fill=255),
                   (cx - rx, cy - ry, cx + rx, cy + ry), body, **kw)

    def stroke(self, pts, width, body, **kw):
        xs = [p[0] for p in pts]
        ys = [p[1] for p in pts]
        wd = max(1, int(round(width)))
        self.shape(lambda d, ox, oy: d.line([(x + ox, y + oy) for x, y in pts], fill=255, width=wd,
                                            joint="curve"),
                   (min(xs) - wd, min(ys) - wd, max(xs) + wd, max(ys) + wd), body, **kw)

    def dot(self, x, y, r, col, a=1.0):
        """A small soft dab, cheaply."""
        rr = max(r, 0.6)
        x0, y0 = int(x - rr - 1), int(y - rr - 1)
        n = int(2 * rr + 3)
        yy, xx = np.mgrid[0:n, 0:n].astype(np.float32)
        d = np.sqrt((xx + x0 + 0.5 - x) ** 2 + (yy + y0 + 0.5 - y) ** 2)
        cov = np.clip(rr + 0.5 - d, 0, 1) * a
        self.lay(x0, y0, np.broadcast_to(np.asarray(col, np.float32), (n, n, 3)), cov)


def warm(col, amount=1.0):
    """Colour as lit by the evening sun."""
    return np.clip(np.asarray(col, np.float32) * (1 + 0.35 * amount * (SUN_COL - 0.8)), 0, 255)


def haze_px(col, z, xf, length):
    f = 1 - math.exp(-z / length)
    hz = haze_colour(np.array([xf]), np.array([z]))[0]
    return np.asarray(col, np.float32) * (1 - f) + hz * f


# -- flowers, grasses and other plants ------------------------------------------------------

POPPY = dict(body=(176, 26, 18), light=(236, 84, 38), dark=(112, 12, 12), rim=(255, 168, 80))


def poppy_head(p, x, y, r, rng, face=None, z=0.0, xf=0.0, hl=1e9, open_=1.0):
    """A wild poppy, mostly seen from the side as they are in a field: four
    papery petals in a loose, crumpled cup, veined, glowing at the rims
    where the sun is behind them, the grey-green seed capsule just showing
    over the front petals.  Now and then one turns its face to us, and the
    irregular dark smudges at the petal bases and the fringe of stamens
    show; the flower is never a flat disc."""
    face = min(0.55, abs(rng.normal(0.18, 0.18))) if face is None else face
    tilt = rng.normal(0, 0.30)
    ca, sa = math.cos(tilt), math.sin(tilt)
    squash = rng.uniform(0.82, 1.15)

    def R(px, py):
        px *= squash
        return (x + (px * ca - py * sa) * r, y + (px * sa + py * ca) * r)

    def hc(c):
        return haze_px(c, z, xf, hl)
    jitter = rng.uniform(0.88, 1.08)
    body = hc(np.array(POPPY["body"]) * jitter)
    light = hc(POPPY["light"])
    dark = hc(POPPY["dark"])
    rim = hc(POPPY["rim"])
    rimw = 0.30 if r > 3 else 0.0
    open_up = 0.25 + 0.5 * face
    petals = [(-0.42, -0.30 - 0.25 * open_up, 0.85, 0.80, dark, 0),       # back left
              (0.46, -0.32 - 0.25 * open_up, 0.85, 0.78, body, 1),        # back right
              (-0.62, 0.08, 0.80, 0.72, body, 2),                         # side
              (0.60, 0.10, 0.78, 0.70, body, 3),
              (0.04, 0.32 - 0.10 * face, 1.00, 0.70 - 0.25 * face, body, 4)]  # front
    capsule_drawn = False
    for ox, oy, sx, sy, c, i in petals:
        if i == 4 and face > 0.32 and r > 2.5:
            # Into the cup before the front petal covers its lower part.
            cy_ = -0.18
            for k in range(4):
                a = 2 * math.pi * (k + rng.uniform(-0.2, 0.2)) / 4 + 0.6
                bx_, by_ = 0.30 * math.cos(a), cy_ + 0.20 * math.sin(a) * face
                p.ellipse(*R(bx_, by_), r * rng.uniform(0.10, 0.17), r * rng.uniform(0.06, 0.11),
                          hc((46, 16, 20)))
            for k in range(14):
                a = 2 * math.pi * k / 14 + rng.uniform(-0.1, 0.1)
                p.dot(*R(0.17 * math.cos(a), cy_ + 0.11 * math.sin(a) * face), max(0.6, r * 0.035),
                      hc((54, 40, 58)))
            p.ellipse(*R(0, cy_), r * 0.15, r * 0.09 * face + r * 0.05, hc((118, 130, 96)),
                      light=hc((168, 176, 128)))
            capsule_drawn = True
        n = 16
        pts = []
        ph = rng.uniform(0, 6)
        for k in range(n):
            a = 2 * math.pi * k / n
            crin = 1 + 0.09 * math.sin(a * 5 + ph) + 0.05 * math.sin(a * 11 + ph * 2) + rng.normal(0, 0.025)
            pts.append(R(ox + sx * 0.62 * math.cos(a) * crin, oy + sy * 0.62 * math.sin(a) * crin))
        p.poly(pts, c, light=light, dark=dark, rim=rim, rim_w=rimw)
        if r > 9:
            # Fine veins fanning from the base of the petal.
            for k in range(4):
                a = -math.pi / 2 + (k - 1.5) * 0.35 + (0.4 if ox > 0 else -0.4)
                p.stroke([R(ox * 0.4, oy + 0.25), R(ox + 0.45 * math.cos(a) * sx, oy + 0.45 * math.sin(a) * sy)],
                         max(1, r * 0.025), hc((130, 18, 16)), alpha=0.35)
        if i == 3 and not capsule_drawn and r > 2.5:
            # The capsule's crown just over the front petals, seen side on.
            p.ellipse(*R(0.02, -0.28), r * 0.17, r * 0.10, hc((116, 128, 94)), light=hc((170, 176, 130)))
            p.stroke([R(-0.13, -0.33), R(0.17, -0.33)], max(1, r * 0.05), hc((70, 56, 70)))
    if r <= 2.5:
        p.dot(*R(0.0, -0.25), max(0.6, r * 0.2), hc((92, 22, 18)))


def bud(p, x, y, r, rng, z, xf, hl, droop=1.0):
    """A poppy bud on its crook: hairy and green, nodding, a seam of red."""
    g = haze_px((86, 110, 58), z, xf, hl)
    p.ellipse(x, y + r * 0.3 * droop, r * 0.48, r * 0.75, g, light=haze_px((140, 156, 92), z, xf, hl),
              dark=haze_px((50, 66, 36), z, xf, hl), rim=haze_px((210, 214, 150), z, xf, hl), rim_w=0.25)
    if rng.random() < 0.4 and r > 3:
        p.stroke([(x - r * 0.05, y - r * 0.1), (x + r * 0.08, y + r * 0.8)], max(1, r * 0.14),
                 haze_px((190, 40, 26), z, xf, hl))


def seed_head(p, x, y, r, z, xf, hl):
    """A ripening poppy capsule: an urn with a flat rayed crown."""
    c = haze_px((120, 130, 92), z, xf, hl)
    p.ellipse(x, y + r * 0.2, r * 0.42, r * 0.55, c, light=haze_px((176, 176, 128), z, xf, hl),
              dark=haze_px((70, 80, 52), z, xf, hl), rim=haze_px((230, 220, 170), z, xf, hl), rim_w=0.25)
    p.ellipse(x, y - r * 0.32, r * 0.40, r * 0.12, haze_px((84, 84, 62), z, xf, hl))


def daisy(p, x, y, r, rng, z, xf, hl, kind="oxeye"):
    """An ox-eye daisy or a sprig of chamomile: white rays round a gold eye."""
    face = rng.uniform(0.3, 0.8)
    n = 16 if kind == "oxeye" else 12
    white = haze_px((236, 232, 214), z, xf, hl)
    shade = haze_px((168, 170, 176), z, xf, hl)
    for k in range(n):
        a = 2 * math.pi * k / n + rng.normal(0, 0.06)
        ex, ey = x + math.cos(a) * r, y + math.sin(a) * r * face
        p.stroke([(x, y), (ex, ey)], max(1, r * 0.22), white if math.cos(a) < 0.3 else shade)
    p.ellipse(x, y, r * 0.32, r * 0.30 * (0.5 + face), haze_px((214, 170, 40), z, xf, hl),
              light=haze_px((246, 210, 90), z, xf, hl))


def cornflower(p, x, y, r, rng, z, xf, hl):
    """A cornflower: a ragged rosette of blue florets."""
    blue = haze_px((54, 84, 176), z, xf, hl)
    lightb = haze_px((110, 140, 220), z, xf, hl)
    for k in range(9):
        a = 2 * math.pi * k / 9 + rng.normal(0, 0.1)
        p.ellipse(x + math.cos(a) * r * 0.55, y + math.sin(a) * r * 0.42, r * 0.38, r * 0.30, blue,
                  light=lightb, dark=haze_px((30, 40, 100), z, xf, hl))
    p.dot(x, y, max(0.7, r * 0.2), haze_px((60, 40, 90), z, xf, hl))


def grass_head(p, x, y, length, lean, rng, z, xf, hl, kind, scale=None):
    """The seed head of a grass: a loose oat-like panicle or a dense spike.
    'scale' is pixels to the metre, which keeps a head to its true size."""
    if scale is not None:
        length = min(length, (0.14 if kind == 0 else 0.07) * scale)
    gold = haze_px((196, 172, 108), z, xf, hl)
    pale = haze_px((226, 206, 150), z, xf, hl)
    if kind == 0:                                     # loose panicle
        for k in range(7):
            t = 0.2 + 0.8 * k / 6
            px_, py_ = x + lean * length * t * 0.6, y - length * t
            side = 1 if k % 2 else -1
            ex = px_ + side * length * 0.12 * (1 - t * 0.5) + lean * length * 0.08
            ey = py_ + length * 0.06
            p.stroke([(px_, py_), (ex, ey)], max(1, length * 0.02), gold)
            p.ellipse(ex, ey, max(0.8, length * 0.025), max(1.2, length * 0.045), pale)
    else:                                             # foxtail spike
        pts = [(x + lean * length * t * 0.5, y - length * t) for t in np.linspace(0.55, 1.0, 5)]
        p.stroke(pts, max(1.0, length * 0.11), gold, light=pale, rim=(255, 236, 190), rim_w=0.3)


def stem(p, x0, y0, x1, y1, bend, width, col, light=None):
    """A stalk from (x0, y0) to (x1, y1), bowed."""
    pts = []
    for k in range(9):
        t = k / 8
        pts.append((x0 + (x1 - x0) * t + bend * math.sin(math.pi * t) * 0.5 * abs(y1 - y0) * 0.2,
                    y0 + (y1 - y0) * t))
    p.stroke(pts, width, col, light=light)


def blade(p, x, y, length, lean, width, col, light=None):
    pts = [(x + lean * length * t * t, y - length * t) for t in np.linspace(0, 1, 7)]
    p.stroke(pts, width, col, light=light)


# -- the field's furniture ------------------------------------------------------------------

def project(g, x, z, y=0.0):
    sx = g.cx + x * g.f / z
    sy = HY * S + (g.eye - y) * g.f / z
    return sx, sy


def ground_at(field, x, z):
    return float(field.height(np.array([[x]]), np.array([[z]]))[0, 0])


def cast(p, g, x, z, top_h, base_h, width, strength=0.45):
    """The long shadow of an upright thing, thrown across the ground to the
    right by the low sun."""
    L = (top_h - base_h) / TAN
    xa, ya = project(g, x, z, base_h)
    xb, yb = project(g, x + L, z, base_h - 0.05)
    w = max(1.0, width * g.f / z)
    pts = [(xa, ya - w * 0.3), (xb, yb - w * 0.15), (xb, yb + w * 0.15), (xa, ya + w * 0.3)]
    xs = [q[0] for q in pts]
    ys = [q[1] for q in pts]
    p.shape(lambda d, ox, oy: d.polygon([(a + ox, b + oy) for a, b in pts], fill=255),
            (min(xs), min(ys), max(xs), max(ys)), (40, 48, 60), alpha=strength)


def picket(p, g, field, x, z, rng, xf):
    """A corkscrew picket, rusted, leaning, its foot lost in the grass."""
    hgt = rng.uniform(0.75, 1.3)
    lean = rng.normal(0, 0.22)
    b = ground_at(field, x, z)
    cast(p, g, x, z, b + hgt, b, 0.05)
    x0, y0 = project(g, x, z, b)
    x1, y1 = project(g, x + lean * hgt, z, b + hgt)
    wd = max(1.5, 0.05 * g.f / z)
    rust = haze_px((96, 52, 30), z, xf, 900)
    p.stroke([(x0, y0), (x1, y1)], wd, rust, light=haze_px((168, 96, 52), z, xf, 900),
             dark=haze_px((52, 30, 22), z, xf, 900))
    for t in (0.42, 0.66, 0.9):
        ex, ey = x0 + (x1 - x0) * t, y0 + (y1 - y0) * t
        rr = wd * 1.5
        p.shape(lambda d, ox, oy, ex=ex, ey=ey, rr=rr: d.ellipse((ex - rr + ox, ey - rr + oy, ex + rr + ox, ey + rr + oy),
                                                                 outline=255, width=max(1, int(wd * 0.6))),
                (ex - rr, ey - rr, ex + rr, ey + rr), rust)
    return x + lean * hgt * 0.8, b + hgt * 0.75


def wire(p, g, a, b, z, rng, xf):
    """Barbed wire sagging between two pickets in loose coils, dipping into
    the grass between them."""
    (xa, ya), (xb, yb) = a, b
    col = haze_px((70, 46, 34), z, xf, 900)
    pts = []
    n = 60
    loops = rng.integers(3, 7)
    for k in range(n + 1):
        t = k / n
        x = xa + (xb - xa) * t + 0.12 * math.sin(t * loops * 2 * math.pi)
        y = ya + (yb - ya) * t - 0.6 * math.sin(math.pi * t) + 0.12 * math.cos(t * loops * 2 * math.pi)
        pts.append(project(g, x, z + 0.25 * math.sin(t * 9), max(y, ya - 0.9)))
    wd = max(1.0, 0.014 * g.f / z)
    p.stroke(pts, wd, col)
    for k in range(3, n, 4):
        px_, py_ = pts[k]
        s = max(1.0, 0.025 * g.f / z)
        p.stroke([(px_ - s, py_ - s), (px_ + s, py_ + s)], max(1, wd * 0.8), col)


def shattered_tree(p, g, field, x, z, rng, xf, height):
    """A tree broken by the shelling: a dark, stripped trunk splintered at
    the top, a few torn limbs, and this summer's fresh leaf breaking from
    the stubs and the foot."""
    b = ground_at(field, x, z)
    cast(p, g, x, z, b + height, b, 0.45, strength=0.5)
    s = g.f / z
    bark = haze_px((62, 56, 50), z, xf, 900)
    lightb = haze_px((150, 118, 84), z, xf, 900)
    darkb = haze_px((34, 32, 34), z, xf, 900)
    # The trunk as a bending column of segments, narrowing.
    pts_l, pts_r = [], []
    cx_, w_ = x, 0.42
    lean = rng.normal(0, 0.05)
    for k in range(8):
        t = k / 7
        yy_ = height * t
        cx_ = x + lean * yy_ + 0.12 * math.sin(t * 5 + x)
        w_ = 0.40 * (1 - 0.55 * t) * (1 + 0.15 * math.sin(t * 13 + z))
        pts_l.append(project(g, cx_ - w_, z, b + yy_))
        pts_r.append(project(g, cx_ + w_, z, b + yy_))
    top = []
    for k in range(6):
        u = (k + 0.5) / 6
        top.append(project(g, cx_ - w_ + 2 * w_ * u, z, b + height + rng.uniform(-0.2, 0.9) * (1 - abs(u - 0.4))))
    p.poly(pts_l + top + pts_r[::-1], bark, light=lightb, dark=darkb, rim=lightb, rim_w=0.12, grain=0.15,
           seed=int(x * 7) % 1000)
    for k in range(int(rng.integers(2, 5))):
        t = rng.uniform(0.35, 0.85)
        sx_, sy_ = project(g, x + lean * height * t, z, b + height * t)
        side = rng.choice([-1, 1])
        segs = [(sx_, sy_)]
        ang = rng.uniform(0.5, 1.1)
        ln = rng.uniform(0.8, 2.2) * s
        for j in range(3):
            ang += rng.normal(0, 0.3)
            segs.append((segs[-1][0] + side * ln / 3 * math.cos(ang), segs[-1][1] - ln / 3 * math.sin(ang)))
        p.stroke(segs, max(1, 0.10 * s), bark, light=lightb)
        if rng.random() < 0.75:
            leafy(p, segs[-1][0], segs[-1][1], 0.45 * s, rng, z, xf)
    bx, by = project(g, x, z, b)
    leafy(p, bx + rng.normal(0, 0.3 * s), by - 0.4 * s, 0.7 * s, rng, z, xf)


def leafy(p, x, y, r, rng, z, xf):
    """A small burst of new leaf, lit on the sun's side."""
    for k in range(int(10 + r)):
        lx, ly = x + rng.normal(0, r * 0.6), y + rng.normal(0, r * 0.45)
        rr = max(0.8, r * rng.uniform(0.12, 0.25))
        p.ellipse(lx, ly, rr, rr * 0.8, haze_px((74, 100, 46), z, xf, 900),
                  light=haze_px((176, 182, 86), z, xf, 900), dark=haze_px((40, 58, 34), z, xf, 900))


def reeds(p, g, field, cx, cz, r, rng, xf):
    """Reeds and rushes round a flooded shell hole, and their reflections."""
    for k in range(int(25 + r * 18)):
        a = rng.uniform(0, 2 * math.pi)
        x = cx + math.cos(a) * r * 0.68 * rng.uniform(0.85, 1.15)
        z = cz + math.sin(a) * r * 0.62 * rng.uniform(0.85, 1.15)
        if z < 3:
            continue
        b = ground_at(field, x, z)
        x0, y0 = project(g, x, z, b)
        s = g.f / z
        ln = rng.uniform(0.5, 1.1) * s
        lean = rng.normal(0.1, 0.18)
        col = haze_px((98, 116, 58), z, xf, 900)
        blade(p, x0, y0, ln, lean, max(1, 0.016 * s), col, light=haze_px((206, 196, 112), z, xf, 900))
        if math.sin(a) < 0:            # on the near bank: its image in the water
            blade(p, x0, y0, -ln * 0.6, lean, max(1, 0.012 * s), haze_px((60, 72, 64), z, xf, 900))
        if rng.random() < 0.15:
            tipx, tipy = x0 + lean * ln, y0 - ln
            p.ellipse(tipx, tipy + ln * 0.08, max(1, 0.03 * s), max(2, 0.10 * s), haze_px((92, 60, 34), z, xf, 900),
                      light=haze_px((170, 120, 70), z, xf, 900))


def duckboard(p, g, field, x, z, rng, xf):
    """A length of duckboard, grey with rot, sunk at one end."""
    s = g.f / z
    b = ground_at(field, x, z)
    for k in range(9):
        sx_ = x + k * 0.28
        x0, y0 = project(g, sx_, z - 0.3, b + 0.05 - k * 0.02)
        x1, y1 = project(g, sx_ + 0.04, z + 0.3, b + 0.05 - k * 0.02)
        p.poly([(x0, y0), (x0 + 0.12 * s, y0), (x1 + 0.12 * s, y1), (x1, y1)], haze_px((116, 104, 86), z, xf, 900),
               light=haze_px((200, 178, 140), z, xf, 900), dark=haze_px((70, 64, 56), z, xf, 900))
    for zz in (z - 0.25, z + 0.25):
        xa, ya = project(g, x - 0.05, zz, b)
        xb, yb = project(g, x + 2.4, zz, b - 0.12)
        p.stroke([(xa, ya), (xb, yb)], max(1, 0.05 * s), haze_px((76, 66, 54), z, xf, 900))


def rag_stake(p, g, field, x, z, rng, xf):
    """A stake leaning in the grass with a ragged scrap of cloth on it."""
    s = g.f / z
    b = ground_at(field, x, z)
    cast(p, g, x, z, b + 1.4, b, 0.06)
    x0, y0 = project(g, x, z, b)
    x1, y1 = project(g, x - 0.25, z, b + 1.4)
    p.stroke([(x0, y0), (x1, y1)], max(1.5, 0.06 * s), haze_px((100, 84, 62), z, xf, 900),
             light=haze_px((196, 162, 112), z, xf, 900))
    cloth = []
    for k in range(9):
        t = k / 8
        cloth.append((x1 + 0.04 * s + t * 0.45 * s, y1 + 0.05 * s + math.sin(t * 5) * 0.05 * s + t * 0.12 * s))
    for k in range(9):
        t = 1 - k / 8
        cloth.append((x1 + 0.02 * s + t * 0.38 * s, y1 + 0.35 * s + rng.uniform(0, 0.12) * s + t * 0.1 * s))
    p.poly(cloth, haze_px((126, 116, 84), z, xf, 900), light=haze_px((214, 196, 150), z, xf, 900),
           dark=haze_px((72, 68, 54), z, xf, 900))


def mid_items(p, g, field, rng):
    """Everything upright in the field, gathered and painted far to near."""
    items = []
    half = (LAYERS["mid"] / 2) / FOCAL * 1.05

    def xf_of(x, z):
        return float(np.clip((g.cx + x * g.f / z) / g.W, 0, 1))
    # Shattered trees along the old line, and one nearer.
    for x, z, hh in ((-80, 98, 9.5), (-62, 92, 7.0), (-41, 102, 11.0), (6, 95, 6.5), (44, 104, 10.0),
                     (70, 96, 8.0), (102, 108, 7.5), (-24, 60, 6.0), (36, 70, 7.5)):
        items.append((z, "tree", (x, hh)))
    # Pickets and wire in broken runs in front of the trench lines.
    for z0, bay, ph in field.lines[:5]:
        zr = z0 - 3.4 - 0.05 * z0
        x = -half * zr
        while x < half * zr:
            if rng.random() < 0.55:
                run = int(rng.integers(2, 5))
                pts = [(x + k * rng.uniform(2.2, 3.4), zr + rng.normal(0, 0.35)) for k in range(run)]
                items.append((zr, "wire", pts))
                x += run * 3.0
            x += rng.uniform(5, 16)
    items.append((31.0, "duckboard", (-22.0,)))
    items.append((38.0, "rag", (30.0,)))
    for cx, cz, r, wet in field.craters:
        if wet:
            items.append((cz, "reeds", (cx, cz, r)))
    # Poppies, other flowers and grass in drifts, from the haze to our feet.
    for z in np.arange(90.0, 2.4, -0.05):
        width = half * z * 2
        area = width * 0.05
        n = rng.poisson(area * (3.0 if z > 40 else 5.0 if z > 18 else 8.0))
        xs = rng.uniform(-half * z, half * z, n)
        dens = field.poppies(xs[None, :], np.full((1, n), z))[0]
        keep = rng.random(n) < dens
        for x in xs[keep]:
            items.append((z + rng.uniform(0, 0.05), "poppy", (x,)))
        m = rng.poisson(area * 0.3)
        for x in rng.uniform(-half * z, half * z, m):
            items.append((z + rng.uniform(0, 0.05), "flower", (x,)))
        if z < 22:
            k = rng.poisson(area * 1.2)
            for x in rng.uniform(-half * z, half * z, k):
                items.append((z + rng.uniform(0, 0.05), "grass", (x,)))
    items = [it for it in items if it[1] not in ("poppy", "flower", "grass") or not field.wet(it[2][0], it[0])]
    items.sort(key=lambda it: -it[0])
    for z, kind, a in items:
        s = g.f / z
        if kind == "tree":
            shattered_tree(p, g, field, a[0], z, rng, xf_of(a[0], z), a[1])
        elif kind == "wire":
            tops = [picket(p, g, field, x, zz, rng, xf_of(x, zz)) for x, zz in a]
            for t0, t1 in zip(tops[:-1], tops[1:]):
                wire(p, g, t0, t1, z, rng, xf_of(t0[0], z))
        elif kind == "duckboard":
            duckboard(p, g, field, a[0], z, rng, xf_of(a[0], z))
        elif kind == "rag":
            rag_stake(p, g, field, a[0], z, rng, xf_of(a[0], z))
        elif kind == "reeds":
            reeds(p, g, field, a[0], a[1], a[2], rng, xf_of(a[0], z))
        elif kind == "poppy":
            x = a[0]
            xf = xf_of(x, z)
            b = ground_at(field, x, z) if z < 25 else 0.0
            hgt = rng.uniform(0.3, 0.65)
            x0, y0 = project(g, x, z, b)
            x1, y1 = project(g, x + rng.normal(0.05, 0.05), z, b + hgt)
            r = 0.045 * s * rng.uniform(0.8, 1.25)
            if r < 1.4:
                p.dot(x1, y1, r * 1.1, haze_px(np.array([186, 36, 24]) * rng.uniform(0.85, 1.15), z, xf, 900))
                continue
            stem(p, x0, y0, x1, y1, rng.normal(0, 0.3), max(1, 0.008 * s), haze_px((80, 104, 54), z, xf, 900))
            if rng.random() < 0.08:
                bud(p, x1, y1, r * 1.1, rng, z, xf, 900)
            else:
                poppy_head(p, x1, y1, r, rng, z=z, xf=xf, hl=900)
        elif kind == "flower":
            x = a[0]
            xf = xf_of(x, z)
            b = ground_at(field, x, z) if z < 25 else 0.0
            x0, y0 = project(g, x, z, b)
            x1, y1 = project(g, x, z, b + rng.uniform(0.25, 0.55))
            r = 0.03 * s
            pick = rng.random()
            if r < 1.2:
                c = (226, 222, 206) if pick < 0.6 else (70, 96, 186)
                p.dot(x1, y1, max(0.7, r), haze_px(c, z, xf, 900))
                continue
            stem(p, x0, y0, x1, y1, rng.normal(0, 0.2), max(1, 0.006 * s), haze_px((84, 104, 58), z, xf, 900))
            if pick < 0.45:
                daisy(p, x1, y1, r * 1.2, rng, z, xf, 900)
            elif pick < 0.7:
                daisy(p, x1, y1, r * 0.7, rng, z, xf, 900, kind="chamomile")
            else:
                cornflower(p, x1, y1, r, rng, z, xf, 900)
        elif kind == "grass":
            x = a[0]
            xf = xf_of(x, z)
            b = ground_at(field, x, z)
            x0, y0 = project(g, x, z, b)
            for j in range(int(rng.integers(3, 7))):
                ln = rng.uniform(0.12, 0.45) * s
                lean = rng.normal(0.15, 0.35)
                c = (78, 98, 48) if rng.random() < 0.7 else (150, 134, 84)
                blade(p, x0 + rng.normal(0, 0.03 * s), y0, ln, lean, max(1, 0.006 * s), haze_px(c, z, xf, 900),
                      light=haze_px((196, 180, 116), z, xf, 900))
                if rng.random() < 0.25 and ln > 6:
                    grass_head(p, x0 + lean * ln, y0 - ln, ln * 0.35, lean, rng, z, xf, 900, int(rng.integers(0, 2)),
                               scale=s)


# -- the far land's ruins, poplars and cross -----------------------------------------------

def far_items(p, g, height_far, rng):
    def place(x, z, up=0.0):
        h = float(height_far(np.array([[x]]), np.array([[z]]))[0, 0])
        return project(g, x, z, h + up), h

    def xf_of(x, z):
        return float(np.clip((g.cx + x * g.f / z) / g.W, 0, 1))
    HL = 650.0
    # Poplars along a road, some broken, in the haze.
    z = 235.0
    for k in range(16):
        x = 40 + k * 12 + rng.normal(0, 1.5)
        (bx, by), h = place(x, z)
        xf = xf_of(x, z)
        tall = rng.uniform(20, 27) * (0.35 if rng.random() < 0.25 else 1.0)
        (tx, ty), _ = place(x, z, tall)
        wdt = 3.4 * g.f / z
        col = haze_px((54, 72, 44), z, xf, HL)
        pts = [(bx - wdt * 0.2, by)]
        for t in np.linspace(0.15, 1.0, 8):
            yy_ = by + (ty - by) * t
            ww = wdt * (1 - t ** 1.6) * (0.8 + 0.4 * rng.random()) + 0.6
            pts.append((bx - ww, yy_))
        pts.append((tx, ty - 1))
        for t in np.linspace(1.0, 0.15, 8):
            yy_ = by + (ty - by) * t
            ww = wdt * (1 - t ** 1.6) * (0.8 + 0.4 * rng.random()) + 0.6
            pts.append((bx + ww * 0.9, yy_))
        pts.append((bx + wdt * 0.2, by))
        p.poly(pts, col, light=haze_px((150, 150, 84), z, xf, HL), dark=haze_px((40, 52, 44), z, xf, HL))
    # The village: roofless gables and broken walls, and the church tower.
    z = 260.0
    vx = -95.0
    for k in range(8):
        x = vx + k * 8 + rng.normal(0, 1.5)
        (bx, by), h = place(x, z)
        xf = xf_of(x, z)
        s = g.f / z
        wdt = rng.uniform(5, 9) * s
        hh = rng.uniform(4, 8) * s
        gable = rng.random() < 0.3
        pts = [(bx - wdt / 2, by), (bx - wdt / 2, by - hh)]
        if gable:
            pts.append((bx, by - hh - wdt * 0.45))
        else:
            for j in range(7):
                pts.append((bx - wdt / 2 + wdt * (j + 0.5) / 7, by - hh * rng.uniform(0.35, 1.25)))
        pts += [(bx + wdt / 2, by - hh), (bx + wdt / 2, by)]
        p.poly(pts, haze_px((104, 94, 84), z, xf, HL), light=haze_px((166, 138, 106), z, xf, HL),
               dark=haze_px((72, 74, 84), z, xf, HL))
        if gable and wdt > 6:
            p.ellipse(bx, by - hh * 0.6, wdt * 0.1, hh * 0.15, haze_px((70, 70, 84), z, xf, HL))
    # The church tower, its top shot away, an empty window in the belfry.
    x = vx + 30
    (bx, by), h = place(x, z)
    xf = xf_of(x, z)
    s = g.f / z
    tw, th = 7.5 * s, 26.0 * s
    jag = [(bx - tw / 2, by - th + 2 * s)]
    for j in range(6):
        jag.append((bx - tw / 2 + tw * (j + 0.5) / 6, by - th + rng.uniform(-1.5, 3.5) * s))
    jag.append((bx + tw / 2, by - th + 4 * s))
    p.poly([(bx - tw / 2, by)] + jag + [(bx + tw / 2, by)], haze_px((116, 104, 92), z, xf, HL),
           light=haze_px((176, 146, 112), z, xf, HL), dark=haze_px((70, 72, 84), z, xf, HL))
    p.poly([(bx - tw * 0.18, by - th * 0.62), (bx + tw * 0.18, by - th * 0.62),
            (bx + tw * 0.18, by - th * 0.48), (bx - tw * 0.18, by - th * 0.48)], haze_px((80, 80, 96), z, xf, HL))
    # Shattered trees about the village.
    for k in range(9):
        x = vx - 40 + k * 14 + rng.normal(0, 4)
        (bx, by), h = place(x, z + 30)
        (tx, ty), _ = place(x + rng.normal(0, 1), z + 30, rng.uniform(5, 11))
        p.stroke([(bx, by), (tx, ty)], max(1, 0.5 * g.f / (z + 30)), haze_px((84, 78, 72), z + 30, xf_of(x, z), HL))
    # A wayside cross on a rise, with its little roof.
    x, z = 150.0, 185.0
    (bx, by), h = place(x, z)
    s = g.f / z
    xf = xf_of(x, z)
    col = haze_px((70, 62, 56), z, xf, HL)
    p.stroke([(bx, by), (bx, by - 4.2 * s)], max(1, 0.25 * s), col, light=haze_px((210, 176, 130), z, xf, HL))
    p.stroke([(bx - 1.0 * s, by - 3.0 * s), (bx + 1.0 * s, by - 3.0 * s)], max(1, 0.22 * s), col)
    p.poly([(bx - 0.6 * s, by - 4.1 * s), (bx, by - 4.6 * s), (bx + 0.6 * s, by - 4.1 * s)], col)


# -- the near layer: flowers at our feet ----------------------------------------------------

class Near:
    """A camera low in the poppies: their stems rise from below the frame."""

    def __init__(self):
        self.W = LAYERS["near"] * S
        self.eye = 1.25
        self.f = FOCAL * S
        self.cx = self.W / 2


def near_layer(field):
    g = Near()
    p = Painter(g.W, H * S)
    rng = np.random.default_rng(1917)
    half = (LAYERS["near"] / 2) / FOCAL * 1.04

    def xf_of(x, z):
        return float(np.clip((g.cx + x * g.f / z) / g.W, 0, 1))

    def proj(x, z, y):
        return g.cx + x * g.f / z, HY * S + (g.eye - y) * g.f / z
    # A dense ground of leaves and stems along the bottom.
    base = np.zeros((H * S, g.W), np.float32)
    yy = np.arange(H * S)[:, None]
    edge = (386 + 8 * tex(N1, np.arange(g.W)[None, :] / (S * 6.0), 5.0) +
            5 * tex(N2, np.arange(g.W)[None, :] / (S * 1.5), 9.0)) * S
    base = (yy > edge).astype(np.float32)
    shade = 0.6 + 0.4 * tex(N2, np.arange(g.W)[None, :] / (S * 1.0), yy / (S * 2.0))
    p.rgb[:] = np.array([44, 58, 30], np.float32) * shade[..., None]
    p.a[:] = base
    items = []
    for z in np.arange(3.2, 0.85, -0.01):
        width = half * z * 2
        area = width * 0.01
        n = rng.poisson(area * (90 if z < 1.8 else 55))
        xs = rng.uniform(-half * z, half * z, n)
        # Drifts: thick in places, thin or gone between.
        drift = np.clip((tex(N1, xs * 0.55 + 31, np.full(n, z * 0.9)) - 0.32) * 3.0, 0, 1) ** 1.2
        for x in xs[rng.random(n) < drift]:
            items.append((z, "poppy", x))
        for x in rng.uniform(-half * z, half * z, rng.poisson(area * 8)):
            items.append((z, "grass", x))
        for x in rng.uniform(-half * z, half * z, rng.poisson(area * 2.0)):
            items.append((z, "flower", x))
    # The things the field keeps.
    items.append((3.5, "helmet", -1.2))
    items.append((3.3, "shellcase", 3.6))
    items.append((3.7, "wire", -4.6))
    items.sort(key=lambda it: -it[0])
    for z, kind, x in items:
        s = g.f / z
        xf = xf_of(x, z)
        if kind == "poppy":
            hgt = rng.uniform(0.55, 0.88) if z < 2.2 else rng.uniform(0.45, 0.72)
            x0, y0 = proj(x, z, 0)
            bend = rng.normal(0.2, 0.25)
            x1, y1 = proj(x + bend * hgt * 0.35, z, hgt)
            if y1 > (H + 4) * S:
                continue
            r = 0.040 * s * rng.uniform(0.75, 1.15)
            stem(p, x0, y0, x1, y1, bend, max(1.5, 0.006 * s), (78, 104, 52), light=(176, 186, 104))
            if rng.random() < 0.15:                       # a cut leaf low down
                lx, ly = proj(x + 0.06, z, hgt * 0.25)
                for k in range(4):
                    p.ellipse(lx + k * 0.012 * s, ly - k * 0.02 * s, 0.018 * s, 0.008 * s, (70, 94, 50),
                              light=(150, 168, 90))
            pick = rng.random()
            if pick < 0.10:
                bud(p, x1, y1, r * 1.0, rng, z, xf, 1e9)
            elif pick < 0.16:
                seed_head(p, x1, y1, r * 0.9, z, xf, 1e9)
            else:
                poppy_head(p, x1, y1, r, rng, z=z, xf=xf, hl=1e9)
        elif kind == "grass":
            x0, y0 = proj(x, z, 0)
            for j in range(int(rng.integers(2, 6))):
                ln = rng.uniform(0.25, 0.62) * s
                lean = rng.normal(0.2, 0.3)
                c = (62, 86, 40) if rng.random() < 0.8 else (136, 122, 76)
                blade(p, x0 + rng.normal(0, 0.02 * s), y0, ln, lean, max(1.5, 0.006 * s), c, light=(178, 170, 104))
                if rng.random() < 0.3:
                    grass_head(p, x0 + lean * ln, y0 - ln, ln * 0.3, lean, rng, z, xf, 1e9, int(rng.integers(0, 2)),
                               scale=s)
        elif kind == "flower":
            x0, y0 = proj(x, z, 0)
            x1, y1 = proj(x + 0.03, z, rng.uniform(0.45, 0.85))
            if y1 > (H + 4) * S:
                continue
            stem(p, x0, y0, x1, y1, rng.normal(0, 0.2), max(1.2, 0.004 * s), (80, 102, 56), light=(160, 176, 100))
            pick = rng.random()
            r = 0.030 * s
            if pick < 0.4:
                daisy(p, x1, y1, r * 1.2, rng, z, xf, 1e9)
            elif pick < 0.65:
                daisy(p, x1, y1, r * 0.7, rng, z, xf, 1e9, kind="chamomile")
            else:
                cornflower(p, x1, y1, r * 0.9, rng, z, xf, 1e9)
        elif kind == "helmet":
            # A soldier's steel helmet, rusted, its brim half sunk.
            hx, hy = proj(x, z, 0.06)
            r = 0.17 * s
            p.ellipse(hx, hy + r * 0.15, r * 1.25, r * 0.36, (78, 46, 30), light=(176, 104, 56),
                      dark=(40, 28, 24), rim=(230, 150, 80), rim_w=0.15)
            p.ellipse(hx, hy - r * 0.05, r * 0.82, r * 0.55, (96, 56, 34), light=(196, 120, 64),
                      dark=(46, 30, 24), rim=(240, 168, 96), rim_w=0.2, grain=0.25, seed=3)
        elif kind == "shellcase":
            # An old brass shell case on its side, tarnished and grown over.
            cx_, cy_ = proj(x, z, 0.05)
            L, R_ = 0.32 * s, 0.05 * s
            p.poly([(cx_ - L / 2, cy_ - R_), (cx_ + L / 2, cy_ - R_ * 0.9), (cx_ + L / 2, cy_ + R_ * 0.9),
                    (cx_ - L / 2, cy_ + R_)], (120, 104, 56), light=(214, 186, 104), dark=(64, 70, 50),
                   rim=(250, 220, 140), rim_w=0.2, grain=0.2, seed=5)
            p.ellipse(cx_ + L / 2, cy_, R_ * 0.45, R_ * 0.9, (70, 66, 44), light=(150, 132, 76))
        elif kind == "wire":
            # A strand of wire, rusted, half buried, looping out of the grass.
            pts = []
            for k in range(50):
                t = k / 49
                xx_ = x + t * 2.6
                yy_ = 0.05 + 0.10 * abs(math.sin(t * 9)) - 0.03
                pts.append(proj(xx_, z + 0.2 * math.sin(t * 5), yy_))
            p.stroke(pts, max(1.5, 0.008 * s), (84, 50, 32), light=(170, 104, 60))
    return p


# -- finishing ----------------------------------------------------------------------------------

BAYER = np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]],
                 np.float32) / 16.0 - 0.5


def finish(rgb, alpha, keyed=True, grade=True):
    """Reduce from three times the size, settle the colour, dither to the
    16-bit colours the game shows and key what is empty."""
    h, w = alpha.shape
    pm = rgb * alpha[..., None]
    pm = pm.reshape(h // S, S, w // S, S, 3).mean((1, 3))
    a = alpha.reshape(h // S, S, w // S, S).mean((1, 3))
    with np.errstate(invalid="ignore", divide="ignore"):
        col = np.where(a[..., None] > 1e-4, pm / a[..., None], 0)
    if grade:
        lum = (col @ np.array([0.299, 0.587, 0.114]))[..., None]
        col = col * 0.92 + lum * 0.08
    hh, ww = a.shape
    d = np.tile(BAYER, (hh // 4 + 1, ww // 4 + 1))[:hh, :ww][..., None]
    q = np.empty_like(col)
    for k, bits in enumerate((5, 6, 5)):
        lv = (1 << bits) - 1
        v = np.clip(np.floor(col[..., k] / 255 * lv + 0.5 + d[..., 0]), 0, lv)
        q[..., k] = (v * 255 / lv)
    out = np.clip(q + 0.5, 0, 255).astype(np.uint8)
    if keyed:
        empty = a < 0.5
        out[empty] = KEY
        clash = (~empty) & (out == np.array(KEY, np.uint8)).all(-1)
        out[clash] = (248, 4, 248)
    return Image.fromarray(out), a


def crop_rows(img, top, bottom):
    return img.crop((0, top, img.width, bottom))


def used_rows(a, pad=2):
    rows = np.where((a > 0.5).any(1))[0]
    top = max(0, (int(rows[0]) - pad) // 2 * 2)
    return top, int(rows[-1]) + 1


# -- the sway and life sheets -----------------------------------------------------------------

VARIANTS = [("tall poppy", 40, 48, "poppy_tall"), ("poppy pair", 40, 40, "poppy_pair"),
            ("poppy and bud", 34, 36, "poppy_bud"), ("seed head and poppy", 30, 32, "seed"),
            ("daisy and cornflower", 30, 30, "daisy"), ("grass in seed", 34, 44, "grass")]
SWAY = (-1.0, -0.35, 0.35, 1.0)        # the four frames; play 0 1 2 3 2 1


def sway_plant(kind, w, h, lean, seed):
    U = S * 2
    p = Painter(w * U, h * U)
    rng = np.random.default_rng(seed)
    fx, fy = w * U / 2, h * U

    def top(ox, hf):
        return fx + ox * w * U + lean * hf * h * U * 0.32, fy - hf * h * U

    def plantstem(ox, hf):
        tx, ty = top(ox, hf)
        stem(p, fx + ox * w * U * 0.3, fy, tx, ty, lean * 0.8, U * 1.2, (78, 104, 52), light=(176, 186, 104))
        return tx, ty
    for k in range(4):                                   # grass at the foot
        ox = rng.normal(0, 0.12)
        ln = h * U * rng.uniform(0.18, 0.4)
        blade(p, fx + ox * w * U, fy, ln, lean * 0.4 + rng.normal(0, 0.25), U * 1.0,
              (84, 108, 52) if k % 2 else (168, 150, 96), light=(232, 214, 150))
    r = w * U * 0.16
    if kind == "poppy_tall":
        x, y = plantstem(0.0, 0.84)
        poppy_head(p, x, y, r * 1.1, rng, face=0.45)
    elif kind == "poppy_pair":
        x, y = plantstem(-0.12, 0.80)
        poppy_head(p, x, y, r, rng, face=0.3)
        x, y = plantstem(0.14, 0.58)
        poppy_head(p, x, y, r * 0.9, rng, face=0.6)
    elif kind == "poppy_bud":
        x, y = plantstem(-0.1, 0.78)
        poppy_head(p, x, y, r * 1.05, rng, face=0.5)
        x, y = plantstem(0.16, 0.66)
        bud(p, x, y, r * 0.9, rng, 0, 0.5, 1e9)
    elif kind == "seed":
        x, y = plantstem(-0.1, 0.80)
        seed_head(p, x, y, r * 0.95, 0, 0.5, 1e9)
        x, y = plantstem(0.15, 0.62)
        poppy_head(p, x, y, r * 0.9, rng, face=0.4)
    elif kind == "daisy":
        x, y = plantstem(-0.12, 0.78)
        daisy(p, x, y, r * 1.2, rng, 0, 0.5, 1e9)
        x, y = plantstem(0.15, 0.58)
        cornflower(p, x, y, r * 0.95, rng, 0, 0.5, 1e9)
    else:
        for k in range(9):
            ox = rng.normal(0, 0.08)
            hf = rng.uniform(0.5, 0.92)
            ln = hf * h * U
            le = lean * 0.33 + rng.normal(0, 0.12)
            blade(p, fx + ox * w * U, fy, ln, le, U * 1.0, (84, 108, 52) if k % 3 else (178, 158, 100),
                  light=(232, 214, 150))
            if k < 3:
                grass_head(p, fx + ox * w * U + le * ln, fy - ln, ln * 0.3, le, rng, 0, 0.5, 1e9, k % 2)
    img, a = finish_small(p, U)
    return img


def finish_small(p, U):
    h, w = p.a.shape
    pm = (p.rgb * p.a[..., None]).reshape(h // U, U, w // U, U, 3).mean((1, 3))
    a = p.a.reshape(h // U, U, w // U, U).mean((1, 3))
    with np.errstate(invalid="ignore", divide="ignore"):
        col = np.where(a[..., None] > 1e-4, pm / a[..., None], 0)
    out = np.clip(col + 0.5, 0, 255).astype(np.uint8)
    out[a < 0.5] = KEY
    return Image.fromarray(out), a


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
            sheet.paste(sway_plant(kind, w, h, lean * 0.75, seed=v * 7 + 1), (x, y))
            row.append((x, y, w, h))
        rects.append((name, row))
        y += h + 1
    return sheet, rects


LIFE = [("small tortoiseshell butterfly", "butterfly", 3), ("cabbage white butterfly", "white", 3),
        ("common blue butterfly", "blue", 3), ("lark, high up", "lark", 2), ("thistledown", "seed", 2)]


def life_frame(kind, k):
    """One frame of a living thing, painted at six times and reduced."""
    U = 6
    if kind in ("butterfly", "white", "blue"):
        size = {"butterfly": 12, "white": 11, "blue": 9}[kind]
        p = Painter(size * U, size * U)
        cx, cy = size * U / 2, size * U / 2
        open_ = (1.0, 0.55, 0.12)[k]
        cols = {"butterfly": ((212, 96, 34), (250, 168, 70), (40, 30, 26)),
                "white": ((232, 230, 220), (255, 255, 246), (90, 90, 96)),
                "blue": ((96, 128, 214), (160, 190, 250), (40, 44, 80))}[kind]
        for side in (-1, 1):
            span = size * U * 0.46 * open_ + 2
            p.poly([(cx, cy - size * U * 0.05), (cx + side * span, cy - size * U * 0.38),
                    (cx + side * span * 1.05, cy - size * U * 0.05), (cx + side * span * 0.4, cy + size * U * 0.06)],
                   cols[0], light=cols[1], dark=cols[2], rim=cols[1], rim_w=0.2)
            p.poly([(cx, cy + size * U * 0.02), (cx + side * span * 0.8, cy + size * U * 0.08),
                    (cx + side * span * 0.55, cy + size * U * 0.33), (cx + side * U * 0.6, cy + size * U * 0.2)],
                   cols[0], light=cols[1], dark=cols[2])
            if kind == "butterfly" and open_ > 0.5:
                p.dot(cx + side * span * 0.7, cy - size * U * 0.22, U * 0.9, (30, 26, 24))
                p.dot(cx + side * span * 0.95, cy - size * U * 0.12, U * 0.7, (250, 230, 190))
        p.ellipse(cx, cy, U * 0.7, size * U * 0.24, (36, 30, 28))
        img, a = finish_small(p, U)
        return img
    if kind == "lark":
        img = Image.new("RGB", (5, 3), KEY)
        d = ImageDraw.Draw(img)
        c = (58, 54, 52)
        if k == 0:
            d.point([(0, 0), (1, 1), (2, 1), (3, 1), (4, 0)], fill=c)
        else:
            d.point([(0, 2), (1, 1), (2, 1), (3, 1), (4, 2)], fill=c)
        return img
    p = Painter(8 * U, 8 * U)                               # thistledown
    cx, cy = 4 * U, 4.5 * U
    for j in range(14):
        a = math.pi * (0.05 + 0.9 * j / 13) + (0.12 if k else 0)
        ex, ey = cx + math.cos(a) * 3.2 * U, cy - math.sin(a) * 3.2 * U
        p.stroke([(cx, cy), (ex, ey)], max(1, U * 0.7), (236, 232, 222))
    p.ellipse(cx, cy + 0.6 * U, U * 0.4, U * 0.6, (120, 100, 70))
    img, a = finish_small(p, U)
    return img


def life_sheet():
    frames = []
    for name, kind, n in LIFE:
        frames.append([life_frame(kind, k) for k in range(n)])
    width = max(sum(f.width + 1 for f in row) for row in frames)
    height = sum(max(f.height for f in row) + 1 for row in frames)
    sheet = Image.new("RGB", (width, height), KEY)
    rects = []
    y = 0
    for (name, kind, n), row in zip(LIFE, frames):
        x = 0
        rr = []
        for f in row:
            sheet.paste(f, (x, y))
            rr.append((x, y, f.width, f.height))
            x += f.width + 1
        rects.append((name, rr))
        y += max(f.height for f in row) + 1
    return sheet, rects


# -- the header ----------------------------------------------------------------------------------

def header(rects, layer_info, life_rects):
    L = ["/* Generated by ports/amigaos3/make_remember.py: do not edit. */",
         "#ifndef CANVAS_REMEMBER_H", "#define CANVAS_REMEMBER_H", "",
         f"#define REMEMBER_POPPY_VARIANTS {len(rects)}   /* kinds of poppy or tuft */",
         f"#define REMEMBER_SWAY_FRAMES {len(SWAY)}",
         "/* per variant, per sway frame: x, y, w, h in remember-poppies.bmp; the",
         " * foot of each frame is its bottom edge.  The frames lean from one",
         " * side to the other: play them 0 1 2 3 2 1 for a to-and-fro. */",
         "static const short remember_poppy_frames[REMEMBER_POPPY_VARIANTS][REMEMBER_SWAY_FRAMES][4] = {"]
    for name, row in rects:
        L.append("    {" + ", ".join("{%d, %d, %d, %d}" % r for r in row) + "}, /* " + name + " */")
    L += ["};", f"#define REMEMBER_FIELD_QUIET_TOP {QUIET[0]}    /* the calm band for lettering */",
          f"#define REMEMBER_FIELD_QUIET_BOTTOM {QUIET[1]}", "",
          "#define REMEMBER_LAYERS 4",
          "/* sky, far, mid, near: width, top y on screen, height.  Slide them at",
          " * about 0.15, 0.35, 0.6 and 1.0 of the near layer's speed. */",
          "static const short remember_layer_info[REMEMBER_LAYERS][3] = {"]
    for name, (w, top, hh) in layer_info:
        L.append(f"    {{{w}, {top}, {hh}}}, /* {name} */")
    nmax = max(len(r) for _, r in life_rects)
    L += ["};", "",
          f"#define REMEMBER_LIFE_SPRITES {len(life_rects)}   /* "
          + ", ".join(f"{k} {n.split(',')[0]}" for k, (n, _) in enumerate(life_rects)) + " */",
          f"#define REMEMBER_LIFE_FRAMES {nmax}",
          "static const unsigned char remember_life_frame_count[REMEMBER_LIFE_SPRITES] = {"
          + ", ".join(str(len(r)) for _, r in life_rects) + "};",
          "/* x, y, w, h in remember-life.bmp; unused frames all zero.  The",
          " * butterflies' frames run open, half, closed: play 0 1 2 1. */",
          "static const short remember_life_frames[REMEMBER_LIFE_SPRITES][REMEMBER_LIFE_FRAMES][4] = {"]
    for name, rr in life_rects:
        rr = list(rr) + [(0, 0, 0, 0)] * (nmax - len(rr))
        L.append("    {" + ", ".join("{%d, %d, %d, %d}" % r for r in rr) + "}, /* " + name + " */")
    L += ["};", "", "#endif", ""]
    HEADER.write_text("\n".join(L))


# -- the preview -----------------------------------------------------------------------------------

def keyed(img):
    a = np.asarray(img)
    return Image.fromarray(((a != np.array(KEY)).any(-1) * 255).astype(np.uint8))


def compose(layers, offsets):
    """The screen at one moment: the layers stacked, each slid by its own
    offset."""
    out = layers["sky"][0].crop((int(offsets["sky"]), 0, int(offsets["sky"]) + 640, 400)).copy()
    for name in ("far", "mid", "near"):
        img, top = layers[name]
        o = int(offsets[name])
        part = img.crop((o, 0, o + 640, img.height))
        out.paste(part, (0, top), keyed(part))
    return out


def preview(layers, sheet, rects, life, life_rects, out):
    fps = 20
    seconds = 12
    frames = []
    rng = np.random.default_rng(5)
    plants = []
    x = -10
    while x < 640:
        v = int(rng.integers(len(rects)))
        plants.append((x, v, rng.uniform(0, 1)))
        x += int(rng.integers(10, 26))
    order = (0, 1, 2, 3, 2, 1)
    near_speed = 26.0
    flies = [(rng.uniform(80, 560), rng.uniform(280, 330), int(rng.integers(0, 3)), rng.uniform(0, 6)) for _ in range(3)]
    seeds = [(rng.uniform(0, 640), rng.uniform(150, 300), rng.uniform(0, 6)) for _ in range(5)]
    for i in range(seconds * fps):
        t = i / fps
        offs = {k: SPEEDS[k] * near_speed * t + 20 for k in SPEEDS}
        im = compose(layers, offs)
        # Wind going across the field: the sway runs as a wave from left
        # to right.
        for px, v, ph in plants:
            wave = math.sin(2 * math.pi * (t * 0.45 - (px + offs["near"]) / 420.0) + ph * 0.8)
            k = int((wave * 0.5 + 0.5) * 3.99)
            k = order[min(3, k)] if False else min(3, max(0, k))
            fx, fy, fw, fh = rects[v][1][k]
            spr = sheet.crop((fx, fy, fx + fw, fy + fh))
            im.paste(spr, (px, 400 - fh), keyed(spr))
        for bx, by, kind, ph in flies:
            x_ = (bx + 18 * math.sin(t * 0.9 + ph) + t * 6) % 680 - 20
            y_ = by + 10 * math.sin(t * 1.7 + ph * 2)
            k = (0, 1, 2, 1)[int(t * 10 + ph) % 4]
            fx, fy, fw, fh = life_rects[kind][1][k]
            spr = life.crop((fx, fy, fx + fw, fy + fh))
            im.paste(spr, (int(x_), int(y_)), keyed(spr))
        lx, ly = 470 + 6 * math.sin(t * 0.7), 150 + 3 * math.sin(t * 1.3)
        fx, fy, fw, fh = life_rects[3][1][int(t * 6) % 2]
        spr = life.crop((fx, fy, fx + fw, fy + fh))
        im.paste(spr, (int(lx), int(ly)), keyed(spr))
        for sx, sy, ph in seeds:
            x_ = (sx + t * 22) % 660 - 10
            y_ = sy - t * 3 + 6 * math.sin(t * 1.1 + ph)
            fx, fy, fw, fh = life_rects[4][1][int(t * 3 + ph) % 2]
            spr = life.crop((fx, fy, fx + fw, fy + fh))
            im.paste(spr, (int(x_), int(y_)), keyed(spr))
        frames.append(im)
    frames[0].save(out / "remember-preview.webp", save_all=True, append_images=frames[1:],
                   duration=1000 // fps, loop=0, quality=70, method=4)
    return frames


# -- the whole -----------------------------------------------------------------------------------------

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--preview")
    args = ap.parse_args()
    out = Path(args.preview) if args.preview else None
    print("sky", flush=True)
    srgb, sa = sky_layer()
    sky_img, _ = finish(srgb, sa, keyed=False)
    sky_arr = np.asarray(sky_img, np.float32)

    def sky_rgb(xf, y):
        """The painted sky, for still water to show."""
        xs = np.clip((xf * (LAYERS["sky"] - 1)).astype(int), 0, LAYERS["sky"] - 1)
        ys = np.clip(y.astype(int), 0, H - 1)
        return sky_arr[ys, xs]
    print("far", flush=True)
    gf, frgb, fa, _ = far_land(sky_rgb)
    pf = Painter(gf.W, H * S, frgb, fa)

    far_items(pf, gf, far_height, np.random.default_rng(14))
    print("mid", flush=True)
    field = Field()
    gm, mrgb, ma, _ = mid_ground(field, sky_rgb)
    ga = ma.reshape(H, S, -1, S).mean((1, 3))
    crest = int(np.where((ga > 0.99).all(1))[0][0])
    pm = Painter(gm.W, H * S, mrgb, ma)
    mid_items(pm, gm, field, np.random.default_rng(18))
    print("near", flush=True)
    pn = near_layer(field)
    far_img, fa1 = finish(pf.rgb, pf.a)
    mid_img, ma1 = finish(pm.rgb, pm.a)
    near_img, na1 = finish(pn.rgb, pn.a)
    # Each keyed layer keeps only the rows it uses.
    layers = {"sky": (sky_img, 0)}
    info = [("sky", (LAYERS["sky"], 0, H))]
    for name, img, a in (("far", far_img, fa1), ("mid", mid_img, ma1), ("near", near_img, na1)):
        top, bottom = used_rows(a)
        if name == "far":
            # Its foot only needs to reach below the field's crest, where the
            # mid layer's ground is whole across its width.
            bottom = min(bottom, crest + 6)
        else:
            bottom = H
        im = crop_rows(img, top, bottom)
        layers[name] = (im, top)
        info.append((name, (LAYERS[name], top, bottom - top)))
    for name, (img, top) in layers.items():
        img.save(ASSETS / f"remember-{name}.bmp")
    # The flattened panorama, kept for the present.
    flat = compose_flat(layers)
    flat.save(ASSETS / "remember-field.bmp")
    sheet, rects = sway_sheet()
    sheet.save(ASSETS / "remember-poppies.bmp")
    life, life_rects = life_sheet()
    life.save(ASSETS / "remember-life.bmp")
    header(rects, info, life_rects)
    ram = 0
    for name, (w, top, hh) in info:
        ram += w * hh * 2
        print(f"{name:5} {w}x{hh} at y {top}: {w * hh * 2} bytes at 16-bit")
    print("layers in RAM", ram, "bytes; sway sheet", sheet.width * sheet.height * 2,
          "; life sheet", life.width * life.height * 2)
    if out:
        out.mkdir(parents=True, exist_ok=True)
        for name, (img, top) in layers.items():
            img.save(out / f"remember-{name}.png")
        sheet.resize((sheet.width * 4, sheet.height * 4), Image.NEAREST).save(out / "remember-poppies-x4.png")
        life.resize((life.width * 6, life.height * 6), Image.NEAREST).save(out / "remember-life-x6.png")
        frames = preview(layers, sheet, rects, life, life_rects, out)
        frames[0].save(out / "remember-still.png")


def compose_flat(layers):
    """All four layers in one 1600-wide picture: each layer's slide over the
    width scaled to its speed, so the pan reads much as the layers do."""
    W = 1600
    out = Image.new("RGB", (W, H))
    for name in ("sky", "far", "mid", "near"):
        img, top = layers[name]
        sw = img.width
        cols = np.clip((np.arange(W) * (sw - 1) / (W - 1)).astype(int), 0, sw - 1)
        arr = np.asarray(img)[:, cols]
        part = Image.fromarray(np.ascontiguousarray(arr))
        if name == "sky":
            out.paste(part, (0, 0))
        else:
            out.paste(part, (0, top), keyed(part))
    return out


if __name__ == "__main__":
    main()
