"""Soldiers and officers built from lit solids (scene3d.py).

soldier() and bench_row() are the picture house's audience, seated and
seen from behind; officer() further down is a whole man for the mess,
seated or standing and turned any way.

Each man is his own: a greatcoat over sloping shoulders with folds down
the back, the turned-up collar, the nape of his neck and his ears, his
head turned or tilted a little, and a soft trench cap, a steel helmet,
an Australian slouch hat or his own hair.  Size, slump, tilt and colour
vary from man to man.
"""

import math

import numpy as np

from scene3d import axis_frame, fbm, rot_x, rot_y, rot_z

COATS = [(84, 74, 50), (92, 82, 56), (76, 70, 52), (88, 76, 48),
         (70, 66, 50), (96, 86, 60)]
SKINS = [(196, 150, 120), (180, 134, 104), (206, 162, 128), (170, 124, 96)]
HAIR = [(46, 34, 24), (64, 46, 30), (30, 24, 20), (92, 70, 44)]


def folds(seed):
    """Greatcoat folds: soft vertical creases down the back, broken up."""
    def tex(q, n):
        wob = np.sin(q[..., 1] * 2.2 + seed) * 0.18
        crease = np.sin((q[..., 0] + wob) * (4.0 + seed % 3)) ** 10
        seam = np.exp(-((q[..., 0] - 0.02 * math.sin(seed)) / 0.04) ** 2) * \
            (q[..., 1] > -0.75)
        weave = 0.97 + 0.03 * np.sin(q[..., 0] * 90) * np.sin(q[..., 1] * 90)
        return (1 - 0.12 * crease - 0.18 * seam) * weave
    return tex


def hair_tex(seed):
    def tex(q, n):
        return 0.85 + 0.15 * np.sin(q[..., 0] * 40 + q[..., 1] * 12 + seed)
    return tex


