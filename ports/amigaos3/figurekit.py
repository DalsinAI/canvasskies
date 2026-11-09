#!/usr/bin/env python3
"""A kit of whole men, rigged and dressed, for the painted scenes.

Every man is built from shaped solids (ellipsoids, tapering cones and
flat quads, scene3d.py) hung on a small skeleton, so that he can be
posed by joint angles and drawn lit by the same lamps as the room he
stands in.  No model, texture or picture from outside goes into him.

The skeleton
    pelvis - spine - chest - neck - head, with shoulder, elbow and wrist
    on each side, and hip, knee and ankle.  Lengths are in head-heights
    ('h'); a man is about seven and a half of them tall, his hips at the
    middle.  The frame is the renderer's: x across, y down, z away from
    us, and a man faces -z until he is turned.  Each joint takes angles
    in degrees (rx bends forward, ry turns, rz swings sideways).  Arms
    and legs can instead be sent to a point (two-bone reach), which is
    how a hand finds the table, a glass, a newspaper or a friend's
    shoulder.

The man
    A head of real masses: the skull, the face in front of it, the brow
    ridge that shades the eyes, cheekbones, the nose with its bridge and
    tip, lips, jaw and chin, ears; hair with some body to it, cut short
    at the back and sides and combed from a parting, grey at the temples
    on an older man; a moustache on some.  The eyes, brows, the shadow
    under the nose, the line of the mouth and the colour in the cheeks
    are not modelled but painted on afterwards in a few strokes, as a
    painter would put a face on a small figure (paint_faces).  Hands with
    a palm, four fingers of three joints and a thumb, which curl round a
    glass, a pipe, a pen or a hand of cards, lie flat, rest on piano keys
    or hold a newspaper open.  Build (broad or slight, a paunch), height
    and face are drawn from a seed, so no two match.

Outfits
    officer   service dress: tunic with its skirts, collar and tie,
              pleated pockets, buttons, the pilot's wings; the Sam Browne
              belt and its cross strap; breeches flared at the thigh;
              riding boots or puttees.  'wrap' gives the double-breasted
              R.F.C. jacket, 'shirt' a man in his shirt-sleeves (rolled
              to the elbow with 'rolled'), 'cardigan' a knitted cardigan
              over shirt and tie.
    mechanic  overalls buttoned up the front, a soft cap, puttees.
    pilot     a long leather flying coat, leather helmet and goggles,
              a scarf, sheepskin-topped boots.

A seated man brings his own chair (a bentwood chair, a leather armchair
by the stove, the piano stool), set under him wherever his pose puts
him; chair() draws one on its own, empty or to be stood on.

Poses (POSES below) cover the mess: at table with a glass, toasting,
drinking, talking, a pipe, writing a letter, a hand of cards, reading,
bowed, a head in the hands, slumped back in a chair with legs crossed,
at the piano, warming at the stove; standing with his weight on one leg,
a glass held up or held low, a hand in a pocket, leaning on the piano,
an arm round a friend, singing, hands clasped before or behind; and the
aerodrome (swinging the propeller, pulling the chocks away, waving,
running, walking clear).

How he is lit
    Drawn into an ordinary Scene he is shaded there and then, like any
    other solid.  Drawn into a GScene he is not lit at all: each pixel
    keeps what it is (its colour, its surface normal, its depth, its
    material and which man it belongs to), and paint() lights the lot
    afterwards as a painter would: the lamp's warm light wrapping softly
    round the forms and falling off with distance, a little red where
    skin turns from the light, shadow pooled where one form meets
    another (under the chin, inside an arm, where a sleeve meets the
    body), the shadows the lamp casts across a man's own body, a thin rim
    where a light is behind him, a faint bounce from below, and the
    whole broken by a soft painterly mottle so that no surface is quite
    flat.  That is what turns an assembly of solids into a man.

    fig = Figure(seed=4, outfit="officer")
    fig.pose("toast")
    fig.draw(sc, x, floor_y, z, px_per_h, yaw)

Run on its own it renders a contact sheet of outfits and poses, and a
close-up of faces and hands, both by lamplight:

    figurekit.py --sheet sheet.png [--closeup closeup.png]
"""

import argparse
import math
import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, str(Path(__file__).resolve().parent))
from scene3d import Scene, cone, fbm, normalize, rot_x, rot_y, rot_z  # noqa: E402

# ---- palettes -------------------------------------------------------------------
SKINS = [(198, 150, 120), (186, 138, 108), (206, 160, 128), (176, 128, 100),
         (194, 142, 112), (212, 168, 138)]
HAIRS = [(40, 30, 22), (58, 42, 28), (28, 22, 18), (76, 54, 32), (96, 72, 40),
         (34, 26, 22), (112, 84, 52)]
GREY_HAIR = (150, 144, 134)
KHAKI = [(80, 72, 48), (74, 68, 46), (84, 76, 50), (72, 68, 48), (82, 72, 48)]
SHIRT = (154, 138, 100)
TIE = (88, 74, 50)
KNITS = [(112, 96, 72), (84, 78, 70), (124, 108, 82), (96, 70, 52)]
BREECHES = [(122, 104, 74), (112, 98, 70), (128, 110, 78), (100, 90, 64)]
PUTTEES = (110, 98, 68)
BOOTS = (86, 48, 24)
LEATHER = (92, 48, 24)
BRASS = (200, 160, 84)
OVERALLS = [(74, 70, 58), (58, 60, 62), (92, 84, 64), (66, 62, 50)]
COAT = (100, 64, 36)
BEER = (200, 132, 44)
FOAM = (236, 226, 200)
GLASS = (150, 156, 140)
CHAIR = (66, 40, 24)
CLUB = (88, 46, 26)
PAPER = (222, 214, 192)
INK = (40, 40, 60)

# What each pixel of a man is made of, for paint(): how far the light
# wraps round it (a soft cloth takes light further round than a hard
# one), how much of it shows through (skin reddens at the edge of the
# light, beer glows), and how much it shines.
MATS = {"wool": 1, "skin": 2, "hair": 3, "shirt": 4, "leather": 5,
        "brass": 6, "glass": 7, "beer": 8, "paper": 9, "boot": 10,
        "wood": 11, "knit": 12, "dark": 13}
WRAP = {1: 0.2, 2: 0.28, 3: 0.1, 4: 0.3, 5: 0.1, 6: 0.0, 7: 0.2, 8: 0.5,
        9: 0.45, 10: 0.05, 11: 0.15, 12: 0.35, 13: 0.1}


def deg(a):
    return math.radians(a)


def unit(v):
    v = np.asarray(v, np.float32)
    n = float(np.linalg.norm(v))
    return v / n if n > 1e-6 else v


def R3(rx=0.0, ry=0.0, rz=0.0):
    """A joint's turn: ry, then rx, then rz, in degrees."""
    return (rot_y(deg(ry)) @ rot_x(deg(rx)) @ rot_z(deg(rz))).astype(
        np.float32)


def frame(y, z_hint):
    """A rotation whose local y runs along 'y' and whose local z lies as
    near 'z_hint' as it can (x completes it)."""
    y = unit(y)
    z = np.asarray(z_hint, np.float32) - np.dot(z_hint, y) * y
    if np.linalg.norm(z) < 1e-4:
        z = np.array((0, 0, 1), np.float32) - y[2] * y
        if np.linalg.norm(z) < 1e-4:
            z = np.array((1, 0, 0), np.float32) - y[0] * y
    z = unit(z)
    x = np.cross(y, z)
    return np.stack([x, y, z], 1).astype(np.float32)


def reach(S, T, a, b, pole):
    """Two-bone reach: from S toward T with bones a and b, the middle
    joint bending toward 'pole'.  Returns (middle joint, end)."""
    S = np.asarray(S, np.float32)
    d = np.asarray(T, np.float32) - S
    L = float(np.linalg.norm(d))
    u = d / max(L, 1e-6)
    L = min(max(L, abs(a - b) + 1e-3), (a + b) * 0.999)
    cosA = (a * a + L * L - b * b) / (2 * a * L)
    A = math.acos(max(-1.0, min(1.0, cosA)))
    p = np.asarray(pole, np.float32) - np.dot(pole, u) * u
    if np.linalg.norm(p) < 1e-4:
        p = np.cross(u, (1, 0, 0))
    p = unit(p)
    E = S + a * (math.cos(A) * u + math.sin(A) * p)
    return E, S + u * L


# ---- the G-buffer and flat pieces --------------------------------------------------
class GScene(Scene):
    """A scene that records what each pixel is instead of lighting it:
    channels 0-2 its colour, 3-5 its surface normal, 6 how much it
    shines, 7 how tight the shine is, 8 its rim, 9 its material (MATS),
    10 which man it belongs to (from 1).  The solids are drawn into it
    exactly as into a Scene; paint() lights it afterwards."""

    C = 11
    gbuffer = True

    def __init__(self, w, h, ss=4):
        super().__init__(w, h, ambient=(0, 0, 0), ss=ss)
        self.rgb = np.zeros((self.h, self.w, self.C), np.float32)
        self.mat = MATS["wool"]
        self.fig = 0

    def light(self, pos, n, albedo, spec=0.0, shine=16, rim=0.0):
        out = np.empty((len(pos), self.C), np.float32)
        out[:, 0:3] = albedo
        out[:, 3:6] = n
        out[:, 6] = spec
        out[:, 7] = shine
        out[:, 8] = rim
        out[:, 9] = self.mat
        out[:, 10] = self.fig
        return out


def quad(sc, pts, albedo, texture=None, spec=0.0, shine=16, rim=0.0):
    """A flat four-cornered piece at 1x (a playing card, a letter, a page
    of newspaper, a book's board): corners in order round its edge.  It
    shows whichever face is toward us.  'texture(u, v)' may return an
    albedo multiplier, or colours, over its face (u along the first
    edge, v along the last, both 0..1)."""
    P = np.asarray(pts, np.float32) * sc.ss
    nrm = np.cross(P[1] - P[0], P[3] - P[0])
    if abs(nrm[2]) < 1e-3 * (np.linalg.norm(nrm) + 1e-6):
        return                                   # edge on: nothing shows
    nrm = unit(nrm)
    if nrm[2] > 0:
        nrm = -nrm
    x0, x1 = int(max(0, P[:, 0].min() - 1)), int(min(sc.w, P[:, 0].max() + 2))
    y0, y1 = int(max(0, P[:, 1].min() - 1)), int(min(sc.h, P[:, 1].max() + 2))
    if x0 >= x1 or y0 >= y1:
        return
    m = Image.new("L", (x1 - x0, y1 - y0), 0)
    ImageDraw.Draw(m).polygon([(float(p[0] - x0), float(p[1] - y0))
                               for p in P], fill=255)
    hit = np.asarray(m) > 127
    yy, xx = np.mgrid[y0:y1, x0:x1].astype(np.float32)
    zz = P[0, 2] - (nrm[0] * (xx - P[0, 0]) + nrm[1] * (yy - P[0, 1])) / nrm[2]
    hit &= zz < sc.z[y0:y1, x0:x1]
    if not hit.any():
        return
    alb = np.broadcast_to(np.array(albedo, np.float32) / 255.0,
                          hit.shape + (3,)).copy()
    if texture is not None:
        e1, e3 = P[1] - P[0], P[3] - P[0]
        q = np.stack([xx - P[0, 0], yy - P[0, 1], zz - P[0, 2]], -1)
        u = np.clip((q @ e1) / float(e1 @ e1), 0, 1)
        v = np.clip((q @ e3) / float(e3 @ e3), 0, 1)
        t = np.asarray(texture(u, v), np.float32)
        alb *= t[..., None] if t.ndim == 2 else t
    pos = np.stack([xx, yy, zz], -1)
    n = np.broadcast_to(nrm, pos.shape)
    sc.rgb[y0:y1, x0:x1][hit] = sc.light(pos[hit], n[hit], alb[hit], spec,
                                         shine, rim)
    sc.z[y0:y1, x0:x1][hit] = zz[hit]


# ---- textures ---------------------------------------------------------------------
def body_tex(seed, yb, ry, outfit):
    """Cloth over one slice of the lofted body (yb its height, ry its
    half-height): soft folds gathered above the belt, the seam and the
    pull across the back, an easy drape below the waist, and for a long
    coat the deep folds of its skirts.  All of it gentle: hard regular
    ridges read as carved wood, not cloth."""
    rng = np.random.default_rng(seed)
    ph = rng.uniform(0, 6, 3)

    def tex(q, n):
        x, z = q[..., 0], q[..., 2]
        y = yb + q[..., 1] * ry
        out = np.ones(x.shape, np.float32)
        gather = np.exp(-((y + 0.62) / 0.14) ** 2)
        out -= 0.1 * gather * (0.5 + 0.5 * np.sin(y * 34 +
                                                  np.sin(x * 5 + ph[0]) * 2))
        back = z > 0.35
        out = np.where(back & (np.abs(x) < 0.05), out * 0.85, out)
        pull = 0.5 + 0.5 * np.sin((np.abs(x) * 3 - y * 2.2) * 2 + ph[1])
        out -= 0.06 * pull * (y < -1.0)
        drape = 0.5 + 0.5 * np.sin(x * (9 if outfit == "pilot" else 5) + ph[2])
        out -= (0.2 if outfit == "pilot" else 0.06) * drape * (y > -0.2)
        return out
    return tex


def knit_tex(z, ang):
    """The ribs of a knitted sleeve."""
    return 0.92 + 0.08 * np.sin(ang * 22) ** 2


def puttee_tex(z, ang):
    band = np.sin((z * 8 + ang * 0.5) * math.pi)
    return 0.86 + 0.14 * (band > -0.3)


