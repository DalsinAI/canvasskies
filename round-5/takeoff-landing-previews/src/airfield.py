#!/usr/bin/env python3
"""Take-off and landing previews for Canvas Skies, finished as old newsreel.

Everything is our own: an aerodrome, a biplane, its ground crew and the
weather, built from simple solids and ray-cast through a pinhole camera,
lit by the sun and the sky, then given a period film finish (silver-sepia,
grain, gate weave, flicker, scratches, dust, vignette, a leader or a cut)
at 17 frames a second.

  python3 airfield.py OUTDIR [clip ...]

writes OUTDIR/<clip>.webp (animated) and OUTDIR/<clip>-keys.png for each
clip, and OUTDIR/contact-sheet.png.
"""

import math
import sys
from multiprocessing import Pool
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFilter, ImageFont

W, H = 640, 400
SS = 2
FPS = 17
FOCAL = 520.0


def unit(v):
    v = np.asarray(v, np.float64)
    return v / np.linalg.norm(v)


def rx(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])


def ry(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def rz(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]])


def smooth(t):
    t = min(1.0, max(0.0, t))
    return t * t * (3 - 2 * t)


def lerp(a, b, t):
    return a + (b - a) * t


def noise2(h, w, seed, base=6, octaves=5):
    rng = np.random.default_rng(seed)
    out = np.zeros((h, w), np.float32)
    amp = tot = 0.0
    amp = 1.0
    for o in range(octaves):
        n = base * 2 ** o
        g = rng.random((n + 1, n + 1)).astype(np.float32)
        out += np.asarray(Image.fromarray((g * 255).astype(np.uint8)).resize(
            (w, h), Image.BICUBIC), np.float32) / 255 * amp
        tot += amp
        amp *= 0.5
    return out / tot


# Textures made once, sampled by every frame.
GRASS = noise2(512, 512, 7, base=8)
CLOUD2 = noise2(1024, 1024, 13, base=6, octaves=6)
MUD = noise2(512, 512, 23, base=10)


class Camera:
    def __init__(self, pos, target, roll=0.0):
        self.pos = np.asarray(pos, np.float64)
        f = unit(np.asarray(target, np.float64) - self.pos)
        r = unit(np.cross([0, 1, 0], f))
        u = np.cross(f, r)
        R = np.stack([r, u, f])
        self.R = rz(roll) @ R          # world -> camera
        w, h = W * SS, H * SS
        yy, xx = np.mgrid[0:h, 0:w].astype(np.float64)
        d = np.stack([((xx + 0.5) / SS - W / 2) / FOCAL,
                      (H / 2 - (yy + 0.5) / SS) / FOCAL,
                      np.ones_like(xx)], -1)
        self.dirs = d @ self.R          # world directions, z-scaled

    def project(self, p):
        q = self.R @ (np.asarray(p, np.float64) - self.pos)
        if q[2] <= 0.05:
            return None
        return (W / 2 + FOCAL * q[0] / q[2], H / 2 - FOCAL * q[1] / q[2], q[2])


class Light:
    def __init__(self, sun, sun_col, sky_col, ambient):
        self.sun = unit(sun)
        self.sun_col = np.array(sun_col)
        self.sky_col = np.array(sky_col)
        self.ambient = np.array(ambient)


