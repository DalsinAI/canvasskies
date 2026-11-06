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

from scene3d import axis_frame, cone, fbm, rot_x, rot_y, rot_z

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
          (84, 78, 56), (90, 80, 52)]
OFFICER_HAIR = [(40, 30, 22), (56, 40, 26), (28, 22, 18), (70, 50, 30),
                (34, 26, 22)]
SHIRT = (172, 156, 114)
BREECHES = [(138, 118, 84), (128, 112, 80), (146, 126, 90), (112, 102, 72)]
PUTTEES = (110, 98, 68)
BOOTS = (74, 42, 24)
LEATHER = (108, 64, 34)
BRASS = (196, 156, 82)
CHAIR = (62, 40, 24)
BEER = (214, 150, 58)
U = 13.0


def frame(d, up=(0, -1, 0)):
    """A rotation whose local z runs along d and whose local y leans
    toward 'up' (the flat of a palm, the face of a hand)."""
    z = np.asarray(d, np.float32)
    z = z / max(1e-6, float(np.linalg.norm(z)))
    y = np.asarray(up, np.float32) - np.dot(up, z) * z
    if np.linalg.norm(y) < 1e-3:
        y = np.array((0, 0, -1), np.float32) - z[2] * z
    y = y / np.linalg.norm(y)
    x = np.cross(y, z)
    return np.stack([x, y, z], 1).astype(np.float32)


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

    def taper(self, a, b, r0, r1, col, **kw):
        """A sleeve or a boot leg: a cone from a (radius r0) to b (r1)."""
        a, b = self.at(a), self.at(b)
        cone(self.sc, tuple((a + b) / 2), b - a,
             float(np.linalg.norm(b - a)) / 2, r0 * self.u, r1 * self.u, col,
             **kw)


# ---- what a man looks like ---------------------------------------------------
def _soft(d):
    return np.exp(-d * d)