def hair_tex(part, comb=1.0):
    """Hair combed back from a parting at x = 'part' (head units): fine
    streaks along the comb, and the parting itself a thin pale line."""
    def tex(q, n):
        x, y, z = q[..., 0], q[..., 1], q[..., 2]
        streak = 0.9 + 0.1 * np.sin((x - part) * 70 * comb + z * 6) * \
            np.sin(z * 23 + x * 9)
        line = np.exp(-((x - part) / 0.035) ** 2) * (y < -0.1) * (z < 0.3)
        return streak * (1 + 0.35 * line)
    return tex


def print_tex(cols=4, lines=34, seed=0):
    """A page of newsprint: columns of grey type under a black headline."""
    rng = np.random.default_rng(seed)
    ph = rng.uniform(0, 6)

    def tex(u, v):
        col = (u * cols) % 1.0
        gutter = (col > 0.06) & (col < 0.94)
        row = np.sin(v * lines * math.pi * 2 + ph) > 0.1
        head = (v < 0.14) & (v > 0.04) & (u > 0.06) & (u < 0.94)
        body = gutter & row & (v > 0.18) & (v < 0.96)
        return 1 - 0.35 * body - 0.6 * head
    return tex


def letter_tex(seed=0):
    """A sheet of writing paper with a few lines of handwriting on it."""
    rng = np.random.default_rng(seed)
    ph = rng.uniform(0, 6)

    def tex(u, v):
        row = np.abs(((v - 0.12) * 11) % 1.0 - 0.5) < 0.12
        wobble = np.sin(u * 37 + v * 13 + ph) > -0.4
        ends = u < (0.86 - 0.3 * (np.sin(v * 53 + ph) > 0.6))
        ink = row & wobble & (u > 0.1) & ends & (v > 0.1) & (v < 0.86)
        return np.where(ink[..., None], np.array(INK, np.float32) /
                        np.array(PAPER, np.float32), 1.0)
    return tex


def card_tex(suit):
    """The face of a playing card: a pip or two in its suit's colour."""
    red = suit in ("h", "d")
    ink = np.array((170, 30, 30) if red else (30, 30, 30), np.float32) / \
        np.array(PAPER, np.float32)

    def tex(u, v):
        pip = ((u - 0.5) ** 2 + ((v - 0.5) * 0.75) ** 2 < 0.03) | \
            ((u < 0.22) & (v < 0.22) & (u > 0.08) & (v > 0.06))
        return np.where(pip[..., None], ink, 1.0)
    return tex


def back_tex(u, v):
    """The back of a playing card: a red pattern inside a white border."""
    inside = (u > 0.1) & (u < 0.9) & (v > 0.08) & (v < 0.92)
    lattice = (np.sin(u * 40) * np.sin(v * 52)) > 0
    red = np.array((170, 70, 60), np.float32) / np.array(PAPER, np.float32)
    return np.where((inside & lattice)[..., None], red,
                    np.where(inside[..., None], 0.5 + red * 0.5, 1.0))


# ---- the skeleton ------------------------------------------------------------------
# (joint, parent, offset in the parent's frame, in head-heights)
BONES = [
    ("pelvis", None, (0, 0, 0)),
    ("spine", "pelvis", (0, -0.75, 0)),
    ("chest", "spine", (0, -0.9, 0)),
    ("neck", "chest", (0, -0.56, 0.03)),
    ("head", "neck", (0, -0.3, 0)),
    ("shoulder_r", "chest", (-0.88, -0.3, 0.04)),
    ("elbow_r", "shoulder_r", (0, 1.42, 0)),
    ("wrist_r", "elbow_r", (0, 1.18, 0)),
    ("shoulder_l", "chest", (0.88, -0.3, 0.04)),
    ("elbow_l", "shoulder_l", (0, 1.42, 0)),
    ("wrist_l", "elbow_l", (0, 1.18, 0)),
    ("hip_r", "pelvis", (-0.4, 0.1, 0)),
    ("knee_r", "hip_r", (0, 1.76, 0)),
    ("ankle_r", "knee_r", (0, 1.7, 0)),
    ("hip_l", "pelvis", (0.4, 0.1, 0)),
    ("knee_l", "hip_l", (0, 1.76, 0)),
    ("ankle_l", "knee_l", (0, 1.7, 0)),
]
STAND_HIP = 3.7            # floor to hip joint, standing (soles on it)
SEAT_HIP = {"bentwood": 2.0, "stool": 2.05, "club": 1.75, None: 2.0}

# Finger lengths (three joints) and their place across the knuckles,
# index first, as fractions of the hand.
FINGERS = [(0.95, -0.075), (1.0, -0.025), (0.95, 0.025), (0.78, 0.072)]


