#!/usr/bin/env python3
"""Take-offs and landings at the aerodrome, finished as old newsreel.

Eight short films for Canvas Skies, every one of them our own: the field,
its hangars and tents, the machines and the men who tend them, the weather
and the light, built from simple solids by render.py and models.py, lit by
the sun and the sky with true shadows, then given a period film finish
(silver-sepia, grain, gate weave, flicker, scratches, dust, vignette, and a
leader or a splice at the head).

Each film is a function of time that says where everything is and what
the engines are doing; the picture is drawn from that, and sound.py reads
the same account to make the soundtrack, so a wheel thumps when it
touches.

  python3 airfield.py OUTDIR [film ...]

writes, for each film, OUTDIR/<film>.webp (the finished picture, animated),
OUTDIR/<film>-keys.png (five key frames), OUTDIR/frames/<film>.npy (the
finished frames in grey, for make_airfield_films.py), and
OUTDIR/contact-sheet.png.
"""

import math
import os
import sys

os.environ.setdefault("OPENBLAS_NUM_THREADS", "1")    # one thread a worker
from multiprocessing import Pool
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFilter, ImageFont

sys.path.insert(0, str(Path(__file__).resolve().parent))
from render import (FINE, GRASS, MUD, H, SS, W, Camera, Frame, Light, Sky,  # noqa: E402
                    bilinear, lerp, smooth)
import models as M  # noqa: E402

FPS = 24
WORKERS = 3


# -- light and weather -------------------------------------------------------------

def light_for(kind):
    if kind == "dawn":
        return Light((0.42, 0.105, 0.90), (2.0, 1.36, 0.85), (0.28, 0.30, 0.40),
                     (0.04, 0.04, 0.05), bounce=(0.08, 0.07, 0.05))
    if kind == "dusk":
        return Light((-0.48, 0.095, -0.87), (2.0, 1.20, 0.66), (0.27, 0.26, 0.36),
                     (0.035, 0.03, 0.04), bounce=(0.08, 0.06, 0.04))
    if kind == "rain":
        return Light((0.2, 0.8, 0.3), (0.10, 0.10, 0.11), (0.62, 0.64, 0.68),
                     (0.10, 0.10, 0.11), bounce=(0.05, 0.05, 0.05))
    if kind == "overcast":
        return Light((0.3, 0.7, 0.4), (0.22, 0.21, 0.20), (0.62, 0.62, 0.64),
                     (0.08, 0.08, 0.08), bounce=(0.07, 0.07, 0.06))
    return Light((0.45, 0.55, 0.62), (1.10, 1.02, 0.88), (0.30, 0.34, 0.42),
                 (0.05, 0.05, 0.06))


SKIES = {
    "dawn": dict(top=(0.30, 0.34, 0.48), low=(0.95, 0.78, 0.58), glow=1.6, cloud=0.42,
                 haze=320.0, sun_disc=2.0, cloud_lit=1.2, cloud_dark=0.6),
    "dusk": dict(top=(0.22, 0.22, 0.34), low=(0.96, 0.66, 0.42), glow=1.7, cloud=0.5,
                 haze=320.0, sun_disc=2.0, cloud_lit=1.3, cloud_dark=0.7),
    "rain": dict(top=(0.46, 0.47, 0.50), low=(0.64, 0.65, 0.67), glow=0.0, cloud=1.0,
                 haze=95.0, cloud_dark=0.35, cloud_lit=0.0),
    "overcast": dict(top=(0.58, 0.59, 0.61), low=(0.76, 0.76, 0.76), glow=0.0, cloud=0.85,
                     haze=210.0, cloud_dark=0.35, cloud_lit=0.0),
    "day": dict(top=(0.42, 0.52, 0.70), low=(0.82, 0.84, 0.86), glow=0.3, cloud=0.55,
                haze=300.0, cloud_lit=0.5),
}


# -- the field -------------------------------------------------------------------

SHEEN = 0.10        # dew or damp on the grass catching the sky at a slant
HEDGES = (("z", 185.0, 1.9), ("z", -165.0, 1.9), ("x", 160.0, 1.9))
GRASS_COL = np.array([0.29, 0.35, 0.18])
WORN = np.array([0.40, 0.36, 0.26])
SOIL = np.array([0.30, 0.24, 0.17])
PLOUGH = np.array([0.33, 0.27, 0.19])
CROP = np.array([0.42, 0.40, 0.24])


def field(mud=0.0, wet=0.0, rain=0.0, tracks=(0.0, -9.0, 7.0), dark=1.0):
    """The ground painter: mown grass with the wear of many take-offs,
    the fields beyond the hedges, and, as the weather wills, mud, ruts
    and standing water."""
    def ground(px, pz, dist, t, sun):
        g1 = bilinear(GRASS, px * 3.0, pz * 3.0)
        g2 = bilinear(GRASS, px * 0.35 + 211, pz * 0.35 + 97)
        fine = bilinear(FINE, px * 24, pz * 24)
        near = np.clip(1 - dist / 45, 0, 1)
        tone = 0.80 + 0.45 * (g2 - 0.5) + 0.30 * (g1 - 0.5) + 0.40 * near * (fine - 0.5)
        tone *= 1 + 0.05 * np.sign(np.sin(px * 0.21 + 0.3))          # mown stripes
        col = GRASS_COL * tone[:, None]
        # The worn tracks of the take-off run, and the tyre lines in them.
        wear = np.zeros_like(px)
        rut = np.zeros_like(px)
        for i, x0 in enumerate(tracks):
            wob = 1.6 * np.sin(pz * 0.031 + i * 2.1) + 0.8 * np.sin(pz * 0.11 + i)
            dx = px - x0 - wob
            wear += np.exp(-(dx / 3.2) ** 2) * np.clip(1 - np.abs(pz - 30) / 150, 0, 1)
            for s in (-0.86, 0.86):
                rut += np.exp(-((dx - s) / 0.13) ** 2)
        wear = np.clip(wear * (0.55 + 0.6 * g1), 0, 1)
        rut = np.clip(rut, 0, 1) * wear
        col = col * (1 - wear[:, None] * 0.75) + WORN * (wear * 0.75 * (0.8 + 0.4 * fine))[:, None]
        col = col * (1 - 0.25 * rut)[:, None]
        # Beyond the hedges, the farmers' fields in strips.
        out = (pz > 185) | (pz < -165) | (np.abs(px) > 160)
        strip = np.floor((px * 0.6 + pz * 0.8) / 38).astype(np.int64) % 3
        fcol = np.where((strip == 0)[:, None], PLOUGH * (0.9 + 0.2 * np.sin(px * 0.8 + pz))[:, None],
                        np.where((strip == 1)[:, None], CROP, GRASS_COL * 1.05))
        col = np.where(out[:, None], fcol * (0.85 + 0.3 * g2)[:, None], col)
        # The long shadows of the hedges and the trees standing in them,
        # thrown across the grass by a low sun.
        if sun[1] < 0.5:
            hz = math.hypot(sun[0], sun[2])
            reach = 1.0 / max(sun[1], 0.05) * hz
            tufts = bilinear(GRASS, px * 0.25 + 40, pz * 0.25)
            for axis, at, height in HEDGES:
                c = pz if axis == "z" else px
                s_ = sun[2] if axis == "z" else sun[0]
                if abs(s_) < 0.05:
                    continue
                k = (at - c) * np.sign(s_) / (abs(s_) / hz)
                hh = height * (0.8 + 0.5 * g2) + 12.0 * np.clip((tufts - 0.58) * 4, 0, 1)
                band = (k > 0) & (k < hh * reach)
                col = col * np.where(band, 0.45, 1.0)[:, None]
        res = {}
        wet_w = np.zeros_like(px)
        if mud:
            # Churned earth: dark and wet where the wheels and boots have
            # been, lumpy with clods, the grass worn through in patches.
            m = bilinear(MUD, px * 1.2, pz * 1.2)
            clods = bilinear(FINE, px * 55, pz * 55)
            muddy = np.clip((m - 0.30) * 3.5 + wear * 1.4, 0, 1) * mud
            brown = SOIL * 0.55 * (0.40 + 1.2 * clods * (0.6 + 0.8 * g1))[:, None]
            col = col * (1 - muddy[:, None]) + brown * muddy[:, None]
            # Ruts along the tyre lines: a dark trough with water standing
            # in its bottom, glinting.
            trough = np.zeros_like(px)
            water = np.zeros_like(px)
            for i, x0 in enumerate(tracks):
                wob = 1.6 * np.sin(pz * 0.031 + i * 2.1) + 0.8 * np.sin(pz * 0.11 + i)
                dx = px - x0 - wob
                for sd in (-0.86, 0.86):
                    trough += np.exp(-((dx - sd) / 0.30) ** 2)
                    water += np.exp(-((dx - sd) / 0.12) ** 2)
            trough = np.clip(trough, 0, 1) * mud
            broken = np.clip((bilinear(MUD, px * 4 + 9, pz * 2.5) - 0.50) * 4, 0, 1)
            water = np.clip(water, 0, 1) * mud * broken
            col = col * (1 - 0.55 * trough)[:, None]
            # Standing puddles in the hollows, holding the sky.
            pm = bilinear(MUD, px * 6.0 + 40, pz * 6.0)
            puddle = np.clip((pm - 0.66) * 14, 0, 1) * mud * np.clip((m - 0.25) * 4, 0, 1) * (1 - out)
            col = col * (1 - 0.5 * np.clip((pm - 0.60) * 9, 0, 1) * mud)[:, None]   # dark wet rims
            wet_w = np.maximum(wet_w, np.maximum(puddle * 0.8, water * 0.55))
            # Everything else has the sheen of wet earth.
            wet_w = np.maximum(wet_w, muddy * 0.12 * (1 - out))
        if wet:
            m = bilinear(MUD, px * 0.8 + 50, pz * 0.8)
            puddle = np.clip((m - 0.60) * 7 + wear * 0.6 - 0.2, 0, 1) * wet
            col = col * (1 - 0.25 * wet)
            wet_w = np.maximum(wet_w, 0.10 * wet * (1 - out))
            wet_w = np.maximum(wet_w, puddle * 0.9 * (1 - out))
        if rain and wet_w.any():
            # Rings spreading on the water where drops fall.
            cell = 0.6
            cx = np.floor(px / cell)
            cz = np.floor(pz / cell)
            hsh = np.sin(cx * 12.9898 + cz * 78.233) * 43758.5453
            hsh = hsh - np.floor(hsh)
            age = np.mod(t * 1.7 + hsh, 1.0)
            ox = (cx + 0.25 + 0.5 * hsh) * cell
            oz = (cz + 0.25 + 0.5 * np.mod(hsh * 7, 1)) * cell
            r = np.sqrt((px - ox) ** 2 + (pz - oz) ** 2)
            ring = np.exp(-((r - age * 0.35) / 0.025) ** 2) * (1 - age) * near
            wet_w = wet_w * (1 - 0.8 * ring * (wet_w > 0.3))
        if wet_w.any():
            res["wet"] = wet_w
        res["sheen"] = np.full(px.shape, SHEEN, np.float32) * (1 - out) + 0.03
        res["albedo"] = col * dark
        return res
    return ground