def face_tex(skin, hair, look):
    """Paints a head: skin with its warmth and shade, eye sockets, brows,
    the shadow under the nose, the mouth, a moustache if he has one, and
    his hair with its hairline, parting and comb marks.  q is the head's
    own frame in radii, front at -z."""
    skin = np.array(skin, np.float32) / 255
    hair = np.array(hair, np.float32) / 255
    grey = np.array((0.58, 0.56, 0.52), np.float32)
    # Where the hair stops, by the angle round the head from the front:
    # forehead, the temples a little higher, the sideburn down in front
    # of the ear, over the ear, and the nape at the back.
    az_pts = [0.0, 0.5, 0.85, 1.15, 1.28, 1.42, 1.62, 1.95, 2.5, math.pi]
    hl_pts = [-0.46, -0.44, -0.56, -0.3, 0.22, 0.2, -0.12, -0.05, 0.4, 0.52]
    if look.get("bald"):
        hl_pts = [-0.86, -0.82, -0.7, -0.3, 0.2, 0.18, -0.12, -0.05, 0.4,
                  0.52]

    def tex(q, n):
        x, y, z = q[..., 0], q[..., 1], q[..., 2]
        out = np.broadcast_to(skin, q.shape).copy()
        front = np.clip(-z / np.maximum(np.hypot(x, z), 1e-3), 0, 1)
        f2 = front ** 2
        # Ruddy cheeks and nose, the jaw a shade darker with his shave.
        warm = (_soft((np.abs(x) - 0.48) / 0.24) * _soft((y - 0.15) / 0.22)
                + 0.6 * _soft(x / 0.12) * _soft((y - 0.12) / 0.15)) * f2
        out *= 1 + warm[..., None] * np.array([0.12, -0.03, -0.06])
        jaw = np.clip((y - 0.45) / 0.4, 0, 1) * front
        out *= 1 - (jaw * look.get("stubble", 0.1))[..., None]
        for sx in (-1, 1):
            ex = x - sx * 0.36
            sock = _soft(ex / 0.22) * _soft((y + 0.04) / 0.12) * f2
            out *= (1 - 0.5 * sock)[..., None]
            eye = _soft(ex / 0.1) * _soft((y + 0.03) / 0.06) * f2
            out = out * (1 - 0.7 * eye[..., None]) + \
                eye[..., None] * 0.7 * np.array([0.2, 0.15, 0.12])
            brow = _soft((y + 0.21 + 0.06 * ex * sx) / 0.05) * \
                _soft(ex / 0.2) * f2
            out = out * (1 - 0.7 * brow[..., None]) + \
                brow[..., None] * 0.7 * hair * 0.8
        # The shadow under the nose, the lips and the cleft of the mouth.
        under = _soft(x / 0.13) * _soft((y - 0.28) / 0.06) * f2
        out *= (1 - 0.35 * under)[..., None]
        smile = look.get("smile", 0.0)
        my = y - (0.5 - smile * 0.25 * x * x)
        lip = _soft(x / 0.22) * _soft(my / 0.08) * f2
        out *= 1 - lip[..., None] * np.array([0.12, 0.3, 0.28])
        cleft = _soft(x / 0.24) * _soft(my / 0.035) * f2
        out *= (1 - 0.55 * cleft)[..., None]
        if look.get("open"):                    # singing, or a toast
            o = _soft(x / 0.13) * _soft((y - 0.52) / 0.07) * f2
            out *= (1 - 0.8 * o)[..., None]
        m = look.get("moustache")
        if m:
            w, h = {"full": (0.3, 0.07), "brush": (0.13, 0.06),
                    "waxed": (0.42, 0.045)}[m]
            mx = np.abs(x)
            up = 0.06 * (mx / w) ** 2 if m == "waxed" else 0
            must = (mx < w) & (np.abs(y - 0.39 + up) < h) & (front > 0.5)
            col = grey if look.get("grey") else hair
            out[must] = col * 0.85
        # The hair.
        az = np.abs(np.arctan2(x, -z))
        hl = np.interp(az, az_pts, hl_pts)
        part = look.get("part", 0.35)
        comb = 0.85 + 0.15 * np.sin((x - part) * 30 + y * 6)
        hm = y < hl
        hc = np.broadcast_to(hair, q.shape)
        if look.get("grey"):
            side = np.clip((az - 0.9) / 0.5, 0, 1) * np.clip((2.4 - az) / 0.5,
                                                              0, 1)
            hc = hc * (1 - side[..., None]) + grey * side[..., None]
        partline = (np.abs(x - part) < 0.05) & (y < -0.55) & (z < 0.3)
        hcol = hc * comb[..., None]
        hcol = np.where(partline[..., None], hcol * 0.55, hcol)
        edge = np.clip((hl - y) / 0.08, 0, 1)[..., None]   # a soft hairline
        out = np.where(hm[..., None], out * (1 - edge) + hcol * edge, out)
        return np.clip(out, 0, 1)
    return tex