class Frame:
    def __init__(self, cam, light):
        self.cam, self.light = cam, light
        self.h, self.w = H * SS, W * SS
        self.rgb = np.zeros((self.h, self.w, 3))
        self.z = np.full((self.h, self.w), np.inf)

    # -- primitives --------------------------------------------------------
    def bbox(self, pts, pad):
        xs, ys = [], []
        behind = 0
        total = 0
        for p in pts:
            for dx in (-pad, pad):
                for dy in (-pad, pad):
                    for dz in (-pad, pad):
                        total += 1
                        q = self.cam.project(np.asarray(p) + [dx, dy, dz])
                        if q is None:
                            behind += 1
                            continue
                        xs.append(q[0])
                        ys.append(q[1])
        if behind == total:
            return 0, 0, 0, 0          # wholly behind the camera
        if behind:
            return 0, 0, self.w, self.h
        x0 = max(0, int(min(xs) * SS) - 2)
        x1 = min(self.w, int(max(xs) * SS) + 3)
        y0 = max(0, int(min(ys) * SS) - 2)
        y1 = min(self.h, int(max(ys) * SS) + 3)
        return x0, y0, x1, y1

    def shade(self, box, t, n, albedo, spec=0.0, mask=None, local=None,
              texture=None):
        x0, y0, x1, y1 = box
        hit = np.isfinite(t) & (t > 0.1) & (t < self.z[y0:y1, x0:x1])
        if mask is not None:
            hit &= mask
        if not hit.any():
            return
        n = n / np.maximum(np.linalg.norm(n, axis=-1, keepdims=True), 1e-9)
        d = self.cam.dirs[y0:y1, x0:x1]
        view = -d / np.linalg.norm(d, axis=-1, keepdims=True)
        # Two-sided: thin wings are lit from whichever face we see.
        flip = (n * view).sum(-1) < 0
        n = np.where(flip[..., None], -n, n)
        L = self.light
        alb = np.broadcast_to(np.asarray(albedo, np.float64), n.shape).copy()
        if texture is not None:
            alb *= texture(local)[..., None]
        lam = np.clip((n * L.sun).sum(-1), 0, 1)[..., None]
        sky = (0.55 + 0.45 * n[..., 1:2])
        col = alb * (L.ambient + L.sky_col * sky + L.sun_col * lam)
        if spec:
            hv = unit(L.sun) + view
            hv /= np.linalg.norm(hv, axis=-1, keepdims=True)
            col += L.sun_col * spec * np.clip((n * hv).sum(-1), 0, 1)[..., None] ** 20
        rim = (1 - np.clip((n * view).sum(-1), 0, 1)) ** 4
        col += L.sun_col * 0.15 * rim[..., None] * np.clip(
            (view * -L.sun).sum(-1) + 0.2, 0, 1)[..., None]
        self.rgb[y0:y1, x0:x1][hit] = col[hit]
        self.z[y0:y1, x0:x1][hit] = t[hit]

    def capsule(self, a, b, r, albedo, spec=0.0):
        a, b = np.asarray(a, np.float64), np.asarray(b, np.float64)
        box = self.bbox([a, b], r)
        if box[0] >= box[2] or box[1] >= box[3]:
            return
        d = self.cam.dirs[box[1]:box[3], box[0]:box[2]]
        o = self.cam.pos
        ba = b - a
        baba = ba @ ba
        dd = (d * d).sum(-1)
        oa = o - a
        bard = d @ ba
        baoa = oa @ ba
        rdoa = d @ oa
        oaoa = oa @ oa
        best = np.full(dd.shape, np.inf)
        if baba > 1e-9:
            A = baba * dd - bard * bard
            B = baba * rdoa - baoa * bard
            C = baba * oaoa - baoa * baoa - r * r * baba
            h = B * B - A * C
            with np.errstate(invalid="ignore", divide="ignore"):
                t = (-B - np.sqrt(h)) / A
                y = baoa + t * bard
            best = np.where((h >= 0) & (y > 0) & (y < baba), t, np.inf)
        for c in (a, b):
            oc = o - c
            bb = d @ oc
            cc = oc @ oc - r * r
            hh = bb * bb - dd * cc
            with np.errstate(invalid="ignore"):
                ts = (-bb - np.sqrt(hh)) / dd
            best = np.minimum(best, np.where(hh >= 0, ts, np.inf))
        pos = o + d * np.where(np.isfinite(best), best, 0)[..., None]
        s = np.clip(((pos - a) @ ba) / max(baba, 1e-9), 0, 1)
        n = pos - (a + s[..., None] * ba)
        self.shade(box, best, n, albedo, spec)

    def box(self, centre, R, half, albedo, spec=0.0, holes=None,
            texture=None):
        """An oriented box; R's rows are its axes in the world."""
        c = np.asarray(centre, np.float64)
        hs = np.asarray(half, np.float64)
        corners = [c + R.T @ (hs * np.array(s)) for s in
                   ((1, 1, 1), (1, 1, -1), (1, -1, 1), (1, -1, -1),
                    (-1, 1, 1), (-1, 1, -1), (-1, -1, 1), (-1, -1, -1))]
        box = self.bbox(corners, 0.0)
        if box[0] >= box[2] or box[1] >= box[3]:
            return
        d = self.cam.dirs[box[1]:box[3], box[0]:box[2]]
        o = R @ (self.cam.pos - c)
        dl = d @ R.T
        with np.errstate(divide="ignore", invalid="ignore"):
            t1 = (-hs - o) / dl
            t2 = (hs - o) / dl
        tn = np.nan_to_num(np.minimum(t1, t2), nan=-np.inf)
        tf = np.nan_to_num(np.maximum(t1, t2), nan=np.inf)
        tmin, tmax = tn.max(-1), tf.min(-1)
        ok = (tmax >= tmin) & (tmax > 0)
        t = np.where(ok, tmin, np.inf)
        face = np.argmax(tn, -1)
        q = o + dl * np.where(np.isfinite(t), t, 0)[..., None]
        nl = np.zeros_like(q)
        ii, jj = np.indices(face.shape)
        nl[ii, jj, face] = np.sign(q[ii, jj, face])
        mask = None
        u = q / hs
        if holes is not None:
            mask = holes(u)
        self.shade(box, t, nl @ R, albedo, spec, mask=mask, local=u,
                   texture=texture)

    def ellipsoid(self, centre, R, radii, albedo, spec=0.0):
        c = np.asarray(centre, np.float64)
        r = np.asarray(radii, np.float64)
        box = self.bbox([c], r.max())
        if box[0] >= box[2] or box[1] >= box[3]:
            return
        d = self.cam.dirs[box[1]:box[3], box[0]:box[2]]
        o = (R @ (self.cam.pos - c)) / r
        dl = (d @ R.T) / r
        A = (dl * dl).sum(-1)
        B = (dl * o).sum(-1)
        C = o @ o - 1
        h = B * B - A * C
        with np.errstate(invalid="ignore"):
            t = (-B - np.sqrt(h)) / A
        t = np.where(h >= 0, t, np.inf)
        q = o + dl * np.where(np.isfinite(t), t, 0)[..., None]
        self.shade(box, t, (q / r) @ R, albedo, spec)

    # -- the world around ----------------------------------------------------
    def backdrop(self, sky_top, sky_low, ground, haze, sun_glow=0.0,
                 mud=0.0, wet=0.0, mown=True, cloud=0.5, seed=0):
        d = self.cam.dirs
        dn = d / np.linalg.norm(d, axis=-1, keepdims=True)
        el = dn[..., 1]
        L = self.light
        # Sky: gradient, the low sun's glow and cloud by direction.
        az = np.arctan2(dn[..., 0], dn[..., 2])
        t = np.clip(el / 0.5, 0, 1)[..., None]
        sky = np.asarray(sky_low) * (1 - t) + np.asarray(sky_top) * t
        if sun_glow:
            g = np.clip((dn * L.sun).sum(-1), 0, 1) ** 24
            sky = sky + np.asarray(L.sun_col) * sun_glow * g[..., None]
        # Cloud on a flat ceiling: no seam, and it runs to the horizon.
        e = np.maximum(el, 0.015)
        cu = ((dn[..., 0] / e * 0.9 + 0.5 + seed * 0.37) * 300).astype(np.int64) % CLOUD2.shape[1]
        cv = ((dn[..., 2] / e * 0.9) * 300).astype(np.int64) % CLOUD2.shape[0]
        c = CLOUD2[cv, cu]
        cover = np.clip((c - (1 - cloud)) * 3.0, 0, 1) * np.clip(el / 0.03, 0, 1)
        cover = cover[..., None]
        sky = sky * (1 - cover * 0.55) + cover * 0.5 * np.asarray(sky_top) * 1.6
        out = sky
        # Ground: the plane y = 0.
        o = self.cam.pos
        with np.errstate(divide="ignore", invalid="ignore"):
            tg = -o[1] / d[..., 1]
        gnd = (d[..., 1] < -1e-4) & (tg > 0)
        px = o[0] + d[..., 0] * tg
        pz = o[2] + d[..., 2] * tg
        dist = np.where(gnd, tg * np.linalg.norm(d, axis=-1), np.inf)
        gx = (px * 6).astype(np.int64) % 512
        gz = (pz * 6).astype(np.int64) % 512
        gx2 = (px * 0.6).astype(np.int64) % 512
        gz2 = (pz * 0.6).astype(np.int64) % 512
        tex = 0.75 + 0.35 * GRASS[gz, gx] * (1 - np.clip(dist / 120, 0, 0.8)) + \
            0.25 * (GRASS[gz2, gx2] - 0.5)
        if mown:
            tex *= 1 + 0.03 * np.sin(px * 0.35)
        albedo = np.asarray(ground)[None, None, :] * tex[..., None]
        if mud:
            m = MUD[(pz * 1.5).astype(np.int64) % 512, (px * 1.5).astype(np.int64) % 512]
            puddle = np.clip((m - 0.62) * 9, 0, 1) * mud
            brown = np.array([0.30, 0.25, 0.18]) * (0.8 + 0.4 * GRASS[gz, gx])[..., None]
            albedo = albedo * (1 - mud * 0.6) + brown * mud * 0.6
            albedo = albedo * (1 - puddle[..., None]) + np.asarray(sky_low) * 0.8 * puddle[..., None] / np.maximum(L.sun_col.mean() + L.sky_col.mean(), 0.5)
        lam = max(0.0, L.sun[1])
        gcol = albedo * (L.ambient + L.sky_col + L.sun_col * lam)
        if wet:
            gcol = gcol * (1 - wet * 0.3) + np.asarray(sky_low) * wet * 0.25
        f = 1 - np.exp(-np.where(np.isfinite(dist), dist, 1e9) / haze)
        gcol = gcol * (1 - f[..., None]) + np.asarray(sky_low) * f[..., None]
        out = np.where(gnd[..., None], gcol, out)
        self.rgb[:] = out
        self.z = np.where(gnd, dist, np.inf)
        self.ground = (gnd, px, pz)

    def shadow(self, x, z, rx_, rz_, strength=0.45, yaw=0.0):
        """A soft shadow on the ground, an ellipse at (x, z)."""
        gnd, px, pz = self.ground
        c, s = math.cos(yaw), math.sin(yaw)
        u = ((px - x) * c - (pz - z) * s) / rx_
        v = ((px - x) * s + (pz - z) * c) / rz_
        r2 = u * u + v * v
        k = np.clip(1.2 - r2, 0, 1) * strength
        m = gnd & (k > 0) & (self.z >= 0)
        self.rgb[m] *= (1 - k[m])[:, None]

    def haze_objects(self, haze, colour):
        f = 1 - np.exp(-np.where(np.isfinite(self.z), self.z, 1e9) / haze)
        obj = np.isfinite(self.z) & ~self.ground[0]
        self.rgb[obj] = self.rgb[obj] * (1 - f[obj][:, None]) + \
            np.asarray(colour) * f[obj][:, None]

    def soft(self, p, radius, colour, alpha):
        """A soft round puff in front of what lies behind it: smoke, dust,
        spray, the propeller's disc."""
        q = self.cam.project(p)
        if q is None:
            return
        cx, cy, depth = q
        r = radius * FOCAL / depth * SS
        x0, x1 = int(max(0, cx * SS - r)), int(min(self.w, cx * SS + r + 1))
        y0, y1 = int(max(0, cy * SS - r)), int(min(self.h, cy * SS + r + 1))
        if x0 >= x1 or y0 >= y1 or r < 0.5:
            return
        yy, xx = np.mgrid[y0:y1, x0:x1]
        d2 = ((xx - cx * SS) ** 2 + (yy - cy * SS) ** 2) / (r * r)
        a = np.clip(1 - d2, 0, 1) ** 1.5 * alpha
        a = np.where(self.z[y0:y1, x0:x1] > depth, a, 0)
        reg = self.rgb[y0:y1, x0:x1]
        reg[:] = reg * (1 - a[..., None]) + np.asarray(colour) * a[..., None]

    def finish(self):
        img = self.rgb.reshape(H, SS, W, SS, 3).mean((1, 3))
        return img