def world(f, t, wind=(0.6, 0.6), sock=True):
    """The aerodrome: three Bessonneau hangars along the west side with
    their tents, drums and a windsock; hedges and poplars round the field,
    and a farm beyond."""
    for i, z in enumerate((22.0, 48.0, 74.0)):
        M.hangar(f, -42.0, z, math.pi / 2, seed=i)
    M.drums(f, -28.0, 30.0, 7, seed=3)
    M.drums(f, -29.0, 60.0, 5, seed=4)
    for i in range(5):
        M.bell_tent(f, -70.0 - (i % 2) * 7, 5.0 + i * 8.0, seed=i)
    M.tent(f, -34.0, 2.0, 0.0, w=5, d=8, seed=7)
    M.tent(f, -60.0, 98.0, 0.3, seed=8)
    if sock:
        M.windsock(f, 34.0, 70.0, wind[0], wind[1], t)
    rng = np.random.default_rng(5)
    # Hedges round the field, gappy, with trees standing in them.
    M.hedge(f, -160, 160, 185, 11)
    M.hedge(f, -160, 160, -165, 12)
    M.hedge(f, -165, 185, 160, 13, along="z")
    for x in np.arange(-150, 160, 22):
        if rng.random() < 0.55:
            M.tree(f, x + rng.uniform(-6, 6), 186 + rng.uniform(-2, 3), rng.uniform(11, 17), rng)
    for x in np.arange(-150, 160, 26):
        if rng.random() < 0.45:
            M.tree(f, x + rng.uniform(-6, 6), -166 + rng.uniform(-2, 2), rng.uniform(10, 15), rng)
    # Poplars along the road to the east.
    for z in np.arange(-150, 190, 11):
        M.tree(f, 172 + rng.uniform(-0.5, 0.5), z, rng.uniform(19, 24), rng, kind="poplar")
    M.farm(f, 95.0, 255.0, 0.25, seed=2)
    M.farm(f, -120.0, -230.0, 1.2, seed=6)
    for k in range(6):
        M.tree(f, rng.uniform(-120, -60), rng.uniform(110, 175), rng.uniform(12, 18), rng)


# -- the account of each film -------------------------------------------------------
#
# A film is a function of time returning a Shot: the camera, the light, and
# for each machine where it is, how it sits and what its engine is doing.
# The engine's 'power' runs 0..1 and 'rpm' 0..1 of full revolutions;
# 'running' is False when the switch is off or the engine dead.

class Plane:
    def __init__(self, kind, pos, R, prop=0.0, spin=0.0, rpm=0.0, power=0.0,
                 running=False, machine=None, crew=("pilot",), sputter=0.0, speed=0.0,
                 tatters=None, smoke=0.0):
        self.kind, self.pos, self.R = kind, np.asarray(pos, np.float64), R
        self.prop, self.spin, self.rpm, self.power = prop, spin, rpm, power
        self.running = running
        self.machine = machine
        self.crew = crew
        self.sputter = sputter
        self.speed = speed
        self.tatters = tatters
        self.smoke = smoke

    def contacts(self):
        return M.contacts(self.kind, self.pos, self.R)


class Shot:
    def __init__(self, cam, weather, planes, men=(), fx=None, wind=(0.6, 0.6),
                 ground=None, rain=0.0, events=None, blobs=None):
        self.cam = cam                  # (pos, target, roll)
        self.weather = weather
        self.planes = planes
        self.men = men                  # callables(frame)
        self.fx = fx                    # callable(frame, light colour)
        self.wind = wind
        self.ground = ground
        self.rain = rain
        self.events = events or {}


# The machines that appear, made once so their damage stays put.
MACHINES = {
    "scout": M.Machine("scout", seed=1, weathered=0.3),
    "scout_mud": M.Machine("scout", seed=5, weathered=0.8),
    "scout_rain": M.Machine("scout", seed=7, weathered=0.5),
    "late_a": M.Machine("late", seed=2, weathered=0.2),
    "late_b": M.Machine("late", seed=3, weathered=0.3),
    "late_c": M.Machine("late", seed=4, weathered=0.25),
    "late_shot": M.Machine("late", seed=9, damage=1.0, weathered=0.6),
    "twoseat": M.Machine("twoseat", seed=6, weathered=0.2),
    "twoseat_glide": M.Machine("twoseat", seed=8, weathered=0.35),
}

CREW = {
    "swinger": M.Man(seed=11, cloth=M.OVERALLS[1]),
    "chocks": M.Man(seed=12, cloth=M.OVERALLS[0], puttees=True),
    "rigger": M.Man(seed=13, cloth=M.OVERALLS[2]),
    "fitter": M.Man(seed=14, cloth=M.OVERALLS[3], cap="none"),
    "observer": M.Man(seed=15, cloth=M.COAT, coat=True, helmet=True),
    "sergeant": M.Man(seed=16, cloth=M.KHAKI, puttees=True),
    "runner1": M.Man(seed=17, cloth=M.OVERALLS[0]),
    "runner2": M.Man(seed=18, cloth=M.OVERALLS[2], cap="none"),
    "runner3": M.Man(seed=19, cloth=M.KHAKI, puttees=True),
}


