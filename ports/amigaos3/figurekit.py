#!/usr/bin/env python3
"""A kit of whole men, rigged and dressed, for the painted scenes.

Every man is built from shaped solids (ellipsoids and tapering cones,
scene3d.py) hung on a small skeleton, so that he can be posed by joint
angles and drawn lit by the same lamps as the room he stands in.  No
model, texture or picture from outside goes into him.

The skeleton
    pelvis - spine - chest - neck - head, with shoulder, elbow and wrist
    on each side, and hip, knee and ankle.  Lengths are in head-heights
    ('h'); a man is about seven and a half of them tall, his hips at the
    middle.  The frame is the renderer's: x across, y down, z away from
    us, and a man faces -z until he is turned.  Each joint takes angles
    in degrees (rx bends forward, ry turns, rz swings sideways).  Arms
    and legs can instead be sent to a point (two-bone reach), which is
    how a hand finds the table, a glass or a propeller blade.

The man
    A face of real parts: skull and face, a brow ridge that shades the
    eye sockets, eyeballs with irises and lids, a nose with its bridge,
    tip and wings, cheekbones, lips, jaw and chin, ears; hair cut to a
    hairline with a parting, sideburns, a moustache on some.  Hands with
    a palm, four fingers of three joints and a thumb, which curl round a
    glass, a pipe or a chock rope, lie flat, rest on piano keys or wave.
    Build (broad or slight, a paunch), height and face are drawn from a
    seed, so no two match.

Outfits
    officer   service dress: tunic with its folds at elbow, waist and
              back, collar and tie, pleated pockets, buttons, wings; the
              Sam Browne; breeches flared at the thigh; riding boots or
              puttees.  'wrap' gives the double-breasted R.F.C. jacket,
              'shirt' a man in his shirt-sleeves.
    mechanic  overalls buttoned up the front, a soft cap, puttees.
    pilot     a long leather flying coat, leather helmet and goggles,
              a scarf, sheepskin-topped boots.

Poses (POSES below) cover the mess (sitting at table, toasting, drinking,
talking, a pipe, bowed, a head in the hands, at the piano, warming at the
stove, standing with a glass, hands behind the back) and the aerodrome
(swinging the propeller, pulling the chocks away, waving, running,
walking clear).

    fig = Figure(seed=4, outfit="officer")
    fig.pose("toast", seated=True)
    fig.draw(sc, x, floor_y, z, px_per_h, yaw)

Run on its own it renders a contact sheet of outfits and poses:

    figurekit.py --sheet sheet.png [--closeup closeup.png]
"""

import argparse
import math
import sys
from pathlib import Path

import numpy as np
from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parent))
from scene3d import Scene, cone, rot_x, rot_y, rot_z  # noqa: E402

# ---- palettes -------------------------------------------------------------------
SKINS = [(198, 152, 122), (184, 138, 108), (208, 164, 132), (172, 126, 98),
         (192, 140, 112), (214, 172, 142)]
HAIRS = [(40, 30, 22), (58, 42, 28), (28, 22, 18), (76, 54, 32), (96, 72, 40),
         (34, 26, 22), (112, 84, 52)]
GREY_HAIR = (138, 132, 122)
KHAKI = [(86, 82, 54), (80, 76, 50), (92, 86, 56), (76, 74, 52), (88, 80, 54)]
SHIRT = (172, 156, 114)
BREECHES = [(138, 118, 84), (128, 112, 80), (146, 126, 90), (112, 102, 72)]
PUTTEES = (110, 98, 68)
BOOTS = (78, 44, 24)
LEATHER = (110, 66, 34)
BRASS = (200, 160, 84)
OVERALLS = [(74, 70, 58), (58, 60, 62), (92, 84, 64), (66, 62, 50)]
COAT = (100, 64, 36)
BEER = (214, 150, 58)


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


# ---- textures ---------------------------------------------------------------------
def cloth_tex(seed, amount=0.1):
    """Woven cloth with soft creases running every way."""
    rng = np.random.default_rng(seed)
    a, b = rng.uniform(0, 6, 2)

    def tex(q, n):
        c = np.sin(q[..., 0] * 5.3 + q[..., 1] * 2.1 + a) * \
            np.sin(q[..., 1] * 4.1 - q[..., 2] * 3.3 + b)
        return 1 - amount * np.clip(c, 0, 1) ** 3
    return tex


def waist_tex(seed):
    """The tunic gathered by the belt: horizontal folds just above it,
    the seam down the back."""
    rng = np.random.default_rng(seed)
    ph = rng.uniform(0, 6)

    def tex(q, n):
        y, x, z = q[..., 1], q[..., 0], q[..., 2]
        gather = np.clip((y - 0.2) / 0.6, 0, 1)
        fold = np.sin(y * 22 + np.sin(x * 4 + ph) * 2) ** 6 * gather
        seam = (z > 0.4) & (np.abs(x) < 0.05)
        return (1 - 0.22 * fold) * np.where(seam, 0.7, 1.0)
    return tex


def back_tex(seed):
    """The chest and back: the back seam and the diagonal pull toward
    the shoulder blades."""
    rng = np.random.default_rng(seed)
    ph = rng.uniform(0, 6)

    def tex(q, n):
        x, y, z = q[..., 0], q[..., 1], q[..., 2]
        seam = (z > 0.35) & (np.abs(x) < 0.04)
        diag = np.sin((np.abs(x) * 3 - y * 2.5) * 3 + ph) ** 8 * (z > 0.1)
        front = np.sin(x * 6 + y * 3 + ph) ** 10 * (z < -0.2) * 0.5
        return np.where(seam, 0.7, 1.0) * (1 - 0.16 * diag - 0.1 * front)
    return tex