class Figure:
    """One man: his make (from a seed), his clothes, and his pose."""

    def __init__(self, seed=0, outfit="officer", style=None, grey=None,
                 moustache=None, build=None, height=None, puttees=None,
                 skin=None, hair=None, cloth=None, wings=None, lace=None,
                 knit=None, rolled=False, belt=None):
        rng = np.random.default_rng(seed)
        self.rng = rng
        self.outfit = outfit
        self.grey = bool(grey) if grey is not None else False
        self.bw = build if build is not None else \
            float(np.clip(rng.normal(1.0, 0.07), 0.88, 1.15))
        self.tall = height if height is not None else \
            float(np.clip(rng.normal(1.0, 0.03), 0.93, 1.07))
        self.paunch = 0.09 if self.grey else \
            float(max(0.0, rng.normal(0.02, 0.035)))
        self.skin = np.array(skin or SKINS[rng.integers(len(SKINS))],
                             np.float32)
        self.hair = np.array(hair or HAIRS[rng.integers(len(HAIRS))],
                             np.float32)
        self.cloth = np.array(cloth or (
            OVERALLS[rng.integers(len(OVERALLS))] if outfit == "mechanic"
            else COAT if outfit == "pilot"
            else KHAKI[rng.integers(len(KHAKI))]), np.float32) * \
            rng.normal(1, 0.03)
        self.breeches = np.array(BREECHES[rng.integers(len(BREECHES))],
                                 np.float32)
        self.knit = np.array(knit or KNITS[rng.integers(len(KNITS))],
                             np.float32)
        self.style = style or (rng.choice(["sd", "sd", "sd", "wrap"])
                               if outfit == "officer" else outfit)
        self.rolled = rolled
        if moustache is None:
            moustache = rng.choice([None, "clipped", "full", "waxed", "walrus"],
                                   p=[0.38, 0.22, 0.24, 0.08, 0.08])
        self.moustache = moustache
        self.puttees = puttees if puttees is not None else (
            outfit == "mechanic" or (outfit == "officer" and not self.grey
                                     and rng.random() < 0.3))
        self.wings = wings if wings is not None else (rng.random() < 0.85)
        self.lace = lace if lace is not None else int(rng.integers(1, 3))
        self.belt = belt if belt is not None else (
            self.grey or rng.random() < 0.8)
        # The face: its proportions, all a little different.
        f = dict(jaw=rng.normal(1, 0.07), nose=rng.normal(1, 0.1),
                 brow=rng.normal(1, 0.15), cheek=rng.normal(1, 0.15),
                 ears=rng.normal(1, 0.1), head=rng.normal(1.07, 0.03),
                 eyes=np.array(rng.choice([(60, 44, 30), (70, 90, 110),
                                           (80, 96, 80), (50, 36, 26)])),
                 part=float(rng.choice([-0.16, 0.16, 0.13, -0.12])),
                 hairline=rng.normal(0, 0.04) + (0.12 if self.grey else 0),
                 volume=rng.normal(1, 0.15),
                 lips=rng.normal(1, 0.1), ruddy=rng.uniform(0.05, 0.25))
        self.face = f
        self.seed = int(rng.integers(1 << 20))
        self.angles = {}
        self.targets = {}
        self.hands = {"r": dict(kind="relax"), "l": dict(kind="relax")}
        self.props = []
        self.mouth = 0.0
        self.eyes_shut = False
        self.seated = False
        self.chair = None
        self.foot_pitch = {"r": 0.0, "l": 0.0}

    # -- posing ---------------------------------------------------------------
    def pose(self, name, seated=None, **over):
        """Take up a named pose (POSES); keyword arguments override it."""
        spec = dict(POSES[name])
        spec.update(over)
        self.seated = spec.pop("seated", False) if seated is None else seated
        self.chair = spec.pop("chair", "bentwood" if self.seated else None)
        self.angles = {}
        for j, a in spec.pop("angles", {}).items():
            self.angles[j] = tuple(a)
        if self.seated:
            for side, sx in (("r", -1), ("l", 1)):
                self.angles.setdefault("hip_" + side, (-86, sx * -6, 0))
                self.angles.setdefault("knee_" + side, (84, 0, 0))
        self.targets = {}
        self.hands = {"r": dict(kind="relax"), "l": dict(kind="relax")}
        for side in ("r", "l"):
            h = spec.pop("hand_" + side, None)
            if h:
                h = dict(h)
                if "at" in h:
                    self.targets[side] = (np.array(h.pop("at"), np.float32),
                                          np.array(h.pop("pole", (0, 0.4, 1)),
                                                   np.float32))
                self.hands[side] = h
        # A man's carriage: the neck carried a little forward of the
        # shoulders and the head brought back up level on it, the chest
        # a touch open.  Without it he stands like a tailor's dummy.
        for j, d in (("neck", (9, 0, 0)), ("head", (-9, 0, 0)),
                     ("chest", (-3, 0, 0))):
            v = list(self.angles.get(j, (0, 0, 0)))
            self.angles[j] = tuple(v[i] + d[i] for i in range(3))
        self.mouth = spec.pop("mouth", 0.0)
        self.eyes_shut = spec.pop("shut", False)
        self.foot_pitch = {"r": spec.pop("foot_r", 0.0),
                           "l": spec.pop("foot_l", 0.0)}
        self.props = list(spec.pop("props", []))
        # Bow, lift and turn of the head, lean and twist of the body, and
        # the set of the hips (weight on one leg) given as plain numbers.
        for key, joint, axis in (("bow", "neck", 0), ("lift", "neck", 0),
                                 ("turn", "head", 1), ("tilt", "head", 2),
                                 ("lean", "spine", 0), ("twist", "chest", 1),
                                 ("slump", "pelvis", 0)):
            v = spec.pop(key, None)
            if v is None:
                continue
            a = list(self.angles.get(joint, (0, 0, 0)))
            a[axis] += -v if key == "lift" else v
            self.angles[joint] = tuple(a)
            if key in ("bow", "lift"):
                h = list(self.angles.get("head", (0, 0, 0)))
                h[0] += (-v if key == "lift" else v) * 0.6
                self.angles["head"] = tuple(h)
        # Weight on one leg ('rest' > 0 on his right, < 0 on his left):
        # the hips tip down toward the free leg, whose knee eases forward,
        # and the shoulders tip back the other way to keep him upright.
        if not self.seated:
            for side, sx in (("r", -1), ("l", 1)):
                v = list(self.angles.get("hip_" + side, (0, 0, 0)))
                self.angles["hip_" + side] = (v[0], v[1] + sx * -8,
                                              v[2] + sx * -3)
        rest = spec.pop("rest", 0)
        if rest:
            s = 1 if rest > 0 else -1
            free = "l" if s > 0 else "r"
            held = "r" if s > 0 else "l"
            a = abs(rest)

            def add(j, d):
                v = list(self.angles.get(j, (0, 0, 0)))
                self.angles[j] = tuple(v[i] + d[i] for i in range(3))
            add("pelvis", (0, 0, -s * a))
            add("spine", (0, 0, s * a * 0.8))
            add("chest", (0, 0, s * a * 0.6))
            add("head", (0, 0, -s * a * 0.3))
            add("hip_" + held, (0, 0, s * a))
            add("hip_" + free, (-a * 0.9, 0, s * a * 0.4))
            add("knee_" + free, (a * 2.6, 0, 0))
            self.foot_pitch[free] = self.foot_pitch.get(free, 0) - a * 0.4
        return self

    # -- the skeleton solved ------------------------------------------------------
    def solve(self):
        bw = self.bw
        W = {}
        for name, parent, off in BONES:
            off = np.array(off, np.float32)
            if name.startswith(("shoulder", "hip")):
                off[0] *= bw if name.startswith("shoulder") else \
                    (0.9 + 0.1 * bw)
            Rl = R3(*self.angles.get(name, (0, 0, 0)))
            if parent is None:
                P, Rp = np.zeros(3, np.float32), np.eye(3, dtype=np.float32)
            else:
                P, Rp = W[parent]
            W[name] = (P + Rp @ off, (Rp @ Rl).astype(np.float32))
            # An arm or leg sent to a point: reach for it from the root
            # joint and set both bones along the result.
            for side in ("r", "l"):
                if name == "wrist_" + side and side in self.targets:
                    S = W["shoulder_" + side][0]
                    T, pole = self.targets[side]
                    T = self.hand_wrist(side, T)
                    E, T2 = reach(S, T, 1.42, 1.18, pole)
                    zh = W["chest"][1][:, 2]
                    W["shoulder_" + side] = (S, frame(E - S, zh))
                    W["elbow_" + side] = (E, frame(T2 - E, zh))
                    W[name] = (T2, W["elbow_" + side][1])
        self.W = W
        return W

    def hand_wrist(self, side, T):
        """Where the wrist must be for the hand to do what is asked at T:
        for a grip, T is the middle of what is held."""
        h = self.hands[side]
        if h.get("kind") != "grip":
            return T
        F = self.hand_frame(side, None)
        r = h.get("radius", 0.16)
        return T - F[:, 1] * 0.36 - F[:, 2] * (r + 0.05)

    def hand_frame(self, side, W):
        """The hand's own frame: y along the fingers, z out of the palm,
        x across the knuckles."""
        h = self.hands[side]
        sx = -1 if side == "r" else 1
        if h.get("kind") == "grip":
            axis = unit(h.get("axis", (0, -1, 0)))
            palm = np.asarray(h.get("palm", (-sx, 0, 0)), np.float32)
            palm = unit(palm - np.dot(palm, axis) * axis)
            # The thumb goes round the 'axis' end of what is held.
            x = axis * (1 if side == "l" else -1)
            y = np.cross(palm, x)
            return np.stack([x, y, palm], 1).astype(np.float32)
        if "fingers" in h or "palm" in h:
            return frame(h.get("fingers", (0, 1, 0)),
                         h.get("palm", (-sx, 0, 0)))
        if W is None:
            return np.eye(3, dtype=np.float32)
        Rw = W["wrist_" + side][1]
        return (Rw @ rot_y(deg(90 * -sx))).astype(np.float32)

    def axis_at(self, W, yb):
        """The point on his spine at body height yb (head-heights from
        the hips, as he stands at rest) and the frame of the body there,
        blended between the joints either side."""
        names = ["pelvis", "spine", "chest", "neck"]
        ys = [0.0, -0.75, -1.65, -2.21]
        if yb >= 0:
            P, R = W["pelvis"]
            return P + R @ np.array((0, yb, 0), np.float32), R
        for i in range(3):
            if ys[i] >= yb >= ys[i + 1] or i == 2:
                t = min(1.0, (ys[i] - yb) / (ys[i] - ys[i + 1]))
                Pa, Ra = W[names[i]]
                Pb, Rb = W[names[i + 1]]
                pa = Pa + Ra @ np.array((0, yb - ys[i], 0), np.float32)
                pb = Pb + Rb @ np.array((0, yb - ys[i + 1], 0), np.float32)
                Rm = Ra * (1 - t) + Rb * t
                return pa * (1 - t) + pb * t, frame(Rm[:, 1], Rm[:, 2])

    # -- building him ----------------------------------------------------------------
    def build(self):
        """Every solid that makes him, in his own frame: a list of
        ('ell', centre, radii, R, colour, kw), ('cone', a, b, r0, r1,
        colour, kw) and ('quad', corners, colour, kw).  kw may name the
        part's material (MATS) as 'mat'; cloth if it does not."""
        W = self.solve()
        self.parts = []
        self._head(W)
        self._torso(W)
        for side in ("r", "l"):
            self._arm(W, side)
            self._hand(W, side)
            self._leg(W, side)
        for p in self.props:
            self._prop(W, p)
        if self.seated and self.chair:
            self.parts += chair_parts(self.chair)
        return self.parts

    def ell(self, joint, c, r, col, Rl=None, W=None, **kw):
        P, R = (W or self.W)[joint]
        Rl = np.eye(3, dtype=np.float32) if Rl is None else Rl
        self.parts.append(("ell", P + R @ np.asarray(c, np.float32),
                           np.asarray(r, np.float32), (R @ Rl), col, kw))

    def ell_at(self, c, r, col, R=None, **kw):
        R = np.eye(3, dtype=np.float32) if R is None else R
        self.parts.append(("ell", np.asarray(c, np.float32),
                           np.asarray(r, np.float32), R, col, kw))

    def cone_at(self, a, b, r0, r1, col, **kw):
        self.parts.append(("cone", np.asarray(a, np.float32),
                           np.asarray(b, np.float32), r0, r1, col, kw))

    def quad_at(self, pts, col, **kw):
        self.parts.append(("quad", np.asarray(pts, np.float32), col, kw))

    # The head --------------------------------------------------------------
    def _head(self, W):
        f, skin, hair = self.face, self.skin, self.hair
        k = f["head"]
        lip = skin * np.array((0.9, 0.72, 0.68))
        shade = skin * 0.95
        H = "head"
        SK = dict(mat="skin")

        def e(c, r, col, Rl=None, **kw):
            kw = dict(SK, **kw)
            self.ell(H, np.asarray(c) * k, np.asarray(r) * k, col, Rl, **kw)
        # The neck, from the collar up into the skull.
        P0, R0 = W["neck"]
        P1, R1 = W["head"]
        self.cone_at(P0 + R0 @ np.array((0, 0.12, 0.02)),
                     P1 + R1 @ np.array((0, -0.18, 0.05)), 0.215, 0.19,
                     shade, rim=0.3, mat="skin")
        # Skull, the face in front of it, the jaw and chin.  The face is a
        # little narrower than the skull and stands forward of it, the
        # jaw squares it off below the cheeks.
        e((0, -0.53, 0.04), (0.35, 0.44, 0.41), skin, rim=0.15)
        e((0, -0.4, -0.13), (0.29 * f["jaw"], 0.39, 0.3), skin)
        e((0, -0.2, -0.09), (0.265 * f["jaw"], 0.19, 0.28), shade)
        e((0, -0.075, -0.28), (0.11 * f["jaw"], 0.08, 0.075), skin)
        # The brow, over the eyes.
        e((0, -0.6, -0.34), (0.26, 0.075 * f["brow"], 0.08), skin)
        for sx in (-1, 1):
            e((sx * 0.19, -0.42, -0.31), (0.09, 0.055 * f["cheek"], 0.07),
              skin)                                               # cheekbone
            e((sx * 0.335, -0.46, 0.04), (0.045, 0.1 * f["ears"], 0.075),
              skin, rot_y(sx * 0.35), rim=0.5)                   # ear
        # The nose: bridge, tip, and its wings.
        n = f["nose"]
        P, R = W["head"]
        self.cone_at(P + R @ (np.array((0, -0.56, -0.4)) * k),
                     P + R @ (np.array((0, -0.38, -0.46 - 0.025 * n)) * k),
                     0.035 * k, 0.045 * k, skin, mat="skin")
        e((0, -0.36, -0.465 - 0.025 * n), (0.045, 0.042, 0.04), skin)
        for sx in (-1, 1):
            e((sx * 0.045, -0.345, -0.43), (0.032, 0.026, 0.03), shade)
        # The mouth: the lips, open when he sings.
        m = self.mouth
        e((0, -0.248, -0.405), (0.085, 0.024 * f["lips"], 0.03), lip)
        e((0, -0.205 + 0.04 * m, -0.39), (0.075, 0.026 * f["lips"], 0.03),
          lip)
        # A moustache.
        mo = self.moustache
        mcol = np.array(GREY_HAIR) * 0.85 if self.grey else hair * 0.95
        HM = dict(mat="hair", spec=0.1)
        if mo == "clipped":
            e((0, -0.282, -0.432), (0.1, 0.026, 0.03), mcol, **HM)
        elif mo == "full":
            e((0, -0.278, -0.43), (0.115, 0.034, 0.035), mcol, **HM)
        elif mo == "walrus":
            e((0, -0.265, -0.43), (0.13, 0.048, 0.04), mcol, **HM)
        elif mo == "waxed":
            for sx in (-1, 1):
                e((sx * 0.07, -0.283, -0.425), (0.09, 0.02, 0.03), mcol,
                  rot_z(sx * 0.3), **HM)
        # Hair: a shell over the skull, cut off by a slanting plane so that
        # it comes down to the forehead in front and to the nape behind;
        # some body on top swept over from the parting; short at the back
        # and sides, and grey at the temples on an older man.
        hl = f["hairline"]
        hcol = hair
        vol = f["volume"]
        part = f["part"]
        HR = dict(mat="hair", rim=0.2, spec=0.35, shine=18)
        e((0, -0.54, 0.04), (0.37, 0.46, 0.44), hcol, rot_x(-0.55),
          clip_y=0.02 - hl, texture=hair_tex(part / 0.37), **HR)
        if not self.grey or vol > 0.9:
            sw = -np.sign(part)                     # swept away from it
            e((sw * 0.06, -0.9 + 0.02 * hl, -0.12 + 0.6 * hl),
              (0.27, 0.075 * vol, 0.25), hcol, rot_z(sw * 0.12),
              texture=hair_tex(part / 0.27), **HR)
        for sx in (-1, 1):
            side = hcol if not self.grey else np.array(GREY_HAIR)
            e((sx * 0.335, -0.45, -0.06), (0.04, 0.085, 0.06), side,
              **HR)                                              # sideburn
            if self.grey:
                e((sx * 0.31, -0.56, 0.06), (0.08, 0.13, 0.22), GREY_HAIR,
                  **HR)
        # Headgear for the aerodrome.
        if self.outfit == "mechanic":
            cap = self.cloth * 1.08
            e((0, -0.82, 0.0), (0.42, 0.14, 0.46), cap, rot_x(0.12),
              mat="wool")
            e((0, -0.72, 0.0), (0.395, 0.1, 0.44), cap * 0.85, mat="wool")
            e((0, -0.69, -0.42), (0.26, 0.03, 0.16), cap * 0.7, rot_x(0.25),
              spec=0.3, mat="wool")                               # peak
        elif self.outfit == "pilot":
            leather = np.array(LEATHER) * 0.92
            e((0, -0.55, 0.03), (0.4, 0.49, 0.46), leather, rot_x(-0.25),
              clip_y=0.3, spec=0.7, shine=24, mat="leather")
            for sx in (-1, 1):
                e((sx * 0.36, -0.38, -0.02), (0.08, 0.17, 0.14), leather,
                  spec=0.6, mat="leather")                         # flaps
                e((sx * 0.13, -0.8, -0.3), (0.085, 0.07, 0.04), BRASS,
                  rot_x(-0.6), spec=1.2, shine=30, mat="brass")    # goggles
                e((sx * 0.13, -0.8, -0.33), (0.06, 0.05, 0.02), (80, 96, 92),
                  rot_x(-0.6), spec=1.8, shine=60, mat="glass")
            e((0, -0.78, -0.27), (0.38, 0.03, 0.2), (56, 48, 40),
              rot_x(-0.5), mat="dark")                             # strap

    def face_strokes(self):
        """The face painted on, in head units: (kind, points, width,
        colour, opacity).  A small face is told by its tones more than
        its lines, so most of these are soft glazes: the eye sockets in
        shadow under the brow, the hollows under the cheekbones and down
        the sides of the jaw, the shadow under the nose and under the
        lower lip, colour in the cheeks and on the nose; then a few
        firm strokes: the brows, the dark of the eye with the lid over
        it, the line of the mouth, and for an older man the lines from
        nose to mouth."""
        f, skin = self.face, self.skin
        hair = np.array(GREY_HAIR) * 0.8 if self.grey else self.hair
        brow = hair * 0.75 + skin * 0.1
        dark = np.array((40, 26, 20), np.float32)
        shadow = skin * np.array((0.42, 0.32, 0.3))
        ruddy = np.array((206, 100, 80), np.float32)
        out = []
        for sx in (-1, 1):
            out.append(("glaze", [(sx * 0.18, -0.39, -0.37)], 0.15, ruddy,
                        f["ruddy"]))
            out.append(("glaze", [(sx * 0.125, -0.51, -0.41)], 0.19,
                        shadow, 0.95))                            # socket
            out.append(("glaze", [(sx * 0.2, -0.3, -0.33)], 0.13,
                        skin * 0.66, 0.5))                  # under cheekbone
            out.append(("glaze", [(sx * 0.25, -0.16, -0.22)], 0.14,
                        skin * 0.62, 0.45))                       # jaw
            out.append(("glaze", [(sx * 0.27, -0.62, -0.25)], 0.14,
                        skin * 0.75, 0.35))                       # temple
            if self.eyes_shut:
                out.append(("line", [(sx * 0.07, -0.497, -0.43),
                                     (sx * 0.125, -0.487, -0.435),
                                     (sx * 0.18, -0.492, -0.42)], 0.035,
                            dark, 0.8))
            else:
                out.append(("blob", [(sx * 0.12, -0.5, -0.435)], 0.075,
                            np.asarray(f["eyes"], np.float32) * 0.45, 0.85))
                out.append(("line", [(sx * 0.06, -0.505, -0.43),
                                     (sx * 0.12, -0.523, -0.435),
                                     (sx * 0.185, -0.502, -0.42)], 0.035,
                            dark, 0.9))
            out.append(("line", [(sx * 0.04, -0.593, -0.43),
                                 (sx * 0.13, -0.622, -0.43),
                                 (sx * 0.22, -0.6, -0.39)],
                        0.045 * f["brow"], brow, 0.9))
            if self.grey:
                out.append(("line", [(sx * 0.07, -0.36, -0.45),
                                     (sx * 0.11, -0.29, -0.43),
                                     (sx * 0.1, -0.21, -0.41)], 0.03,
                            skin * 0.55, 0.6))
        out.append(("glaze", [(0, -0.325, -0.47)], 0.12, shadow, 0.75))
        out.append(("glaze", [(0, -0.4, -0.5)], 0.08, ruddy, f["ruddy"] * 0.8))
        m = self.mouth
        if m > 0.1:
            out.append(("blob", [(0, -0.222, -0.43)], 0.07 + 0.06 * m,
                        (36, 16, 14), 0.95))
        else:
            out.append(("line", [(-0.08, -0.233, -0.43), (0, -0.228, -0.44),
                                 (0.08, -0.233, -0.43)], 0.032,
                        skin * np.array((0.4, 0.27, 0.25)), 0.9))
        out.append(("glaze", [(0, -0.17, -0.42)], 0.09, shadow, 0.5))
        # The hairline, feathered into the forehead rather than cut.
        hl = f["hairline"]
        for x in (-0.2, 0.0, 0.2):
            out.append(("glaze", [(x, -0.8 + hl * 0.8 + abs(x) * 0.25,
                                   -0.3 + abs(x) * 0.2)], 0.14,
                        self.hair, 0.0 if self.grey else 0.55))
        return out

    # The body --------------------------------------------------------------
    def _torso(self, W):
        bw, pa, c = self.bw, self.paunch, self.cloth
        o, st = self.outfit, self.style
        mat = "wool"
        if st == "shirt":
            c = np.array(SHIRT, np.float32) * self.rng.normal(1, 0.03)
            mat = "shirt"
        elif st == "cardigan":
            c = self.knit
            mat = "knit"
        elif o == "pilot":
            mat = "leather"
        sd = self.seed
        legs = self.breeches if o == "officer" else c
        # The body itself, lofted along the spine: slices from the hem up
        # to the shoulders, each as wide and deep as a man is at that
        # height, overlapping so closely that they make one smooth form.
        prof_y = [1.9, 1.0, 0.5, 0.3, 0.0, -0.4, -0.8, -1.2, -1.6, -1.95,
                  -2.15, -2.3]
        wx = [0.84, 0.79, 0.74, 0.7, 0.66, 0.6, 0.63, 0.69, 0.75, 0.77,
              0.7, 0.42]
        wz = [0.5, 0.47, 0.44, 0.41, 0.4, 0.36, 0.37, 0.41, 0.43, 0.4,
              0.33, 0.23]
        hem = {"pilot": 1.85, "mechanic": 0.3}.get(
            o, {"shirt": 0.3, "cardigan": 0.4}.get(st, 0.8))
        top = -2.36
        self.profile = (prof_y, wx, wz)

        def width(yb):
            belly = math.exp(-((yb + 0.65) / 0.45) ** 2) * pa
            return (float(np.interp(-yb, [-v for v in prof_y], wx)) * bw +
                    belly * 0.5,
                    float(np.interp(-yb, [-v for v in prof_y], wz)) + belly,
                    belly * 0.6)
        self.width = width
        yb = hem
        while yb >= top:
            P, R = self.axis_at(W, yb)
            ax, az, fwd = width(yb)
            ry = min(0.8, yb - top + 0.02, hem - yb + 0.02)
            col, m = c, mat
            if st in ("shirt", "cardigan") and yb > -0.32 and o == "officer":
                col, m = legs, "wool"
            self.ell_at(P + R @ np.array((0, 0, -fwd)), (ax, ry, az), col, R,
                        rim=0.3, texture=body_tex(sd, yb, ry, o), mat=m)
            yb -= 0.05
        # The shoulders: the slope from the collar out to the point of
        # each shoulder, and the deltoid rounding into the sleeve, laid
        # along the upper arm so that it follows the arm when it lifts.
        Pc, Rc = W["chest"]
        for sx in (-1, 1):
            side = "r" if sx < 0 else "l"
            S, E = W["shoulder_" + side][0], W["elbow_" + side][0]
            self.ell_at(Pc + Rc @ np.array((sx * 0.46 * bw, -0.43, 0.03)),
                        (0.42 * bw, 0.11, 0.27), c, Rc @ rot_z(sx * 0.3),
                        rim=0.3, mat=mat)                          # yoke
            d = unit(E - S)
            self.ell_at(S + d * 0.14, (0.22, 0.27, 0.215), c,
                        frame(d, Rc[:, 2]), rim=0.3, mat=mat)      # deltoid

        def front(x, y, lift=0.0, side=-1):
            """A point on the front of the body (or with side +1 on the
            back) at chest-frame height y (the chest joint is 1.65 above
            the hips), and its frame."""
            yb = y - 1.65
            P, R = self.axis_at(W, yb)
            ax, az, fwd = width(yb)
            t = 1 - (x / ax) ** 2
            depth = az * math.sqrt(max(t, 0.05)) + lift
            return P + R @ np.array((x, 0, -fwd + side * depth)), R

        def _F(x, y, lift=0.0):
            return front(x, y, lift)[0]
        P, R = W["chest"]
        BR = dict(mat="brass", spec=1.1, shine=30)
        if o in ("officer", "mechanic"):
            skirt = o == "officer" and st in ("sd", "wrap")
            if skirt:
                # The big pockets on the skirts of the tunic.
                for sx in (-1, 1):
                    self.ell("pelvis", (sx * 0.33 * bw, 0.12, -0.41 - pa * 0.6),
                             (0.18, 0.2, 0.04), c * 1.02, rim=0.2, mat=mat)
                    self.ell("pelvis", (sx * 0.33 * bw, -0.07, -0.44 - pa * 0.6),
                             (0.19, 0.055, 0.035), c * 1.06, mat=mat)
                    self.ell("pelvis", (sx * 0.33 * bw, -0.06, -0.48 - pa * 0.6),
                             (0.022, 0.022, 0.012), BRASS, **BR)
            # Collar, and the open neck with shirt and tie (officers), or
            # the overalls buttoned to the throat.
            if st != "cardigan":
                self.ell("neck", (0, 0.04, 0.04), (0.27, 0.13, 0.26),
                         c * 0.96, rim=0.3, mat=mat)
            if o == "officer" and st in ("sd", "shirt", "cardigan"):
                if st == "sd":
                    for sx in (-1, 1):
                        self.ell_at(_F(sx * 0.11, -0.66, 0.0),
                                    (0.08, 0.22, 0.03), c * 1.1,
                                    R @ rot_z(sx * 0.42), mat=mat)  # lapel
                # The shirt in the open neck, its collar points, the tie.
                deep = -0.35 if st == "cardigan" else -0.62
                self.ell_at(_F(0, deep - 0.08, -0.005),
                            (0.085, 0.24 if st == "cardigan" else 0.16, 0.03),
                            SHIRT, R, mat="shirt")
                for sx in (-1, 1):
                    self.ell("neck", (sx * 0.08, 0.13, -0.19),
                             (0.07, 0.05, 0.03), SHIRT, rot_z(sx * 0.6),
                             mat="shirt")                       # collar point
                self.ell_at(_F(0, deep - 0.04, 0.015),
                            (0.032, 0.2 if st == "cardigan" else 0.15, 0.02),
                            TIE, R, mat="wool")                  # tie
                self.ell("neck", (0, 0.1, -0.2), (0.05, 0.045, 0.04), TIE,
                         mat="wool")                              # knot
            if st == "cardigan":
                # The cardigan's V and its button band, and its pockets.
                for sx in (-1, 1):
                    self.ell_at(_F(sx * 0.12, -0.3, 0.0), (0.04, 0.5, 0.03),
                                c * 0.9, R @ rot_z(sx * 0.2), mat="knit")
                    self.ell_at(_F(sx * 0.3, 0.75, 0.0), (0.15, 0.13, 0.03),
                                c * 0.95, R, mat="knit")
                for y in (0.0, 0.3, 0.6):
                    self.ell_at(_F(0.02, y, 0.02), (0.022, 0.022, 0.012),
                                (60, 44, 30), R, mat="dark", spec=0.5)
            # Breast pockets with their pleats and flaps, buttons.
            pockets = (o == "officer" and st in ("sd", "shirt")) or \
                o == "mechanic"
            if pockets:
                for sx in (-1, 1):
                    px = sx * 0.28 * bw
                    self.ell_at(_F(px, -0.24, 0.0),
                                (0.13, 0.13, 0.025), c * 1.03, R, mat=mat)
                    self.ell_at(_F(px, -0.39, 0.012),
                                (0.135, 0.04, 0.025), c * 1.1, R, mat=mat)
                    self.ell_at(_F(px, -0.36, 0.03),
                                (0.02, 0.02, 0.01), BRASS if st == "sd"
                                else (60, 56, 50), R, **BR)
            if o == "officer" and st == "sd":
                for y in (-0.42, -0.1):
                    self.ell_at(_F(0, y, 0.01), (0.024, 0.024, 0.012),
                                BRASS, R, **BR)
                self.ell_at(_F(0, 1.0, 0.01), (0.024, 0.024, 0.012),
                            BRASS, **BR)
            if o == "officer" and st == "wrap":
                for k in range(3):
                    y = -0.55 + k * 0.28
                    self.ell_at(_F(0.2 + 0.1 * k, y, 0.01),
                                (0.024, 0.024, 0.012), BRASS, R, **BR)
            if o == "officer" and self.wings and st in ("sd", "wrap"):
                # The wings over the left breast: a pale spread of
                # embroidery either side of a dark centre.
                for sx in (-1, 1):
                    self.ell_at(_F(0.27 * bw + sx * 0.08, -0.53, 0.012),
                                (0.085, 0.022, 0.012), (210, 200, 170),
                                R @ rot_z(sx * -0.12), mat="shirt")
                self.ell_at(_F(0.27 * bw, -0.53, 0.02),
                            (0.03, 0.03, 0.012), (60, 52, 40), R, mat="dark")
            if o == "mechanic":
                for k in range(6):
                    self.ell_at(_F(0, -0.6 + k * 0.32, 0.01),
                                (0.022, 0.022, 0.01), (50, 46, 40), spec=0.5,
                                mat="dark")
        if o == "pilot":
            # The leather coat: collar turned up, a white scarf, the long
            # skirts (lofted with the body) falling to the knee.
            self.ell("neck", (0, 0.05, 0.03), (0.3, 0.16, 0.29), c * 0.95,
                     rim=0.3, spec=0.5, mat="leather")
            self.ell("neck", (0, 0.18, -0.12), (0.2, 0.08, 0.16), (220, 214,
                     196), mat="shirt")
            for k in range(5):
                self.ell_at(_F(0.08, 1.4 + k * 0.35, 0.0), (0.012, 0.2, 0.012),
                            c * 0.55, mat="leather")      # the coat's edge
            ax, az, fwd = width(-0.4)
            Pb, Rb = self.axis_at(W, -0.4)
            self.ell_at(Pb + Rb @ np.array((0, 0, -fwd)), (ax + 0.03, 0.07,
                        az + 0.03), np.array(LEATHER) * 0.8, Rb,
                        spec=0.6, mat="leather")                   # its belt
        # The Sam Browne: the belt, its buckle, the strap over the right
        # shoulder and across the back.  Not worn with a cardigan; a man in
        # his shirt-sleeves has only a plain belt.
        if o == "officer" and st in ("sd", "wrap", "shirt") and \
                (self.belt or st == "shirt"):
            ax, az, fwd = width(-0.43)
            Pb, Rb = self.axis_at(W, -0.43)
            belt = LEATHER if st != "shirt" else (90, 70, 46)
            self.ell_at(Pb + Rb @ np.array((0, 0, -fwd)), (ax + 0.025, 0.065,
                        az + 0.025), belt, Rb, rim=0.3, spec=0.7, shine=22,
                        mat="leather")
            self.ell_at(Pb + Rb @ np.array((0, 0, -fwd - az - 0.03)),
                        (0.06, 0.055, 0.02), BRASS, Rb, **BR)
            if st != "shirt":
                # Over the right shoulder, down across the chest to the
                # belt at the left hip, and the same across the back:
                # laid on the body at a few points so that it follows it.
                for zf in (-1, 1):
                    pts = [front(-0.42 * bw + 0.9 * bw * t, -0.62 + 1.9 * t,
                                 0.025, zf)[0] for t in np.linspace(0, 1, 5)]
                    for a, b in zip(pts, pts[1:]):
                        self.cone_at(a, b, 0.036, 0.036, LEATHER, spec=0.7,
                                     shine=22, mat="leather")

    def _arm(self, W, side):
        st = self.style
        c = {"shirt": np.array(SHIRT, np.float32),
             "cardigan": self.knit}.get(st, self.cloth)
        mat = {"shirt": "shirt", "cardigan": "knit"}.get(
            st, "leather" if self.outfit == "pilot" else "wool")
        S, E, Wr = (W["shoulder_" + side][0], W["elbow_" + side][0],
                    W["wrist_" + side][0])
        lace = self.lace if self.outfit == "officer" and \
            st in ("sd", "wrap") else 0
        tex = knit_tex if st == "cardigan" else None
        self.ell_at(S, (0.23, 0.23, 0.23), c, rim=0.3, mat=mat)
        self.cone_at(S, E, 0.25, 0.215, c, rim=0.3, mat=mat, texture=tex)
        self.ell_at(E, (0.215, 0.215, 0.215), c * 0.97, rim=0.3, mat=mat)
        cuff = Wr - unit(Wr - E) * 0.04
        if self.rolled and st == "shirt":
            # Sleeves rolled to the elbow: the roll, then the bare forearm.
            mid = E + (cuff - E) * 0.18
            self.cone_at(E, mid, 0.2, 0.2, c * 1.04, rim=0.3, mat="shirt")
            self.cone_at(mid, cuff, 0.15, 0.115, self.skin, rim=0.3,
                         mat="skin")
            return
        self.cone_at(E, cuff, 0.215, 0.18 if self.outfit != "pilot"
                     else 0.16, c * 0.97, rim=0.3, mat=mat, texture=tex)
        if lace:
            # The cuff's rank lace: thin rings near the end of the sleeve.
            d = unit(cuff - E)
            for kk in range(lace):
                p = cuff - d * (0.16 + kk * 0.07)
                self.cone_at(p - d * 0.012, p + d * 0.012, 0.192, 0.192,
                             (150, 132, 92), mat="wool")
        if self.outfit == "pilot":                     # gauntlet cuffs
            self.cone_at(cuff - unit(Wr - E) * 0.18, cuff + unit(Wr - E) * 0.02,
                         0.16, 0.17, np.array(LEATHER) * 0.8, spec=0.5,
                         mat="leather")

    def _hand(self, W, side):
        """Palm, four fingers of three joints, and the thumb, curled as
        the hand's kind asks."""
        h = self.hands[side]
        kind = h.get("kind", "relax")
        if kind == "hidden":                # in a pocket, or behind a paper
            return
        sx = -1 if side == "r" else 1
        F = self.hand_frame(side, W)
        P = W["wrist_" + side][0]
        skin = self.skin
        mat = "skin"
        if self.outfit == "pilot":
            skin = np.array(LEATHER) * 0.85
            mat = "leather"
        knuckle = skin * 0.96
        SK = dict(mat=mat)
        curl = {"relax": (25, 30, 20), "flat": (6, 8, 5),
                "open": (2, 4, 2), "fist": (90, 100, 70),
                "key": (30, 40, 25), "grip": None, "wave": (4, 6, 4),
                "pinch": (55, 65, 45)}[kind]
        thumb_curl = {"relax": 20, "flat": 5, "open": -10, "fist": 60,
                      "key": 15, "grip": 50, "wave": -15, "pinch": 45}[kind]
        r = h.get("radius", 0.16)
        if kind == "grip":
            # Wrap the fingers round a bar of radius r lying across the
            # palm: the total turn is the finger's length over the radius.
            wrap = math.degrees(0.33 / (r + 0.035))
            curl = (min(wrap * 0.32, 95), min(wrap * 0.36, 105),
                    min(wrap * 0.32, 90))
        # The palm and the heel of the hand: a man's hand is broad.
        self.ell_at(P + F @ np.array((0, 0.17, 0.0)), (0.12, 0.19, 0.055),
                    skin, F, rim=0.4, **SK)
        self.ell_at(P + F @ np.array((-sx * 0.06, 0.1, 0.02)),
                    (0.065, 0.09, 0.055), skin * 0.98, F, **SK)
        spread = {"open": 9, "wave": 11}.get(kind, 2)
        for i, (ln, fx) in enumerate(FINGERS):
            xx = fx * sx * -1 * 1.1
            base = P + F @ np.array((xx, 0.33, 0.0))
            d = F @ np.array((xx * spread * 0.04, 1.0, 0.0))
            n = F[:, 2]                         # bends toward the palm
            segs = (0.15 * ln, 0.1 * ln, 0.085 * ln)
            rad = (0.033, 0.029, 0.025)
            p = base
            self.ell_at(p, (0.036, 0.034, 0.034), knuckle, **SK)
            for j in range(3):
                a = deg(curl[j])
                d = unit(d * math.cos(a) + n * math.sin(a))
                n = unit(np.cross(np.cross(d, n), d))
                q = p + d * segs[j]
                self.cone_at(p, q, rad[j], rad[j] * 0.9,
                             knuckle if j == 0 else skin, rim=0.3, **SK)
                self.ell_at(q, (rad[j] * 0.9,) * 3, skin, **SK)
                p = q
        # The thumb, from the heel of the palm toward the index side.
        tb = P + F @ np.array((sx * -0.09, 0.08, 0.03))
        td = unit(F @ np.array((sx * -0.55, 0.65, 0.35)))
        tn = unit(F @ np.array((sx * 0.3, 0.0, 1.0)))
        p = tb
        for j, (ln, rd) in enumerate(((0.14, 0.04), (0.1, 0.033))):
            a = deg(thumb_curl * (0.6 + 0.4 * j))
            td = unit(td * math.cos(a) + tn * math.sin(a))
            q = p + td * ln
            self.cone_at(p, q, rd, rd * 0.92, skin, rim=0.3, **SK)
            self.ell_at(q, (rd * 0.92,) * 3, skin, **SK)
            p = q
        # What it holds.
        prop = h.get("prop")
        if prop in ("glass", "glass_low"):
            cen = P + F @ np.array((0, 0.36, r + 0.05))
            self._glass(cen, h.get("axis", (0, -1, 0)), h.get("full", 1.0),
                        r)
        elif prop == "pipe":
            cen = P + F @ np.array((0, 0.33, r + 0.04))
            ax = unit(F[:, 0])
            self.ell_at(cen, (0.07, 0.07, 0.07), (78, 48, 28), spec=0.7,
                        shine=24, mat="wood")
            tip = np.asarray(h.get("to"), np.float32) if "to" in h else \
                cen + ax * 0.35
            self.cone_at(cen, tip, 0.022, 0.016, (34, 24, 18), spec=0.6,
                         mat="dark")
        elif prop == "pen":
            tip = P + F @ np.array((0, 0.5, 0.06))
            self.cone_at(tip, tip + F @ np.array((0.05, -0.38, -0.12)),
                         0.012, 0.022, (30, 26, 24), spec=0.8, mat="dark")
        elif prop == "cards":
            # A hand of cards fanned out from between finger and thumb,
            # their faces toward him.
            base = P + F @ np.array((sx * 0.02, 0.3, 0.06))
            for i in range(5):
                a = deg(-40 + 20 * i) * -sx
                up = F @ np.array((math.sin(a), -math.cos(a), 0.0))
                ac = F @ np.array((math.cos(a), math.sin(a), 0.0))
                off = F[:, 2] * (0.004 * i)
                c0 = base + off - ac * 0.16 + up * 0.02
                self.quad_at([c0, c0 + ac * 0.32, c0 + ac * 0.32 + up * 0.46,
                              c0 + up * 0.46], PAPER, mat="paper",
                             texture=back_tex if h.get("backs", True)
                             else card_tex("hscd"[i % 4]))
        elif prop == "letter":
            # A letter held up to read: a sheet, a little curled.
            up = F @ np.array((0, -1, 0.0))
            ac = F @ np.array((1, 0, 0.0)) * -sx
            c0 = P + F @ np.array((0, 0.3, 0.04)) - up * 0.5
            self.quad_at([c0, c0 + ac * 0.5, c0 + ac * 0.5 + up * 0.7,
                          c0 + up * 0.7], PAPER, mat="paper",
                         texture=letter_tex(self.seed))

    def _glass(self, cen, axis, full, r):
        """A pint pot: tapered glass, the beer in it, the head on it."""
        ax = unit(axis)                       # toward its mouth
        top = cen + ax * 0.3
        bot = cen - ax * 0.24
        col = BEER if full else GLASS
        self.cone_at(bot, top, r * 0.92, r * 1.08, col, spec=1.4, shine=48,
                     mat="beer" if full else "glass")
        R = frame(ax, (0, 0, 1))
        if full:
            self.ell_at(top, (r * 1.08, 0.04, r * 1.08), FOAM, R, spec=0.3,
                        mat="paper")
        self.ell_at(bot, (r * 0.95, 0.03, r * 0.95), (170, 140, 90), R,
                    spec=1.0, mat="glass")

    def _leg(self, W, side):
        sx = -1 if side == "r" else 1
        H, K, A = (W["hip_" + side][0], W["knee_" + side][0],
                   W["ankle_" + side][0])
        o = self.outfit
        legs = self.breeches if o == "officer" else self.cloth
        if o == "pilot":
            legs = np.array(BREECHES[3], np.float32)
        LW = dict(mat="wool", rim=0.3)
        self.cone_at(H, K, 0.38, 0.215, legs, **LW)
        Rk = W["knee_" + side][1]
        Rh = W["hip_" + side][1]
        if o == "officer":
            # The flare of the breeches on the outside of the thigh.
            self.ell("hip_" + side, (sx * 0.13, 0.78, 0.02), (0.31, 0.5, 0.28),
                     legs, **LW)
        self.ell_at(K, (0.215, 0.215, 0.215), legs, **LW)
        # Lower leg: riding boot, puttees over an ankle boot, overalls
        # trousers, or a flying boot with its sheepskin top.
        BT = dict(mat="boot", rim=0.3, spec=0.8, shine=26)
        if o == "pilot":
            self.cone_at(K + Rk @ np.array((0, 0.25, 0)), A, 0.22, 0.16,
                         (70, 46, 28), spec=0.5, mat="leather")
            self.cone_at(K + Rk @ np.array((0, 0.1, 0)),
                         K + Rk @ np.array((0, 0.3, 0)), 0.25, 0.25,
                         (200, 184, 150), mat="knit")
        elif self.puttees:
            self.cone_at(K, A, 0.2, 0.14, PUTTEES, texture=puttee_tex, **LW)
        elif o == "mechanic":
            self.cone_at(K, A, 0.2, 0.16, legs, **LW)
        else:
            # The riding boot: full in the calf, close at the ankle.
            calf = K + (A - K) * 0.35
            self.cone_at(K + Rk @ np.array((0, 0.08, 0)), calf, 0.235, 0.26,
                         BOOTS, **BT)
            self.cone_at(calf, A, 0.26, 0.17, BOOTS, **BT)
        # The foot, flat on the floor unless the pose tips it.
        fwd = unit(Rh @ np.array((0, 0, -1)) * np.array((1, 0, 1)) +
                   np.array((0, 0, -1e-3)))
        if self.seated:
            fwd = unit(np.array((0.15 * sx, 0, -1), np.float32))
        pitch = deg(self.foot_pitch[side])
        d = unit(fwd * math.cos(pitch) + np.array((0, 1, 0)) * math.sin(pitch))
        Rf = frame(np.array((0, 1, 0)) * math.cos(pitch) - fwd *
                   math.sin(pitch), d)
        boot = (56, 36, 24) if self.puttees or o == "mechanic" else BOOTS
        self.ell_at(A + np.array((0, 0.13, 0)) + d * 0.13,
                    (0.13, 0.1, 0.31), boot, Rf, **BT)
        self.ell_at(A + np.array((0, 0.06, 0)), (0.14, 0.12, 0.14), boot,
                    **BT)

    def _prop(self, W, p):
        if isinstance(p, str):
            p = dict(kind=p)
        kind = p["kind"]
        if kind == "pipe_mouth":
            P, R = W["head"]
            a = P + R @ np.array((0.02, -0.22, -0.38))
            b = P + R @ np.array((0.06, -0.12, -0.62))
            self.cone_at(a, b, 0.02, 0.018, (34, 24, 18), spec=0.6,
                         mat="dark")
            self.ell_at(b + np.array((0, -0.05, 0)), (0.06, 0.08, 0.06),
                        (78, 48, 28), spec=0.7, shine=24, mat="wood")
        elif kind == "cigarette":
            P, R = W["head"]
            a = P + R @ np.array((0.06, -0.23, -0.42))
            b = P + R @ np.array((0.14, -0.2, -0.62))
            self.cone_at(a, b, 0.016, 0.016, (230, 226, 214), mat="paper")
            self.ell_at(b, (0.02, 0.02, 0.02), (255, 120, 40), mat="beer")
        elif kind == "newspaper":
            # A newspaper held open in both hands: two leaves, folded a
            # little toward him down the middle, the type toward us.
            Pr, Pl = W["wrist_r"][0], W["wrist_l"][0]
            mid = (Pr + Pl) / 2
            across = unit(Pl - Pr)
            up = np.array((0, -1, 0), np.float32)
            up = unit(up - np.dot(up, across) * across)
            out = unit(np.cross(across, up))       # toward the reader
            h = p.get("h", 1.5)
            fold = mid + out * 0.12 + up * 0.15
            for s in (-1, 1):
                edge = mid + across * s * 0.85 + up * 0.15
                self.quad_at([fold - up * h * 0.55, edge - up * h * 0.55,
                              edge + up * h * 0.45, fold + up * h * 0.45],
                             PAPER, mat="paper",
                             texture=print_tex(seed=self.seed + s))
        elif kind == "book":
            # A book open in his hands, its boards dark behind the pages.
            Pl = W["wrist_l"][0]
            Pr = W["wrist_r"][0]
            mid = (Pr + Pl) / 2 + np.array((0, -0.12, 0), np.float32)
            across = unit(Pl - Pr)
            up = np.array((0, -0.6, 0.8), np.float32)
            up = unit(up - np.dot(up, across) * across)
            out = unit(np.cross(across, up))
            for s in (-1, 1):
                a = mid + out * 0.04
                b = mid + across * s * 0.42 + out * 0.08
                self.quad_at([a - up * 0.3, b - up * 0.3, b + up * 0.3,
                              a + up * 0.3], PAPER, mat="paper",
                             texture=print_tex(cols=1, lines=22,
                                               seed=self.seed + s))
                a2, b2 = a - out * 0.025, b - out * 0.025
                self.quad_at([a2 - up * 0.32, b2 - up * 0.32 + across * s *
                              0.02, b2 + up * 0.32 + across * s * 0.02,
                              a2 + up * 0.32], (74, 34, 26), mat="leather")
        elif kind == "sheet":
            # A sheet of paper (a letter being written, a hand of cards
            # face down, a photograph) lying on the table at 'at', in his
            # own frame.
            c = np.asarray(p["at"], np.float32)
            w, d = p.get("size", (0.5, 0.65))
            a = deg(p.get("angle", 0))
            # The table as the painted rooms draw it rises up the picture
            # as it goes away from us ('rise' rows to a unit of depth), so
            # a sheet lying on it is laid in that slope, not edge-on.
            rise = p.get("rise", 0.386)
            ax = np.array((math.cos(a), 0, math.sin(a)), np.float32)
            az = np.array((-math.sin(a), -rise, math.cos(a)), np.float32)
            tex = {"letter": letter_tex(self.seed), "back": back_tex,
                   None: None}.get(p.get("tex"), None)
            if p.get("tex", "").startswith("card"):
                tex = card_tex(p["tex"][-1])
            self.quad_at([c - ax * w / 2 - az * d / 2,
                          c + ax * w / 2 - az * d / 2,
                          c + ax * w / 2 + az * d / 2,
                          c - ax * w / 2 + az * d / 2],
                         p.get("col", PAPER), mat="paper", texture=tex)

    # -- into the scene ------------------------------------------------------------
    def draw(self, sc, x, floor_y, z, px_per_h, yaw=0.0, fig=1):
        """Stand (or sit) him with his feet at (x, floor_y), depth z, at
        px_per_h pixels to a head-height, turned by yaw (0 faces us).
        Into a GScene he goes as man number 'fig'."""
        k = px_per_h * self.tall
        hip = (SEAT_HIP[self.chair] if self.seated else STAND_HIP) * k
        o = np.array((x, floor_y - hip, z), np.float32)
        Y = rot_y(yaw).astype(np.float32)
        self.placed = (o, k, Y, fig)
        draw_parts(sc, self.build(), o, k, Y, fig)
        return o

    def to_screen(self, p):
        """A point in his own frame, on the screen at 1x (x, y, depth)."""
        o, k, Y, _ = self.placed
        return o + k * (Y @ np.asarray(p, np.float32))

    def head_point(self, p):
        """A point on his head (head units) in his own frame."""
        P, R = self.W["head"]
        return P + R @ (np.asarray(p, np.float32) * self.face["head"])

    def hip_at(self, x, floor_y, z, px_per_h):
        k = px_per_h * self.tall
        return (x, floor_y - (SEAT_HIP[self.chair] if self.seated
                              else STAND_HIP) * k, z)