# -- motion ------------------------------------------------------------------------

def integrate(fn, t, t0=0.0, step=1 / 240):
    """The integral of fn from t0 to t, by the trapezium rule."""
    if t <= t0:
        return 0.0
    n = max(2, int((t - t0) / step))
    xs = np.linspace(t0, t, n)
    ys = np.array([fn(x) for x in xs])
    return float(np.trapezoid(ys, xs))


FULL_REVS = 21.0          # the airscrew's turns a second at full revolutions


def screw(t, rpm_fn, start=0.0):
    """Angle of the airscrew at t, turning the way it does seen from ahead."""
    return start - integrate(rpm_fn, t) * 2 * math.pi * FULL_REVS


def takeoff(kind, t, t_roll, accel, v_tail=9.0, v_lift=16.0, climb=math.radians(9),
            v0=0.0, wobble=0.0, seed=0):
    """The take-off run: rolling from t_roll, the tail coming up, then the
    machine lifting off and climbing away.  Returns position, pitch, speed
    and the time spent airborne."""
    s = max(0.0, t - t_roll)
    v = v0 + accel * s
    z = v0 * s + 0.5 * accel * s * s
    pitch0 = three_point(kind)
    tail = smooth((v - v_tail) / 3.5)
    lift = smooth((v - v_lift) / 3.0)
    pitch = pitch0 * (1 - tail) + math.radians(1.0) * tail + climb * lift
    # Height gained once the speed is there.
    t_off = None
    if v > v_lift and accel > 0:
        t_off = t_roll + (v_lift - v0) / accel
    air = 0.0 if t_off is None else t - t_off
    y = rest_height(kind, pitch) + (0.0 if t_off is None else
                                    v * math.tan(climb) * air * smooth(air / 1.2) * 0.8)
    bump = 0.0
    if 0.0 < s and air <= 0.0:
        bump = 0.025 * math.sin(t * 17 + seed) + 0.015 * math.sin(t * 31 + seed * 2)
    roll = wobble * math.sin(t * 2.3 + seed) * (0.3 + air)
    return np.array([0.0, y + bump * min(1.0, v / 4), z]), pitch, roll, v, air


def three_point(kind):
    return M.three_point(kind)


def rest_height(kind, pitch, roll=0.0):
    return M.rest_height(kind, pitch, roll)


def landing(kind, t, t_touch, z_touch, speed, glide, flare=1.0, stop=2.6,
            bounce=0.0, bounce_t=0.45):
    """A gliding approach that flares and touches down at t_touch, z_touch
    on all three points, bounces if it must, and rolls to a stop.
    Returns position, pitch, speed and the height of any bounce."""
    pitch3 = three_point(kind)
    if t < t_touch:
        dt = t_touch - t
        z = z_touch - speed * dt
        f = smooth(1 - dt / flare)
        h = speed * dt * math.tan(glide) * (1 - 0.75 * f) * (1 - 0.6 * f) + 0.0
        pitch = -glide * 0.6 * (1 - f) + pitch3 * f
        v = speed
        return np.array([0.0, rest_height(kind, pitch) + h, z]), pitch, v, h
    s = t - t_touch
    dec = speed / stop
    s_ = min(s, stop)
    z = z_touch + speed * s_ - 0.5 * dec * s_ * s_
    v = max(0.0, speed - dec * s)
    h = 0.0
    if bounce and s < bounce_t:
        h = bounce * math.sin(math.pi * s / bounce_t)
    elif bounce and s < bounce_t * 1.6:
        h = bounce * 0.25 * math.sin(math.pi * (s - bounce_t) / (bounce_t * 0.6))
    pitch = pitch3 - math.radians(3) * (h / max(bounce, 0.01) if bounce else 0)
    return np.array([0.0, rest_height(kind, pitch) + h, z]), pitch, v, h


def rpm_curve(points):
    """A function through (time, value) points, eased between them."""
    def fn(x):
        if x <= points[0][0]:
            return points[0][1]
        for (t0, a), (t1, b) in zip(points[:-1], points[1:]):
            if x < t1:
                return lerp(a, b, smooth((x - t0) / (t1 - t0)))
        return points[-1][1]
    return fn


def _along(path, t):
    """Walk a path of (time, (x, z)) points: position, whether moving, the
    heading and the speed."""
    if t <= path[0][0]:
        (x, z) = path[0][1]
        return x, z, False, 0.0, 0.0
    for (t0, a), (t1, b) in zip(path[:-1], path[1:]):
        if t0 <= t < t1:
            k = (t - t0) / (t1 - t0)
            x = a[0] + (b[0] - a[0]) * k
            z = a[1] + (b[1] - a[1]) * k
            dist = math.hypot(b[0] - a[0], b[1] - a[1])
            face = math.atan2(b[0] - a[0], b[1] - a[1])
            return x, z, dist > 0.05, face, dist / (t1 - t0)
    (x, z) = path[-1][1]
    return x, z, False, 0.0, 0.0


def chock(f, p, yaw=0.0):
    """A wooden chock with its rope eye."""
    f.add(M.Box(p, M.ry(yaw).T, (0.30, 0.11, 0.12), M.WOOD * 1.15))


# -- the films -----------------------------------------------------------------------

