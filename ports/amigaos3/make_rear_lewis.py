#!/usr/bin/env python3
"""Render the observer's Lewis gun on its Scarff ring, at every aim.

The rear-seat Lewis gun used to be a painted atlas of 5 x 5 frames, from
level to 75 degrees up.  It had no frame below level, so the gunner could
not aim down, and painting more frames to match it by hand was not
possible.  This builds the whole mounting instead, as
make_rear_carbine.py builds the carbine: the ring, the bow across it and
the gun on top are simple solids in the observer's own view, every pixel
ray-cast through a pinhole for its true depth and surface normal and lit
by the low sun beyond the tail, the sky behind the observer and a rim of
sunlight round the edges.  The colours are taken from the old painting:
dark oiled bronze for the gun and the ring, red-brown walnut for the grip,
brass on the fittings.

The ring and the bow do not move with the aim: the observer stands in
the ring and turns with it, so to him they stay where they are.  They are
rendered once.  The gun is rendered for a grid of aims, traverse by
elevation, from 30 degrees below level to 75 degrees above, each frame
holding only the gun and its fork where they lie in front of the mounting.
A frame is cropped to what it covers and the frames are shelf-packed into
one sheet, so the game holds only the pixels of the gun.

  assets/canvas-skies/rear-lewis.bmp   the mounting and the gun frames,
                                       256 colours with the last as key,
                                       run-length encoded
  src/modern/canvas_rear_lewis.h       where each lies on the sheet and
                                       on the screen, and each muzzle

The observer's frame: the eye at the origin looking aft over the tail,
x to his right, y up, z aft (away from him), in metres.
"""

import math
import sys
from pathlib import Path

import numpy as np
from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parent))
import make_rear_carbine as carbine  # noqa: E402  the renderer and packing

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"
HEADER = ROOT / "src" / "modern" / "canvas_rear_lewis.h"

W, H = carbine.W, carbine.H
SS = carbine.SS
KEY = carbine.KEY
CX, CY = 320.0, 200.0
FOCAL = 330.0                     # the game's own rear view, as the carbine

# The camera is tipped a little up from the mounting, which the old
# painting showed low in the picture with the gun standing over it.
CAMERA_DOWN = math.radians(-2.0)

# The aim grid.  Traverse runs across the crosshair's width (0 hard left,
# 1 hard right); elevation is in the game's units, 1 = 75 degrees up, so
# -0.4 is 30 degrees down.  The gun swings further than the view turns:
# hard over it lies across the picture, as in the painting.
TRAVERSE = np.linspace(0.0, 1.0, 11)
ELEVATION = np.linspace(-0.4, 1.0, 8)
YAW_LIMIT = math.radians(78.0)
ELEVATION_UNIT = math.radians(75.0)
GUN_SCALE = 1.35

# Colours, measured from the painted gun and ring.
BRONZE = np.array([0.34, 0.21, 0.13])
BRONZE_DARK = np.array([0.20, 0.125, 0.08])
BRONZE_LIGHT = np.array([0.48, 0.31, 0.18])
BLACK = np.array([0.08, 0.07, 0.065])
BRASS = np.array([0.55, 0.40, 0.20])
WALNUT = np.array([0.36, 0.15, 0.06])

# Lights, in the camera's frame (x right, y up, z into the picture).
KEY_LIGHT = np.array([-0.35, 0.80, -0.48]); KEY_LIGHT /= np.linalg.norm(KEY_LIGHT)
KEY_COLOUR = np.array([1.05, 0.92, 0.78])
SUN = np.array([0.10, 0.30, 1.0]); SUN /= np.linalg.norm(SUN)
SUN_COLOUR = np.array([1.40, 0.98, 0.62])
AMBIENT = np.array([0.22, 0.22, 0.26])

MOUNT, GUN = 1, 2


def unit(v):
    v = np.asarray(v, np.float64)
    return v / np.linalg.norm(v)


def rot_x(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])


# World (the observer's frame) to camera: the camera tipped down.
TO_CAMERA = rot_x(-CAMERA_DOWN)


def cam(p):
    return TO_CAMERA @ np.asarray(p, np.float64)


def project(p):
    q = cam(p)
    return CX + FOCAL * q[0] / q[2], CY - FOCAL * q[1] / q[2]