def sleeve_tex(colour, end, lace=0):
    """A sleeve (cone texture: z -1 at the near joint, +1 at the far):
    rings of creases bunched at the elbow end ('end' is +1 for an upper
    arm, -1 for a forearm), rank lace near the cuff."""
    base = np.array(colour, np.float32) / 255
    braid = np.array((0.62, 0.54, 0.38), np.float32)

    def tex(z, ang):
        near = np.clip(1 - np.abs(z - end) / 0.7, 0, 1)
        crease = np.sin(z * 26 + np.sin(ang * 2) * 1.5) ** 6 * near
        out = np.broadcast_to(base, z.shape + (3,)).copy()
        out *= (1 - 0.28 * crease)[..., None]
        out *= (0.94 + 0.06 * np.sin(ang * 3 + z * 3))[..., None]
        if lace:
            for k in range(lace):
                out[np.abs(z - (0.6 - k * 0.12)) < 0.035] = braid
            out[z > 0.8] *= 0.9
        return out
    return tex


def body_tex(seed, yb, ry, outfit):
    """Cloth over one slice of the lofted body (yb its height, ry its
    half-height): folds gathered above the belt, the seam and the pull
    across the back, soft vertical drape below the waist, and for a
    long coat the deep folds of its skirts."""
    rng = np.random.default_rng(seed)
    ph = rng.uniform(0, 6, 3)

    def tex(q, n):
        x, z = q[..., 0], q[..., 2]
        y = yb + q[..., 1] * ry
        out = np.ones(x.shape, np.float32)
        gather = np.exp(-((y + 0.62) / 0.16) ** 2)
        out -= 0.2 * gather * np.sin(y * 40 + np.sin(x * 5 + ph[0]) * 2) ** 6
        back = z > 0.35
        out = np.where(back & (np.abs(x) < 0.06), out * 0.72, out)
        pull = np.sin((np.abs(x) * 3 - y * 2.2) * 3 + ph[1]) ** 8
        out -= 0.14 * pull * (z > 0.1) * (y < -1.0)
        drape = np.sin(x * (9 if outfit == "pilot" else 6) + ph[2]) ** 6
        out -= (0.25 if outfit == "pilot" else 0.1) * drape * (y > -0.2)
        crease = np.sin(x * 4 + y * 3 + ph[0]) ** 12 * (z < -0.2)
        out -= 0.08 * crease
        return out
    return tex


def puttee_tex(z, ang):
    band = np.sin((z * 8 + ang * 0.5) * math.pi)
    return 0.78 + 0.22 * (band > -0.3)


def hair_tex(part, comb=1.0):
    """Hair combed back from a parting at x = 'part' (head units)."""
    def tex(q, n):
        x, y, z = q[..., 0], q[..., 1], q[..., 2]
        streak = 0.93 + 0.07 * np.sin((x - part) * 40 * comb + z * 8)
        line = np.exp(-((x - part) / 0.05) ** 2) * (y < 0.2) * (z < 0.4)
        return streak * (1 - 0.45 * line)
    return tex


# ---- the skeleton ------------------------------------------------------------------
# (joint, parent, offset in the parent's frame, in head-heights)
BONES = [
    ("pelvis", None, (0, 0, 0)),
    ("spine", "pelvis", (0, -0.75, 0)),
    ("chest", "spine", (0, -0.9, 0)),
    ("neck", "chest", (0, -0.56, 0.03)),
    ("head", "neck", (0, -0.3, 0)),
    ("shoulder_r", "chest", (-0.82, -0.42, 0.04)),
    ("elbow_r", "shoulder_r", (0, 1.42, 0)),
    ("wrist_r", "elbow_r", (0, 1.18, 0)),
    ("shoulder_l", "chest", (0.82, -0.42, 0.04)),
    ("elbow_l", "shoulder_l", (0, 1.42, 0)),
    ("wrist_l", "elbow_l", (0, 1.18, 0)),
    ("hip_r", "pelvis", (-0.4, 0.1, 0)),
    ("knee_r", "hip_r", (0, 1.8, 0)),
    ("ankle_r", "knee_r", (0, 1.75, 0)),
    ("hip_l", "pelvis", (0.4, 0.1, 0)),
    ("knee_l", "hip_l", (0, 1.8, 0)),
    ("ankle_l", "knee_l", (0, 1.75, 0)),
]
STAND_HIP = 3.9            # floor to hip joint, standing
SEAT_HIP = 2.0             # floor to hip joint, on a chair

# Finger lengths (three joints) and their place across the knuckles,
# index first, as fractions of the hand.
FINGERS = [(0.95, -0.075), (1.0, -0.025), (0.95, 0.025), (0.78, 0.072)]