def draw_parts(sc, parts, o, k, Y, fig=1):
    """Draws parts built in a man's (or a chair's) own frame, placed at
    'o', k pixels to a head-height and turned by Y."""
    G = getattr(sc, "gbuffer", False)
    if G:
        sc.fig = fig
    for part in parts:
        kw = dict(part[-1])
        mat = kw.pop("mat", "wool")
        if G:
            sc.mat = MATS[mat]
        if part[0] == "ell":
            _, c, r, R, col = part[:5]
            sc.ellipsoid(tuple(o + k * (Y @ c)), tuple(r * k), col,
                         R=(Y @ R).astype(np.float32), **kw)
        elif part[0] == "cone":
            _, a, b, r0, r1, col = part[:6]
            a, b = o + k * (Y @ a), o + k * (Y @ b)
            if np.linalg.norm(b - a) < 1e-3:
                continue
            cone(sc, tuple((a + b) / 2), b - a,
                 float(np.linalg.norm(b - a)) / 2, r0 * k, r1 * k, col,
                 **kw)
        else:
            _, pts, col = part[:3]
            quad(sc, [o + k * (Y @ p) for p in pts], col, **kw)


# ---- chairs ------------------------------------------------------------------------
def _rod(parts, a, b, r, col, **kw):
    parts.append(("cone", np.asarray(a, np.float32), np.asarray(b, np.float32),
                  r, r, col, dict(kw)))