class View(carbine.View):
    """The carbine's renderer, with a part number kept for every sample
    (so the gun can be lifted out of a picture of the whole mounting) and
    two more solids: a capped cylinder and a ring of capsules."""

    def __init__(self):
        super().__init__()
        self.part = np.zeros((self.h, self.w), np.uint8)
        self.current = MOUNT

    def rays(self, box):
        x0, y0, x1, y1 = box
        yy, xx = np.mgrid[y0:y1, x0:x1].astype(np.float64)
        return np.stack([((xx + 0.5) / SS - CX) / FOCAL,
                         (CY - (yy + 0.5) / SS) / FOCAL,
                         np.ones_like(xx)], -1)

    def bbox(self, points, pad):
        pts = np.asarray(points, np.float64)
        if (pts[:, 2] - pad < 0.05).any():
            return 0, 0, self.w, self.h
        xs, ys = [], []
        for p in pts:
            for dx in (-pad, pad):
                for dy in (-pad, pad):
                    z = p[2] - pad
                    xs.append(CX + FOCAL * (p[0] + dx) / z)
                    ys.append(CY - FOCAL * (p[1] + dy) / z)
        x0 = max(0, int(math.floor(min(xs) * SS)) - 2)
        x1 = min(self.w, int(math.ceil(max(xs) * SS)) + 2)
        y0 = max(0, int(math.floor(min(ys) * SS)) - 2)
        y1 = min(self.h, int(math.ceil(max(ys) * SS)) + 2)
        return x0, y0, x1, y1

    def shade(self, box, t, n, albedo, spec, local=None, texture=None):
        """The carbine's lighting turned round: the old painting lit the
        gun from above and in front, from over the observer's shoulder,
        with the sunset beyond the tail catching its edges."""
        x0, y0, x1, y1 = box
        hit = np.isfinite(t) & (t > 0.30) & (t < self.z[y0:y1, x0:x1])
        if not hit.any():
            return
        n = n / np.maximum(np.linalg.norm(n, axis=-1, keepdims=True), 1e-9)
        alb = np.broadcast_to(albedo, n.shape).copy()
        if texture is not None:
            alb *= texture(local)[..., None]
        view = -self.rays(box)
        view /= np.linalg.norm(view, axis=-1, keepdims=True)
        key = np.clip((n * KEY_LIGHT).sum(-1), 0, 1)[..., None]
        sun = np.clip((n * SUN).sum(-1), 0, 1)[..., None]
        sky = 0.6 + 0.4 * n[..., 1:2]
        col = alb * (AMBIENT * sky + KEY_COLOUR * key + SUN_COLOUR * sun * 0.5)
        edge = (1 - np.clip((n * view).sum(-1), 0, 1)) ** 3
        col += SUN_COLOUR * 0.30 * (edge * np.clip((n * SUN).sum(-1) + 0.45,
                                                   0, 1))[..., None]
        if spec:
            for light, colour in ((KEY_LIGHT, KEY_COLOUR), (SUN, SUN_COLOUR)):
                hv = view + light
                hv /= np.linalg.norm(hv, axis=-1, keepdims=True)
                col += colour * spec * (np.clip((n * hv).sum(-1), 0, 1)
                                        ** 28)[..., None]
        self.rgb[y0:y1, x0:x1][hit] = col[hit]
        self.z[y0:y1, x0:x1][hit] = t[hit]
        self.part[y0:y1, x0:x1][hit] = self.current

    def cylinder(self, a, b, r, albedo, spec=0.0, texture=None):
        """A solid round bar from a to b with flat ends.  The texture is
        given (angle round the axis, 0..1 along it, 1 on an end face)."""
        a, b = cam(a), cam(b)
        box = self.bbox([a, b], r)
        if box[0] >= box[2] or box[1] >= box[3]:
            return
        d = self.rays(box)
        dn = d / np.linalg.norm(d, axis=-1, keepdims=True)
        ba = b - a
        oc = -a
        baba = ba @ ba
        bard = dn @ ba
        baoc = ba @ oc
        k2 = baba - bard * bard
        k1 = baba * (dn @ oc) - baoc * bard
        k0 = baba * (oc @ oc) - baoc * baoc - r * r * baba
        h = k1 * k1 - k2 * k0
        with np.errstate(invalid="ignore", divide="ignore"):
            hs = np.sqrt(h)
            t = (-k1 - hs) / k2
            y = baoc + t * bard
            body = (h >= 0) & (y > 0) & (y < baba)
            tc = (np.where(y < 0, 0.0, baba) - baoc) / bard
            cap = (h >= 0) & ~body & (np.abs(k1 + k2 * tc) < hs)
        t = np.where(body, t, np.where(cap, tc, np.inf))
        pos = dn * np.where(np.isfinite(t), t, 0)[..., None]
        s = np.clip(((pos - a) @ ba) / baba, 0, 1)
        axis = a + s[..., None] * ba
        nb = (pos - axis) / r
        nc = np.where((y < 0)[..., None], -ba, ba) / math.sqrt(baba)
        n = np.where(body[..., None], nb, nc)
        # Angle round the axis, for ribs.
        ref = np.cross(ba, [0.0, 1.0, 0.0])
        if np.linalg.norm(ref) < 1e-6:
            ref = np.cross(ba, [1.0, 0.0, 0.0])
        ref = unit(ref)
        ref2 = np.cross(unit(ba), ref)
        rel = pos - axis
        ang = np.arctan2(rel @ ref2, rel @ ref)
        radial = np.linalg.norm(rel, axis=-1) / r
        face = np.where(body, 0.0, 1.0)
        # Depth along the view axis, as the rest of the renderer keeps it.
        self.shade(box, t * dn[..., 2], n, albedo, spec,
                   local=(ang, s, radial, face), texture=texture)

    def bar(self, a, b, r, albedo, spec=0.0, texture=None):
        self.capsule(cam(a), cam(b), r, albedo, spec, texture)

    def block(self, centre, axes, half, albedo, spec=0.0, bevel=0.25):
        axes = np.asarray(axes, np.float64) @ TO_CAMERA.T
        self.box(cam(centre), axes, half, albedo, spec, bevel=bevel)

    def knob(self, centre, axes, radii, albedo, spec=0.0):
        axes = np.asarray(axes, np.float64) @ TO_CAMERA.T
        self.ellipsoid(cam(centre), axes, radii, albedo, spec)

    def hoop(self, centre, normal, radius, tube, albedo, spec=0.0,
             segments=48, start=0.0, end=2 * math.pi):
        """A ring of round bar: a circle of short capsules."""
        normal = unit(normal)
        e1 = unit(np.cross(normal, [0.0, 0.0, 1.0])
                  if abs(normal[2]) < 0.9 else
                  np.cross(normal, [1.0, 0.0, 0.0]))
        e2 = np.cross(normal, e1)
        c = np.asarray(centre, np.float64)
        angles = np.linspace(start, end, segments + 1)
        pts = [c + radius * (math.cos(a) * e1 + math.sin(a) * e2)
               for a in angles]
        for p, q in zip(pts[:-1], pts[1:]):
            self.bar(p, q, tube, albedo, spec)