class Figure:
    """One man: his make (from a seed), his clothes, and his pose."""

    def __init__(self, seed=0, outfit="officer", style=None, grey=None,
                 moustache=None, build=None, height=None, puttees=None,
                 skin=None, hair=None, cloth=None, wings=None, lace=None):
        rng = np.random.default_rng(seed)
        self.rng = rng
        self.outfit = outfit
        self.grey = bool(grey) if grey is not None else False
        self.bw = build if build is not None else \
            float(np.clip(rng.normal(1.0, 0.07), 0.86, 1.15))
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
        self.style = style or (rng.choice(["sd", "sd", "sd", "wrap"])
                               if outfit == "officer" else outfit)
        if moustache is None:
            moustache = rng.choice([None, "brush", "full", "waxed", "walrus"],
                                   p=[0.38, 0.22, 0.24, 0.08, 0.08])
        self.moustache = moustache
        self.puttees = puttees if puttees is not None else (
            outfit == "mechanic" or (outfit == "officer" and not self.grey
                                     and rng.random() < 0.3))
        self.wings = wings if wings is not None else (rng.random() < 0.85)
        self.lace = lace if lace is not None else int(rng.integers(1, 3))
        # The face: its proportions, all a little different.
        f = dict(jaw=rng.normal(1, 0.07), nose=rng.normal(1, 0.1),
                 brow=rng.normal(1, 0.15), cheek=rng.normal(1, 0.15),
                 ears=rng.normal(1, 0.1), head=rng.normal(1, 0.03),
                 eyes=np.array(rng.choice([(60, 44, 30), (70, 90, 110),
                                           (80, 96, 80), (50, 36, 26)])),
                 part=float(rng.choice([-0.16, 0.16, 0.13, -0.12])),
                 hairline=rng.normal(0, 0.04) + (0.14 if self.grey else 0),
                 lips=rng.normal(1, 0.1))
        self.face = f
        self.seed = int(rng.integers(1 << 20))
        self.angles = {}
        self.targets = {}
        self.hands = {"r": dict(kind="relax"), "l": dict(kind="relax")}
        self.props = []
        self.mouth = 0.0
        self.seated = False
        self.foot_pitch = {"r": 0.0, "l": 0.0}

    # -- posing ---------------------------------------------------------------
    def pose(self, name, seated=None, **over):
        """Take up a named pose (POSES); keyword arguments override it."""
        spec = dict(POSES[name])
        spec.update(over)
        self.seated = spec.pop("seated", False) if seated is None else seated
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
        self.mouth = spec.pop("mouth", 0.0)
        self.foot_pitch = {"r": spec.pop("foot_r", 0.0),
                           "l": spec.pop("foot_l", 0.0)}
        self.props = spec.pop("props", [])
        # Bow, lift and turn of the head given as plain numbers.
        for key, joint, axis in (("bow", "neck", 0), ("lift", "neck", 0),
                                 ("turn", "head", 1), ("tilt", "head", 2),
                                 ("lean", "spine", 0), ("twist", "chest", 1)):
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
                    self.targets[side] = (np.array(self.targets[side][0]),
                                          pole)
                    zh = W["chest"][1][:, 2]
                    W["shoulder_" + side] = (S, frame(E - S, zh))
                    W["elbow_" + side] = (E, frame(T2 - E, zh))
                    W[name] = (T2, W["elbow_" + side][1])
                    self._reached = getattr(self, "_reached", {})
                    self._reached[side] = T2 - T
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
        ('ell', centre, radii, R, colour, kw) and
        ('cone', a, b, r0, r1, colour, kw)."""
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

    # The head --------------------------------------------------------------
    def _head(self, W):
        f, skin, hair = self.face, self.skin, self.hair
        k = f["head"]
        lip = skin * np.array((0.86, 0.68, 0.66))
        shade = skin * 0.93
        H = "head"

        def e(c, r, col, Rl=None, **kw):
            self.ell(H, np.asarray(c) * k, np.asarray(r) * k, col, Rl, **kw)
        # The neck, from the collar up into the skull.
        P0, R0 = W["neck"]
        P1, R1 = W["head"]
        self.cone_at(P0 + R0 @ np.array((0, 0.12, 0.02)),
                     P1 + R1 @ np.array((0, -0.18, 0.04)), 0.2, 0.17,
                     shade, rim=0.3)
        # Skull, the face in front of it, the jaw and chin.  Every feature
        # below is set on the surface of the face mass (its front is at
        # about z -0.42 at eye height), not buried in it.
        e((0, -0.52, 0.03), (0.36, 0.45, 0.42), skin, rim=0.15, spec=0.05)
        e((0, -0.38, -0.14), (0.31 * f["jaw"], 0.4, 0.3), skin, spec=0.06)
        e((0, -0.18, -0.1), (0.27 * f["jaw"], 0.2, 0.28), shade)
        e((0, -0.07, -0.3), (0.1 * f["jaw"], 0.08, 0.07), skin, spec=0.08)
        # The brow, rounded, over the eyes.
        e((0, -0.6, -0.355), (0.26, 0.07 * f["brow"], 0.06), skin,
          spec=0.06)
        for sx in (-1, 1):
            e((sx * 0.13, -0.62, -0.4), (0.075, 0.017, 0.02),
              hair * 0.9 if not self.grey else np.array(GREY_HAIR) * 0.8,
              rot_z(sx * -0.15))                                  # brow
            e((sx * 0.19, -0.42, -0.33), (0.085, 0.05 * f["cheek"], 0.06),
              skin * 1.02, spec=0.08)                            # cheekbone
            # The socket, the eye in it, the upper lid.
            e((sx * 0.12, -0.5, -0.385), (0.072, 0.05, 0.02),
              skin * np.array((0.7, 0.62, 0.6)))
            e((sx * 0.12, -0.5, -0.37), (0.04, 0.026, 0.04),
              skin * np.array((0.78, 0.74, 0.72)), spec=0.5, shine=40)
            e((sx * 0.12, -0.5, -0.405), (0.019, 0.02, 0.01),
              f["eyes"] * 0.6, spec=0.9, shine=60)
            e((sx * 0.12, -0.522, -0.395), (0.052, 0.018, 0.02), shade * 0.85)
            e((sx * 0.355, -0.46, 0.0), (0.04, 0.1 * f["ears"], 0.075),
              skin * 0.95, rot_y(sx * 0.35), rim=0.5)            # ear
            e((sx * 0.045, -0.335, -0.43), (0.03, 0.024, 0.026), shade)
        # The nose: bridge, tip, and its wings.
        n = f["nose"]
        P, R = W["head"]
        self.cone_at(P + R @ (np.array((0, -0.56, -0.39)) * k),
                     P + R @ (np.array((0, -0.38, -0.45 - 0.02 * n)) * k),
                     0.03 * k, 0.04 * k, skin, spec=0.15)
        e((0, -0.355, -0.455 - 0.02 * n), (0.045, 0.04, 0.04), skin * 1.03,
          spec=0.25, shine=24)
        # The mouth: lips, the line between them, open when he sings.
        m = self.mouth
        e((0, -0.25, -0.41), (0.085, 0.02 * f["lips"], 0.025), lip)
        e((0, -0.205 + 0.03 * m, -0.395), (0.075, 0.022 * f["lips"], 0.025),
          lip * 1.05)
        e((0, -0.228 + 0.015 * m, -0.4), (0.075, 0.006 + 0.03 * m, 0.025),
          (50, 30, 26))
        # A moustache.
        mo = self.moustache
        mcol = np.array(GREY_HAIR) * 0.9 if self.grey else hair * 0.95
        if mo == "brush":
            e((0, -0.278, -0.43), (0.05, 0.026, 0.03), mcol)
        elif mo == "full":
            e((0, -0.275, -0.425), (0.11, 0.032, 0.035), mcol)
        elif mo == "walrus":
            e((0, -0.265, -0.425), (0.125, 0.045, 0.04), mcol)
        elif mo == "waxed":
            for sx in (-1, 1):
                e((sx * 0.07, -0.278, -0.42), (0.085, 0.018, 0.03), mcol,
                  rot_z(sx * 0.35))
        # Hair: a shell over the skull, cut off by a slanting plane so that
        # it comes down to the forehead in front and to the nape behind.
        hl = f["hairline"]
        hcol = hair
        e((0, -0.53, 0.035), (0.385, 0.475, 0.45), hcol, rot_x(-0.53),
          clip_y=0.04 - hl, rim=0.1, spec=0.15, shine=14,
          texture=hair_tex(f["part"] / 0.385))
        for sx in (-1, 1):
            e((sx * 0.33, -0.43, -0.07), (0.04, 0.085, 0.06),
              hcol if not self.grey else np.array(GREY_HAIR))     # sideburn
            if self.grey:
                e((sx * 0.3, -0.52, 0.08), (0.09, 0.12, 0.2), GREY_HAIR)
        # Headgear for the aerodrome.
        if self.outfit == "mechanic":
            cap = self.cloth * 1.08
            e((0, -0.82, 0.0), (0.42, 0.14, 0.46), cap, rot_x(0.12),
              texture=cloth_tex(self.seed, 0.25))
            e((0, -0.72, 0.0), (0.395, 0.1, 0.44), cap * 0.85)
            e((0, -0.69, -0.42), (0.26, 0.03, 0.16), cap * 0.7, rot_x(0.25),
              spec=0.3)                                           # peak
        elif self.outfit == "pilot":
            leather = np.array(LEATHER) * 0.92
            e((0, -0.55, 0.03), (0.4, 0.49, 0.46), leather, rot_x(-0.25),
              clip_y=0.3, spec=0.7, shine=24, texture=cloth_tex(self.seed,
                                                              0.25))
            for sx in (-1, 1):
                e((sx * 0.36, -0.38, -0.02), (0.08, 0.17, 0.14), leather,
                  spec=0.6)                                        # flaps
                e((sx * 0.13, -0.8, -0.3), (0.085, 0.07, 0.04), BRASS,
                  rot_x(-0.6), spec=1.2, shine=30)                 # goggles
                e((sx * 0.13, -0.8, -0.33), (0.06, 0.05, 0.02), (80, 96, 92),
                  rot_x(-0.6), spec=1.8, shine=60)
            e((0, -0.78, -0.27), (0.38, 0.03, 0.2), (56, 48, 40),
              rot_x(-0.5))                                         # strap

    # The body --------------------------------------------------------------
    def _torso(self, W):
        bw, pa, c = self.bw, self.paunch, self.cloth
        o, st = self.outfit, self.style
        if st == "shirt":
            c = np.array(SHIRT, np.float32) * self.rng.normal(1, 0.03)
        sd = self.seed
        legs = self.breeches if o == "officer" else c
        # The body itself, lofted along the spine: slices from the hem up
        # to the shoulders, each as wide and deep as a man is at that
        # height, overlapping so closely that they make one smooth form.
        prof_y = [1.9, 1.0, 0.5, 0.3, 0.0, -0.4, -0.8, -1.2, -1.6, -1.95,
                  -2.15, -2.3]
        wx = [0.75, 0.69, 0.64, 0.6, 0.58, 0.51, 0.52, 0.57, 0.62, 0.64,
              0.6, 0.38]
        wz = [0.5, 0.46, 0.42, 0.38, 0.37, 0.32, 0.32, 0.35, 0.37, 0.35,
              0.3, 0.22]
        hem = {"pilot": 1.85, "mechanic": 0.3}.get(o, 0.45 if st != "shirt"
                                                    else 0.3)
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
        k = 0
        while yb >= top:
            P, R = self.axis_at(W, yb)
            ax, az, fwd = width(yb)
            ry = min(0.8, yb - top + 0.02, hem - yb + 0.02)
            col = c
            if st == "shirt" and yb > -0.32:
                col = legs
            self.ell_at(P + R @ np.array((0, 0, -fwd)), (ax, ry, az), col, R,
                        rim=0.3, texture=body_tex(sd, yb, ry, o))
            yb -= 0.05
            k += 1
        for sx in (-1, 1):
            self.ell("shoulder_" + ("r" if sx < 0 else "l"), (0, 0.05, 0),
                     (0.25, 0.24, 0.26), c, rim=0.3)              # deltoid

        def front(x, y, lift=0.0):
            """A point on the front of the body at chest-frame height y
            (the chest joint is 1.65 above the hips), and its frame."""
            yb = y - 1.65
            P, R = self.axis_at(W, yb)
            ax, az, fwd = width(yb)
            t = 1 - (x / ax) ** 2
            return P + R @ np.array((x, 0, -fwd - az * math.sqrt(max(t, 0.05))
                                     - lift)), R
        def _F(x, y, lift=0.0):
            return front(x, y, lift)[0]
        P, R = W["chest"]
        if o in ("officer", "mechanic"):
            skirt = o == "officer" and st != "shirt"
            if skirt:
                # The big pockets on the skirts of the tunic.
                for sx in (-1, 1):
                    self.ell("pelvis", (sx * 0.32 * bw, 0.08, -0.4 - pa * 0.6),
                             (0.18, 0.2, 0.04), c * 1.02, rim=0.2)
                    self.ell("pelvis", (sx * 0.32 * bw, -0.1, -0.43 - pa * 0.6),
                             (0.19, 0.055, 0.035), c * 1.06)
                    self.ell("pelvis", (sx * 0.32 * bw, -0.09, -0.47 - pa * 0.6),
                             (0.022, 0.022, 0.012), BRASS, spec=1.0)
            # Collar, and the open neck with shirt and tie (officers), or
            # the overalls buttoned to the throat.
            self.ell("neck", (0, 0.1, 0.03), (0.24, 0.1, 0.23), c * 0.96,
                     rim=0.3)
            if o == "officer" and st in ("sd", "shirt"):
                for sx in (-1, 1):
                    self.ell_at(_F(sx * 0.1, -0.68, 0.0),
                                (0.075, 0.2, 0.03), c * 1.08,
                                R @ rot_z(sx * 0.42))                # lapel
                self.ell_at(_F(0, -0.7, -0.005), (0.075, 0.16, 0.03),
                            SHIRT)
                self.ell_at(_F(0, -0.66, 0.01), (0.03, 0.15, 0.02),
                            (84, 72, 48))                            # tie
                self.ell("neck", (0, 0.07, -0.18), (0.06, 0.05, 0.04),
                         (84, 72, 48))                               # knot
            # Breast pockets with their pleats and flaps, buttons.
            pockets = (o == "officer" and st != "wrap") or o == "mechanic"
            if pockets:
                for sx in (-1, 1):
                    px = sx * 0.28 * bw
                    self.ell_at(_F(px, -0.26, 0.0),
                                (0.13, 0.13, 0.025), c * 1.03, R)
                    self.ell_at(_F(px, -0.4, 0.012),
                                (0.135, 0.04, 0.025), c * 1.1, R)    # flap
                    self.ell_at(_F(px, -0.37, 0.03),
                                (0.02, 0.02, 0.01), BRASS if o == "officer"
                                else (60, 56, 50), R, spec=1.0)
            if o == "officer" and st == "sd":
                for y in (-0.42, -0.1):
                    self.ell_at(_F(0, y, 0.01), (0.024, 0.024, 0.012),
                                BRASS, R, spec=1.1, shine=30)
                self.ell_at(_F(0, 1.0, 0.01), (0.024, 0.024, 0.012),
                            BRASS, spec=1.1, shine=30)
            if o == "officer" and st == "wrap":
                for k in range(3):
                    y = -0.55 + k * 0.28
                    self.ell_at(_F(0.2 + 0.1 * k, y, 0.01),
                                (0.024, 0.024, 0.012), BRASS, R, spec=1.1)
            if o == "officer" and self.wings and st != "shirt":
                self.ell_at(_F(0.27 * bw, -0.5, 0.01),
                            (0.13, 0.025, 0.012), (200, 190, 160), R)
            if o == "mechanic":
                for k in range(6):
                    self.ell_at(_F(0, -0.6 + k * 0.32, 0.01),
                                (0.022, 0.022, 0.01), (50, 46, 40), spec=0.5)
        if o == "pilot":
            # The leather coat: collar turned up, a white scarf, the long
            # skirts (lofted with the body) falling to the knee.
            self.ell("neck", (0, 0.05, 0.03), (0.3, 0.16, 0.29), c * 0.95,
                     rim=0.3, spec=0.5)
            self.ell("neck", (0, 0.18, -0.12), (0.2, 0.08, 0.16), (220, 214,
                     196))
            for k in range(5):
                self.ell_at(_F(0.08, 1.4 + k * 0.35, 0.0), (0.012, 0.2, 0.012),
                            c * 0.55)                     # the coat's edge
            ax, az, fwd = width(-0.4)
            Pb, Rb = self.axis_at(W, -0.4)
            self.ell_at(Pb + Rb @ np.array((0, 0, -fwd)), (ax + 0.03, 0.07,
                        az + 0.03), np.array(LEATHER) * 0.8, Rb,
                        spec=0.6)                                  # its belt
        # The Sam Browne: the belt, its buckle, the strap over the right
        # shoulder and across the back.
        if o == "officer" and st != "shirt" and \
                (self.grey or self.rng.random() < 0.8):
            ax, az, fwd = width(-0.43)
            Pb, Rb = self.axis_at(W, -0.43)
            self.ell_at(Pb + Rb @ np.array((0, 0, -fwd)), (ax + 0.025, 0.065,
                        az + 0.025), LEATHER, Rb, rim=0.3, spec=0.7, shine=22)
            self.ell_at(Pb + Rb @ np.array((0, 0, -fwd - az - 0.03)),
                        (0.06, 0.055, 0.02), BRASS, Rb, spec=1.2, shine=30)
            P0, R0 = W["chest"]
            P1, R1 = W["spine"]
            for zf in (-1, 1):
                a = P0 + R0 @ np.array((-0.45 * bw, -0.58, zf * 0.3))
                b = P1 + R1 @ np.array((0.46 * bw, 0.3, zf * (0.36 + pa)))
                self.cone_at(a, b, 0.035, 0.035, LEATHER, spec=0.7, shine=22)

    def _arm(self, W, side):
        c = self.cloth if self.style != "shirt" else np.array(SHIRT)
        S, E, Wr = (W["shoulder_" + side][0], W["elbow_" + side][0],
                    W["wrist_" + side][0])
        lace = self.lace if self.outfit == "officer" and \
            self.style != "shirt" else 0
        white = (255, 255, 255)
        self.ell_at(S, (0.24, 0.24, 0.24), c, rim=0.3)
        self.cone_at(S, E, 0.235, 0.185, white, rim=0.3,
                     texture=sleeve_tex(c, 1))
        self.ell_at(E, (0.185, 0.185, 0.185), c * 0.97, rim=0.3)
        cuff = Wr - unit(Wr - E) * 0.04
        self.cone_at(E, cuff, 0.185, 0.135 if self.outfit != "pilot"
                     else 0.16, white, rim=0.3,
                     texture=sleeve_tex(c * 0.97, -1, lace))
        if self.outfit == "pilot":                     # gauntlet cuffs
            self.cone_at(cuff - unit(Wr - E) * 0.18, cuff + unit(Wr - E) * 0.02,
                         0.16, 0.17, np.array(LEATHER) * 0.8, spec=0.5)

    def _hand(self, W, side):
        """Palm, four fingers of three joints, and the thumb, curled as
        the hand's kind asks."""
        h = self.hands[side]
        kind = h.get("kind", "relax")
        sx = -1 if side == "r" else 1
        F = self.hand_frame(side, W)
        P = W["wrist_" + side][0]
        skin = self.skin
        if self.outfit == "pilot":
            skin = np.array(LEATHER) * 0.85
        knuckle = skin * 0.94
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
        # The palm and the heel of the hand.
        self.ell_at(P + F @ np.array((0, 0.17, 0.0)), (0.11, 0.18, 0.05),
                    skin, F, rim=0.4, spec=0.08)
        self.ell_at(P + F @ np.array((-sx * 0.06, 0.1, 0.02)),
                    (0.06, 0.09, 0.05), skin * 0.98, F)
        spread = {"open": 9, "wave": 11}.get(kind, 2)
        for i, (ln, fx) in enumerate(FINGERS):
            xx = fx * sx * -1
            base = P + F @ np.array((xx, 0.33, 0.0))
            d = F @ np.array((xx * spread * 0.04, 1.0, 0.0))
            n = F[:, 2]                         # bends toward the palm
            segs = (0.15 * ln, 0.1 * ln, 0.085 * ln)
            rad = (0.029, 0.025, 0.021)
            p = base
            self.ell_at(p, (0.032, 0.03, 0.03), knuckle)
            for j in range(3):
                a = deg(curl[j])
                d = unit(d * math.cos(a) + n * math.sin(a))
                n = unit(np.cross(np.cross(d, n), d))
                q = p + d * segs[j]
                self.cone_at(p, q, rad[j], rad[j] * 0.9,
                             knuckle if j == 0 else skin, rim=0.3)
                self.ell_at(q, (rad[j] * 0.9,) * 3, skin)
                p = q
        # The thumb, from the heel of the palm toward the index side.
        tb = P + F @ np.array((sx * -0.09, 0.08, 0.03))
        td = unit(F @ np.array((sx * -0.55, 0.65, 0.35)))
        tn = unit(F @ np.array((sx * 0.3, 0.0, 1.0)))
        p = tb
        for j, (ln, rd) in enumerate(((0.13, 0.036), (0.1, 0.03))):
            a = deg(thumb_curl * (0.6 + 0.4 * j))
            td = unit(td * math.cos(a) + tn * math.sin(a))
            q = p + td * ln
            self.cone_at(p, q, rd, rd * 0.92, skin, rim=0.3)
            self.ell_at(q, (rd * 0.92,) * 3, skin)
            p = q
        # What it holds.
        prop = h.get("prop")
        if prop == "glass":
            cen = P + F @ np.array((0, 0.36, r + 0.05))
            self._glass(cen, h.get("axis", (0, -1, 0)), h.get("full", 1.0),
                        r)
        elif prop == "pipe":
            cen = P + F @ np.array((0, 0.33, r + 0.04))
            ax = unit(F[:, 0])
            self.ell_at(cen, (0.07, 0.07, 0.07), (78, 48, 28), spec=0.7,
                        shine=24)
            tip = np.asarray(h.get("to"), np.float32) if "to" in h else \
                cen + ax * 0.35
            self.cone_at(cen, tip, 0.022, 0.016, (34, 24, 18), spec=0.6)

    def _glass(self, cen, axis, full, r):
        """A pint pot: tapered glass, the beer in it, the head on it."""
        ax = unit(axis)                       # toward its mouth
        top = cen + ax * 0.3
        bot = cen - ax * 0.24
        col = BEER if full else (150, 136, 112)
        self.cone_at(bot, top, r * 0.92, r * 1.08, col, spec=1.2, shine=40)
        R = frame(ax, (0, 0, 1))
        if full:
            self.ell_at(top, (r * 1.08, 0.04, r * 1.08), (236, 228, 206), R,
                        spec=0.3)
        self.ell_at(bot, (r * 0.95, 0.03, r * 0.95), (170, 140, 90), R,
                    spec=1.0)

    def _leg(self, W, side):
        sx = -1 if side == "r" else 1
        H, K, A = (W["hip_" + side][0], W["knee_" + side][0],
                   W["ankle_" + side][0])
        o = self.outfit
        legs = self.breeches if o == "officer" else self.cloth
        if o == "pilot":
            legs = np.array(BREECHES[3], np.float32)
        self.cone_at(H, K, 0.33, 0.21, legs, rim=0.3,
                     texture=lambda z, a: 1 - 0.12 * np.sin(z * 9 + a * 2) ** 8)
        Rk = W["knee_" + side][1]
        Rh = W["hip_" + side][1]
        if o == "officer":
            # The flare of the breeches on the outside of the thigh.
            self.ell("hip_" + side, (sx * 0.12, 0.62, 0.0), (0.33, 0.5, 0.3),
                     legs, rim=0.3, texture=cloth_tex(self.seed + 5 + sx, 0.2))
        self.ell_at(K, (0.21, 0.21, 0.21), legs, rim=0.3)
        # Lower leg: riding boot, puttees over an ankle boot, overalls
        # trousers, or a flying boot with its sheepskin top.
        if o == "pilot":
            self.cone_at(K + Rk @ np.array((0, 0.25, 0)), A, 0.22, 0.16,
                         (70, 46, 28), spec=0.5)
            self.cone_at(K + Rk @ np.array((0, 0.1, 0)),
                         K + Rk @ np.array((0, 0.3, 0)), 0.25, 0.25,
                         (200, 184, 150),
                         texture=lambda z, a: 0.8 + 0.2 * np.sin(a * 9) ** 2)
        elif self.puttees:
            self.cone_at(K, A, 0.2, 0.13, PUTTEES, rim=0.3,
                         texture=puttee_tex)
        elif o == "mechanic":
            self.cone_at(K, A, 0.2, 0.16, legs, rim=0.3)
        else:
            self.cone_at(K, A, 0.215, 0.145, BOOTS, rim=0.3, spec=0.9,
                         shine=26)
            self.cone_at(K - Rk @ np.array((0, 0.06, 0)),
                         K + Rk @ np.array((0, 0.12, 0)), 0.235, 0.225, BOOTS,
                         spec=0.9, shine=26)                        # boot top
        # The foot, flat on the floor unless the pose tips it.
        fwd = unit(Rh @ np.array((0, 0, -1)) * np.array((1, 0, 1)) +
                   np.array((0, 0, -1e-3)))
        if self.seated:
            fwd = np.array((0, 0, -1), np.float32)
        pitch = deg(self.foot_pitch[side])
        d = unit(fwd * math.cos(pitch) + np.array((0, 1, 0)) * math.sin(pitch))
        Rf = frame(np.array((0, 1, 0)) * math.cos(pitch) - fwd *
                   math.sin(pitch), d)
        boot = (60, 40, 26) if self.puttees or o == "mechanic" else BOOTS
        self.ell_at(A + np.array((0, 0.13, 0)) + d * 0.12,
                    (0.12, 0.1, 0.3), boot, Rf, spec=0.8, shine=24)
        self.ell_at(A + np.array((0, 0.06, 0)), (0.13, 0.12, 0.13), boot,
                    spec=0.6)

    def _prop(self, W, p):
        if p == "pipe_mouth":
            P, R = W["head"]
            a = P + R @ np.array((0.02, -0.22, -0.38))
            b = P + R @ np.array((0.06, -0.12, -0.62))
            self.cone_at(a, b, 0.02, 0.018, (34, 24, 18), spec=0.6)
            self.ell_at(b + np.array((0, -0.05, 0)), (0.06, 0.08, 0.06),
                        (78, 48, 28), spec=0.7, shine=24)

    # -- into the scene ------------------------------------------------------------
    def draw(self, sc, x, floor_y, z, px_per_h, yaw=0.0):
        """Stand (or sit) him with his feet at (x, floor_y), depth z, at
        px_per_h pixels to a head-height, turned by yaw (0 faces us)."""
        k = px_per_h * self.tall
        hip = (SEAT_HIP if self.seated else STAND_HIP) * k
        o = np.array((x, floor_y - hip, z), np.float32)
        Y = rot_y(yaw).astype(np.float32)
        for part in self.build():
            if part[0] == "ell":
                _, c, r, R, col, kw = part
                sc.ellipsoid(tuple(o + k * (Y @ c)), tuple(r * k), col,
                             R=(Y @ R).astype(np.float32), **kw)
            else:
                _, a, b, r0, r1, col, kw = part
                a, b = o + k * (Y @ a), o + k * (Y @ b)
                if np.linalg.norm(b - a) < 1e-3:
                    continue
                cone(sc, tuple((a + b) / 2), b - a,
                     float(np.linalg.norm(b - a)) / 2, r0 * k, r1 * k, col,
                     **kw)
        return o

    def hip_at(self, x, floor_y, z, px_per_h):
        k = px_per_h * self.tall
        return (x, floor_y - (SEAT_HIP if self.seated else STAND_HIP) * k, z)