def tunic_tex(colour, part, look, seed):
    """Paints the service dress over one piece of the body: the open
    collar with its lapels, shirt and tie; the pleated breast pockets
    with their flaps; four brass buttons; the wings over the left breast;
    the big skirt pockets; creases; the seam down the back.  'wrap' is
    the double-breasted R.F.C. jacket, buttoned to the side and up to a
    closed collar; 'shirt' is a man in his shirt-sleeves."""
    base = np.array(colour, np.float32) / 255
    shirt = np.array(SHIRT, np.float32) / 255
    tie = np.array((0.36, 0.3, 0.2), np.float32)
    brass = np.array(BRASS, np.float32) / 255
    style = look.get("style", "sd")
    rng = np.random.default_rng(seed)
    ph = rng.uniform(0, 6)

    def tex(q, n):
        x, y, z = q[..., 0], q[..., 1], q[..., 2]
        out = np.broadcast_to(base, q.shape).copy()
        front = np.clip(-z, 0, 1)
        fr = front > 0.35
        back = z > 0.25
        # Creases: soft diagonal folds pulling from the arms and the belt.
        crease = np.sin((x * 3 + y * 2 * np.sign(x) + ph) * 2.2) ** 8
        out *= (1 - 0.12 * crease)[..., None]
        out[back & (np.abs(x) < 0.04)] *= 0.7            # the back seam

        def line(m, k=0.55):
            out[m] *= k

        def dot(cx, cy, r, col):
            m = (((x - cx) ** 2 + (y - cy) ** 2) < r * r) & fr
            out[m] = col
        if part == "chest":
            if style == "wrap":
                edge = x - (-0.15 + 0.5 * (y + 1))
                line(fr & (np.abs(edge) < 0.04) & (y > -0.95))
                for by in (-0.6, -0.1, 0.4):
                    dot(-0.15 + 0.5 * (by + 1) + 0.1, by, 0.07, brass)
            else:
                v = (y + 1.0) / 0.8                       # 0 at the collar
                w = 0.34 * (1 - v)
                inside = fr & (v < 1) & (np.abs(x) < w)
                out[inside] = shirt
                out[inside & (np.abs(x) < 0.07 + 0.02 * v)] = tie
                lap = fr & (v < 1.05) & (np.abs(x) >= w) & (np.abs(x) < w + 0.16)
                out[lap] *= 1.12                          # the lapels
                line(fr & (v < 1.05) & (np.abs(np.abs(x) - w - 0.16) < 0.035),
                     0.4)
                line(fr & (v < 1) & (np.abs(np.abs(x) - w) < 0.025), 0.45)
                if style != "shirt":
                    for by in (0.0, 0.5):
                        dot(0, by, 0.065, brass)
            if style != "wrap":
                for sx in (-1, 1):
                    px = x - sx * 0.5
                    box = fr & (np.abs(px) < 0.22) & (y > -0.42) & (y < 0.2)
                    line(box & ((np.abs(np.abs(px) - 0.22) < 0.03) |
                                (np.abs(y - 0.2) < 0.03)), 0.6)
                    line(box & (np.abs(px) < 0.025), 0.75)  # its pleat
                    flap = fr & (np.abs(px) < 0.24) & (y > -0.44) & (y < -0.28)
                    out[flap] *= 1.08
                    line(fr & (np.abs(px) < 0.24) & (np.abs(y + 0.27) < 0.03))
                    if style != "shirt":
                        dot(sx * 0.5, -0.33, 0.05, brass)
            if look.get("wings") and style != "shirt":
                wx, wy = x - 0.48, y + 0.6
                m = fr & (np.abs(wx) < 0.3) & (np.abs(wy) < 0.07 - 0.12 *
                                                np.abs(wx) ** 2)
                out[m] = np.array((0.78, 0.74, 0.62))
                dot(0.48, -0.6, 0.06, np.array((0.5, 0.42, 0.3)))
        elif part == "belly":
            if style != "wrap":
                dot(0, -0.45, 0.07, brass)
                if style != "shirt":
                    dot(0, 0.1, 0.07, brass)
            else:
                dot(0.42, -0.4, 0.07, brass)
        elif part == "skirt":
            out *= (1 + 0.3 * np.clip(y, 0, 1))[..., None]
        if part == "skirt" and style != "shirt":
            for sx in (-1, 1):
                px = x - sx * 0.52
                line(fr & (np.abs(px) < 0.3) & (np.abs(y + 0.35) < 0.05), 0.5)
                out[fr & (np.abs(px) < 0.3) & (y > -0.55) & (y < -0.36)] *= 1.1
                line(fr & (np.abs(np.abs(px) - 0.3) < 0.035) & (y > -0.35), 0.65)
                dot(sx * 0.52, -0.43, 0.06, brass)
            if style == "wrap":
                line(fr & (np.abs(x - 0.42) < 0.04), 0.6)
        return np.clip(out, 0, 1)
    return tex