def ribs(count, depth):
    """Ribs round a cylinder, and radial ribs on its end faces."""
    def tex(local):
        ang, s, radial, face = local
        wave = 0.5 + 0.5 * np.cos(ang * count)
        side = 1 - depth * wave ** 3
        end = 1 - depth * wave ** 3 * np.clip((radial - 0.30) / 0.2, 0, 1)
        return np.where(face > 0.5, end, side)
    return tex


def fins(count, depth):
    """Cooling fins along a barrel: rings at even spacing."""
    def tex(local):
        ang, s, radial, face = local
        return 1 - depth * (0.5 + 0.5 * np.cos(s * count * 2 * math.pi)) ** 4
    return tex


# ---- The mounting -------------------------------------------------------

RING_CENTRE = np.array([0.0, -0.58, 1.55])
RING_RADIUS = 0.47
RING_TUBE = 0.020
CROSSHEAD = RING_CENTRE + np.array([0.0, 0.275, -0.16])
PIVOT = CROSSHEAD + np.array([0.0, 0.085, 0.0])


def ring_point(angle, lift=0.0):
    """A point on the ring; angle 0 is the observer's right, pi/2 aft."""
    return RING_CENTRE + np.array([RING_RADIUS * math.cos(angle), lift,
                                   RING_RADIUS * math.sin(angle)])