# ---- poses ----------------------------------------------------------------------------
# Hand targets are in his own frame (head-heights from the hips, -z in
# front of him).  For a 'grip' the target is the middle of what he holds;
# 'axis' points from its foot to its mouth; 'palm' the way the palm
# faces.  Seated poses suit a table whose top is about 1.3 above the
# seat, its far edge a forearm in front of him.
GLASS_R = dict(kind="grip", prop="glass", radius=0.16)
FLAT = dict(kind="flat", fingers=(0, 0.25, -1), palm=(0, 1, 0))


def _flat(x, y=-1.0, z=-1.4, pole=None):
    sx = 1 if x > 0 else -1
    return dict(FLAT, at=(x, y, z), pole=pole or (sx, 0.4, 0.5))


POSES = {
    "stand": dict(angles={"shoulder_r": (0, 0, 6), "shoulder_l": (0, 0, -6),
                          "elbow_r": (-12, 0, 0), "elbow_l": (-12, 0, 0)}),
    # At table.
    "table": dict(seated=True, lean=8,
                  hand_r=_flat(-0.6), hand_l=_flat(0.6)),
    "glass": dict(seated=True, lean=6, hand_l=_flat(0.6),
                  hand_r=dict(GLASS_R, at=(-0.6, -1.08, -1.45),
                              palm=(1, 0, -0.3), pole=(-1, 0.6, 0.4))),
    "toast": dict(seated=True, lean=-2, lift=12, hand_l=_flat(0.6),
                  hand_r=dict(GLASS_R, at=(-1.15, -4.2, -0.75),
                              palm=(1, 0, -0.2), pole=(-1, 0.2, 0.4))),
    "cheer": dict(seated=True, lean=-4, lift=16, mouth=1.0,
                  hand_l=dict(kind="wave", at=(1.3, -4.0, -0.7),
                              fingers=(0.1, -1, 0), palm=(0, 0, -1),
                              pole=(1, 0.2, 0.4)),
                  hand_r=dict(GLASS_R, at=(-1.2, -4.25, -0.75),
                              palm=(1, 0, -0.2), pole=(-1, 0.2, 0.4))),
    "drink": dict(seated=True, lean=4, lift=18, hand_l=_flat(0.6),
                  hand_r=dict(GLASS_R, at=(-0.1, -2.5, -0.68),
                              axis=(0.15, -0.75, 0.6), palm=(1, 0, 0),
                              pole=(-1, 1.0, 0.1))),
    "talk": dict(seated=True, lean=6, turn=-15, hand_l=_flat(0.6),
                 hand_r=dict(kind="open", at=(-0.9, -2.3, -1.3),
                             fingers=(0.3, -1, -0.4), palm=(0.6, 0, -0.8),
                             pole=(-1, 0.5, 0.3))),
    "pipe": dict(seated=True, lean=8, bow=4, hand_l=_flat(0.62),
                 hand_r=dict(kind="grip", prop="pipe", radius=0.06,
                             at=(-0.18, -2.62, -0.75), palm=(1, 0, 0),
                             axis=(0, -1, 0), pole=(-1, 0.6, 0.3))),
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
    "piano": dict(seated=True, lean=10, bow=8,
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
    "sglass": dict(angles={"hip_l": (-4, 0, 3), "knee_l": (10, 0, 0)},
                   hand_l=dict(kind="fist", at=(0.68, -0.4, -0.15),
                               fingers=(-0.5, 0.3, -0.2), palm=(-1, 0, 0),
                               pole=(1, -0.2, 0.3)),
                   hand_r=dict(GLASS_R, at=(-0.55, -1.75, -0.95),
                               palm=(1, 0, 0.2), pole=(-1, 0.6, 0.2))),
    "sraise": dict(lift=14, angles={"hip_l": (-4, 0, 3), "knee_l": (10, 0, 0)},
                   hand_l=dict(kind="fist", at=(0.68, -0.4, -0.15),
                               fingers=(-0.5, 0.3, -0.2), palm=(-1, 0, 0),
                               pole=(1, -0.2, 0.3)),
                   hand_r=dict(GLASS_R, at=(-1.15, -4.45, -0.6),
                               palm=(1, 0, -0.2), pole=(-1, 0.2, 0.4))),
    "lean": dict(lift=18, mouth=1.0, lean=6,
                 angles={"hip_r": (-3, 0, -3), "knee_r": (8, 0, 0)},
                 hand_l=dict(FLAT, at=(0.55, -1.0, -1.25), pole=(1, 0.4, 0.4)),
                 hand_r=dict(GLASS_R, at=(-1.1, -4.4, -0.6),
                             palm=(1, 0, -0.2), pole=(-1, 0.2, 0.4))),
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
    "wave": dict(lift=6, angles={"hip_r": (-3, 0, 2), "knee_r": (6, 0, 0)},
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


# ---- on its own: the contact sheet ---------------------------------------------------
def _sheet_scene(w, h, ss):
    sc = Scene(w, h, ambient=(0.18, 0.17, 0.16), ss=ss)
    sc.point_rim = 0.4
    sc.point((w * 0.3, -h * 0.4, -200), (1.6, 1.5, 1.35), 700)
    sc.point((w * 0.9, h * 0.3, 300), (0.5, 0.48, 0.45), 500)
    yy = np.arange(sc.h, dtype=np.float32)[:, None] / sc.h
    bg = np.broadcast_to((0.42 - 0.1 * yy)[..., None] *
                         np.array((0.92, 0.9, 0.86), np.float32),
                         (sc.h, sc.w, 3))
    sc.surface(np.ones((sc.h, sc.w), bool), 900, bg, normal=(0, 0, -1))
    sc.rgb[:] = bg
    return sc


def contact_sheet(path, ss=4):
    W_, H_ = 640, 400
    sc = _sheet_scene(W_, H_, ss)
    cells = [("officer", "stand", None, 0.0), ("officer", "sraise", None, 0.0),
             ("officer", "behind", None, math.pi / 2 + 0.3),
             ("officer", "sglass", "wrap", -0.4),
             ("officer", "lean", "shirt", 0.3),
             ("officer", "wave", None, 0.0),
             ("officer", "glass", None, 0.0),
             ("officer", "toast", None, 0.2), ("officer", "drink", None, -0.2),
             ("officer", "pipe", None, 0.3), ("officer", "clasp", None, 0.0),
             ("officer", "head", None, 0.0), ("officer", "piano", None,
                                              math.pi - 0.4),
             ("officer", "warm", None, math.pi / 2),
             ("mechanic", "swing_prop", None, -math.pi / 2),
             ("mechanic", "chocks", None, math.pi / 2),
             ("mechanic", "wave", None, 0.3), ("mechanic", "run", None,
                                                -math.pi / 2),
             ("mechanic", "walk", None, -0.6), ("pilot", "stand", None, 0.3),
             ("pilot", "walk", None, -math.pi / 2),
             ("pilot", "wave", None, 0.0)]
    per_row = 8
    for i, (outfit, pose, style, yaw) in enumerate(cells):
        row, col = divmod(i, per_row)
        fig = Figure(seed=100 + i, outfit=outfit, style=style,
                     grey=(i == 0))
        fig.pose(pose)
        x = 40 + col * 80
        floor = 125 + row * 130
        fig.draw(sc, x, floor, 400, 14.0, yaw)
    sc.finish().save(path)


def closeup(path, ss=4):
    """Faces and hands large: a toast, a pipe, a bowed head, a walrus
    moustache, a mechanic in his cap and a pilot in his helmet."""
    sc = _sheet_scene(640, 400, ss)
    specs = [("officer", "toast", 7), ("officer", "pipe", 8),
             ("officer", "clasp", 9), ("officer", "glass", 11),
             ("mechanic", "stand", 12), ("pilot", "stand", 13)]
    for i, (outfit, pose, seed) in enumerate(specs):
        fig = Figure(seed=seed, outfit=outfit,
                     moustache="walrus" if i == 3 else None, grey=(i == 1))
        fig.pose(pose)
        # Feet placed so that every head comes out near the top third.
        floor = 120 + (5.6 if fig.seated else 7.5) * 44.0
        fig.draw(sc, 58 + i * 105, floor, 400, 44.0, 0.15 * (i % 3 - 1))
    sc.finish().save(path)


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