def film_takeoff_dawn(t):
    """Dawn.  The mechanic swings the airscrew and the rotary catches in a
    cloud of castor-oil smoke; he steps back and away, and the chocks are
    hauled out by their ropes from beyond the wingtip.  Cut to the men
    standing well clear and aft, out of the machine's path and the sweep
    of its wings: the pilot opens up, the machine runs away from us, lifts
    its tail and climbs into the sun."""
    kind = "scout"
    mach = MACHINES["scout"]
    t_catch, t_cut, gap = 1.0, 3.3, 3.0
    story = t if t < t_cut else t + gap       # the cut leaves out a while
    t_open = 3.85 + gap                       # in story time
    rpm_fn = rpm_curve([(0.0, 0.0), (0.62, 0.0), (0.80, 0.12), (0.98, 0.04),
                        (t_catch, 0.05), (t_catch + 0.2, 0.62), (t_catch + 1.2, 0.38),
                        (t_open, 0.38), (t_open + 1.0, 1.0)])
    rpm = rpm_fn(story)
    blade0 = math.radians(14)
    ang = screw(story, rpm_fn, blade0)
    pos, pitch, roll, v, air = takeoff(kind, story, t_open + 0.3, 5.6, v_lift=15.0,
                                       wobble=0.01)
    R = M.attitude(0.0, pitch, roll)
    power = 0.0 if story < t_catch else (0.3 if story < t_open else
                                         0.3 + 0.7 * smooth((story - t_open) / 0.7))
    plane = Plane(kind, pos, R, prop=ang, spin=smooth((story - t_catch) / 0.25),
                  rpm=rpm, power=power, running=story >= t_catch, machine=mach, speed=v)
    swinger, chocks = CREW["swinger"], CREW["chocks"]
    hub = mach.world(pos, R, mach.hub())
    if t < t_cut:
        cam = ((5.6, 1.55, 7.6), (-0.6, 1.25, 1.2), 0.0)
    else:
        follow = max(0.0, pos[2] - 2.0)
        cam = ((10.5, 2.0, -9.0), (0.4, 1.2 + max(0.0, pos[1] - 1.3) * 0.9,
                                   1.0 + follow * 0.94), 0.0)

    def men(f):
        if t < t_cut:
            # The swing: hands on the blade at two o'clock, a sharp pull
            # down, his weight going back and away from the disc.
            pull = smooth((story - 0.62) / 0.22)
            if story < 1.15:
                a = ang if story < 0.9 else blade0 - math.radians(110)
                tip = mach.world(pos, R, mach.hub() + np.array(
                    [math.cos(a), math.sin(a), 0.06]) * np.array([0.88, 0.88, 1]))
                hand2 = mach.world(pos, R, mach.hub() + np.array(
                    [math.cos(a) * 0.70, math.sin(a) * 0.70, 0.08]))
                if story > 0.9:
                    tip = tip + np.array([0.1, -0.2, 0.6])
                    hand2 = hand2 + np.array([0.1, -0.2, 0.6])
                base = np.array([0.55, 0.0, 2.75]) + np.array([0.05, 0, 0.45]) * pull
                pel = base + np.array([0.0, 0.92 - 0.10 * pull, 0.0])
                swinger.draw(f, pel, math.pi - 0.15,
                             [base + [-0.15, 0, -0.10 + 0.1 * pull], base + [0.18, 0, 0.18 + 0.3 * pull]],
                             [hand2, tip], lean=0.10 + 0.25 * pull - 0.35 * smooth((story - 0.85) / 0.3),
                             head_tilt=0.25 * (1 - pull))
            else:
                path = [(1.15, (0.70, 3.45)), (1.55, (-0.6, 4.0)), (2.4, (-3.4, 3.6)),
                        (3.3, (-5.3, 2.4))]
                x, zz, moving, face, spd = _along(path, story)
                swinger.walk(f, x, zz, face, story, speed=max(spd, 1.0),
                             head_turn=0.5 * smooth((story - 1.2) / 0.5))
            # The chocks man, outboard of the left wingtip with the ropes.
            hk = smooth((story - 1.9) / 0.6)
            cx, cz = -5.9 - 1.5 * hk, 1.0 - 0.6 * hk
            hands = [np.array([cx + 0.35, 0.80, cz + 0.05]), np.array([cx + 0.40, 0.76, cz - 0.05])]
            chocks.draw(f, np.array([cx, 0.90 - 0.12 * hk, cz]), math.pi / 2,
                        [np.array([cx - 0.12 - 0.25 * hk, 0, cz + 0.12]),
                         np.array([cx + 0.18, 0, cz - 0.12])],
                        hands, lean=0.05 - 0.40 * hk, head_turn=-0.3)
            k = mach.k
            for side in (-1, 1):
                start = mach.world(pos, R, (side * k.wheel[0], 0, k.wheel[2] + 0.42))
                start[1] = 0.11
                end = np.array([cx + 0.9 + 0.5 * (side > 0), 0.11, cz + 0.4 * side])
                p = start + (end - start) * hk
                chock(f, p)
                f.add(M.Capsule(hands[0], p + [0.25, 0, 0], 0.012, M.ROPE, shadow=False))
        else:
            # Well clear, aft of the left wing, watching it go.
            look = smooth((story - t_open) / 3.0)
            swinger.stand(f, -9.4, -3.6, 0.35, story, arms="behind",
                          head_turn=0.45 * look)
            chocks.stand(f, -10.6, -2.4, 0.55, story, arms="hang", head_turn=0.3 * look)

    def fx(f):
        if t >= t_cut:
            chock(f, np.array([-10.6, 0.11, -3.3]), 0.3)
            chock(f, np.array([-11.2, 0.11, -3.0]), 0.1)
        if story > t_catch:
            # Castor-oil smoke: a burst at the catch, then wisps blown back.
            for i in range(14):
                young = story - t_catch
                if young < 0.7:
                    age = young * 1.5 - i * 0.05
                else:
                    age = (young * 1.3 + i / 14) % 1.0
                if not 0 < age < 1:
                    continue
                blow = 3.0 + 7 * power
                p = mach.world(pos, R, (0.5 * math.sin(i * 2.4), 0.3 + 0.2 * math.cos(i),
                                        1.7 - age * blow))
                p = p + np.array([0.6 * age, age * (0.9 - 0.6 * power), 0])
                burst = 1.7 if young < 0.7 else 1.0
                f.puff(p, (0.25 + age * 0.9) * burst, np.array([0.60, 0.61, 0.64]),
                       0.32 * (1 - age) * burst / 1.4)
        if story > t_open and air < 1.0:
            for i in range(12):
                age = (i / 12 + story * 2.2) % 1.0
                side = (i % 2) * 2 - 1
                p = np.array([side * (0.9 + age * 1.6), 0.25 + age * 1.1,
                              pos[2] + 0.4 - age * (3 + v * 0.35)])
                f.puff(p, 0.4 + age * 1.4, np.array([0.55, 0.48, 0.38]),
                       0.22 * (1 - age) * min(1.0, power) * (1 - min(air, 1.0)))

    shot = Shot(cam, "dawn", [plane], men=[men], fx=fx, wind=(0.2, 0.45), ground=field())
    shot.events = {"open": t_open - gap, "cut": t_cut, "story": story}
    return shot


def film_takeoff_three(t):
    """A flight of three of the flat-nosed scouts, in a vee, rolls across
    the field and lifts together past the camera, each engine a little
    apart from the others."""
    planes = []
    names = ("late_a", "late_b", "late_c")
    for k, (dx, dz, lag) in enumerate(((0.0, 0.0, 0.0), (-13.0, -10.0, 0.35), (13.0, -10.0, 0.2))):
        pos, pitch, roll, v, air = takeoff("late", t, 0.2 + lag, 4.4, v0=6.0,
                                           wobble=0.02, seed=k)
        pos = pos + np.array([dx, 0.0, dz - 15.0])
        R = M.attitude(0.0, pitch, roll)
        rpm = 0.9 + 0.1 * smooth((t - 0.4 - lag) / 0.8) + 0.015 * math.sin(t * 3 + k)
        planes.append(Plane("late", pos, R, prop=t * 3.0 + k, spin=1.0, rpm=rpm,
                            power=0.85 + 0.15 * smooth((t - lag) / 0.6), running=True,
                            machine=MACHINES[names[k]], speed=v))
    # The camera keeps the whole vee in the frame: it looks at the middle
    # of the three, from far enough out that the nearer wingman passes
    # well inside the edge.
    ctr = sum(p.pos for p in planes) / 3
    top = max(p.pos[1] for p in planes)
    cam = ((44.0, 2.0, 4.0), (ctr[0] - 2.0, 1.4 + max(0.0, top - 1.3) * 0.7, ctr[2]), 0.0)

    def fx(f):
        for p in planes:
            air = p.pos[1] - rest_height("late", 0.0)
            if air > 2.5:
                continue
            for j in range(8):
                age = (j / 8 + t * 2.1 + p.pos[0] * 0.1) % 1.0
                side = (j % 2) * 2 - 1
                q = np.array([p.pos[0] + side * (1.0 + age), 0.3 + age * 1.3,
                              p.pos[2] + 0.6 - age * 7])
                f.puff(q, 0.5 + age * 1.6, np.array([0.62, 0.58, 0.48]),
                       0.20 * (1 - age) * (1 - air / 2.5))

    shot = Shot(cam, "day", planes, fx=fx, wind=(0.4, 0.7), ground=field())
    return shot