def mounting(v):
    up = np.array([0.0, 1.0, 0.0])
    v.current = MOUNT
    # The rail and, a little inside and under it, the toothed racer.
    v.hoop(RING_CENTRE, up, RING_RADIUS, RING_TUBE, BRONZE, spec=0.45,
           segments=72)
    v.hoop(RING_CENTRE + [0, -0.028, 0], up, RING_RADIUS - 0.012, 0.012,
           BRONZE_DARK, spec=0.25, segments=72)
    # Brass rollers at even spacing round the rail.
    for k in range(10):
        a = (k + 0.5) * 2 * math.pi / 10
        p = ring_point(a, -0.010)
        tangent = np.array([-math.sin(a), 0.0, math.cos(a)])
        v.cylinder(p - tangent * 0.022, p + tangent * 0.022, 0.030,
                   BRASS, spec=0.6, texture=ribs(10, 0.25))
        v.knob(p + [0, -0.032, 0], np.eye(3), (0.016, 0.018, 0.016),
               BRONZE_DARK, spec=0.3)
    # The bow: two arms from the rail at either side, up and in to the
    # crosshead under the gun, with an elbow clamp on each.
    for side in (-1, 1):
        foot = ring_point(0.0 if side > 0 else math.pi, 0.012)
        elbow = RING_CENTRE + [side * 0.30, 0.15, -0.08]
        top = CROSSHEAD + [side * 0.045, -0.01, 0.0]
        v.bar(foot, elbow, 0.016, BRONZE, spec=0.45)
        v.bar(elbow, top, 0.015, BRONZE, spec=0.45)
        v.cylinder(elbow - [0, 0.025, 0], elbow + [0, 0.025, 0], 0.024,
                   BRASS, spec=0.6, texture=ribs(8, 0.3))
        v.cylinder(foot - [0, 0.03, 0], foot + [0, 0.02, 0], 0.028,
                   BRASS, spec=0.6, texture=ribs(8, 0.3))
    # The pillar from the near side of the rail up to the crosshead, and
    # the bracket where it clamps on.
    near = ring_point(-math.pi / 2, 0.0)
    v.block(near + [0, 0.012, 0.004], np.eye(3), (0.050, 0.030, 0.030),
            BRONZE_DARK, spec=0.35)
    v.cylinder(near + [0, 0.042, 0.004], near + [0, 0.060, 0.004], 0.030,
               BRASS, spec=0.6, texture=ribs(12, 0.3))
    v.bar(near + [0, 0.05, 0.004], CROSSHEAD + [0, -0.03, 0], 0.017,
          BRONZE, spec=0.45)
    v.cylinder(CROSSHEAD + [0, -0.045, 0], CROSSHEAD + [0, 0.012, 0], 0.036,
               BRONZE_DARK, spec=0.4, texture=ribs(14, 0.25))
    v.cylinder(CROSSHEAD + [0, 0.010, 0], CROSSHEAD + [0, 0.020, 0], 0.046,
               BRASS, spec=0.6, texture=ribs(16, 0.2))


# ---- The gun ------------------------------------------------------------

def aim(traverse, elevation):
    yaw = (traverse - 0.5) * 2.0 * YAW_LIMIT
    pitch = elevation * ELEVATION_UNIT
    return yaw, pitch