def _box(parts, c, size, col, R=None, **kw):
    """A box of six flat faces, centred at c, size (w, h, d), turned by R
    (an upholstered seat, a chair's arm, its back)."""
    R = np.eye(3, dtype=np.float32) if R is None else np.asarray(R, np.float32)
    c = np.asarray(c, np.float32)
    hx, hy, hz = (np.asarray(size, np.float32) / 2)
    X, Y, Z = R[:, 0] * hx, R[:, 1] * hy, R[:, 2] * hz
    for a, u, v in ((X, Y, Z), (Y, Z, X), (Z, X, Y)):
        for s in (-1, 1):
            o = c + a * s
            parts.append(("quad", np.array([o - u - v, o + u - v, o + u + v,
                                            o - u + v], np.float32), col,
                          dict(kw)))


def chair_parts(kind, cap=False, col=None):
    """A chair in the frame of the man who sits in it (his hips at the
    origin, the floor SEAT_HIP[kind] below, his back toward +z).

    bentwood  the cafe chair every mess had: a round caned seat on four
              splayed legs with a ring between them, and the back bent in
              one loop.
    club      a deep leather armchair, for the corner by the stove.
    stool     the round piano stool."""
    p = []
    fl = SEAT_HIP[kind]
    W = dict(mat="wood", spec=0.5, shine=22)
    if kind == "bentwood":
        col = CHAIR if col is None else col
        p.append(("ell", np.array((0, 0.3, 0.12), np.float32),
                  np.array((0.78, 0.07, 0.74), np.float32), np.eye(3),
                  col, dict(W)))
        p.append(("ell", np.array((0, 0.27, 0.12), np.float32),
                  np.array((0.68, 0.04, 0.64), np.float32), np.eye(3),
                  (150, 116, 70), dict(mat="wood", spec=0.2)))  # the cane
        ring = []
        for i in range(4):
            a = math.pi / 4 + i * math.pi / 2
            top = (math.cos(a) * 0.6, 0.34, 0.12 + math.sin(a) * 0.56)
            foot = (math.cos(a) * 0.74, fl, 0.12 + math.sin(a) * 0.72)
            _rod(p, top, foot, 0.06, col, **W)
        for i in range(16):
            a0, a1 = i * math.pi / 8, (i + 1) * math.pi / 8
            r = 0.66
            ring.append(((math.cos(a0) * r, 1.3, 0.12 + math.sin(a0) * r),
                         (math.cos(a1) * r, 1.3, 0.12 + math.sin(a1) * r)))
        for a, b in ring:
            _rod(p, a, b, 0.035, col, **W)
        # The back: two uprights rising from the back of the seat that bend
        # over into one loop, and a smaller loop inside it.
        pts = []
        for i in range(25):
            t = i / 24
            th = math.pi * t
            x = -0.48 * math.cos(th)
            y = 0.3 - 1.6 * math.sin(th) ** 0.35 - 0.55 * math.sin(th) ** 6
            pts.append((x, y, 0.82 + 0.1 * (0.3 - y) / 2.0))
        for a, b in zip(pts, pts[1:]):
            _rod(p, a, b, 0.065, col, **W)
        inner = [(x * 0.62, -1.35 + (y + 1.35) * 0.45 - 0.2, z + 0.01)
                 for x, y, z in pts[3:-3]]
        for a, b in zip(inner, inner[1:]):
            _rod(p, a, b, 0.045, col, **W)
        if cap:
            # His cap hung on the chair's back (or, with cap="seat", left
            # on the seat): khaki crown, the dark band, the glazed peak
            # and the badge catching the light.
            if cap == "seat":
                Rc = (rot_y(0.5) @ rot_z(0.06)).astype(np.float32)
                c = np.array((0.05, 0.02, 0.1), np.float32)
            else:
                Rc = (rot_z(0.35) @ rot_x(-0.25)).astype(np.float32)
                c = np.array((0.38, -1.62, 0.9), np.float32)

            def blob(q, r, colr, **kw):
                p.append(("ell", c + Rc @ np.asarray(q, np.float32),
                          np.asarray(r, np.float32), Rc, colr, kw))
            blob((0, -0.08, 0), (0.56, 0.15, 0.54), (122, 110, 74),
                 rim=0.7, mat="wool")
            blob((0, 0.06, 0), (0.47, 0.13, 0.45), (66, 58, 42), rim=0.6,
                 mat="wool")
            blob((0, 0.14, -0.42), (0.4, 0.045, 0.3), (40, 30, 22), spec=1.0,
                 shine=30, mat="leather")
            blob((0, -0.02, -0.5), (0.09, 0.09, 0.04), (210, 186, 120),
                 spec=1.2, shine=30, mat="brass")
    elif kind == "club":
        # Built square, as the old leather club chairs were: a deep seat
        # cushion on a box, a high back leaning back a little, and broad
        # arms with a roll along their tops.
        L = dict(mat="leather", spec=0.45, shine=14, rim=0.3)
        col = np.array(CLUB, np.float32)
        _box(p, (0, 1.08, 0.2), (2.1, 1.3, 1.9), col * 0.8, **L)
        _box(p, (0, 0.36, 0.08), (1.6, 0.26, 1.6), col, **L)
        Rb = rot_x(-0.2).astype(np.float32)
        _box(p, (0, -0.55, 1.02), (1.7, 2.3, 0.4), col * 0.92, Rb, **L)
        for sx in (-1, 1):
            _box(p, (sx * 0.98, 0.45, 0.2), (0.32, 1.1, 1.9), col * 0.88,
                 **L)
            p.append(("ell", np.array((sx * 1.0, -0.12, 0.15), np.float32),
                      np.array((0.24, 0.13, 0.98), np.float32), np.eye(3),
                      col, dict(L)))                          # its roll
    elif kind == "stool":
        col = CHAIR
        p.append(("ell", np.array((0, 0.32, 0.05), np.float32),
                  np.array((0.62, 0.12, 0.6), np.float32), np.eye(3), col,
                  dict(W)))
        for i in range(4):
            a = math.pi / 4 + i * math.pi / 2
            _rod(p, (math.cos(a) * 0.45, 0.4, 0.05 + math.sin(a) * 0.45),
                 (math.cos(a) * 0.55, fl, 0.05 + math.sin(a) * 0.55), 0.06,
                 col, **W)
    return p