def soldier(sc, x, y, s, z, rng, kind=None):
    """One man.  (x, y) is the middle of his head at 1x, s his scale."""
    r = 9.0 * s
    coat = np.array(COATS[rng.integers(len(COATS))]) * rng.normal(1, 0.05)
    skin = SKINS[rng.integers(len(SKINS))]
    hair = HAIR[rng.integers(len(HAIR))]
    kind = rng.choice(4, p=[0.42, 0.33, 0.1, 0.15]) if kind is None else kind
    slump = rng.normal(0, 0.06)
    lean = rng.normal(0, 0.05)
    turn = rng.normal(0, 0.25)
    tilt = rng.normal(0, 0.08)
    sh_w = rng.normal(1.0, 0.06)
    seed = int(rng.integers(0, 1000))
    hx = x + math.sin(lean) * r * 2.4
    # Greatcoat: the back, then each shoulder sloping away, then the collar.
    # The back: a broad upper back carrying the shoulders, the body below
    # it, the shoulder caps rounding off its corners, the collar turned up.
    sc.ellipsoid((x, y + r * 6.4, z + r * 1.0), (r * 2.35 * sh_w, r * 4.0,
                 r * 1.3), coat * 0.92, R=rot_z(lean), rim=0.8,
                 texture=folds(seed))
    sc.ellipsoid((x, y + r * 3.1, z + r * 0.8), (r * 2.35 * sh_w, r * 1.85,
                 r * 1.15), coat, R=rot_z(lean + slump * 0.5), rim=0.9,
                 texture=folds(seed + 3))
    for side in (-1, 1):
        sc.ellipsoid((x + side * r * 1.55 * sh_w, y + r * (2.1 + slump * side),
                      z + r * 0.85), (r * 0.72, r * 0.55, r * 0.75), coat,
                     R=rot_z(side * (0.5 + slump)), rim=0.95)
    sc.ellipsoid((hx, y + r * 1.2, z + r * 0.4), (r * 0.95, r * 0.5,
                 r * 0.8), coat * 0.8, R=rot_z(lean), rim=0.85)
    # The nape of the neck, the head, its ears.
    sc.ellipsoid((hx, y + r * 0.72, z + r * 0.1), (r * 0.42, r * 0.55,
                 r * 0.42), skin, rim=0.4)
    Rh = rot_z(tilt) @ rot_y(turn)
    sc.ellipsoid((hx, y, z), (r * 0.78, r * 0.95, r * 0.86), skin, R=Rh,
                 rim=0.5, spec=0.05)
    for side in (-1, 1):
        ex = hx + side * r * 0.76 * math.cos(turn) - r * 0.2 * math.sin(turn)
        sc.ellipsoid((ex, y + r * 0.08, z - r * 0.05 + side * r * 0.2 *
                      math.sin(turn)), (r * 0.17, r * 0.33, r * 0.14),
                     np.array(skin) * (0.95, 0.85, 0.85), R=rot_z(side * 0.25),
                     rim=0.95)
    # Hair over the back of the head, cut short above the nape.
    sc.ellipsoid((hx, y - r * 0.06, z - r * 0.04), (r * 0.82, r * 0.98,
                 r * 0.9), hair, R=Rh, clip_y=0.55 if kind == 3 else 0.62,
                 rim=0.8, texture=hair_tex(seed))
    if kind == 0:                                   # soft trench cap
        cap = (78, 72, 48) if rng.random() < 0.8 else (64, 62, 52)
        Rc = Rh @ rot_x(rng.normal(-0.15, 0.08))
        sc.ellipsoid((hx, y - r * 0.92, z + r * 0.05), (r * 1.0, r * 0.42,
                     r * 1.05), cap, R=Rc, rim=0.65, texture=folds(seed + 7))
        sc.ellipsoid((hx, y - r * 0.6, z), (r * 0.88, r * 0.2, r * 0.9),
                     np.array(cap) * 0.78, R=Rc, rim=0.85)
        sc.ellipsoid((hx, y - r * 0.58, z + r * 0.75), (r * 0.72, r * 0.07,
                     r * 0.45), (40, 36, 30), R=Rc, rim=0.8, spec=0.2)
    elif kind == 1:                                 # steel helmet
        steel = (70, 74, 64)
        Rc = Rh @ rot_x(rng.normal(-0.1, 0.1))
        sc.ellipsoid((hx, y - r * 0.62, z), (r * 1.6, r * 0.13, r * 1.45),
                     steel, R=Rc, rim=0.9, spec=0.25, shine=10)
        sc.ellipsoid((hx, y - r * 0.68, z), (r * 1.0, r * 0.82, r * 1.0),
                     steel, R=Rc, rim=0.8, spec=0.3, shine=10, clip_y=0.1)
    elif kind == 2:                                 # slouch hat
        felt = (110, 92, 62)
        Rc = Rh @ rot_z(0.12)
        sc.ellipsoid((hx, y - r * 0.6, z), (r * 1.7, r * 0.1, r * 1.55),
                     felt, R=Rc, rim=0.7)
        sc.ellipsoid((hx, y - r * 0.95, z), (r * 0.9, r * 0.72, r * 0.85),
                     felt, R=Rc, rim=0.7, clip_y=0.2)
        sc.ellipsoid((hx, y - r * 0.7, z - r * 0.1), (r * 0.93, r * 0.14,
                     r * 0.88), (54, 44, 32), R=Rc, rim=0.85)


def bench_row(sc, y, s, z, x0, seed, width=640, gap=4.9, avoid=None):
    """A row of men on a bench, irregularly spaced."""
    rng = np.random.default_rng(seed)
    x = x0 + rng.integers(-6, 6)
    while x < width + 30:
        if not (avoid and avoid[0] < x < avoid[1]):
            soldier(sc, x, y + rng.normal(0, 1.5 * s), s * rng.normal(1, 0.06),
                    z + rng.normal(0, 2), rng)
        x += 9 * s * gap + rng.integers(0, 16)