# -- the machines and the men --------------------------------------------------

DOPE = np.array([0.62, 0.58, 0.46])
DOPE_DARK = np.array([0.42, 0.40, 0.30])
WOOD = np.array([0.40, 0.27, 0.15])
METAL = np.array([0.42, 0.42, 0.42])
TYRE = np.array([0.08, 0.08, 0.08])
LEATHER = np.array([0.30, 0.20, 0.12])
KHAKI = np.array([0.38, 0.35, 0.24])
OVERALL = np.array([0.32, 0.30, 0.26])
SKIN = np.array([0.66, 0.50, 0.40])


def holes_fn(seed, amount):
    rng = np.random.default_rng(seed)
    spots = rng.random((int(40 * amount) + 1, 3))

    def holes(u):
        keep = np.ones(u.shape[:2], bool)
        for sx, sz, r in spots:
            x = sx * 2 - 1
            z = sz * 2 - 1
            rr = 0.02 + r * 0.06
            keep &= ((u[..., 0] - x) ** 2 * 4 + (u[..., 2] - z) ** 2) > rr * rr * 4
        return keep
    return holes


def ribs(local):
    """Fabric over ribs: faint stripes across the wing."""
    return 1 + 0.05 * np.sin(local[..., 0] * 28.0)


def biplane(f, pos, R, prop_angle=None, prop_spin=False, two_seat=False,
            damage=0.0, seed=0, pilot=True, observer=False, dope=DOPE):
    """A biplane at 'pos' (the centre of its lower wing root), its axes the
    rows of R: span (x), up (y), nose (z)."""
    pos = np.asarray(pos, np.float64)

    def P(x, y, z):
        return pos + R.T @ np.array([x, y, z])

    def B(c, half, alb, **kw):
        f.box(P(*c), R, half, alb, **kw)

    def C(a, b, r, alb, **kw):
        f.capsule(P(*a), P(*b), r, alb, **kw)

    L = 1.25 if two_seat else 1.0
    wing_holes = holes_fn(seed, damage) if damage else None
    tatters = holes_fn(seed + 9, damage * 0.7) if damage else None
    # Fuselage, nose to tail, narrowing.
    C((0, 0.55, 1.55), (0, 0.55, 1.95), 0.42, METAL * 0.7, spec=0.4)  # cowl
    B((0, 0.55, 0.95), (0.40, 0.44, 0.65), dope, holes=tatters)
    B((0, 0.55, -0.3 * L), (0.37, 0.40, 0.65 * L), dope, holes=tatters, texture=ribs)
    B((0, 0.55, -1.55 * L), (0.27, 0.30, 0.65 * L), dope, holes=tatters)
    B((0, 0.58, -2.75 * L), (0.15, 0.20, 0.6 * L), dope)
    # Cockpits, and the heads in them.
    B((0, 0.96, -0.15), (0.24, 0.02, 0.32), np.array([0.05, 0.05, 0.05]))
    if pilot:
        f.ellipsoid(P(0, 1.08, -0.15), R, (0.15, 0.17, 0.16), LEATHER, spec=0.2)
    if two_seat:
        B((0, 0.92, -1.05), (0.22, 0.02, 0.28), np.array([0.05, 0.05, 0.05]))
        if observer:
            f.ellipsoid(P(0, 1.03, -1.05), R, (0.15, 0.17, 0.16), LEATHER, spec=0.2)
    # Wings: upper above the cockpit, lower at the root.
    for y, half_span, z in ((1.75, 4.2, 0.55), (0.12, 3.9, 0.35)):
        B((0, y, z), (half_span, 0.05, 0.72), dope, holes=wing_holes, texture=ribs)
        B((0, y + 0.03, z + 0.70), (half_span, 0.04, 0.05), dope * 0.9)   # leading edge
    for side in (-1, 1):
        for zz in (0.0, 0.95):
            C((side * 2.8, 0.15, zz + 0.05), (side * 2.8, 1.72, zz + 0.25), 0.04, WOOD)
        C((side * 0.35, 0.95, 0.9), (side * 0.3, 1.72, 0.85), 0.035, WOOD)
        C((side * 0.35, 0.95, 0.1), (side * 0.3, 1.72, 0.2), 0.035, WOOD)
        # Bracing wires as thin dark lines.
        C((side * 0.4, 0.15, 0.5), (side * 2.8, 1.72, 0.5), 0.008, METAL)
        C((side * 0.4, 1.72, 0.5), (side * 2.8, 0.15, 0.5), 0.008, METAL)
        # Roundels on the upper wing.
        f.ellipsoid(P(side * 3.0, 1.81, 0.55), R, (0.55, 0.012, 0.55), np.array([0.15, 0.17, 0.30]))
        f.ellipsoid(P(side * 3.0, 1.815, 0.55), R, (0.37, 0.012, 0.37), np.array([0.85, 0.83, 0.78]))
        f.ellipsoid(P(side * 3.0, 1.82, 0.55), R, (0.18, 0.012, 0.18), np.array([0.55, 0.12, 0.10]))
    # Fuselage roundels.
    for side in (-1, 1):
        f.ellipsoid(P(side * 0.29, 0.58, -1.6 * L), R, (0.012, 0.25, 0.25), np.array([0.15, 0.17, 0.30]))
        f.ellipsoid(P(side * 0.295, 0.58, -1.6 * L), R, (0.012, 0.16, 0.16), np.array([0.85, 0.83, 0.78]))
        f.ellipsoid(P(side * 0.30, 0.58, -1.6 * L), R, (0.012, 0.08, 0.08), np.array([0.55, 0.12, 0.10]))
    # Undercarriage: V struts, the axle and two wheels.
    for side in (-1, 1):
        C((side * 0.3, 0.2, 1.3), (side * 0.75, -0.75, 1.05), 0.04, WOOD)
        C((side * 0.3, 0.2, 0.5), (side * 0.75, -0.75, 1.0), 0.04, WOOD)
        f.ellipsoid(P(side * 0.85, -0.8, 1.02), R, (0.08, 0.36, 0.36), TYRE, spec=0.2)
        f.ellipsoid(P(side * 0.86, -0.8, 1.02), R, (0.085, 0.18, 0.18), np.array([0.6, 0.6, 0.58]))
    C((-0.8, -0.8, 1.02), (0.8, -0.8, 1.02), 0.03, METAL)
    # Tail: tailplane, fin and rudder, skid.
    tl = -3.3 * L
    B((0, 0.62, tl - 0.15), (1.5, 0.03, 0.42), dope, holes=wing_holes)
    B((0, 1.05, tl - 0.25), (0.03, 0.45, 0.35), dope)
    B((0, 1.0, tl - 0.68), (0.035, 0.52, 0.12), np.array([0.55, 0.14, 0.12]))
    B((0, 1.0, tl - 0.56), (0.036, 0.52, 0.04), np.array([0.85, 0.83, 0.78]))
    B((0, 1.0, tl - 0.46), (0.037, 0.52, 0.06), np.array([0.15, 0.17, 0.30]))
    C((0, 0.45, tl + 0.1), (0, 0.05, tl - 0.25), 0.04, WOOD)
    # Propeller: a pale disc when turning, two blades when still.
    hub = P(0, 0.55, 2.15)
    f.ellipsoid(hub, R, (0.12, 0.12, 0.12), METAL, spec=0.5)
    if prop_spin:
        return [(hub, 1.35)]
    a = prop_angle or 0.0
    for k in (0, math.pi):
        d = np.array([math.cos(a + k), math.sin(a + k), 0])
        f.box(hub + R.T @ (d * 0.68), rz_in(R, a + k), (0.68, 0.08, 0.03), WOOD, spec=0.3)
    return []