def sleeve_tex(colour, lace):
    """A sleeve from elbow to wrist, rank lace round the cuff near the
    wrist (z runs -1 at the elbow to 1 at the wrist)."""
    base = np.array(colour, np.float32) / 255
    braid = np.array((0.6, 0.52, 0.36), np.float32)

    def tex(z, ang):
        out = np.broadcast_to(base, z.shape + (3,)).copy()
        out *= (0.92 + 0.08 * np.sin(ang * 3 + z * 4))[..., None]
        out[z > 0.78] *= 0.88                              # the cuff
        for k in range(lace):
            out[np.abs(z - (0.62 - k * 0.1)) < 0.03] = braid
        return out
    return tex


def puttee_tex(z, ang):
    """Puttees: the khaki strip wound spirally up the shin."""
    band = np.sin((z * 9 + ang * 0.6) * math.pi)
    return 0.8 + 0.2 * (band > -0.3)


# ---- hands --------------------------------------------------------------------
def glass(sc, c, u, how, full=1.0):
    """A pint pot of beer centred at c, catching the lamp.  Returns the
    rows of its top and bottom."""
    x, y, z = c
    h = 0.72 * u
    y0, y1 = y - h * 0.55, y + h * 0.45
    if how == "table":
        y0, y1 = y - h * 0.75, y + h * 0.25
    sc.cylinder(x, y0, y1, 0.18 * u, z, BEER if full else (150, 136, 112),
                spec=1.1, shine=40,
                texture=lambda a, v: 0.85 + 0.25 * np.clip(a, 0, 1) ** 4)
    if full:
        sc.ellipsoid((x, y0, z), (0.18 * u, 0.07 * u, 0.18 * u),
                     (236, 228, 206), spec=0.3)
    sc.ellipsoid((x, y1, z), (0.18 * u, 0.04 * u, 0.18 * u),
                 (150, 120, 70), spec=0.8)                 # its heavy base
    return y0, y1


def hand(sc, wrist, d, u, skin, kind, side, how=None, full=1.0, toward=None):
    """A hand at the wrist, the forearm running along d (world).
    kind: 'grip' (round a glass), 'flat' (laid on the table), 'open'
    (talking), 'fist'.  'side' is which of his hands, +1 or -1; 'toward'
    is the world direction of his own front, so the palm turns in."""
    w = np.asarray(wrist, np.float32)
    d = np.asarray(d, np.float32)
    d = d / max(1e-6, float(np.linalg.norm(d)))
    skin = np.array(skin, np.float32)
    knuckle = skin * 0.92
    if kind == "grip":
        c = w + d * 0.26 * u
        y0, y1 = glass(sc, (c[0], c[1], c[2] + 0.02 * u), u, how, full)
        gy = c[1] + (0.05 if how != "table" else -0.05) * u
        # The palm and the heel of the hand round the far side of the
        # glass, the fingers wrapped across its front, the thumb over.
        sc.ellipsoid((c[0] - side * 0.06 * u, gy, c[2] + 0.06 * u),
                     (0.21 * u, 0.2 * u, 0.13 * u), skin, rim=0.6)
        fz = c[2] - 0.19 * u
        for k in range(4):
            fy = gy - 0.12 * u + k * 0.085 * u
            sc.ellipsoid((c[0] + side * 0.03 * u, fy, fz),
                         (0.16 * u - k * 0.01 * u, 0.04 * u, 0.06 * u),
                         knuckle if k % 2 else skin, rim=0.5, spec=0.1)
        sc.ellipsoid((c[0] - side * 0.15 * u, gy - 0.13 * u, c[2] - 0.12 * u),
                     (0.06 * u, 0.12 * u, 0.06 * u), skin,
                     R=rot_z(side * 0.3), rim=0.5)
        return
    if kind == "flat":
        c = w + d * 0.3 * u
        R = frame(d, (0, -1, 0))

        def fingers(q, n):
            return np.where((q[..., 2] > 0.25) &
                            (np.abs(np.sin(q[..., 0] * 7.5)) < 0.25), 0.7, 1.0)
        sc.ellipsoid(tuple(c), (0.2 * u, 0.08 * u, 0.33 * u), skin, R=R,
                     rim=0.6, texture=fingers)
        sc.ellipsoid(tuple(c + R[:, 0] * side * -0.2 * u - d * 0.06 * u),
                     (0.06 * u, 0.06 * u, 0.15 * u), skin, R=R, rim=0.5)
        return
    if kind == "open":
        c = w + d * 0.3 * u
        R = frame(d, toward if toward is not None else (0, 0, -1))

        def fingers(q, n):
            return np.where((q[..., 2] > 0.2) &
                            (np.abs(np.sin(q[..., 0] * 7.5)) < 0.3), 0.72, 1.0)
        sc.ellipsoid(tuple(c), (0.2 * u, 0.08 * u, 0.32 * u), skin, R=R,
                     rim=0.6, texture=fingers)
        sc.ellipsoid(tuple(c + R[:, 0] * side * -0.22 * u - d * 0.05 * u),
                     (0.06 * u, 0.06 * u, 0.16 * u), skin,
                     R=frame(d + R[:, 0] * side * -0.8), rim=0.5)
        return
    c = w + d * 0.2 * u                                   # a fist
    sc.ellipsoid(tuple(c), (0.19 * u, 0.2 * u, 0.17 * u), skin, rim=0.6)
    for k in range(4):
        sc.ellipsoid((c[0] - 0.12 * u + k * 0.08 * u, c[1] + 0.02 * u,
                      c[2] - 0.15 * u), (0.045 * u, 0.06 * u, 0.04 * u),
                     knuckle, rim=0.4)