# ---- Officers in the mess ---------------------------------------------------
#
# Whole men, seated or standing, facing any way round, for the squadron
# mess (make_mess.py).  Each is built in his own frame, in head-heights
# ('u', thirteen pixels at scale 1): x to his left as we look at him, y
# down, z away from us, with the origin at his hips.  The frame is then
# turned about the upright ('yaw': 0 faces us, pi turns his back) and set
# into the room, so the same man serves face-on at the far side of the
# table, back-on at the near side, or side-on at the stove.  At this size
# a face is a dozen pixels, so what tells is the set of the head and
# shoulders and where the hands are: a bowed head, a glass held up to the
# lamp, hands spread to the fire.

TUNICS = [(86, 82, 54), (80, 76, 50), (92, 86, 56), (76, 74, 52),
          (84, 78, 56)]
BREECHES = [(138, 118, 84), (128, 112, 80), (146, 126, 90)]
BOOTS = (74, 42, 24)
LEATHER = (108, 64, 34)
CHAIR = (62, 40, 24)
BEER = (214, 150, 58)
U = 13.0


def limb(sc, a, b, rad, albedo, **kw):
    """A rounded limb (an ellipsoid drawn out along a to b, at 1x)."""
    a = np.asarray(a, np.float32)
    b = np.asarray(b, np.float32)
    d = b - a
    L = float(np.linalg.norm(d))
    if L < 1e-3:
        sc.ellipsoid(tuple(a), (rad, rad, rad), albedo, **kw)
        return
    sc.ellipsoid(tuple((a + b) / 2), (rad, rad, L / 2 + rad * 0.55), albedo,
                 R=axis_frame(d), **kw)


class Body:
    """One man's frame: local points in head-heights, turned and placed."""

    def __init__(self, sc, hip, s, yaw):
        self.sc = sc
        self.o = np.array(hip, np.float32)
        self.u = U * s
        self.Ry = rot_y(yaw)

    def at(self, p):
        return self.o + self.Ry @ (np.asarray(p, np.float32) * self.u)

    def blob(self, p, r, col, R=None, **kw):
        Rl = np.eye(3, dtype=np.float32) if R is None else R
        self.sc.ellipsoid(tuple(self.at(p)), tuple(np.asarray(r) * self.u),
                          col, R=(self.Ry @ Rl).astype(np.float32), **kw)

    def limb(self, a, b, rad, col, **kw):
        limb(self.sc, self.at(a), self.at(b), rad * self.u, col, **kw)


# Where the elbows and hands go, per pose: (left elbow, left hand, right
# elbow, right hand), and which hand holds a glass ('L', 'R' or None) and
# how ('table', 'up', 'mouth').  For the seated poses the table top runs
# back from about 1.3u above the hips at its far edge; a hand laid on it
# a forearm in front of him shows lower down than that, on the cloth.
POSES = {
    "rest":   ((-1.2, -1.05, -0.75), (-0.55, -0.9, -1.45),
               (1.2, -1.05, -0.75), (0.55, -0.9, -1.45), None, None),
    "glass":  ((-1.2, -1.05, -0.75), (-0.6, -0.9, -1.45),
               (1.25, -1.05, -0.8), (0.75, -0.95, -1.5), "R", "table"),
    "raise":  ((-1.2, -1.05, -0.75), (-0.6, -0.9, -1.45),
               (1.65, -3.05, -0.3), (1.35, -4.2, -0.45), "R", "up"),
    "cheer":  ((-1.65, -3.05, -0.3), (-1.35, -4.2, -0.45),
               (1.65, -3.05, -0.3), (1.4, -4.15, -0.5), "R", "up"),
    "drink":  ((-1.2, -1.05, -0.75), (-0.6, -0.9, -1.45),
               (1.25, -1.75, -0.85), (0.3, -2.75, -0.85), "R", "mouth"),
    "talk":   ((-1.2, -1.05, -0.75), (-0.6, -0.9, -1.45),
               (1.3, -1.5, -0.85), (1.1, -2.55, -1.45), None, None),
    "clasp":  ((-1.15, -1.05, -0.8), (-0.12, -0.95, -1.35),
               (1.15, -1.05, -0.8), (0.12, -0.95, -1.35), None, None),
    "head":   ((-1.25, -1.05, -0.95), (-0.3, -2.85, -0.85),
               (1.15, -1.05, -0.8), (0.4, -0.95, -1.35), None, None),
    "behind": ((-1.1, -1.0, 0.35), (-0.3, -0.45, 0.62),
               (1.1, -1.0, 0.35), (0.3, -0.45, 0.62), None, None),
    "sglass": ((-1.12, -0.85, 0.05), (-1.05, 0.3, -0.15),
               (1.2, -1.05, -0.45), (0.6, -1.55, -1.0), "R", "table"),
    "sraise": ((-1.12, -0.85, 0.05), (-1.05, 0.3, -0.15),
               (1.65, -3.1, -0.25), (1.4, -4.25, -0.4), "R", "up"),
    "piano":  ((-1.1, -1.15, -0.9), (-0.5, -1.3, -1.85),
               (1.1, -1.15, -0.9), (0.6, -1.3, -1.85), None, None),
    "warm":   ((-1.0, -1.3, -1.05), (-0.42, -1.6, -2.3),
               (1.0, -1.3, -1.05), (0.42, -1.6, -2.3), None, None),
    "lean":   ((-1.25, -1.3, -0.8), (-0.85, -1.35, -1.7),
               (1.65, -3.1, -0.3), (1.4, -4.25, -0.45), "R", "up"),
}