def rz_in(R, a):
    """Axes for a blade turned by a about the machine's nose axis."""
    c, s = math.cos(a), math.sin(a)
    x = R[0] * c + R[1] * s
    y = -R[0] * s + R[1] * c
    return np.stack([x, y, R[2]])


def prop_disc(f, hub, R, spin_light=0.55):
    """The turning airscrew: a translucent pale disc with a darker rim."""
    for k in range(5):
        r = 1.35 * (1 - k * 0.18)
        f.soft(hub + R[2] * (0.02 * k), r, np.array([0.55, 0.50, 0.42]) * spin_light / 0.55,
               0.10)


def man(f, foot, facing, pose, t=0.0, cloth=OVERALL, cap=True):
    """A man standing at 'foot' facing 'facing' (radians about y).

    pose: 'stand', 'walk', 'run', 'swing' (arms up then down: t 0..1),
    'pull' (crouched, hauling backwards), 'wave', 'climb' (t 0..1 up a
    step into a cockpit)."""
    foot = np.asarray(foot, np.float64)
    Ry = ry(facing)

    def P(x, y, z):
        return foot + Ry @ np.array([x, y, z])

    lean = 0.0
    hip = 0.92
    stride = 0.0
    arm_l = arm_r = (0.0, -0.55, 0.05)
    if pose == "walk" or pose == "run":
        k = 0.35 if pose == "walk" else 0.6
        stride = math.sin(t * (9 if pose == "run" else 6)) * k
        lean = 0.25 if pose == "run" else 0.05
        arm_l = (0.0, -0.45, -stride * 0.8)
        arm_r = (0.0, -0.45, stride * 0.8)
    elif pose == "swing":
        a = smooth(t * 2) if t < 0.5 else 1 - smooth((t - 0.5) * 2) * 0.0
        reach = 0.65 - 1.2 * smooth(max(0, t - 0.35) / 0.4)
        arm_l = arm_r = (0.0, reach, 0.45)
        lean = 0.15 + 0.25 * smooth(max(0, t - 0.35) / 0.4)
    elif pose == "pull":
        hip = 0.62
        lean = -0.35
        arm_l = arm_r = (0.0, -0.25, 0.55)
    elif pose == "wave":
        arm_r = (0.0, 0.6, 0.0 + 0.25 * math.sin(t * 12))
    elif pose == "climb":
        up = smooth(t) * 1.0
        foot = foot + np.array([0, up, 0])
    hipp = P(0, hip, 0)
    for side, s in ((-1, stride), (1, -stride)):
        knee = P(side * 0.12, hip * 0.5, s * 0.3 + (0.12 if pose == "pull" else 0))
        f.capsule(P(side * 0.12, hip, 0), knee, 0.085, cloth)
        f.capsule(knee, P(side * 0.12, 0.08, s * 0.5), 0.075, cloth)
        f.capsule(P(side * 0.12, 0.06, s * 0.5), P(side * 0.12, 0.06, s * 0.5 + 0.16), 0.06,
                  LEATHER)
    sh = hipp + Ry @ np.array([0, 0.55, lean * 0.5])
    f.capsule(hipp, sh, 0.19, cloth)
    for side, arm in ((-1, arm_l), (1, arm_r)):
        s0 = sh + Ry @ np.array([side * 0.22, 0, 0])
        hand = s0 + Ry @ np.array([side * 0.05 + arm[0], arm[1], arm[2]])
        f.capsule(s0, hand, 0.065, cloth)
        f.ellipsoid(hand, Ry.T, (0.06, 0.06, 0.06), SKIN)
    head = sh + Ry @ np.array([0, 0.27, lean * 0.15])
    f.ellipsoid(head, Ry.T, (0.11, 0.13, 0.12), SKIN)
    if cap:
        f.ellipsoid(head + np.array([0, 0.08, 0]), Ry.T, (0.13, 0.06, 0.14),
                    np.array([0.2, 0.19, 0.17]))