def chair(sc, x, floor_y, z, px_per_h, yaw=0.0, kind="bentwood", fig=1,
          cap=False, col=None):
    """A chair on its own, its feet at (x, floor_y), depth z, turned by
    yaw (0: its seat toward us)."""
    hip = SEAT_HIP[kind] * px_per_h
    o = np.array((x, floor_y - hip, z), np.float32)
    Y = rot_y(yaw).astype(np.float32)
    draw_parts(sc, chair_parts(kind, cap, col), o, px_per_h, Y, fig)
    return o


def soften_mask(a, r):
    """A quick soft blur of a small 2-D array: two passes of a box of
    half-width r along each axis."""
    for _ in range(2):
        for ax in (0, 1):
            c = np.cumsum(np.pad(a, [(r + 1, r) if i == ax else (0, 0)
                                     for i in range(2)]), axis=ax)
            n = a.shape[ax]
            a = (np.take(c, np.arange(n) + 2 * r + 1, axis=ax) -
                 np.take(c, np.arange(n), axis=ax)) / (2 * r + 1)
    return a


# ---- painting the face -----------------------------------------------------------------
def paint_faces(G, figs):
    """Puts each man's face on in strokes (Figure.face_strokes), into the
    colour of a GScene before it is lit, so that the lamp lights the
    paint as it lights the skin.  A stroke goes down only on that man's
    own head, only where the head is the nearest thing (not over a glass
    or a hand in front of his face), and only on the side of the head
    turned toward us."""
    ss = G.ss
    for f in figs:
        if not hasattr(f, "placed"):
            continue
        o, k, Y, fid = f.placed
        P, R = f.W["head"]
        hk = f.face["head"]
        fwd = Y @ (R @ np.array((0, 0, -1), np.float32))
        centre = f.to_screen(P + R @ (np.array((0, -0.45, 0.0)) * hk))
        rad = 0.75 * k * hk * ss
        x0, x1 = int(max(0, centre[0] * ss - rad)), \
            int(min(G.w, centre[0] * ss + rad))
        y0, y1 = int(max(0, centre[1] * ss - rad)), \
            int(min(G.h, centre[1] * ss + rad))
        if x0 >= x1 or y0 >= y1:
            continue
        buf = G.rgb[y0:y1, x0:x1]
        zb = G.z[y0:y1, x0:x1]
        own = (buf[..., 10] == fid) & np.isin(buf[..., 9], (MATS["skin"],
                                                           MATS["hair"]))
        for kind, pts, width, col, alpha in f.face_strokes():
            loc = [np.asarray(q, np.float32) for q in pts]
            # Facing: the surface normal there, roughly, is the direction
            # from the middle of the skull to the point.
            mid = np.mean(loc, 0)
            nrm = Y @ (R @ unit(mid - np.array((0, -0.45, 0.05))))
            if nrm[2] > -0.2 or fwd[2] > 0.25:
                continue
            scr = [f.to_screen(f.head_point(q)) for q in loc]
            w = max(1.0, width * k * hk * ss)
            m = Image.new("L", (x1 - x0, y1 - y0), 0)
            d = ImageDraw.Draw(m)
            xy = [(float(s[0] * ss - x0), float(s[1] * ss - y0)) for s in scr]
            if kind in ("blob", "glaze"):
                cx, cy = xy[0]
                d.ellipse([cx - w / 2, cy - w / 2 * 0.75, cx + w / 2,
                           cy + w / 2 * 0.75], fill=255)
            else:
                d.line(xy, fill=255, width=int(round(w)), joint="curve")
            a = np.asarray(m, np.float32) / 255.0
            if kind == "glaze":
                # A glaze is soft all through, strongest in its middle.
                a = soften_mask(a, max(1, int(w * 0.3)))
            elif kind == "blob":
                a = soften_mask(a, max(1, int(w * 0.12)))
            zs = np.mean([s[2] for s in scr]) * ss
            near = np.abs(zb - zs) < 0.3 * k * ss
            a = a * alpha * own * near
            c = np.asarray(col, np.float32) / 255.0
            buf[..., 0:3] = buf[..., 0:3] * (1 - a[..., None]) + \
                c * a[..., None]


# ---- painting the light ------------------------------------------------------------------
def _shift(a, dy, dx, fill):
    """'a' moved by (dy, dx) pixels, the edge filled with 'fill'."""
    out = np.full_like(a, fill)
    h, w = a.shape[:2]
    ys, yd = (slice(0, h - dy), slice(dy, h)) if dy >= 0 else \
        (slice(-dy, h), slice(0, h + dy))
    xs, xd = (slice(0, w - dx), slice(dx, w)) if dx >= 0 else \
        (slice(-dx, w), slice(0, w + dx))
    out[yd, xd] = a[ys, xs]
    return out