def glass(sc, hand, u, how, full=1.0):
    """A pint pot of beer in a man's hand, catching the lamp."""
    x, y, z = hand
    if how == "up":
        y0, y1 = y - 0.8 * u, y + 0.15 * u
    elif how == "mouth":
        y0, y1 = y - 0.55 * u, y + 0.35 * u
    else:
        y0, y1 = y - 0.75 * u, y + 0.1 * u
    sc.cylinder(x, y0, y1, 0.2 * u, z - 0.12 * u, BEER, spec=1.1, shine=40)
    if full:
        sc.ellipsoid((x, y0, z - 0.12 * u), (0.2 * u, 0.07 * u, 0.2 * u),
                     (236, 228, 206), spec=0.3)


def head(b, hc, Rh, skin, hair, face=True, moustache=False, grey=False):
    """His head about its centre 'hc' (local), tipped by Rh."""
    def hp(p):
        return np.asarray(hc, np.float32) + Rh @ np.asarray(p, np.float32)
    b.blob(hp((0, 0, 0)), (0.4, 0.5, 0.44), skin, R=Rh, rim=0.6, spec=0.06)
    # Hair over the crown and the back, cut short above the nape and the
    # ears, brilliantined flat; grey at the sides for an older man.
    b.blob(hp((0, -0.1, 0.065)), (0.44, 0.5, 0.46), hair, R=Rh, rim=0.8,
           spec=0.25, shine=20, clip_y=0.42)
    if grey:
        for side in (-1, 1):
            b.blob(hp((side * 0.36, -0.02, 0.1)), (0.1, 0.22, 0.25),
                   (150, 144, 132), R=Rh, rim=0.6)
    for side in (-1, 1):
        b.blob(hp((side * 0.41, 0.03, 0.04)), (0.08, 0.15, 0.08),
               np.array(skin) * 0.92, R=Rh, rim=0.9)
    if face:
        for side in (-1, 1):
            b.blob(hp((side * 0.15, -0.03, -0.395)), (0.06, 0.04, 0.04),
                   (52, 36, 28), R=Rh)
        b.blob(hp((0, 0.09, -0.45)), (0.065, 0.13, 0.08), skin, R=Rh,
               spec=0.2)                                         # the nose
        if moustache:
            b.blob(hp((0, 0.22, -0.4)), (0.17, 0.05, 0.06),
                   (150, 144, 132) if grey else hair, R=Rh)