def hangar(f, x, z, yaw, w=16.0, d=12.0, h=6.0):
    Ry = ry(yaw)
    R = Ry.T
    c = np.array([x, h / 2, z])
    f.box(c, R, (w / 2, h / 2, d / 2), np.array([0.36, 0.34, 0.30]))
    # The curved canvas roof as a long ellipsoid.
    f.ellipsoid(c + np.array([0, h / 2, 0]), R, (w / 2 * 1.02, h * 0.45, d / 2 * 1.02),
                np.array([0.55, 0.52, 0.44]))
    # The open doors: a dark mouth on the front.
    front = c + Ry @ np.array([0, -h * 0.12, d / 2 + 0.02])
    f.box(front, R, (w * 0.38, h * 0.38, 0.02), np.array([0.04, 0.04, 0.04]))


def treeline(f, z0, x0, x1, seed, height=9.0):
    rng = np.random.default_rng(seed)
    x = x0
    while x < x1:
        r = rng.uniform(3, 6)
        hh = height * rng.uniform(0.7, 1.2)
        f.ellipsoid((x, hh * 0.55, z0 + rng.uniform(-4, 4)), np.eye(3),
                    (r, hh * 0.55, r), np.array([0.20, 0.25, 0.16]))
        x += r * 1.3


def windsock(f, x, z, wind=1.0):
    f.capsule((x, 0, z), (x, 6, z), 0.06, np.array([0.5, 0.5, 0.5]))
    for k in range(5):
        f.capsule((x + k * 0.35 * wind, 5.9 - k * 0.05, z), (x + (k + 1) * 0.35 * wind, 5.85 - (k + 1) * 0.06, z),
                  0.25 - k * 0.03, np.array([0.85, 0.8, 0.7]) if k % 2 == 0 else np.array([0.55, 0.12, 0.1]))


# -- the clips ---------------------------------------------------------------------

def attitude(yaw, pitch=0.0, roll=0.0):
    """Rows: span, up, nose, for a machine heading 'yaw' (about y; 0 =
    along +z), nose up by 'pitch', right wing down by 'roll'."""
    M = ry(yaw) @ rx(-pitch) @ rz(-roll)
    return M.T


GROUND_PITCH = math.radians(11)   # tail down on its skid
WHEEL_H = 1.16                     # lower wing root above the ground


def scene_lights(kind):
    if kind == "dawn":
        return Light((0.6, 0.12, 0.8), (1.25, 0.95, 0.70), (0.30, 0.32, 0.40), (0.06, 0.06, 0.07))
    if kind == "dusk":
        return Light((-0.7, 0.10, 0.7), (1.30, 0.80, 0.50), (0.28, 0.27, 0.36), (0.06, 0.05, 0.06))
    if kind == "rain":
        return Light((0.2, 0.8, 0.3), (0.35, 0.36, 0.38), (0.55, 0.57, 0.62), (0.10, 0.10, 0.11))
    if kind == "overcast":
        return Light((0.3, 0.7, 0.4), (0.55, 0.53, 0.50), (0.50, 0.50, 0.52), (0.08, 0.08, 0.08))
    return Light((0.4, 0.6, 0.5), (1.05, 0.98, 0.86), (0.36, 0.40, 0.48), (0.06, 0.06, 0.07))


def world(f, sky, mud=0.0, wet=0.0, seed=0, haze=260.0, sun_glow=0.0, cloud=0.5):
    tops = {"dawn": (0.42, 0.45, 0.58), "dusk": (0.30, 0.30, 0.42), "rain": (0.42, 0.43, 0.46),
            "overcast": (0.55, 0.56, 0.58), "day": (0.42, 0.52, 0.70)}
    lows = {"dawn": (0.95, 0.80, 0.62), "dusk": (0.92, 0.66, 0.45), "rain": (0.58, 0.58, 0.60),
            "overcast": (0.72, 0.72, 0.72), "day": (0.80, 0.82, 0.84)}
    f.backdrop(tops[sky], lows[sky], (0.30, 0.36, 0.20), haze, sun_glow=sun_glow,
               mud=mud, wet=wet, cloud=cloud, seed=seed)
    treeline(f, 170, -200, 200, 3)
    treeline(f, -150, -200, 200, 5, height=7)
    hangar(f, -34, 40, 0.15)
    hangar(f, -52, 36, 0.15, w=14)
    windsock(f, 46, 70, 1.0)
    return np.asarray(lows[sky])


def clip_takeoff_dawn(t):
    """Dawn: a mechanic swings the airscrew, another pulls the chocks,
    and the machine rolls away and climbs into the low sun."""
    L = scene_lights("dawn")
    roll_ = smooth((t - 2.8) / 3.0)
    zz_ = 30 * roll_ * roll_ * 1.6
    cam = Camera((11.0, 2.0, -5.0), (0.0, 1.4 + 2.0 * smooth((t - 4.6) / 1.4), 1.0 + zz_ * 0.85))
    f = Frame(cam, L)
    low = world(f, "dawn", sun_glow=1.4, haze=150.0, cloud=0.35)
    roll = smooth((t - 2.8) / 3.0)
    z = 0.0 + 30 * roll * roll * 1.6
    lift = smooth((t - 4.7) / 1.3)
    pitch = GROUND_PITCH * (1 - smooth((t - 3.6) / 0.8)) + math.radians(6) * lift
    y = WHEEL_H + 3.0 * lift * lift
    R = attitude(0.0, pitch)
    pos = np.array([0, y, z])
    spinning = t > 1.25
    blade = math.radians(50) - math.radians(140) * smooth((t - 0.7) / 0.35)
    f.shadow(0, z + 0.5, 4.0 * (1 - lift * 0.4), 2.5, strength=0.5 - lift * 0.3)
    discs = biplane(f, pos, R, prop_angle=blade, prop_spin=spinning)
    # The man at the airscrew: swing, then step back smartly.
    back = smooth((t - 1.1) / 0.6)
    man(f, (0.4 + back * 1.6, 0, 3.4 + back * 0.4), math.pi, "swing" if t < 1.4 else "stand",
        t=min(1.0, t / 1.3))
    # The man at the chocks: hauls them clear, then waves the machine off.
    if t < 2.6:
        man(f, (1.2, 0, 1.0 + 0.2 * smooth((t - 1.8) / 0.6)), -math.pi / 2,
            "pull" if t > 1.7 else "stand", t=t)
        f.box((0.9 + 0.5 * smooth((t - 1.8) / 0.6), 0.12, 1.05), np.eye(3), (0.25, 0.12, 0.15), WOOD)
    else:
        man(f, (2.4, 0, 1.4), -math.pi / 2 - 0.4, "wave", t=t)
        f.box((1.6, 0.12, 1.1), np.eye(3), (0.25, 0.12, 0.15), WOOD)
    for d in discs:
        prop_disc(f, d[0], R)
    if spinning:
        # Exhaust smoke from the start, blown back along the fuselage.
        for k in range(10):
            age = (t - 1.25) * 1.5 - k * 0.12
            if 0 < age < 1.6:
                p = pos + R.T @ np.array([0.5, 0.6, 1.6 - age * 4.0]) + np.array([0, age * 0.6, 0])
                f.soft(p, 0.3 + age * 0.6, low * 0.75, 0.35 * (1 - age / 1.6))
    if roll > 0:
        for k in range(8):
            age = (k * 0.13 + t * 3) % 1.0
            p = pos + R.T @ np.array([((k % 2) * 2 - 1) * 0.9, -1.1 * (1 - lift), 1.0 - age * 4.5]) + np.array([0, age, 0])
            if lift < 0.5:
                f.soft(p, 0.4 + age, np.array([0.6, 0.52, 0.42]), 0.25 * (1 - age) * min(1, roll * 3))
    f.haze_objects(150.0, low)
    return f.finish()