def gun(v, yaw, pitch):
    """The Lewis gun on its fork, pointing (yaw, pitch); returns the
    muzzle in the world.  Sizes are a real Lewis's, in metres, made
    GUN_SCALE times larger about the trunnion: the painting drew the gun
    large against the ring, so that it reads at 640 x 400."""
    k = GUN_SCALE
    a = np.array([math.sin(yaw) * math.cos(pitch), math.sin(pitch),
                  math.cos(yaw) * math.cos(pitch)])
    flat = np.array([math.sin(yaw), 0.0, math.cos(yaw)])
    side = np.array([math.cos(yaw), 0.0, -math.sin(yaw)])   # gun's right
    up = np.cross(a, side)
    if up[1] < 0:
        up = -up
    axes = np.stack([side, up, a])
    level = np.array([0.0, 1.0, 0.0])

    def at(s, du=0.0, dr=0.0):
        return PIVOT + k * (a * s + up * du + side * dr)

    def part(s0, s1, w, h, du, albedo, dr=0.0, **kw):
        v.block(at((s0 + s1) / 2, du, dr), axes,
                (k * w / 2, k * h / 2, k * (s1 - s0) / 2), albedo, **kw)

    def rod(s0, s1, r, albedo, du=0.0, dr=0.0, **kw):
        v.cylinder(at(s0, du, dr), at(s1, du, dr), k * r, albedo, **kw)

    v.current = GUN
    # The fork turns with the gun on the crosshead; the trunnion pin is
    # the elevation axis.
    v.cylinder(CROSSHEAD + level * 0.018, CROSSHEAD + level * 0.040,
               0.030, BRONZE_DARK, spec=0.4, texture=ribs(14, 0.25))
    for sgn in (-1, 1):
        cheek = PIVOT + side * sgn * k * 0.036
        foot = CROSSHEAD + level * 0.035 + side * sgn * k * 0.036
        v.block((cheek + foot) / 2, np.stack([side, level, flat]),
                (k * 0.006, float(np.linalg.norm(cheek - foot)) / 2 + 0.01,
                 k * 0.024), BRONZE, spec=0.4)
    v.cylinder(PIVOT - side * k * 0.048, PIVOT + side * k * 0.048,
               k * 0.011, BRASS, spec=0.6)

    # The body: the receiver with its top rib, the gas-cylinder housing
    # in front of it, the trigger group under the back.
    part(-0.26, 0.12, 0.060, 0.070, 0.042, BRONZE, spec=0.4)
    part(-0.24, 0.10, 0.030, 0.012, 0.081, BRONZE_LIGHT, spec=0.5)
    part(0.06, 0.20, 0.046, 0.040, 0.010, BRONZE_DARK, spec=0.3)
    part(-0.22, -0.10, 0.044, 0.040, -0.006, BRONZE_DARK, spec=0.3)
    # Cocking handle on the right side, and the back plate.
    v.bar(at(-0.08, 0.040, 0.030), at(-0.08, 0.040, 0.062), k * 0.008,
          BRASS, spec=0.6)
    part(-0.275, -0.255, 0.064, 0.074, 0.040, BRONZE_DARK, spec=0.3)
    # The walnut pistol grip, raked back under the body, and its guard.
    v.block(at(-0.225, -0.068) - a * k * 0.018, np.stack(
        [side, unit(up * 0.92 - a * 0.40), unit(a * 0.92 + up * 0.40)]),
        (k * 0.017, k * 0.062, k * 0.024), WALNUT, spec=0.15)
    v.bar(at(-0.18, -0.026), at(-0.14, -0.046), k * 0.005, BRONZE_DARK,
          spec=0.3)
    # The pan magazine on the top, ribbed round its rim and across its
    # face, with the centre boss and its latch.
    pan_c = at(-0.07, 0.112)
    v.cylinder(pan_c - up * k * 0.030, pan_c + up * k * 0.030, k * 0.135,
               BRONZE_DARK, spec=0.35, texture=ribs(36, 0.6))
    v.cylinder(pan_c + up * k * 0.028, pan_c + up * k * 0.044, k * 0.040,
               BRONZE, spec=0.5, texture=ribs(12, 0.3))
    v.cylinder(pan_c + up * k * 0.042, pan_c + up * k * 0.052, k * 0.016,
               BRASS, spec=0.6)
    # The barrel: the finned radiator next to the body, then the bare
    # barrel with the gas cylinder under it, and the flash cone.
    rod(0.12, 0.36, 0.031, BRONZE, spec=0.4, texture=fins(16, 0.45))
    rod(0.36, 0.62, 0.012, BRONZE_DARK, spec=0.55)
    rod(0.20, 0.50, 0.009, BLACK, du=-0.028, spec=0.3)
    rod(0.47, 0.50, 0.019, BRASS, du=-0.014, spec=0.5)   # gas block
    rod(0.62, 0.68, 0.018, BLACK, spec=0.4)
    # Ring foresight on its post near the muzzle, and the rear sight on
    # the body.
    v.bar(at(0.585, 0.012), at(0.585, 0.046), k * 0.004, BLACK, spec=0.3)
    v.hoop(at(0.585, 0.084), a, k * 0.038, k * 0.0035, BLACK, spec=0.4,
           segments=28)
    part(-0.215, -0.195, 0.020, 0.026, 0.090, BLACK, spec=0.3)
    return at(0.68)


# ---- Frames -------------------------------------------------------------