def officer(sc, x, floor_y, z, s, yaw, pose, rng, seated=True, bow=0.0,
            turn=0.0, lift=0.0, moustache=None, grey=False, chair=True,
            tunic=None, skin=None, hair=None, full=1.0):
    """One officer, his feet on the floor at (x, floor_y) and depth z."""
    u = U * s
    hip_y = floor_y - (1.9 if seated else 4.0) * u
    b = Body(sc, (x, hip_y, z), s, yaw)
    tunic = tunic or TUNICS[rng.integers(len(TUNICS))]
    tunic = np.array(tunic) * rng.normal(1, 0.04)
    breeches = BREECHES[rng.integers(len(BREECHES))]
    skin = skin or SKINS[rng.integers(len(SKINS))]
    hair = hair or HAIR[rng.integers(len(HAIR))]
    if moustache is None:
        moustache = rng.random() < 0.5
    rim = 0.5
    # Legs: breeches flaring at the thigh, then polished riding boots.
    for side in (-1, 1):
        hipj = (side * 0.38, 0.05, 0)
        if seated:
            knee, ankle = (side * 0.45, 0.05, -1.85), (side * 0.42, 1.8, -1.95)
        else:
            knee, ankle = (side * 0.36, 1.95, -0.05), (side * 0.34, 3.7, 0)
        b.limb(hipj, knee, 0.38, breeches, rim=rim)
        if not seated:
            b.blob((side * 0.5, 0.68, 0), (0.42, 0.58, 0.43), breeches,
                   rim=rim)                     # the flare of the breeches
        b.limb(knee, ankle, 0.28, BOOTS, rim=rim, spec=0.7, shine=24)
        if not seated:
            b.blob((knee[0], knee[1] + 0.15, knee[2]), (0.33, 0.2, 0.33),
                   BOOTS, rim=rim, spec=0.7, shine=24)  # the boot tops
        b.blob((ankle[0], ankle[1] + 0.12, ankle[2] - 0.25),
               (0.17, 0.12, 0.38), BOOTS, rim=rim, spec=0.7, shine=24)
    if seated and chair:
        bentwood(b)
    # The tunic: skirts, waist and chest in one run of cloth, the
    # shoulders rounding it off; a man is broader than a tailor's dummy.
    b.blob((0, -0.02, 0.02), (0.84, 0.55, 0.62), tunic * 0.92, rim=rim,
           texture=folds(int(rng.integers(1000))))
    b.blob((0, -0.75, 0), (0.72, 0.8, 0.52), tunic, rim=rim)
    b.blob((0, -1.65, 0), (0.84, 0.82, 0.56), tunic, rim=rim,
           texture=folds(int(rng.integers(1000))))
    b.blob((0, -1.98, 0.02), (1.08, 0.36, 0.5), tunic, rim=rim)  # the yoke
    # The Sam Browne: the waist belt and the strap over the right shoulder.
    b.blob((0, -0.42, 0), (0.76, 0.13, 0.56), LEATHER, rim=rim, spec=0.6,
           shine=20)
    b.limb((0.6, -2.2, -0.5), (-0.68, -0.5, -0.58), 0.07, LEATHER,
           spec=0.6, shine=20)
    b.blob((0, -2.45, 0.0), (0.4, 0.15, 0.36), tunic * 0.9, rim=rim)
    b.blob((0, -2.42, -0.24), (0.13, 0.16, 0.12), (168, 150, 108))  # shirt
    b.blob((0, -2.35, -0.3), (0.06, 0.12, 0.05), (90, 78, 52))      # tie
    # The head on its neck: bowed, turned, or tipped back.
    Rh = rot_y(turn) @ rot_x(bow - lift)
    neck = np.array((0, -2.72, 0), np.float32)
    hc = neck + Rh @ np.array((0, -0.6, 0), np.float32)
    b.blob(neck + Rh @ np.array((0, -0.08, 0.04), np.float32),
           (0.22, 0.3, 0.22), skin, rim=0.5)
    head(b, hc, Rh, skin, hair, face=True, moustache=moustache, grey=grey)
    # Arms, hands and whatever they hold.
    le, lh, re, rh, holds, how = POSES[pose]
    for side, e, h in ((-1, le, lh), (1, re, rh)):
        sh = (side * 0.98, -1.98, 0.02)
        b.limb(sh, e, 0.25, tunic, rim=rim)
        b.blob(e, (0.23, 0.23, 0.23), tunic * 0.97, rim=rim)
        b.limb(e, h, 0.21, tunic * 0.97, rim=rim)
        b.blob(h, (0.16, 0.18, 0.14), skin, rim=0.7, spec=0.1)
        if holds == ("L" if side < 0 else "R"):
            glass(sc, b.at(h), u, how, full)
    return b