def film_takeoff_mud(t):
    """A field gone to mud: the machine wallows, slews, bounces off a rut
    and throws mud from its wheels, and gets off at the far end."""
    kind = "scout"
    pos, pitch, roll, v, air = takeoff(kind, t, 0.0, 3.6, v0=4.0, v_lift=17.5)
    pos = pos + np.array([0.0, 0.0, -6.0])
    bump = max(0.0, math.sin((t - 2.0) * 4.6)) * 0.55 if 2.0 < t < 2.68 else 0.0
    wallow = math.sin(t * 6.3) * 0.045 * (1 - min(1.0, air))
    pos[1] += bump
    pitch += math.radians(4) * bump
    R = M.attitude(0.03 * math.sin(t * 1.7), pitch, wallow)
    plane = Plane(kind, pos, R, prop=t, spin=1.0, rpm=1.0, power=1.0, running=True,
                  machine=MACHINES["scout_mud"], speed=v)
    plane.bump_t = 2.0
    cam = ((-11.0, 2.6, 8.0), (0.0, 0.9 + max(0.0, pos[1] - 1.4) * 0.8, pos[2] + 1.5), 0.0)

    def fx(f):
        rng = np.random.default_rng(int(t * FPS))
        if air < 0.6:
            k = mach_k("scout")
            for side in (-1, 1):
                w = MACHINES["scout_mud"].world(pos, R, (side * k.wheel[0], k.wheel[1] - k.wheel[3], k.wheel[2]))
                w[1] = 0.1
                for i in range(26):
                    age = rng.random()
                    p = w + np.array([side * age * 0.9 + rng.normal(0, 0.2),
                                      age * 2.2 - age * age * 1.9, -age * (2.5 + v * 0.2)])
                    f.puff(p, 0.08 + 0.13 * rng.random(), np.array([0.13, 0.10, 0.07]), 1.0,
                           lit=True)
                for i in range(3):
                    age = rng.random()
                    p = w + np.array([side * 0.4, 0.2 + age * 0.8, -age * 3.5])
                    f.puff(p, 0.4 + age, np.array([0.40, 0.34, 0.27]), 0.18 * (1 - age))
                # Water thrown up white from the ruts and the puddles in a
                # sheet off each wheel, falling back.
                for i in range(18):
                    age = rng.random()
                    p = w + np.array([side * (0.2 + age * 1.4) + rng.normal(0, 0.1),
                                      age * 1.5 - age * age * 1.3, -age * (1.5 + v * 0.18)])
                    f.puff(p, 0.10 + 0.30 * age, np.array([0.78, 0.78, 0.76]),
                           0.55 * (1 - age) * min(1.0, v / 6))

    shot = Shot(cam, "overcast", [plane], fx=fx, wind=(0.9, 0.85),
                ground=field(mud=1.0, wet=0.3, tracks=(0.0, -3.5, 4.0, -9.0)))
    return shot


def mach_k(kind):
    return M.KINDS[kind]


def film_takeoff_1914(t):
    """1914.  The engine of the two-seater ticking over, the pilot already
    in front; the observer walks up and climbs into the rear cockpit by
    the step in its side.  Cut to the crew standing well clear behind the
    wing as the machine trundles away and lifts."""
    kind = "twoseat"
    mach = MACHINES["twoseat"]
    t_cut, gap = 3.7, 2.0
    story = t if t < t_cut else t + gap
    t_open = 4.0 + gap
    rpm_fn = rpm_curve([(0.0, 0.40), (t_open, 0.40), (t_open + 1.0, 1.0)])
    pos, pitch, roll, v, air = takeoff(kind, story, t_open + 0.35, 5.4, v_lift=13.0,
                                       climb=math.radians(10))
    R = M.attitude(0.0, pitch, roll)
    power = 0.3 if story < t_open else 0.3 + 0.7 * smooth((story - t_open) / 0.9)
    seated = story > 3.15
    plane = Plane(kind, pos, R, prop=screw(story, rpm_fn), spin=1.0, rpm=rpm_fn(story),
                  power=power, running=True, machine=mach,
                  crew=("pilot", "observer" if seated else None), speed=v)
    obs, rigger, fitter = CREW["observer"], CREW["rigger"], CREW["fitter"]
    k = mach.k
    if t < t_cut:
        cam = ((-6.8, 1.75, -4.6), (-0.4, 1.35, -1.0), 0.0)
    else:
        follow = max(0.0, pos[2])
        cam = ((-11.0, 2.1, -11.0), (-0.5, 1.3 + max(0.0, pos[1] - 1.4) * 0.9,
                                     0.5 + follow * 0.95), 0.0)

    def W_(v):
        return mach.world(pos, R, v)

    def men(f):
        if t < t_cut and not seated:
            cz = k.cockpits[1]
            rim_l = W_((-0.36, 0.83, cz + 0.25))
            rim_r = W_((-0.36, 0.83, cz - 0.25))
            step = W_((-0.43, 0.02, cz + 0.05))
            if story < 1.35:
                x, zz, moving, face, spd = _along([(0.0, (-4.6, -3.4)), (1.35, (-1.25, -1.95))], story)
                obs.walk(f, x, zz, face, story, speed=spd)
            else:
                u = smooth((story - 1.35) / 0.45)        # reaching up
                w = smooth((story - 1.8) / 0.75)         # hauling himself up
                s_ = smooth((story - 2.55) / 0.6)        # in and down
                base = np.array([-1.25, 0.0, -1.95])
                pel = base + np.array([0.0, 0.93, 0.0])
                pel = pel + (W_((-0.75, 0.55, cz)) - pel) * w
                pel = pel + (W_((0.0, 0.30, cz)) - pel) * s_
                hands = [base + [0.2, 1.0, 0.2] + (rim_l - base - [0.2, 1.0, 0.2]) * u,
                         base + [0.2, 1.0, -0.2] + (rim_r - base - [0.2, 1.0, -0.2]) * u]
                foot_l = base + [0.05, 0.0, 0.12]
                foot_l = foot_l + (step - foot_l) * smooth((story - 1.55) / 0.3)
                foot_l = foot_l + (W_((-0.1, 0.25, cz + 0.15)) - foot_l) * s_
                foot_r = base + [-0.05, 0.0, -0.15]
                over = W_((-0.15, 1.05, cz - 0.1))
                inn = W_((0.08, 0.15, cz - 0.15))
                foot_r = foot_r + (over - foot_r) * w
                foot_r = foot_r + (inn - foot_r) * s_
                obs.draw(f, pel, math.pi / 2 - 0.15 * w, [foot_l, foot_r], hands,
                         lean=0.15 + 0.35 * u - 0.3 * s_, head_tilt=0.1)
        # Rigger and fitter: a hand ready, then standing back, clear.
        if t < t_cut:
            rigger.stand(f, -6.9, -2.8, 0.9, story, arms="hips", head_turn=0.4)
            fitter.stand(f, -7.8, 0.6, 1.4, story, arms="behind", head_turn=-0.2)
        else:
            look = smooth((story - t_open) / 2.5)
            rigger.stand(f, -9.8, -4.2, 0.5, story, arms="hips", head_turn=0.4 * look)
            if story < t_open + 1.5:
                hand = np.array([-10.6 + 0.0, 2.05 + 0.15 * math.sin(story * 9), -2.6])
                fitter.stand(f, -10.9, -2.8, 0.7, story,
                             hands=[np.array([-11.1, 0.4, -2.8]), hand])
            else:
                fitter.stand(f, -10.9, -2.8, 0.7, story, arms="hang", head_turn=0.3)

    def fx(f):
        if power > 0.4 and air < 1.0:
            for i in range(10):
                age = (i / 10 + story * 2.0) % 1.0
                side = (i % 2) * 2 - 1
                p = np.array([side * (1.0 + age * 1.5), 0.25 + age, pos[2] - age * (3 + v * 0.3)])
                f.puff(p, 0.4 + age * 1.3, np.array([0.60, 0.56, 0.48]),
                       0.18 * (1 - age) * power * (1 - min(air, 1.0)))
        # A thin haze from the exhaust stack, blown back.
        for i in range(6):
            age = (i / 6 + story * 0.9) % 1.0
            top = W_((0.0, k.gap + 0.65, 1.95 - age * (2 + 6 * power)))
            f.puff(top + np.array([0, age * 0.4, 0]), 0.15 + age * 0.5,
                   np.array([0.45, 0.45, 0.47]), 0.18 * (1 - age))

    shot = Shot(cam, "day", [plane], men=[men], fx=fx, wind=(0.3, 0.5), ground=field())
    shot.events = {"open": t_open - gap, "cut": t_cut, "story": story}
    return shot