def clip_takeoff_three(t):
    """A flight of three in a vee rolls across the field and climbs
    together past the camera."""
    L = scene_lights("day")
    base_ = -10 + 65 * (t / 5.5) ** 1.6
    cam = Camera((30, 1.8, 18), (0, 2.0 + 5 * smooth((t - 3) / 2), base_ - 3))
    f = Frame(cam, L)
    low = world(f, "day", haze=300.0, cloud=0.6)
    run = smooth(t / 5.5)
    base = -10 + 65 * (t / 5.5) ** 1.6
    for k, (dx, dz) in enumerate(((0, 0), (-12, -9), (12, -9))):
        z = base + dz
        lift = smooth((t - 3.0 - k * 0.15) / 1.6)
        y = WHEEL_H + 6 * lift * lift
        pitch = GROUND_PITCH * (1 - smooth((t - 1.8) / 0.8)) + math.radians(7) * lift
        R = attitude(0.0, pitch, 0.03 * math.sin(t * 3 + k))
        pos = np.array([dx, y, z])
        f.shadow(dx, z + 0.5, 4.0, 2.5, strength=0.45 - lift * 0.3)
        discs = biplane(f, pos, R, prop_spin=True, seed=k)
        for d in discs:
            prop_disc(f, d[0], R)
        if lift < 0.6:
            for j in range(6):
                age = (j / 6 + t * 2.2) % 1.0
                p = np.array([dx + ((j % 2) * 2 - 1) * 0.9, 0.3 + age * 1.2, z + 0.6 - age * 6])
                f.soft(p, 0.5 + age * 1.2, np.array([0.62, 0.58, 0.50]), 0.22 * (1 - age))
    f.haze_objects(300.0, low)
    return f.finish()


def clip_takeoff_mud(t):
    """A muddy field: the machine wallows, bounces off a rut, throws up
    mud and gets off at the second try."""
    L = scene_lights("overcast")
    zc_ = 6 + 34 * (t / 5.0) ** 1.5
    cam = Camera((-9.0, 1.1, -6.0), (0, 1.5 + 2 * smooth((t - 3.6) / 1.4), zc_ * 0.9))
    f = Frame(cam, L)
    low = world(f, "overcast", mud=1.0, haze=200.0, cloud=0.8)
    z = 6 + 34 * (t / 5.0) ** 1.5
    bounce = max(0.0, math.sin((t - 2.1) * 4.5)) * 0.9 * (1 if 2.1 < t < 2.8 else 0)
    lift = smooth((t - 3.6) / 1.4)
    wallow = math.sin(t * 7.0) * 0.05 * (1 - lift)
    y = WHEEL_H + bounce + 4 * lift * lift
    pitch = GROUND_PITCH * (1 - smooth((t - 1.5) / 1.0)) + math.radians(8) * lift + \
        math.radians(4) * bounce
    R = attitude(0.0, pitch, wallow)
    pos = np.array([0, y, z])
    f.shadow(0, z + 0.5, 4.0, 2.5, strength=0.4)
    discs = biplane(f, pos, R, prop_spin=True, dope=DOPE_DARK * 1.1)
    for d in discs:
        prop_disc(f, d[0], R)
    rng = np.random.default_rng(int(t * FPS))
    if lift < 0.7:
        # Mud thrown up and back by the wheels.
        for side in (-1, 1):
            w = pos + R.T @ np.array([side * 0.85, -1.15, 1.0])
            w[1] = 0.1
            for k in range(14):
                age = rng.random()
                p = w + np.array([side * age * 0.8 + rng.normal(0, 0.2), age * 1.6 - age * age * 1.4, -age * 3.5])
                f.soft(p, 0.08 + 0.12 * rng.random(), np.array([0.18, 0.14, 0.10]), 0.9)
    f.haze_objects(200.0, low)
    return f.finish()


def clip_takeoff_1914(t):
    """1914: the observer climbs into the back of the two-seater, the
    pilot already in front, and they trundle away."""
    L = scene_lights("day")
    cam = Camera((-10.5, 2.0, 1.0), (0.0, 1.2, -0.6 + 12 * smooth((t - 3.0) / 2.5)))
    f = Frame(cam, L)
    low = world(f, "day", haze=280.0, cloud=0.5)
    roll = smooth((t - 3.0) / 2.5)
    z = 18 * roll * roll
    R = attitude(0.0, GROUND_PITCH * (1 - 0.5 * smooth((t - 4.4) / 1.0)))
    pos = np.array([0, WHEEL_H, z])
    f.shadow(0, z, 4.2, 3.0)
    seated = t > 2.3
    discs = biplane(f, pos, R, prop_spin=True, two_seat=True, observer=seated,
                    dope=np.array([0.66, 0.62, 0.52]))
    for d in discs:
        prop_disc(f, d[0], R)
    if not seated:
        # Up the step on the lower wing root and over the coaming.
        k = smooth(t / 2.3)
        foot = pos + R.T @ np.array([-0.9 + 0.6 * k, -1.16 + 1.2 * k, -1.05])
        man(f, foot, math.pi / 2, "climb" if t > 0.6 else "walk", t=k, cloth=KHAKI)
    man(f, (-3.4, 0, 4.2), math.pi / 2 + 0.6, "wave" if t > 2.6 else "stand", t=t)
    f.haze_objects(280.0, low)
    return f.finish()


def approach(t, t_touch, z_touch, glide=math.radians(5), speed=24.0, flare=0.9):
    """Height, distance and pitch on a gliding approach that touches down
    at t_touch, z_touch and then rolls out, slowing."""
    if t < t_touch:
        dt = t_touch - t
        z = z_touch - speed * dt
        h = speed * dt * math.tan(glide)
        f_ = smooth(1 - dt / flare)
        h = h * (1 - 0.7 * f_)
        pitch = -glide * (1 - f_) + GROUND_PITCH * f_
        return h, z, pitch
    s = min(t - t_touch, 1.8)
    z = z_touch + speed * s * (1 - s / 3.6)       # slowing to a stop
    return 0.0, z, GROUND_PITCH