def finish(v, which):
    """The view's picture, keeping only one part: colour where at least
    half the pixel shows that part, the key elsewhere."""
    keep = (v.part == which) & np.isfinite(v.z)
    cov = keep.astype(np.float64)
    col = v.rgb * cov[..., None]
    cov = cov.reshape(H, SS, W, SS).mean((1, 3))
    col = col.reshape(H, SS, W, SS, 3).sum((1, 3)) / (SS * SS)
    col = col / np.maximum(cov, 1e-6)[..., None]
    col = col / (1 + col * 0.35) * 1.18
    col = np.clip(col, 0, 1) ** (1 / 1.25)
    out = (col * 255 + 0.5).astype(np.uint8)
    mask = cov >= 0.5
    out[~mask] = KEY
    return out, mask


def render_mounting():
    v = View()
    mounting(v)
    return finish(v, MOUNT)


def render_gun(traverse, elevation):
    v = View()
    mounting(v)
    yaw, pitch = aim(traverse, elevation)
    muzzle = gun(v, yaw, pitch)
    img, mask = finish(v, GUN)
    return img, mask, project(muzzle)


def main():
    frames = []
    img, mask = render_mounting()
    part, origin = carbine.crop(img, mask)
    frames.append((part, origin, (0, 0)))
    print(f"mounting: {part.shape[1]}x{part.shape[0]} at {origin}")
    for e in ELEVATION:
        for t in TRAVERSE:
            img, mask, muzzle = render_gun(t, e)
            part, origin = carbine.crop(img, mask)
            frames.append((part, origin, muzzle))
            print(f"aim {t:4.2f},{e:5.2f}: {part.shape[1]}x{part.shape[0]}"
                  f" at {origin}, muzzle {muzzle[0]:.0f},{muzzle[1]:.0f}")
    sheet, place = carbine.pack(frames)
    rgb = Image.fromarray(sheet)
    keyed = (sheet == KEY).all(-1)
    q = rgb.quantize(colors=255, method=Image.Quantize.MEDIANCUT,
                     dither=Image.Dither.FLOYDSTEINBERG)
    pal = q.getpalette()[:255 * 3] + list(KEY)
    idx = np.asarray(q).copy()
    idx[keyed] = 255
    carbine.save_rle8(ASSETS / "rear-lewis.bmp", idx.astype(np.uint8), pal)
    rows, cols = len(ELEVATION), len(TRAVERSE)
    with open(HEADER, "w") as f:
        f.write("/* Generated by ports/amigaos3/make_rear_lewis.py: "
                "do not edit. */\n")
        f.write("#ifndef CANVAS_REAR_LEWIS_H\n#define CANVAS_REAR_LEWIS_H\n\n")
        f.write(f"#define LEWIS_TRAVERSE_COLUMNS {cols}\n")
        f.write(f"#define LEWIS_ELEVATION_ROWS {rows}\n")
        f.write(f"#define LEWIS_ELEVATION_LOW ({ELEVATION[0]:.2f}f)\n")
        f.write(f"#define LEWIS_ELEVATION_HIGH ({ELEVATION[-1]:.2f}f)\n\n")
        f.write("/* The ring and its bow: where they lie in the sheet "
                "(x, y, w, h) and\n * where they are drawn on the screen "
                "(x, y). */\n")
        (part, origin, _), (px, py) = frames[0], place[0]
        f.write(f"static const short lewis_mounting[6] = "
                f"{{{px}, {py}, {part.shape[1]}, {part.shape[0]}, "
                f"{origin[0]}, {origin[1]}}};\n\n")
        f.write("/* The gun, per aim, elevation row by row from the lowest, "
                "traverse\n * from hard left: where the frame lies in the "
                "sheet (x, y, w, h),\n * where it is drawn on the screen "
                "(x, y) and the muzzle on the screen. */\n")
        f.write("static const short lewis_frames[LEWIS_ELEVATION_ROWS * "
                "LEWIS_TRAVERSE_COLUMNS][8] = {\n")
        for i, (part, origin, muzzle) in enumerate(frames[1:], 1):
            h, w = part.shape[:2]
            f.write(f"    {{{place[i][0]}, {place[i][1]}, {w}, {h}, "
                    f"{origin[0]}, {origin[1]}, {muzzle[0]:.0f}, "
                    f"{muzzle[1]:.0f}}},\n")
        f.write("};\n\n#endif\n")
    print(f"sheet {sheet.shape[1]}x{sheet.shape[0]}")


if __name__ == "__main__":
    main()