def film_land_dusk(t):
    """Dusk.  The scout comes in over the hedge against the last of the
    light, blipping its rotary, cuts it, floats, and puts down on all
    three points; the tail skid ploughs, and it rolls up towards us."""
    kind = "scout"
    t_touch = 3.4
    pos, pitch, v, h = landing(kind, t, t_touch, 20.0, 17.0, math.radians(5), flare=1.4,
                               stop=3.0, bounce=0.12, bounce_t=0.35)
    R = M.attitude(0.0, pitch, 0.03 * math.sin(t * 1.9) * (t < t_touch))
    # The blip switch: bursts of power on the way down, then off for good.
    blip = 0.0
    if t < 2.7:
        blip = 1.0 if (math.sin(t * 5.2) > 0.1) else 0.0
    rpm_fn = rpm_curve([(0.0, 0.55), (2.7, 0.50), (t_touch, 0.30), (t_touch + 2.0, 0.08),
                        (t_touch + 2.6, 0.0)])
    plane = Plane(kind, pos, R, prop=screw(t, rpm_fn), spin=min(1.0, rpm_fn(t) * 3.5),
                  rpm=rpm_fn(t), power=0.55 * blip, running=t < 2.7, machine=MACHINES["scout"],
                  speed=v)
    plane.blip = blip
    cam = ((8.5, 1.6, 48.0), (0.5, 1.5 + max(0.0, pos[1] - 1.3) * 0.8, pos[2]), 0.0)

    def fx(f):
        if t_touch - 0.05 < t < t_touch + 2.4:
            for i in range(8):
                age = (i / 8 + t * 2.0) % 1.0
                side = (i % 2) * 2 - 1
                p = np.array([side * 0.9, 0.2 + age * 0.9, pos[2] - 0.5 - age * 3])
                f.puff(p, 0.4 + age * 1.0, np.array([0.55, 0.47, 0.38]),
                       0.22 * (1 - age) * min(1.0, v / 10))
            # Skid furrow dust from the tail.
            sk = MACHINES["scout"].world(pos, R, (0, mach_k(kind).skid[1], mach_k(kind).skid[0]))
            for i in range(4):
                age = (i / 4 + t * 2.6) % 1.0
                f.puff(sk + np.array([0, 0.1 + age * 0.5, -age * 1.5]), 0.25 + age * 0.6,
                       np.array([0.5, 0.42, 0.33]), 0.25 * (1 - age) * min(1.0, v / 8))

    shot = Shot(cam, "dusk", [plane], fx=fx, wind=(0.4, 0.3), ground=field())
    shot.events = {"touch": t_touch}
    return shot


def film_land_shot_up(t):
    """Shot about: the flat-nosed scout comes home with its fabric in
    ribbons and holed through, a trail of smoke from the engine, which
    misses and coughs; it lands heavily, bounces, drops onto one wheel,
    lurches and comes to rest whole.  The crew run out from the hangars."""
    kind = "late"
    mach = MACHINES["late_shot"]
    t_touch = 2.6
    pos, pitch, v, h = landing(kind, t, t_touch, 6.0, 20.0, math.radians(7), flare=0.6,
                               stop=2.2, bounce=1.0, bounce_t=0.62)
    wob = (math.sin(t * 5.3) * 0.12 + math.sin(t * 2.1) * 0.07) * (1 - smooth((t - t_touch) / 0.8))
    lurch = 0.0
    if t > t_touch:
        s = t - t_touch
        lurch = smooth((s - 0.95) / 0.25) * (1 - smooth((s - 1.5) / 0.9))
    roll = wob + math.radians(-11) * lurch
    yaw = math.radians(14) * smooth((t - t_touch - 1.0) / 1.4)
    R = M.attitude(yaw, pitch, roll)
    pos = pos.copy()
    pos[1] = rest_height(kind, pitch, roll) + h if t > t_touch else pos[1] + abs(math.sin(roll)) * 0.5
    if t > t_touch + 1.0:
        pos[0] += -1.8 * smooth((t - t_touch - 1.0) / 1.5)
    rpm_fn = rpm_curve([(0.0, 0.5), (t_touch - 0.2, 0.45), (t_touch + 0.4, 0.2), (t_touch + 1.6, 0.0)])
    miss = (math.sin(t * 13.1) + math.sin(t * 7.7)) > 0.6
    running = t < t_touch + 0.3
    plane = Plane(kind, pos, R, prop=screw(t, rpm_fn, 0.4), spin=min(1.0, rpm_fn(t) * 3),
                  rpm=rpm_fn(t), power=(0.15 if miss else 0.5) * running, running=running,
                  machine=mach, speed=v, sputter=1.0)
    rng = np.random.default_rng(4)
    plane.tatters = [(x, y, z, rng.uniform(0.6, 1.1), rng.uniform(0.14, 0.30), rng.uniform(0, 6))
                     for x, y, z in ((2.6, 1.56, -0.42), (3.3, 1.56, -0.45), (1.5, 0.06, -0.55),
                                     (-2.2, 1.56, -0.42), (0.42, 0.55, -2.6), (2.0, 1.56, -0.5),
                                     (-1.4, 0.06, -0.55), (0.42, 0.35, -1.5))]
    cam = ((8.5, 1.65, 35.0), (pos[0] * 0.8, 1.6 + max(0.0, pos[1] - 1.4) * 0.7,
                               pos[2] - 1.0), 0.0)
    runners = [CREW["runner1"], CREW["runner2"], CREW["runner3"]]

    def men(f):
        if t < t_touch + 1.2:
            return
        for k, m in enumerate(runners):
            s = t - (t_touch + 1.2) - k * 0.25
            if s < 0:
                continue
            x0, z0 = -24.0 - k * 1.5, 18.0 + k * 2.5
            tx, tz = pos[0] - 7.0 - k * 1.2, pos[2] + 2.0 + k * 1.8
            d = math.hypot(tx - x0, tz - z0)
            run = min(s * 4.6, d)
            x = x0 + (tx - x0) * run / d
            z = z0 + (tz - z0) * run / d
            face = math.atan2(tx - x0, tz - z0)
            m.walk(f, x, z, face, t, speed=4.6, run=True)

    def fx(f):
        # The trail of smoke, thinning as the engine dies.
        for i in range(22):
            age = i * 0.08
            tt = t - age
            if tt < 0:
                continue
            p_, _, _, _ = landing(kind, tt, t_touch, 6.0, 20.0, math.radians(7), flare=0.6,
                                  stop=2.2, bounce=1.0, bounce_t=0.62)
            src = p_ + np.array([0.45, 0.35, 1.2])
            src = src + np.array([age * 0.9, age * 0.8, 0])
            dens = 0.7 * (1 - age / 1.8) * (1 - 0.6 * smooth((tt - t_touch) / 1.5))
            if dens > 0:
                f.puff(src, 0.30 + age * 1.2, np.array([0.12, 0.115, 0.11]), dens)
            # Steam from the holed radiator, white and quick to go.
            if i < 9:
                st = p_ + np.array([-0.2, 0.75, 2.0]) + np.array([age * 0.5, age * 1.0, 0])
                f.puff(st, 0.2 + age * 0.8, np.array([0.95, 0.95, 0.95]), 0.45 * (1 - age / 0.72))
        if t_touch - 0.05 < t < t_touch + 2.5:
            s = t - t_touch
            for i in range(12):
                age = (i / 12 + t * 1.8) % 1.0
                side = (i % 2) * 2 - 1
                p = np.array([pos[0] + side * 1.3, 0.2 + age * 1.7, pos[2] + 1 - age * 4])
                f.puff(p, 0.6 + age * 1.6, np.array([0.62, 0.56, 0.46]),
                       0.32 * (1 - age) * (1 - smooth((s - 1.8) / 0.7)))

    shot = Shot(cam, "day", [plane], men=[men], fx=fx, wind=(0.5, 0.6), ground=field())
    shot.events = {"touch": t_touch, "lurch": t_touch + 1.05}
    return shot