# Where the elbows and hands go, per pose: (left elbow, left wrist, right
# elbow, right wrist) in his own frame, and what each hand does: 'flat',
# 'grip:table', 'grip:up', 'grip:mouth', 'open', 'fist', or None for a
# hand out of sight.  For the seated poses the table top runs back from
# about 1.3u above the hips at its far edge; a hand laid on it a forearm
# in front of him shows lower down than that, on the cloth.
L_FLAT = ((-1.2, -1.05, -0.75), (-0.62, -0.95, -1.35), "flat")
R_FLAT = ((1.2, -1.05, -0.75), (0.62, -0.95, -1.35), "flat")
POSES = {
    "rest":   (L_FLAT, R_FLAT),
    "glass":  (L_FLAT, ((1.25, -1.05, -0.8), (0.8, -1.05, -1.45),
                        "grip:table")),
    "raise":  (L_FLAT, ((1.62, -2.95, -0.35), (1.42, -3.95, -0.55),
                        "grip:up")),
    "cheer":  (((-1.6, -2.95, -0.35), (-1.55, -3.95, -0.6), "open"),
               ((1.62, -2.95, -0.35), (1.42, -3.95, -0.55), "grip:up")),
    "drink":  (L_FLAT, ((1.3, -1.75, -0.8), (0.42, -2.55, -0.95),
                        "grip:mouth")),
    "talk":   (L_FLAT, ((1.35, -1.45, -0.85), (1.15, -2.3, -1.35), "open")),
    "pipe":   (L_FLAT, ((1.3, -1.6, -0.8), (0.45, -2.45, -0.9), "pipe")),
    "chin":   (((-1.0, -1.05, -0.95), (-0.22, -2.3, -0.95), "fist"),
               ((1.15, -1.05, -0.8), (0.55, -1.0, -1.35), "grip:table")),
    "clasp":  (((-1.15, -1.05, -0.8), (-0.3, -1.0, -1.25), "fist"),
               ((1.15, -1.05, -0.8), (0.12, -1.02, -1.35), "flat")),
    "head":   (((-1.25, -1.05, -0.95), (-0.32, -2.55, -0.8), "fist"),
               R_FLAT),
    "behind": (((-1.1, -1.0, 0.35), (-0.3, -0.45, 0.62), None),
               ((1.1, -1.0, 0.35), (0.3, -0.45, 0.62), None)),
    "sglass": (((-1.25, -1.1, -0.05), (-0.6, -0.45, -0.35), "hip"),
               ((1.2, -1.05, -0.45), (0.62, -1.6, -1.0), "grip:up")),
    "sraise": (((-1.25, -1.1, -0.05), (-0.6, -0.45, -0.35), "hip"),
               ((1.62, -3.0, -0.25), (1.45, -4.05, -0.45), "grip:up")),
    "shang":  (((-1.12, -0.85, 0.05), (-1.08, 0.2, -0.12), "fist"),
               ((1.62, -3.0, -0.25), (1.45, -4.05, -0.45), "grip:up")),
    "piano":  (((-1.1, -1.15, -0.9), (-0.5, -1.3, -1.75), "flat"),
               ((1.1, -1.15, -0.9), (0.6, -1.3, -1.75), "flat")),
    "warm":   (((-1.0, -1.3, -1.05), (-0.42, -1.6, -2.2), "open"),
               ((1.0, -1.3, -1.05), (0.42, -1.6, -2.2), "open")),
    "lean":   (((-1.25, -1.3, -0.8), (-0.85, -1.35, -1.6), "flat"),
               ((1.62, -3.0, -0.3), (1.45, -4.05, -0.5), "grip:up")),
}