def bentwood(b, cap=False):
    """A bentwood chair under (or behind) a seated man."""
    b.blob((0, 0.22, -0.05), (0.85, 0.1, 0.8), CHAIR, spec=0.4)
    for sx in (-1, 1):
        for sz in (-1, 1):
            b.limb((sx * 0.65, 0.3, sz * 0.62), (sx * 0.75, 1.9, sz * 0.72),
                   0.07, CHAIR, spec=0.4)
        b.limb((sx * 0.62, 0.2, 0.7), (sx * 0.55, -1.8, 0.88), 0.08, CHAIR,
               spec=0.5, shine=20)
    b.blob((0, -1.75, 0.88), (0.62, 0.12, 0.1), CHAIR, spec=0.5, shine=20)
    b.blob((0, -1.05, 0.84), (0.55, 0.07, 0.08), CHAIR, spec=0.5, shine=20)
    if cap:
        # His cap hung on the chair's back: khaki crown, the dark band, the
        # glazed peak and the badge catching the light.
        Rc = rot_z(0.35) @ rot_x(-0.25)
        c = np.array((0.42, -1.92, 0.95), np.float32)

        def cp(p):
            return c + Rc @ np.asarray(p, np.float32)
        b.blob(cp((0, -0.08, 0)), (0.56, 0.15, 0.54), (122, 110, 74),
               R=Rc, rim=0.7)
        b.blob(cp((0, 0.06, 0)), (0.47, 0.13, 0.45), (66, 58, 42), R=Rc,
               rim=0.6)
        b.blob(cp((0, 0.14, -0.42)), (0.4, 0.045, 0.3), (40, 30, 22), R=Rc,
               spec=1.0, shine=30)
        b.blob(cp((0, -0.02, -0.5)), (0.09, 0.09, 0.04), (210, 186, 120),
               R=Rc, spec=1.2, shine=30)


def dog(sc, x, y, z, s, coat=(212, 200, 178), patch=(120, 78, 44)):
    """A terrier asleep, curled nose to tail.  (x, y) is the middle of his
    back as he lies on the floor."""
    k = s
    def tex(q, n):
        return np.where(np.sin(q[..., 0] * 3 + 1.3) + q[..., 2] * 1.4 > 0.6,
                        np.mean(patch) / np.mean(coat), 1.0)
    sc.ellipsoid((x, y, z), (15 * k, 6.5 * k, 9 * k), coat, rim=0.6,
                 texture=tex)
    sc.ellipsoid((x - 9 * k, y + 1 * k, z - 4 * k), (6.5 * k, 5 * k, 5 * k),
                 coat, R=rot_z(-0.3), rim=0.6)             # haunch
    sc.ellipsoid((x + 10 * k, y + 2.5 * k, z - 6 * k), (6 * k, 4.2 * k, 5 * k),
                 coat, R=rot_z(0.2), rim=0.6)               # his head, down
    sc.ellipsoid((x + 15 * k, y + 4 * k, z - 7 * k), (3 * k, 2.4 * k, 2.6 * k),
                 coat, rim=0.5)                            # the muzzle
    sc.ellipsoid((x + 17.5 * k, y + 3.6 * k, z - 7.5 * k),
                 (1.0 * k, 0.9 * k, 0.9 * k), (24, 20, 18), spec=0.8)
    sc.ellipsoid((x + 8 * k, y - 1 * k, z - 6.5 * k), (3 * k, 2 * k, 1.5 * k),
                 patch, R=rot_z(0.5), rim=0.6)              # an ear, flopped
    sc.ellipsoid((x + 4 * k, y + 4.5 * k, z - 8 * k), (5 * k, 1.6 * k, 2 * k),
                 coat, R=rot_z(0.15), rim=0.5)              # the tail round
