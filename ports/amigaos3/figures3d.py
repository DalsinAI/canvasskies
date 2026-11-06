"""Seated soldiers seen from behind, built from lit solids (scene3d.py).

Each man is his own: a greatcoat over sloping shoulders with folds down
the back, the turned-up collar, the nape of his neck and his ears, his
head turned or tilted a little, and a soft trench cap, a steel helmet,
an Australian slouch hat or his own hair.  Size, slump, tilt and colour
vary from man to man.
"""

import math

import numpy as np

from scene3d import fbm, rot_x, rot_y, rot_z

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