def film_land_dead_stick(t):
    """Dead stick: the two-seater's engine is stopped and its airscrew
    stands still.  A steep, quiet glide, a flare over the grass, a short
    run and stillness."""
    kind = "twoseat"
    t_touch = 3.3
    pos, pitch, v, h = landing(kind, t, t_touch, 10.0, 16.0, math.radians(11), flare=1.1,
                               stop=1.9, bounce=0.08, bounce_t=0.3)
    R = M.attitude(0.0, pitch, 0.035 * math.sin(t * 1.3) * (t < t_touch))
    plane = Plane(kind, pos, R, prop=math.radians(78), spin=0.0, rpm=0.0, power=0.0,
                  running=False, machine=MACHINES["twoseat_glide"], speed=v,
                  crew=("pilot", "observer"))
    cam = ((-12.5, 1.5, 34.0), (0.0, 1.9 + max(0.0, pos[1] - 1.4) * 0.75,
                                  pos[2] + 1.0), 0.0)

    def fx(f):
        if t_touch - 0.05 < t < t_touch + 1.6:
            for i in range(8):
                age = (i / 8 + t * 2.0) % 1.0
                side = (i % 2) * 2 - 1
                p = np.array([side * 1.0, 0.2 + age * 0.8, pos[2] - age * 2.5])
                f.puff(p, 0.4 + age, np.array([0.6, 0.56, 0.48]), 0.18 * (1 - age) * min(1, v / 8))

    shot = Shot(cam, "overcast", [plane], fx=fx, wind=(0.7, 0.4), ground=field())
    shot.events = {"touch": t_touch}
    return shot


def film_land_rain(t):
    """In rain: grey murk, the field standing in water, the scout feeling
    its way down blipping its engine, spray from its wheels."""
    kind = "scout"
    t_touch = 3.0
    pos, pitch, v, h = landing(kind, t, t_touch, 16.0, 16.0, math.radians(5), flare=1.0,
                               stop=2.6, bounce=0.18, bounce_t=0.32)
    R = M.attitude(0.0, pitch, 0.05 * math.sin(t * 2.4) * (t < t_touch))
    blip = 1.0 if (t < 2.4 and math.sin(t * 4.4 + 1) > 0.0) else 0.0
    rpm_fn = rpm_curve([(0.0, 0.55), (2.4, 0.50), (t_touch, 0.32), (t_touch + 2.0, 0.05),
                        (t_touch + 2.5, 0.0)])
    plane = Plane(kind, pos, R, prop=screw(t, rpm_fn), spin=min(1.0, rpm_fn(t) * 3.5),
                  rpm=rpm_fn(t), power=0.55 * blip, running=t < 2.4,
                  machine=MACHINES["scout_rain"], speed=v)
    plane.blip = blip
    cam = ((-8.5, 1.6, 40.0), (0.0, 1.5 + max(0.0, pos[1] - 1.3) * 0.8, pos[2]), 0.0)

    def fx(f):
        if t > t_touch - 0.05:
            rng = np.random.default_rng(int(t * FPS))
            k = mach_k(kind)
            for side in (-1, 1):
                w = MACHINES["scout_rain"].world(pos, R, (side * k.wheel[0], 0, k.wheel[2]))
                w[1] = 0.05
                for i in range(14):
                    age = rng.random()
                    p = w + np.array([side * age * 1.3, age * 1.2 - age * age * 0.9, -age * 3.2])
                    f.puff(p, 0.12 + 0.25 * age, np.array([0.80, 0.81, 0.83]),
                           0.45 * (1 - age) * min(1.0, v / 6))

    shot = Shot(cam, "rain", [plane], fx=fx, wind=(0.8, 0.9),
                ground=field(wet=1.0, mud=0.35, rain=1.0), rain=1.0)
    shot.events = {"touch": t_touch}
    return shot


FILMS = {
    "takeoff-1-dawn-prop-swing": (film_takeoff_dawn, 8.0, "leader"),
    "takeoff-2-flight-of-three": (film_takeoff_three, 6.0, "cut"),
    "takeoff-3-muddy-field": (film_takeoff_mud, 5.5, "cut"),
    "takeoff-4-1914-observer-climbs-in": (film_takeoff_1914, 8.2, "cut"),
    "landing-1-dusk-three-point": (film_land_dusk, 6.5, "leader"),
    "landing-2-shot-up": (film_land_shot_up, 6.5, "cut"),
    "landing-3-dead-stick": (film_land_dead_stick, 5.5, "cut"),
    "landing-4-rain": (film_land_rain, 6.0, "cut"),
}


# -- checking the men keep clear ---------------------------------------------------------

class Collector:
    """Stands in for a Frame to gather the solids a shot would draw."""

    def __init__(self):
        self.solids = []

    def add(self, s):
        self.solids.append(s)
        return s

    def puff(self, *a, **k):
        pass

    def disc(self, *a, **k):
        pass

    def blob(self, *a, **k):
        pass