def head(b, hc, Rh, skin, hair, look):
    """His head about its centre 'hc' (local), tipped by Rh: the skull
    painted with his face and hair, the jaw and chin, nose and ears."""
    def hp(p):
        return np.asarray(hc, np.float32) + Rh @ np.asarray(p, np.float32)
    k = look.get("head", 1.0)
    jaw = look.get("jaw", 1.0)
    b.blob(hp((0, 0, 0)), (0.4 * k * jaw, 0.52 * k, 0.45 * k), (255, 255, 255),
           R=Rh, rim=0.12, spec=0.08, texture=face_tex(skin, hair, look))
    # The hair has some body to it over the crown, so it still shows as
    # hair when he tips his head back to the lamp.
    b.blob(hp((0, -0.13 * k, 0.07 * k)), (0.43 * k * jaw, 0.5 * k, 0.46 * k),
           np.array(hair) * 1.05, R=Rh, rim=0.15, spec=0.3, shine=20,
           clip_y=-0.3, texture=hair_tex(int(look.get("part", 0) * 100)))
    b.blob(hp((0, 0.09 * k, -0.43 * k)), (0.07 * k, 0.15 * k, 0.09 * k),
           np.array(skin) * np.array((1.04, 0.97, 0.95)), R=Rh @ rot_x(0.25),
           spec=0.25, shine=20, rim=0.3)                     # the nose
    for side in (-1, 1):
        b.blob(hp((side * 0.4 * k, 0.03 * k, 0.04 * k)),
               (0.07 * k, 0.15 * k, 0.09 * k), np.array(skin) * 0.9,
               R=Rh @ rot_y(side * 0.4), rim=0.9)
    if look.get("pipe"):
        b.blob(hp((0.08 * k, 0.52 * k, -0.7 * k)), (0.03 * k, 0.03 * k,
               0.22 * k), (40, 28, 20), R=Rh, spec=0.6)
        b.blob(hp((0.1 * k, 0.48 * k, -0.95 * k)), (0.1 * k, 0.12 * k,
               0.1 * k), (78, 48, 28), R=Rh, spec=0.7, shine=24)