def clip_land_dusk(t):
    """Dusk: a clean three-point landing, rolling out towards us."""
    L = scene_lights("dusk")
    h_, z_, _ = approach(t, 3.2, 22.0, speed=17.0, flare=1.3)
    cam = Camera((9, 1.6, 50), (0, 1.5 + h_ * 0.8, z_))
    f = Frame(cam, L)
    low = world(f, "dusk", sun_glow=1.2, haze=180.0, cloud=0.45)
    h, z, pitch = approach(t, 3.2, 22.0, speed=17.0, flare=1.3)
    R = attitude(0.0, pitch)
    pos = np.array([0, WHEEL_H + h, z])
    f.shadow(0, z + 0.5, 4.0, 2.5, strength=0.5 * smooth(1 - h / 10))
    discs = biplane(f, pos, R, prop_spin=True)
    for d in discs:
        prop_disc(f, d[0], R, spin_light=0.45)
    if 3.2 < t < 4.6:
        for k in range(6):
            age = ((k / 6) + t * 2) % 1.0
            p = pos + np.array([((k % 2) * 2 - 1) * 0.9, -1.0 + age, 1.0 - age * 3])
            f.soft(p, 0.4 + age, low * 0.8, 0.25 * (1 - age))
    f.haze_objects(180.0, low)
    return f.finish()


def clip_land_shot_up(t):
    """Shot up: tattered fabric, a wisp of smoke, a hard bounce, a lurch
    onto one wheel and the ground crew running out."""
    L = scene_lights("day")
    cam = Camera((16, 1.6, 34), (0, 2.0, 14 + 10 * smooth((t - 2.5) / 3)))
    f = Frame(cam, L)
    low = world(f, "day", haze=280.0, cloud=0.55)
    h, z, pitch = approach(t, 2.4, 8.0, speed=19.0, glide=math.radians(7), flare=0.6)
    wob = math.sin(t * 5.3) * 0.12 + math.sin(t * 2.1) * 0.06
    roll = wob
    if t > 2.4:
        s = t - 2.4
        h = max(0.0, math.sin(s * 3.6)) * 1.4 * (1 if s < 0.88 else 0)
        # The second arrival: onto the left wheel, the right wing up.
        lurch = smooth((s - 0.85) / 0.25) * (1 - smooth((s - 1.6) / 0.8))
        roll = math.radians(-14) * lurch + wob * 0.2 * (1 - smooth(s))
        pitch = GROUND_PITCH * (0.6 + 0.4 * smooth(s - 0.9))
    R = attitude(0.0, pitch, roll)
    pos = np.array([0, WHEEL_H + h + abs(math.sin(roll)) * 0.9, z])
    f.shadow(0, z + 0.5, 4.0, 2.5, strength=0.5 * smooth(1 - h / 12))
    discs = biplane(f, pos, R, prop_spin=t < 3.7, prop_angle=0.4, damage=1.0, seed=3,
                    dope=DOPE * 0.85)
    for d in discs:
        prop_disc(f, d[0], R, spin_light=0.4)
    # A trailing wisp of smoke from the engine.
    for k in range(18):
        age = k * 0.09
        tt = t - age
        hh, zz, _ = approach(tt, 2.4, 8.0, speed=19.0, glide=math.radians(7), flare=0.6)
        p = np.array([0.3 + age * 0.8, WHEEL_H + hh + 0.6 + age * 0.9, zz + 1.6 if tt < 2.4 else z + 1.6 - age * 3])
        f.soft(p, 0.25 + age * 0.9, np.array([0.30, 0.30, 0.30]), 0.45 * (1 - age / 1.7))
    # The bounce's dust.
    if 2.35 < t < 4.5:
        for k in range(10):
            age = ((k / 10) + t * 1.8) % 1.0
            p = np.array([((k % 2) * 2 - 1) * 1.2, 0.2 + age * 1.5, z + 1 - age * 4])
            f.soft(p, 0.6 + age * 1.5, np.array([0.62, 0.56, 0.46]), 0.3 * (1 - age))
    # Ground crew running out from the hangars.
    if t > 3.0:
        for k in range(3):
            run = (t - 3.0) * 5.2
            man(f, (-14 + run * 0.9 + k * 1.3, 0, z - 6 + k * 1.5 + run * 0.35), 1.2, "run",
                t=t + k * 0.3)
    f.haze_objects(280.0, low)
    return f.finish()


def clip_land_dead_stick(t):
    """Dead stick: the airscrew stopped, a steep silent glide, a short
    roll and stillness."""
    L = scene_lights("overcast")
    cam = Camera((-10, 1.4, 24), (0, 3.0, 8 + 8 * smooth(t / 4)))
    f = Frame(cam, L)
    low = world(f, "overcast", haze=230.0, cloud=0.7)
    h, z, pitch = approach(t, 3.3, 12.0, speed=15.0, glide=math.radians(11), flare=1.0)
    if t > 3.3:
        s = t - 3.3
        z = 12 + 15 * s - 6.5 * s * s if s < 1.15 else 12 + 15 * 1.15 - 6.5 * 1.15 ** 2
    R = attitude(0.0, pitch, 0.04 * math.sin(t * 1.3))
    pos = np.array([0, WHEEL_H + h, z])
    f.shadow(0, z + 0.5, 4.0, 2.5, strength=0.45 * smooth(1 - h / 12))
    biplane(f, pos, R, prop_spin=False, prop_angle=math.radians(72))
    f.haze_objects(230.0, low)
    return f.finish()


def clip_land_rain(t):
    """Landing in rain: grey murk, streaks of rain, spray off the wheels."""
    L = scene_lights("rain")
    h_, z_, _ = approach(t, 3.0, 18.0, speed=16.0, flare=1.0)
    cam = Camera((-9, 1.6, 42), (0, 1.5 + h_ * 0.8, z_))
    f = Frame(cam, L)
    low = world(f, "rain", wet=1.0, mud=0.4, haze=110.0, cloud=0.95)
    h, z, pitch = approach(t, 3.0, 18.0, speed=16.0, flare=1.0)
    R = attitude(0.0, pitch, 0.05 * math.sin(t * 2.4))
    pos = np.array([0, WHEEL_H + h, z])
    f.shadow(0, z + 0.5, 4.0, 2.5, strength=0.25 * smooth(1 - h / 10))
    discs = biplane(f, pos, R, prop_spin=True)
    for d in discs:
        prop_disc(f, d[0], R, spin_light=0.5)
    if t > 3.0:
        rng = np.random.default_rng(int(t * FPS))
        for side in (-1, 1):
            w = pos + R.T @ np.array([side * 0.85, -1.15, 1.0])
            w[1] = 0.05
            for k in range(12):
                age = rng.random()
                p = w + np.array([side * age * 1.2, age * 1.1 - age * age * 0.8, -age * 3])
                f.soft(p, 0.12 + 0.2 * age, np.array([0.75, 0.76, 0.78]), 0.45 * (1 - age))
    f.haze_objects(110.0, low)
    img = f.finish()
    # Rain: slanting streaks, pale on the dark and dark on the pale.
    rng = np.random.default_rng(int(t * FPS) + 99)
    layer = Image.new("L", (W, H), 0)
    d = ImageDraw.Draw(layer)
    for _ in range(420):
        x, y = rng.uniform(-40, W), rng.uniform(-30, H)
        ln = rng.uniform(10, 26)
        d.line((x, y, x + ln * 0.25, y + ln), fill=int(rng.uniform(60, 140)), width=1)
    a = np.asarray(layer, np.float64)[..., None] / 255 * 0.55
    img = img * (1 - a) + 0.85 * a
    return img