def paint(G, lights, px_per_h, ambient=(0.02, 0.017, 0.014),
          bounce=(0.01, 0.007, 0.004), seed=0, smooth=0.06, occlusion=0.8,
          shadows=0.75, mottle=0.1, crumple=0.15, skin_key=1.12):
    """Lights a GScene as a painter would and returns (rgb, coverage),
    both full size.  'lights' are (position at 1x, colour, reach, rim):
    the strongest first, since only it casts shadows.  px_per_h is the
    size of a head-height in pixels at 1x, which sets the reach of the
    softening, the pooled shadow and the cast shadows.

    In turn: the surface normals are smoothed a little within each man,
    so that a sleeve flows into a shoulder rather than meeting it at a
    seam, and the cloth is given a soft crumple of small folds; ambient
    occlusion is gathered from the depth buffer (a form in
    front of another darkens it near the join); the lamp's shadow is
    marched toward it across the depth buffer, so an arm raised in front
    of the face shades it and the chin shades the throat; then each light
    wraps round the forms according to their material, skin reddening
    where it turns from the light, with a rim where the light is behind
    and a shine on leather, brass and glass; beer glows with the light
    through it.  The colours are first broken with a low mottle, as paint
    is never quite even, and faces and hands are keyed up a little."""
    ss = G.ss
    cov = np.isfinite(G.z)
    rgb = np.zeros((G.h, G.w, 3), np.float32)
    if not cov.any():
        return rgb, cov
    ys, xs = np.nonzero(cov)
    pad = int(px_per_h * ss * 0.6) + 2
    y0, y1 = max(0, ys.min() - pad), min(G.h, ys.max() + pad + 1)
    x0, x1 = max(0, xs.min() - pad), min(G.w, xs.max() + pad + 1)
    buf = G.rgb[y0:y1, x0:x1]
    c = cov[y0:y1, x0:x1]
    hpx = px_per_h * ss
    big = 1e9
    z = np.where(c, G.z[y0:y1, x0:x1], big).astype(np.float32)
    fig = buf[..., 10]
    mat = buf[..., 9].astype(np.int32)
    n = buf[..., 3:6].copy()
    # Smoothed normals: a bilateral average over a small disc, only
    # across surfaces of the same man that lie close in depth.
    r = max(1, int(round(smooth * hpx)))
    acc = n.copy()
    wsum = np.ones(z.shape, np.float32)
    for i in range(12):
        a = i * math.pi / 6
        for rr in (r * 0.5, r):
            dy, dx = int(round(math.sin(a) * rr)), int(round(math.cos(a) * rr))
            if dy == 0 and dx == 0:
                continue
            zn = _shift(z, dy, dx, big)
            w = ((np.abs(zn - z) < 0.12 * hpx) &
                 (_shift(fig, dy, dx, -1) == fig)).astype(np.float32)
            acc += _shift(n, dy, dx, 0) * w[..., None]
            wsum += w
    n = normalize(acc / wsum[..., None])
    # Cloth is never smooth: a soft crumple of small folds over the wool,
    # the shirts and the knitting, as a bump on the normals.
    if crumple:
        h, w = z.shape
        bump = fbm(h, w, seed + 7, 3, max(2, int(h / (0.22 * hpx))))
        gy, gx = np.gradient(bump * hpx * 0.22)
        cloth = np.isin(mat, (MATS["wool"], MATS["shirt"], MATS["knit"]))
        k = crumple * cloth
        n = normalize(n + np.stack([-gx * k, -gy * k, np.zeros_like(gx)],
                                   -1))
    # Ambient occlusion from the depth buffer.
    occ = np.zeros(z.shape, np.float32)
    rng = np.random.default_rng(seed)
    R = 0.3 * hpx
    taps = 16
    for i in range(taps):
        a = (i + rng.uniform(0, 1)) * 2 * math.pi / taps
        rr = R * (0.3 + 0.7 * ((i * 7) % taps) / taps)
        dy, dx = int(round(math.sin(a) * rr)), int(round(math.cos(a) * rr))
        dz = z - _shift(z, dy, dx, big)            # > 0: neighbour nearer
        occ += np.clip(dz / R, 0, 1) * (dz < 1.6 * R)
    occ = np.clip(occ / taps * 2.2, 0, 1)
    # Skin takes less of it: light scatters into the ear and the eye
    # socket where it would leave cloth dark.
    ao = 1 - occlusion * occ * np.where(mat == MATS["skin"], 0.5, 1.0)
    # The colour, mottled.
    alb = buf[..., 0:3].copy()
    if mottle:
        h, w = z.shape
        m1 = fbm(h, w, seed + 1, 3, max(2, int(h / (0.5 * hpx))))
        m2 = fbm(h, w, seed + 2, 2, max(2, int(h / (0.15 * hpx))))
        alb *= (1 + mottle * (m1 - 0.5) * 2 + mottle * 0.6 *
                (m2 - 0.5) * 2)[..., None]
        warm = (m1 - 0.5)[..., None] * np.array((0.04, 0.0, -0.04))
        alb *= 1 + warm
    # Faces and hands keyed up a little, as a painter keeps them the
    # brightest things on a figure after a white collar or a letter.
    alb *= np.where(mat == MATS["skin"], skin_key, 1.0)[..., None]
    yy, xx = np.mgrid[y0:y1, x0:x1].astype(np.float32)
    pos = np.stack([xx, yy, z], -1)
    wrap = np.zeros(z.shape, np.float32)
    for mid, wv in WRAP.items():
        wrap[mat == mid] = wv
    skin = (mat == MATS["skin"])[..., None]
    glow = np.isin(mat, (MATS["beer"], MATS["glass"]))[..., None]
    spec = buf[..., 6:7]
    shine = buf[..., 7:8]
    rimk = buf[..., 8:9]
    view = np.array((0, 0, -1), np.float32)
    out = alb * np.asarray(ambient, np.float32) * ao[..., None]
    # Bounce from below: the floor, the cloth, the table under them.
    up = np.clip(n[..., 1:2], 0, 1)
    out += alb * np.asarray(bounce, np.float32) * up * ao[..., None]
    facing = np.clip((n * view).sum(-1, keepdims=True), 0, 1)
    edge = (1 - facing) ** 3
    for li, (lp, col, reach_, rim) in enumerate(lights):
        L = np.array(lp, np.float32) * ss
        col = np.asarray(col, np.float32)
        d = L - pos
        dist = np.linalg.norm(d, axis=-1, keepdims=True)
        l = d / np.maximum(dist, 1e-6)
        fall = 1.0 / (1.0 + (dist / (reach_ * ss)) ** 2)
        ndl = (n * l).sum(-1, keepdims=True)
        w_ = wrap[..., None]
        diff = np.clip((ndl + w_) / (1 + w_), 0, 1)
        sh = np.ones(z.shape, np.float32)
        if li == 0 and shadows:
            # Cast shadow: march toward the light across the depth buffer;
            # anything standing in the way, nearer to us than the ray but
            # no more than a body's thickness nearer, shades the point.
            blocked = np.zeros(z.shape, np.float32)
            steps = 14
            for s in range(1, steps + 1):
                t = (s / steps) ** 1.5 * 1.6 * hpx
                q = pos + l * t
                qx = np.clip(np.round(q[..., 0] - x0).astype(np.int32), 0,
                             x1 - x0 - 1)
                qy = np.clip(np.round(q[..., 1] - y0).astype(np.int32), 0,
                             y1 - y0 - 1)
                zs = z[qy, qx]
                hit = (zs < q[..., 2] - 0.04 * hpx) & \
                    (zs > q[..., 2] - 0.9 * hpx)
                blocked = np.maximum(blocked, hit * (1 - 0.4 * s / steps))
            sh = 1 - shadows * blocked
        lit = diff * fall * sh[..., None]
        out += alb * col * lit
        # Skin: light scattered under it warms the edge of the shadow.
        band = np.clip(1 - np.abs(ndl + 0.05) / 0.45, 0, 1) * fall * \
            sh[..., None]
        out += skin * alb * col * np.array((0.3, 0.08, 0.03)) * band
        # Beer and glass let the light through.
        out += glow * alb * col * fall * 0.45
        # A rim where the light is behind the form.
        out += col * fall * rim * rimk * edge * np.clip(ndl + 0.2, 0, 1) * \
            sh[..., None] ** 0.5
        # And the shine on what shines.
        hv = normalize(l + view)
        out += col * spec * 0.6 * fall * sh[..., None] * \
            np.clip((n * hv).sum(-1, keepdims=True), 0, 1) ** shine
    out *= c[..., None]
    rgb[y0:y1, x0:x1] = out
    return rgb, cov


# ---- on its own: the contact sheet ---------------------------------------------------
def _lamplit(path, w, h, ss, place, lamp, scale):
    """A dark room with one warm lamp: the men placed by 'place(G)',
    painted, and laid on a dim warm ground."""
    G = GScene(w, h, ss=ss)
    figs = place(G)
    paint_faces(G, figs)
    lights = [(lamp, (1.5, 1.15, 0.75), 260, 2.0),
              ((w * 0.95, h * 0.5, 600), (0.25, 0.22, 0.2), 500, 1.5)]
    rgb, cov = paint(G, lights, scale)
    yy = np.arange(G.h, dtype=np.float32)[:, None] / G.h
    xx = np.arange(G.w, dtype=np.float32)[None, :] / G.w
    g = 0.05 + 0.08 * np.exp(-((xx - lamp[0] / w) ** 2 + (yy - 0.3) ** 2) * 3)
    bg = g[..., None] * np.array((1.0, 0.8, 0.55), np.float32)
    rgb = np.where(cov[..., None], rgb, bg)
    x = 1.0 - np.exp(-rgb * 1.6)
    img = Image.fromarray(np.clip(x * 255 + 0.5, 0, 255).astype(np.uint8))
    img.resize((w, h), Image.LANCZOS).save(path)


def contact_sheet(path, ss=4):
    cells = [("officer", "stand", None, 0.0), ("officer", "sraise", None, 0.0),
             ("officer", "behind", None, math.pi / 2 + 0.3),
             ("officer", "sglass", "wrap", -0.4),
             ("officer", "lean", "shirt", 0.3),
             ("officer", "pocket", None, 0.3),
             ("officer", "glass", None, 0.0),
             ("officer", "toast", None, 0.2), ("officer", "drink", None, -0.2),
             ("officer", "pipe", None, 0.3), ("officer", "write", None, 0.0),
             ("officer", "cards", None, 0.3), ("officer", "head", None, 0.0),
             ("officer", "piano", None, math.pi - 0.4),
             ("officer", "slump", "cardigan", 0.4),
             ("officer", "read", "cardigan", 0.3),
             ("officer", "glass_low", None, 0.2),
             ("officer", "sing", "shirt", 0.2),
             ("officer", "clasped", None, 0.1),
             ("mechanic", "wave", None, 0.3), ("mechanic", "walk", None, -0.6),
             ("pilot", "stand", None, 0.3), ("pilot", "wave", None, 0.0)]

    def place(G):
        figs = []
        for i, (outfit, pose, style, yaw) in enumerate(cells):
            row, col = divmod(i, 8)
            fig = Figure(seed=100 + i, outfit=outfit, style=style,
                         grey=(i == 0))
            fig.pose(pose)
            fig.draw(G, 40 + col * 80, 125 + row * 130, 400, 14.0, yaw,
                     fig=i + 1)
            figs.append(fig)
        return figs
    _lamplit(path, 640, 400, ss, place, (320, 40, 300), 14.0)


def closeup(path, ss=4):
    """Faces and hands large: a toast, a pipe, a letter, a hand of
    cards, a walrus moustache, a man singing."""
    specs = [("officer", "toast", 7, None), ("officer", "pipe", 8, None),
             ("officer", "write", 9, None), ("officer", "cards", 11, "walrus"),
             ("officer", "sing", 12, None), ("officer", "glass_low", 13, None)]

    def place(G):
        figs = []
        for i, (outfit, pose, seed, mo) in enumerate(specs):
            fig = Figure(seed=seed, outfit=outfit, moustache=mo,
                         grey=(i == 1), style="shirt" if i == 4 else None)
            fig.pose(pose)
            floor = 120 + (5.6 if fig.seated else 7.5) * 44.0
            fig.draw(G, 58 + i * 105, floor, 400, 44.0, 0.15 * (i % 3 - 1),
                     fig=i + 1)
            figs.append(fig)
        return figs
    _lamplit(path, 640, 400, ss, place, (320, 30, 250), 44.0)


# ---- poses ----------------------------------------------------------------------------
# Hand targets are in his own frame (head-heights from the hips, -z in
# front of him).  For a 'grip' the target is the middle of what he holds;
# 'axis' points from its foot to its mouth; 'palm' the way the palm
# faces.  Seated poses suit a table whose top is about 1.0 above the
# seat, its far edge a forearm in front of him.
GLASS_R = dict(kind="grip", prop="glass", radius=0.16)
FLAT = dict(kind="flat", fingers=(0, 0.25, -1), palm=(0, 1, 0))


def _flat(x, y=-1.0, z=-1.4, pole=None):
    sx = 1 if x > 0 else -1
    return dict(FLAT, at=(x, y, z), pole=pole or (sx, 0.4, 0.5))