def officer(sc, x, floor_y, z, s, yaw, pose, rng, seated=True, bow=0.0,
            turn=0.0, lift=0.0, moustache=None, grey=False, chair=True,
            tunic=None, skin=None, hair=None, full=1.0, style=None,
            build=None, puttees=None, bald=False, sing=False, lace=None):
    """One officer, his feet on the floor at (x, floor_y) and depth z.
    No two are alike: his build (broad or slight, a paunch on the older
    men), the cut of his jacket, his face, moustache and hair, and
    whether he wears boots or puttees are drawn from rng unless given."""
    u = U * s
    hip_y = floor_y - (1.9 if seated else 4.0) * u
    b = Body(sc, (x, hip_y, z), s, yaw)
    tunic = tunic or TUNICS[rng.integers(len(TUNICS))]
    tunic = np.array(tunic) * rng.normal(1, 0.04)
    breeches = BREECHES[rng.integers(len(BREECHES))]
    skin = skin or SKINS[rng.integers(len(SKINS))]
    hair = hair or OFFICER_HAIR[rng.integers(len(OFFICER_HAIR))]
    style = style or rng.choice(["sd", "sd", "sd", "wrap"])
    bw = build if build is not None else float(np.clip(rng.normal(1, 0.07),
                                                       0.86, 1.14))
    paunch = 0.07 if grey else max(0.0, rng.normal(0.02, 0.04))
    if puttees is None:
        puttees = not grey and rng.random() < 0.3
    if moustache is None:
        moustache = rng.choice([None, "brush", "full", "waxed"],
                               p=[0.4, 0.25, 0.25, 0.1])
    elif moustache is True:
        moustache = "full"
    look = dict(moustache=moustache, grey=grey, bald=bald,
                part=float(rng.choice([-0.35, 0.35, 0.3])),
                smile=float(rng.uniform(-0.2, 0.6)) + (1.0 if sing else 0),
                open=sing, stubble=float(rng.uniform(0.04, 0.16)),
                head=float(rng.normal(1, 0.035)),
                jaw=float(rng.normal(1, 0.08)),
                style=style, wings=style != "shirt" and rng.random() < 0.85,
                pipe=pose == "pipe")
    lace = lace if lace is not None else int(rng.integers(1, 3))
    cloth = tunic if style != "shirt" else np.array(SHIRT) * rng.normal(1, 0.03)
    rim = 0.5
    sd = int(rng.integers(1000))
    # Legs: breeches flaring at the thigh, then polished riding boots or
    # puttees wound over ankle boots.
    for side in (-1, 1):
        hipj = (side * 0.38 * bw, 0.05, 0)
        if seated:
            knee, ankle = (side * 0.45, 0.05, -1.85), (side * 0.42, 1.8, -1.95)
        else:
            kz = -0.12 if side > 0 else 0.0              # weight on one leg
            knee = (side * 0.36, 1.95, kz)
            ankle = (side * (0.34 + 0.06 * (side > 0)), 3.7, 0)
        b.limb(hipj, knee, 0.38, breeches, rim=rim, texture=folds(sd + side))
        if not seated:
            b.blob((side * 0.5 * bw, 0.72, 0), (0.39, 0.55, 0.41), breeches,
                   rim=rim, texture=folds(sd + 4 + side))  # the flare
        if puttees:
            b.taper(knee, ankle, 0.27, 0.21, PUTTEES, rim=rim,
                    texture=puttee_tex)
            b.blob((ankle[0], ankle[1] + 0.1, ankle[2] - 0.15),
                   (0.2, 0.15, 0.4), (60, 40, 26), rim=rim, spec=0.5)
        else:
            b.taper(knee, ankle, 0.31, 0.24, BOOTS, rim=rim, spec=0.8,
                    shine=26)
            if not seated:
                b.blob((knee[0], knee[1] + 0.12, knee[2]), (0.34, 0.17, 0.34),
                       BOOTS, rim=rim, spec=0.8, shine=26)  # the boot tops
            b.blob((ankle[0], ankle[1] + 0.12, ankle[2] - 0.25),
                   (0.18, 0.13, 0.4), BOOTS, rim=rim, spec=0.8, shine=26)
    if seated and chair:
        bentwood(b)
    # The jacket: skirts, waist and chest in one run of cloth, the yoke of
    # the shoulders over it, each piece painted with its pockets.
    b.blob((0, 0.08, -paunch * 0.5), ((0.86 + paunch) * bw, 0.64,
           0.54 + paunch * 0.5),
           (255, 255, 255), rim=rim,
           texture=tunic_tex(cloth * 0.95, "skirt", look, sd + 1))
    b.blob((0, -0.8, -paunch * 0.5), ((0.8 + paunch) * bw, 0.88,
           0.55 + paunch), (255, 255, 255), rim=rim,
           texture=tunic_tex(cloth, "belly", look, sd + 2))
    b.blob((0, -1.65, 0), (0.84 * bw, 0.82, 0.56), (255, 255, 255), rim=rim,
           texture=tunic_tex(cloth, "chest", look, sd + 3))
    b.blob((0, -1.98, 0.04), (1.0 * bw, 0.34, 0.48), cloth, rim=rim,
           texture=folds(sd + 5))                            # the yoke
    for side in (-1, 1):                             # shoulder straps
        b.limb((side * 0.35 * bw, -2.27, 0.02), (side * 0.85 * bw, -2.12, 0.02),
               0.07, cloth * 1.05, rim=rim)
    # The collar round the neck.
    if style == "wrap":
        b.blob((0, -2.45, 0.0), (0.38, 0.17, 0.36), cloth * 0.95, rim=rim)
    else:
        b.blob((0, -2.44, 0.04), (0.4, 0.15, 0.37), cloth * 0.95, rim=rim)
        b.blob((0, -2.5, -0.08), (0.27, 0.12, 0.3), np.array(SHIRT) * 0.95,
               rim=rim)
    # The Sam Browne: the waist belt with its brass buckle and the strap
    # over the right shoulder; not every man has kept his on.
    if style != "shirt" and (grey or rng.random() < 0.75):
        b.blob((0, -0.42, -paunch * 0.4), ((0.77 + paunch) * bw, 0.12,
               0.57 + paunch), LEATHER, rim=rim, spec=0.7, shine=22)
        b.blob((0, -0.42, -0.6 - paunch * 1.2), (0.1, 0.1, 0.04), BRASS,
               spec=1.2, shine=30)
        b.limb((0.58 * bw, -2.15, -0.5), (-0.62 * bw, -0.5, -0.6 - paunch),
               0.065, LEATHER, spec=0.7, shine=22)
        b.limb((0.58 * bw, -2.15, 0.48), (-0.62 * bw, -0.5, 0.6), 0.065,
               LEATHER, spec=0.5, shine=22)                 # across the back
    # The head on its neck: bowed, turned, or tipped back.
    Rh = rot_y(turn) @ rot_x(bow - lift)
    neck = np.array((0, -2.66, 0), np.float32)
    hc = neck + Rh @ np.array((0, -0.56, 0), np.float32)
    b.blob(neck + Rh @ np.array((0, -0.08, 0.04), np.float32),
           (0.25, 0.3, 0.25), np.array(skin) * 0.9, rim=0.5)
    head(b, hc, Rh, skin, hair, look)
    # Arms in their sleeves, the cuffs with his rank, and the hands.
    toward = b.Ry @ np.array((0, 0, -1), np.float32)
    for side, (e, h, kind) in ((-1, POSES[pose][0]), (1, POSES[pose][1])):
        sh = (side * 0.98 * bw, -1.98, 0.02)
        e = (e[0] * (0.6 + 0.4 * bw), e[1], e[2])
        if kind == "hip":
            # A hand on his hip, the elbow out.
            e = (side * 1.45 * bw, -1.15, 0.05)
            h = (side * 0.95 * bw, -0.5, -0.1)
        b.blob(sh, (0.28, 0.28, 0.28), cloth, rim=rim)
        b.taper(sh, e, 0.3, 0.25, cloth, rim=rim)
        b.blob(e, (0.25, 0.25, 0.25), cloth * 0.96, rim=rim)
        b.taper(e, h, 0.25, 0.2, (255, 255, 255), rim=rim,
                texture=sleeve_tex(cloth * 0.96, 0 if style == "shirt"
                                   else lace))
        if kind is None:
            continue
        we, wh = b.at(e), b.at(h)
        d = wh - we
        if kind.startswith("grip"):
            hand(sc, wh, d, u, skin, "grip", side, kind.split(":")[1], full)
        elif kind == "hip":
            hand(sc, wh, d, u, skin, "fist", side)
        elif kind == "pipe":
            hand(sc, wh, d, u, skin, "fist", side)
        else:
            hand(sc, wh, d, u, skin, kind, side, toward=toward)
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