CLIPS = {
    "takeoff-1-dawn-prop-swing": (clip_takeoff_dawn, 6.0, "leader"),
    "takeoff-2-flight-of-three": (clip_takeoff_three, 5.5, "cut"),
    "takeoff-3-muddy-field": (clip_takeoff_mud, 5.0, "cut"),
    "takeoff-4-1914-observer-climbs-in": (clip_takeoff_1914, 5.5, "cut"),
    "landing-1-dusk-three-point": (clip_land_dusk, 5.5, "leader"),
    "landing-2-shot-up": (clip_land_shot_up, 6.0, "cut"),
    "landing-3-dead-stick": (clip_land_dead_stick, 5.0, "cut"),
    "landing-4-rain": (clip_land_rain, 5.0, "cut"),
}


# -- the newsreel finish -------------------------------------------------------------

def film(img, n, seed, total):
    """Silver-sepia, grain, gate weave, flicker, scratches, dust and a
    vignette over one frame."""
    rng = np.random.default_rng(seed * 1000 + n)
    g = img @ np.array([0.30, 0.59, 0.11])
    g = np.clip(g, 0, None)
    g = g / (1 + g * 0.6) * 1.55                     # the film's shoulder
    g = np.clip(g, 0, 1) ** 1.08
    g = (g - 0.5) * 1.12 + 0.5                       # contrast of the print
    g *= 1 + rng.normal(0, 0.035)                    # flicker
    yy, xx = np.mgrid[0:H, 0:W]
    v = ((xx - W / 2) / (W * 0.62)) ** 2 + ((yy - H / 2) / (H * 0.62)) ** 2
    g *= np.clip(1.05 - v * 0.55, 0, 1)              # vignette
    g += rng.normal(0, 0.055, (H, W))                # grain
    g = np.asarray(Image.fromarray(np.clip(g * 255, 0, 255).astype(np.uint8)).filter(
        ImageFilter.GaussianBlur(0.5)), np.float64) / 255
    # Scratches that run for a few frames, dust and hairs for one.
    srng = np.random.default_rng(seed * 77 + n // 5)
    for _ in range(srng.integers(0, 3)):
        x = int(srng.uniform(20, W - 20) + rng.normal(0, 1.5))
        g[:, x:x + 1] = g[:, x:x + 1] * 0.4 + (0.9 if srng.random() < 0.5 else 0.05) * 0.6
    for _ in range(rng.integers(2, 9)):
        x, y = rng.uniform(0, W), rng.uniform(0, H)
        r = rng.uniform(0.6, 2.4)
        m = (xx - x) ** 2 + (yy - y) ** 2 < r * r
        g[m] = 0.03 if rng.random() < 0.7 else 0.95
    if rng.random() < 0.25:
        x, y = rng.uniform(0, W), rng.uniform(0, H)
        pts = [(x + k * 3, y + math.sin(k * 0.6) * 4) for k in range(12)]
        im = Image.fromarray(np.clip(g * 255, 0, 255).astype(np.uint8))
        ImageDraw.Draw(im).line(pts, fill=12, width=1)
        g = np.asarray(im, np.float64) / 255
    # Gate weave: the whole frame drifts a pixel or two.
    dx = int(round(rng.normal(0, 0.8) + math.sin(n * 0.37) * 0.8))
    dy = int(round(rng.normal(0, 0.9)))
    g = np.roll(np.roll(g, dx, 1), dy, 0)
    g = np.clip(g, 0, 1)
    sepia = np.stack([g * 1.04 + 0.03, g * 0.95 + 0.015, g * 0.80], -1)
    return Image.fromarray(np.clip(sepia * 255, 0, 255).astype(np.uint8))


def leader_frame(k, seed):
    """Academy-style leader: a number in a ring and the sweep round it."""
    num = 3 - k // 4
    img = np.full((H, W), 0.62)
    im = Image.fromarray((img * 255).astype(np.uint8))
    d = ImageDraw.Draw(im)
    cx, cy = W // 2, H // 2
    d.ellipse((cx - 150, cy - 150, cx + 150, cy + 150), outline=30, width=4)
    d.ellipse((cx - 120, cy - 120, cx + 120, cy + 120), outline=30, width=2)
    d.line((0, cy, W, cy), fill=30, width=2)
    d.line((cx, 0, cx, H), fill=30, width=2)
    sweep = (k % 4) / 4 * 360
    d.pieslice((cx - 150, cy - 150, cx + 150, cy + 150), -90, -90 + sweep, fill=90)
    try:
        font = ImageFont.truetype("DejaVuSerif-Bold.ttf", 170)
    except OSError:
        font = ImageFont.load_default()
    d.text((cx, cy), str(max(num, 1)), fill=15, font=font, anchor="mm")
    return np.repeat((np.asarray(im, np.float64) / 255)[..., None], 3, -1)


def cut_frames(seed):
    """A splice: a black frame, a flash of clear base, a frame of fogged
    film."""
    return [np.zeros((H, W, 3)), np.full((H, W, 3), 0.95),
            np.full((H, W, 3), 0.35)]


def render_one(args):
    name, i = args
    fn, length, _ = CLIPS[name]
    return fn(i / FPS)


def main():
    out = Path(sys.argv[1])
    out.mkdir(parents=True, exist_ok=True)
    names = sys.argv[2:] or list(CLIPS)
    keys_all = []
    with Pool() as pool:
        for ci, name in enumerate(names):
            fn, length, intro = CLIPS[name]
            n = int(round(length * FPS))
            raw = pool.map(render_one, [(name, i) for i in range(n)])
            pre = [leader_frame(k, ci) for k in range(9)] if intro == "leader" else cut_frames(ci)
            frames = [film(f, k, ci, n) for k, f in enumerate(pre + raw)]
            frames[0].save(out / f"{name}.webp", save_all=True, append_images=frames[1:],
                           duration=int(1000 / FPS), loop=0, quality=62, method=6)
            picks = [len(pre) + int(n * q) for q in (0.05, 0.3, 0.55, 0.8, 0.98)]
            keys = [frames[min(p, len(frames) - 1)] for p in picks]
            sheet = Image.new("RGB", (5 * 320, 200))
            for k, im in enumerate(keys):
                sheet.paste(im.resize((320, 200), Image.LANCZOS), (k * 320, 0))
            sheet.save(out / f"{name}-keys.png", optimize=True)
            keys_all.append((name, sheet))
            print(name, n, "frames", (out / f"{name}.webp").stat().st_size // 1024, "KB", flush=True)
    contact = Image.new("RGB", (5 * 320, 200 * len(keys_all) + 24 * len(keys_all)), (20, 18, 16))
    d = ImageDraw.Draw(contact)
    for k, (name, sheet) in enumerate(keys_all):
        y = k * 224
        d.text((6, y + 5), name, fill=(220, 210, 190))
        contact.paste(sheet, (0, y + 24))
    contact.save(out / "contact-sheet.png", optimize=True)


if __name__ == "__main__":
    main()