POSES = {
    "stand": dict(rest=8, angles={"shoulder_r": (0, 0, 6),
                                  "shoulder_l": (0, 0, -6),
                                  "elbow_r": (-12, 0, 0),
                                  "elbow_l": (-12, 0, 0)}),
    # At table.
    "table": dict(seated=True, lean=8,
                  hand_r=_flat(-0.6), hand_l=_flat(0.6)),
    "glass": dict(seated=True, lean=6, hand_l=_flat(0.6),
                  hand_r=dict(GLASS_R, at=(-0.6, -1.08, -1.45),
                              palm=(1, 0, -0.3), pole=(-1, 0.6, 0.4))),
    "toast": dict(seated=True, lean=-2, lift=12, hand_l=_flat(0.6),
                  hand_r=dict(GLASS_R, at=(-1.0, -3.55, -0.95),
                              palm=(1, 0, -0.2), pole=(-1, 0.5, 0.0))),
    "cheer": dict(seated=True, lean=-4, lift=16, mouth=1.0,
                  hand_l=dict(kind="wave", at=(1.2, -3.6, -0.8),
                              fingers=(0.1, -1, 0), palm=(0, 0, -1),
                              pole=(1, 0.2, 0.4)),
                  hand_r=dict(GLASS_R, at=(-1.1, -3.7, -0.85),
                              palm=(1, 0, -0.2), pole=(-1, 0.5, 0.0))),
    "drink": dict(seated=True, lean=4, lift=18, hand_l=_flat(0.6),
                  hand_r=dict(GLASS_R, at=(-0.1, -2.5, -0.68),
                              axis=(0.15, -0.75, 0.6), palm=(1, 0, 0),
                              pole=(-1, 1.0, 0.1))),
    "talk": dict(seated=True, lean=6, turn=-15, mouth=0.4, hand_l=_flat(0.6),
                 hand_r=dict(kind="open", at=(-0.9, -2.3, -1.3),
                             fingers=(0.3, -1, -0.4), palm=(0.6, 0, -0.8),
                             pole=(-1, 0.5, 0.3))),
    "pipe": dict(seated=True, lean=8, bow=4, hand_l=_flat(0.62),
                 hand_r=dict(kind="grip", prop="pipe", radius=0.06,
                             at=(-0.18, -2.62, -0.75), palm=(1, 0, 0),
                             axis=(0, -1, 0), pole=(-1, 0.6, 0.3))),
    # Writing a letter: bent over the sheet, pen in hand, the other hand
    # holding the paper flat.
    "write": dict(seated=True, lean=20, bow=26, turn=-6, tilt=-6,
                  hand_r=dict(kind="pinch", prop="pen",
                              at=(-0.12, -1.02, -1.5),
                              fingers=(0.4, 0.3, -1), palm=(0.3, 1, 0),
                              pole=(-1, 0.4, 0.5)),
                  hand_l=dict(FLAT, at=(0.42, -1.0, -1.45),
                              fingers=(-0.5, 0.2, -1), pole=(1, 0.4, 0.5)),
                  props=[dict(kind="sheet", at=(0.05, -0.97, -1.55),
                              size=(0.62, 0.8), angle=-8, tex="letter")]),
    # A hand of cards held up, the other hand on the table by his stake.
    "cards": dict(seated=True, lean=10, bow=10,
                  hand_l=dict(kind="pinch", prop="cards",
                              at=(0.25, -2.0, -1.25), fingers=(-0.1, -1, 0.6),
                              palm=(0, 0.2, 1), pole=(1, 0.6, 0.2)),
                  hand_r=_flat(-0.55),
                  props=[dict(kind="sheet", at=(-0.15, -0.97, -1.7),
                              size=(0.24, 0.34), angle=20, tex="back")]),
    "chin": dict(seated=True, lean=14, bow=10,
                 hand_l=dict(kind="fist", at=(0.1, -2.45, -0.75),
                             fingers=(0, -1, -0.3), palm=(0, 0, 1),
                             pole=(1, 0.6, -0.2)),
                 hand_r=_flat(-0.6)),
    "clasp": dict(seated=True, lean=12, bow=34,
                  hand_r=dict(kind="fist", at=(-0.12, -1.02, -1.3),
                              fingers=(0.5, 0, -1), palm=(1, 0.3, 0),
                              pole=(-1, 0.3, 0.6)),
                  hand_l=dict(kind="relax", at=(0.1, -1.08, -1.32),
                              fingers=(-0.6, 0.2, -1), palm=(-0.3, 1, 0),
                              pole=(1, 0.3, 0.6))),
    "head": dict(seated=True, lean=18, bow=30, turn=8,
                 hand_l=dict(kind="relax", at=(0.22, -2.55, -0.82),
                             fingers=(-0.3, -1, 0.1), palm=(-1, 0, 0.3),
                             pole=(1, 0.8, -0.4)),
                 hand_r=_flat(-0.6)),
    # Slumped back in his chair, legs out and crossed at the ankle,
    # hands folded on his stomach.
    "slump": dict(seated=True, slump=-14, lean=-6, bow=14, chair="club",
                  angles={"hip_r": (-62, 8, 0), "knee_r": (40, 0, 0),
                          "hip_l": (-66, -4, 0), "knee_l": (44, 0, 0)},
                  hand_r=dict(kind="relax", at=(-0.05, -0.75, -0.62),
                              fingers=(1, 0.2, -0.2), palm=(0, 0.3, 1),
                              pole=(-1, 0.6, -0.3)),
                  hand_l=dict(kind="relax", at=(0.1, -0.85, -0.6),
                              fingers=(-1, 0.3, -0.2), palm=(0, 0.3, 1),
                              pole=(1, 0.6, -0.3))),
    # Reading a newspaper held open in front of him, sat back, one leg
    # over the other.
    "read": dict(seated=True, slump=-8, lean=2, bow=10, chair="club",
                 angles={"hip_r": (-100, 14, 0), "knee_r": (100, 0, 0),
                         "hip_l": (-80, -6, 0), "knee_l": (80, 0, 0)},
                 hand_r=dict(kind="pinch", at=(-0.55, -1.95, -1.55),
                             fingers=(0.2, -1, 0), palm=(0.3, 0, 1),
                             pole=(-1, 0.6, 0.4)),
                 hand_l=dict(kind="pinch", at=(0.55, -1.95, -1.55),
                             fingers=(-0.2, -1, 0), palm=(-0.3, 0, 1),
                             pole=(1, 0.6, 0.4)),
                 props=[dict(kind="newspaper")]),
    "readbook": dict(seated=True, lean=4, bow=24,
                     hand_r=dict(kind="relax", at=(-0.3, -1.35, -1.25),
                                 fingers=(0.4, -0.6, -0.4), palm=(0.4, -0.3, 1),
                                 pole=(-1, 0.6, 0.2)),
                     hand_l=dict(kind="relax", at=(0.3, -1.35, -1.25),
                                 fingers=(-0.4, -0.6, -0.4),
                                 palm=(-0.4, -0.3, 1), pole=(1, 0.6, 0.2)),
                     props=[dict(kind="book")]),
    "piano": dict(seated=True, lean=10, bow=8, chair="stool",
                  hand_r=dict(kind="key", at=(-0.45, -1.3, -1.7),
                              fingers=(0, 0.6, -1), palm=(0, 1, 0),
                              pole=(-1, 0.3, 0.3)),
                  hand_l=dict(kind="key", at=(0.55, -1.3, -1.7),
                              fingers=(0, 0.6, -1), palm=(0, 1, 0),
                              pole=(1, 0.3, 0.3))),
    "warm": dict(seated=True, lean=16, bow=6,
                 hand_r=dict(kind="open", at=(-0.35, -1.65, -2.05),
                             fingers=(0, -0.4, -1), palm=(0, 0, -1),
                             pole=(-1, 0.5, 0.3)),
                 hand_l=dict(kind="open", at=(0.35, -1.65, -2.05),
                             fingers=(0, -0.4, -1), palm=(0, 0, -1),
                             pole=(1, 0.5, 0.3))),
    # Standing in the mess.
    "sglass": dict(rest=10,
                   hand_l=dict(kind="hidden", at=(0.62, 0.3, -0.25),
                               pole=(1, -0.2, 0.6)),
                   hand_r=dict(GLASS_R, at=(-0.5, -1.75, -0.95),
                               palm=(1, 0, 0.2), pole=(-1, 0.6, 0.2))),
    "pocket": dict(rest=-10, turn=10,
                   hand_l=dict(kind="hidden", at=(0.62, 0.3, -0.25),
                               pole=(1, -0.2, 0.6)),
                   hand_r=dict(kind="hidden", at=(-0.62, 0.3, -0.25),
                               pole=(-1, -0.2, 0.6))),
    "glass_low": dict(rest=9, bow=26,
                      hand_r=dict(GLASS_R, at=(-0.48, -0.6, -0.55),
                                  palm=(1, 0, 0.2), pole=(-1, 0.4, 0.4)),
                      hand_l=dict(kind="relax", at=(0.6, 0.35, -0.12),
                                  fingers=(0, 1, -0.2), palm=(-1, 0, 0),
                                  pole=(1, 0.1, 0.4))),
    "clasped": dict(rest=-7, bow=30,
                    hand_r=dict(kind="relax", at=(-0.08, -0.25, -0.62),
                                fingers=(1, 0.6, -0.3), palm=(0.2, 0, 1),
                                pole=(-1, 0.3, 0.3)),
                    hand_l=dict(kind="fist", at=(0.06, -0.28, -0.58),
                                fingers=(-1, 0.6, -0.3), palm=(-0.2, 0, 1),
                                pole=(1, 0.3, 0.3))),
    "sraise": dict(lift=14, rest=9, mouth=0.6,
                   hand_l=dict(kind="hidden", at=(0.62, 0.3, -0.25),
                               pole=(1, -0.2, 0.6)),
                   hand_r=dict(GLASS_R, at=(-0.95, -3.7, -0.9),
                               palm=(1, 0, -0.2), pole=(-1, 0.5, 0.0))),
    "sing": dict(lift=16, rest=-9, mouth=1.0,
                 hand_l=dict(kind="relax", at=(1.25, -2.4, -0.4),
                             fingers=(0.2, -1, 0), palm=(0, 0, -1),
                             pole=(1, 0.6, 0.2)),
                 hand_r=dict(GLASS_R, at=(-1.0, -3.6, -0.85),
                             palm=(1, 0, -0.2), pole=(-1, 0.5, 0.0))),
    "lean": dict(lift=18, mouth=1.0, lean=6, rest=9,
                 hand_l=dict(FLAT, at=(0.55, -1.0, -1.25), pole=(1, 0.4, 0.4)),
                 hand_r=dict(GLASS_R, at=(-0.95, -3.4, -0.95),
                             palm=(1, 0, -0.2), pole=(-1, 0.5, 0.0))),
    "behind": dict(bow=26, angles={"hip_r": (0, 0, 2), "hip_l": (0, 0, -2)},
                   hand_r=dict(kind="relax", at=(-0.15, -0.45, 0.62),
                               fingers=(1, 0.3, 0.3), palm=(0, 0, 1),
                               pole=(-1, 0.3, 0.8)),
                   hand_l=dict(kind="fist", at=(0.12, -0.5, 0.6),
                               fingers=(-1, 0.3, 0.3), palm=(0, 0, -1),
                               pole=(1, 0.3, 0.8))),
    # At the aerodrome.
    "swing_prop": dict(lean=22, bow=-4,
                       angles={"hip_r": (-30, 0, 4), "knee_r": (34, 0, 0),
                               "hip_l": (12, 0, -4), "knee_l": (8, 0, 0)},
                       foot_l=-20,
                       hand_r=dict(kind="flat", at=(-0.25, -3.7, -2.0),
                                   fingers=(0, -0.2, -1), palm=(0, 1, 0),
                                   pole=(-1, 0.6, 0.6)),
                       hand_l=dict(kind="flat", at=(0.3, -3.6, -2.1),
                                   fingers=(0, -0.2, -1), palm=(0, 1, 0),
                                   pole=(1, 0.6, 0.6))),
    "chocks": dict(lean=-18, bow=6,
                   angles={"hip_r": (-55, 0, 6), "knee_r": (70, 0, 0),
                           "hip_l": (20, 0, -6), "knee_l": (40, 0, 0)},
                   foot_l=-10,
                   hand_r=dict(kind="fist", at=(-0.18, -0.9, -1.7),
                               fingers=(0.8, 0, -0.5), palm=(0, 0, 1),
                               pole=(-1, 0.8, 0.3)),
                   hand_l=dict(kind="fist", at=(0.18, -1.0, -1.85),
                               fingers=(-0.8, 0, -0.5), palm=(0, 0, 1),
                               pole=(1, 0.8, 0.3))),
    "wave": dict(lift=6, rest=3,
                 hand_r=dict(kind="wave", at=(-1.4, -4.3, -0.4),
                             fingers=(-0.2, -1, 0), palm=(0, 0, -1),
                             pole=(-1, 0.4, 0.4)),
                 hand_l=dict(kind="relax")),
    "run": dict(lean=14,
                angles={"hip_r": (-55, 0, 0), "knee_r": (80, 0, 0),
                        "hip_l": (28, 0, 0), "knee_l": (40, 0, 0),
                        "shoulder_r": (45, 0, 8), "elbow_r": (-80, 0, 0),
                        "shoulder_l": (-50, 0, -8), "elbow_l": (-70, 0, 0)},
                foot_l=-35, foot_r=10),
    "walk": dict(lean=3,
                 angles={"hip_r": (-22, 0, 0), "knee_r": (12, 0, 0),
                         "hip_l": (16, 0, 0), "knee_l": (18, 0, 0),
                         "shoulder_r": (18, 0, 6), "elbow_r": (-20, 0, 0),
                         "shoulder_l": (-18, 0, -6), "elbow_l": (-25, 0, 0)},
                 foot_l=-14),
}


def main():
    ap = argparse.ArgumentParser(description="The figure kit's contact sheet.")
    ap.add_argument("--sheet", type=Path)
    ap.add_argument("--closeup", type=Path)
    ap.add_argument("--ss", type=int, default=4)
    args = ap.parse_args()
    if args.sheet:
        contact_sheet(args.sheet, args.ss)
    if args.closeup:
        closeup(args.closeup, args.ss)


if __name__ == "__main__":
    main()