def silhouettes_meet(cam, a, b):
    """True when any ray from the camera (one a pixel) strikes both a
    solid of 'a' and a solid of 'b': the two overlap on the screen."""
    d = cam.dirs[::SS, ::SS]

    def cover(solids):
        m = np.zeros(d.shape[:2], bool)
        for s in solids:
            box = cam.screen_box(s.pts, s.pad)
            if box is None:
                continue
            x0, y0, x1, y1 = (v // SS for v in box)
            x1, y1 = max(x1, x0 + 1), max(y1, y0 + 1)
            ts, _, _ = s.hit(cam.pos, d[y0:y1, x0:x1])
            m[y0:y1, x0:x1] |= np.isfinite(ts)
        return m
    ca = cover(a)
    if not ca.any():
        return False
    # Grow the men by a pixel so that merely touching counts.
    g = ca.copy()
    g[1:] |= ca[:-1]
    g[:-1] |= ca[1:]
    g[:, 1:] |= ca[:, :-1]
    g[:, :-1] |= ca[:, 1:]
    return bool((g & cover(b)).any())


def clearance(name, step=1 / 12):
    """After the throttle is opened: the least distance from any man to the
    machine and to the sweep of its wings, and any frame in which the two
    overlap on the screen."""
    fn, length, _ = FILMS[name]
    worst = (np.inf, None)
    overlaps = []
    for i in range(int(length / step)):
        t = i * step
        shot = fn(t)
        if t < shot.events.get("open", 1e9) or not shot.men:
            continue
        cam = Camera(*shot.cam)
        cm, cp = Collector(), Collector()
        for m in shot.men:
            m(cm)
        for p in shot.planes:
            p.machine.build(cp, p.pos, p.R, prop=p.prop, prop_spin=p.spin, crew=p.crew)
        mp = np.concatenate([s.pts for s in cm.solids])
        pp = np.concatenate([s.pts for s in cp.solids])
        d = np.sqrt(((mp[:, None, :] - pp[None, :, :]) ** 2).sum(-1)).min()
        if d < worst[0]:
            worst = (d, t)
        if silhouettes_meet(cam, cm.solids, cp.solids):
            overlaps.append(round(t, 2))
    return worst, overlaps


# -- drawing a shot -------------------------------------------------------------------

def draw(shot, t):
    pos, target, roll = shot.cam
    cam = Camera(pos, target, roll)
    L = light_for(shot.weather)
    sky = Sky(light=L, seed=3, **SKIES[shot.weather])
    f = Frame(cam, sky, shot.ground or field())
    world(f, t, wind=shot.wind)
    for p in shot.planes:
        p.machine.build(f, p.pos, p.R, prop=p.prop, prop_spin=p.spin, t=t,
                        crew=p.crew, tatters=p.tatters)
        x, z = p.pos[0], p.pos[2]
        h = max(0.0, min(p.contacts()))
        f.blob(x, z + 0.2, 1.4 + h * 0.3, 3.0 + h * 0.3, 0.30 / (1 + h * 0.6))
        f.blob(x, z + 0.6, 4.0 + h * 0.3, 1.4, 0.15 / (1 + h * 0.6))
    for m in shot.men:
        m(f)
    if shot.fx:
        shot.fx(f)
    img = f.render(t) * EXPOSURE[shot.weather]
    if shot.rain:
        img = rain(img, t, shot.wind, sky.low * EXPOSURE[shot.weather])
    return img


def rain(img, t, wind, tint):
    """Rain: streaks in three depths, long and soft near the lens, fine
    and close-set far off, all slanting with the wind; and drops bursting
    on the ground nearby."""
    n = int(round(t * FPS))
    rng = np.random.default_rng(n * 7 + 1)
    S = 2
    lay = Image.new("L", (W * S, H * S), 0)
    d = ImageDraw.Draw(lay)
    slant = 0.18 + 0.25 * wind[1]
    for count, length, bright, width in ((1100, 10, 80, 1), (420, 24, 120, 2), (55, 70, 150, 4)):
        for _ in range(count):
            x, y = rng.uniform(-100, W * S), rng.uniform(-150, H * S)
            ln = length * S * rng.uniform(0.7, 1.3)
            d.line((x, y, x + ln * slant, y + ln), fill=int(bright * rng.uniform(0.6, 1.0)),
                   width=width)
    for _ in range(140):
        x, y = rng.uniform(0, W * S), rng.uniform(H * S * 0.62, H * S)
        r = rng.uniform(1, 3) * (y / (H * S)) ** 2 * S
        d.ellipse((x - r * 1.8, y - r * 0.6, x + r * 1.8, y + r * 0.6), fill=int(rng.uniform(60, 140)))
    lay = lay.filter(ImageFilter.GaussianBlur(1.2)).resize((W, H), Image.LANCZOS)
    k = np.asarray(lay, np.float64)[..., None] / 255 * 0.55
    return img * (1 - k) + np.asarray(tint) * 1.05 * k


EXPOSURE = {"dawn": 1.35, "dusk": 1.35, "rain": 1.15, "overcast": 1.1, "day": 1.0}


# -- the newsreel finish ---------------------------------------------------------------

def tone(img):
    """Light to the grey of the print: the film's shoulder and its contrast."""
    g = img @ np.array([0.30, 0.59, 0.11])
    g = np.clip(g, 0, None)
    g = g / (1 + g * 0.55) * 1.5
    g = np.clip(g, 0, 1) ** 1.1
    return np.clip((g - 0.5) * 1.28 + 0.5, 0, 1)


def film(g, n, seed):
    """Grain, gate weave, flicker, scratches, dust and a vignette over one
    grey frame; returns the grey frame (0..1)."""
    rng = np.random.default_rng(seed * 1000 + n)
    g = g * (1 + rng.normal(0, 0.03))                                   # flicker
    yy, xx = np.mgrid[0:H, 0:W]
    v = ((xx - W / 2) / (W * 0.62)) ** 2 + ((yy - H / 2) / (H * 0.62)) ** 2
    g = g * np.clip(1.05 - v * 0.55, 0, 1)                              # vignette
    g = g + rng.normal(0, 0.038, (H, W))                                # grain
    g = np.asarray(Image.fromarray(np.clip(g * 255, 0, 255).astype(np.uint8)).filter(
        ImageFilter.GaussianBlur(0.55)), np.float64) / 255
    # Scratches that run for a few frames, dust and hairs for one.
    srng = np.random.default_rng(seed * 77 + n // 7)
    for _ in range(int(srng.random() < 0.45) + int(srng.random() < 0.1)):
        x = int(srng.uniform(20, W - 20) + rng.normal(0, 1.2))
        g[:, x:x + 1] = g[:, x:x + 1] * 0.45 + (0.9 if srng.random() < 0.5 else 0.06) * 0.55
    for _ in range(rng.integers(1, 7)):
        x, y = rng.uniform(0, W), rng.uniform(0, H)
        r = rng.uniform(0.7, 2.4)
        m = (xx - x) ** 2 + (yy - y) ** 2 < r * r
        g[m] = 0.03 if rng.random() < 0.7 else 0.95
    if rng.random() < 0.18:
        x, y = rng.uniform(0, W), rng.uniform(0, H)
        pts = [(x + k * 3, y + math.sin(k * 0.6) * 4) for k in range(12)]
        im = Image.fromarray(np.clip(g * 255, 0, 255).astype(np.uint8))
        ImageDraw.Draw(im).line(pts, fill=12, width=1)
        g = np.asarray(im, np.float64) / 255
    # Gate weave: the whole frame drifts a pixel or so.
    dx = int(round(rng.normal(0, 0.6) + math.sin(n * 0.26) * 0.7))
    dy = int(round(rng.normal(0, 0.7)))
    g = np.roll(np.roll(g, dx, 1), dy, 0)
    return np.clip(g, 0, 1)


def sepia(g):
    s = np.stack([g * 1.04 + 0.03, g * 0.95 + 0.015, g * 0.80], -1)
    return Image.fromarray(np.clip(s * 255, 0, 255).astype(np.uint8))


LEADER_SECONDS = 0.75
SPLICE = 3


def leader_frame(k):
    """Academy-style leader: a number in a ring and the sweep round it,
    three, two, one, each a quarter of a second."""
    per = int(round(FPS * LEADER_SECONDS / 3))
    num = 3 - k // per
    im = Image.new("L", (W, H), 158)
    d = ImageDraw.Draw(im)
    cx, cy = W // 2, H // 2
    d.ellipse((cx - 150, cy - 150, cx + 150, cy + 150), outline=30, width=4)
    d.ellipse((cx - 120, cy - 120, cx + 120, cy + 120), outline=30, width=2)
    d.line((0, cy, W, cy), fill=30, width=2)
    d.line((cx, 0, cx, H), fill=30, width=2)
    sweep = (k % per) / per * 360
    d.pieslice((cx - 150, cy - 150, cx + 150, cy + 150), -90, -90 + sweep, fill=92)
    try:
        font = ImageFont.truetype("DejaVuSerif-Bold.ttf", 170)
    except OSError:
        font = ImageFont.load_default()
    d.text((cx, cy), str(max(num, 1)), fill=15, font=font, anchor="mm")
    return np.asarray(im, np.float64) / 255


def intro_frames(kind):
    if kind == "leader":
        return [leader_frame(k) for k in range(int(round(FPS * LEADER_SECONDS)))]
    # A splice: a black frame, a flash of clear base, a frame of fogged film.
    return [np.zeros((H, W)), np.full((H, W), 0.95), np.full((H, W), 0.35)]


def intro_length(kind):
    return len(intro_frames(kind)) / FPS


def render_one(args):
    name, i = args
    fn, length, _ = FILMS[name]
    t = i / FPS
    return tone(draw(fn(t), t)).astype(np.float32)


def main():
    out = Path(sys.argv[1])
    (out / "frames").mkdir(parents=True, exist_ok=True)
    names = sys.argv[2:] or list(FILMS)
    keys_all = []
    with Pool(WORKERS, maxtasksperchild=12) as pool:
        for ci, name in enumerate(list(FILMS)):
            if name not in names:
                continue
            fn, length, intro = FILMS[name]
            n = int(round(length * FPS))
            raw = pool.map(render_one, [(name, i) for i in range(n)], chunksize=1)
            pre = intro_frames(intro)
            grey = [film(g, k, ci) for k, g in enumerate(pre + raw)]
            np.save(out / "frames" / f"{name}.npy",
                    np.stack([(g * 255 + 0.5).astype(np.uint8) for g in grey]))
            frames = [sepia(g) for g in grey]
            frames[0].save(out / f"{name}.webp", save_all=True, append_images=frames[1:],
                           duration=int(round(1000 / FPS)), loop=0, quality=80, method=4)
            picks = [len(pre) + int(n * q) for q in (0.05, 0.3, 0.55, 0.8, 0.98)]
            keys = [frames[min(p, len(frames) - 1)] for p in picks]
            sheet = Image.new("RGB", (5 * 320, 200))
            for k, im in enumerate(keys):
                sheet.paste(im.resize((320, 200), Image.LANCZOS), (k * 320, 0))
            sheet.save(out / f"{name}-keys.png", optimize=True)
            keys_all.append((name, sheet))
            print(name, n, "frames", (out / f"{name}.webp").stat().st_size // 1024, "KB",
                  flush=True)
    if len(keys_all) == len(FILMS):
        contact = Image.new("RGB", (5 * 320, 224 * len(keys_all)), (20, 18, 16))
        d = ImageDraw.Draw(contact)
        for k, (name, sheet) in enumerate(keys_all):
            y = k * 224
            d.text((6, y + 5), name, fill=(220, 210, 190))
            contact.paste(sheet, (0, y + 24))
        contact.save(out / "contact-sheet.png", optimize=True)


if __name__ == "__main__":
    os.nice(0)
    main()
