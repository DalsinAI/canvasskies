#!/usr/bin/env python3
"""Paint the enemy's riflemen, who fire up at our machines from the ground.

German infantry in field grey with black leather belt, pouches and boots,
each with his long bolt-action rifle: in the 1914 set he wears the spiked
helmet under its grey cloth cover, in the 1916 set the deep steel helmet
with its flared skirt.  Side on, facing left toward an aeroplane above
him, he crouches, rises onto one knee, aims kneeling, aims standing, and
lies where he fell; from directly above he lies prone and aims at each of
the eight points of the compass, or lies sprawled.  Every man is built of
lit solids (scene3d.py), the light from the upper left as in the rest of
the side-on art, and filtered down with hard edges against the magenta.
Writes assets/canvas-skies/riflemen.bmp and src/modern/canvas_riflemen.h
(each sprite's rectangle and the point of its rifle's muzzle).
"""

import math
import sys
from pathlib import Path

import numpy as np
from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parent))
from paint import KEY  # noqa: E402
from scene3d import Scene, cone, dither565, rot_z  # noqa: E402
import scene3d  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
ATLAS = ROOT / "assets" / "canvas-skies" / "riflemen.bmp"
HEADER = ROOT / "src" / "modern" / "canvas_riflemen.h"

K = 4                       # each man is built at four times his final size
SIDE_W, SIDE_H = 22, 30
TOP_W, TOP_H = 16, 16
ATLAS_W = 229               # ten side-on cells to a row, with gutters

GREY = (110, 112, 92)       # field grey tunic
TROUSER = (100, 102, 84)
LEATHER = (30, 28, 26)
BOOT = (26, 24, 22)
SKIN = (192, 148, 118)
COVER = (150, 144, 112)     # the cloth cover over the spiked helmet
STEEL_HELM = (70, 74, 64)
COVER_TOP = (168, 160, 122)    # the cover seen from above, in full sun
WOOD = (138, 86, 46)
STEEL = (46, 48, 52)
SHADOW = (40, 38, 30)

NEAR, MID, FAR = 46.0, 50.0, 54.0


class Man:
    """A scene at K times size, drawn in final-size pixel coordinates."""

    def __init__(self, w, h):
        self.w, self.h = w, h
        self.sc = Scene(w * K, h * K, ambient=(0.30, 0.29, 0.28))
        self.sc.direction((-0.6, -0.7, -0.5), (1.15, 1.05, 0.92))
        self.sc.direction((0.5, -0.2, 0.6), (0.32, 0.30, 0.28))
        self.lines = []      # (p0, p1, colour) the rifle, kept unbroken
        self.bars = []       # (p0, p1, colour) drawn over all, last

    def ball(self, p, radii, col, R=None, clip_y=None, spec=0.1, rim=0.4):
        self.sc.ellipsoid(tuple(v * K for v in p), tuple(v * K for v in radii),
                          col, R=R, clip_y=clip_y, spec=spec, rim=rim)

    def limb(self, p0, p1, r0, r1, col, spec=0.05, rim=0.4, caps=True):
        """A tapering round limb from p0 to p1 (x, y, z), rounded at both
        ends."""
        a, b = np.array(p0, float), np.array(p1, float)
        d = b - a
        L = np.linalg.norm(d)
        if L < 1e-3:
            self.ball(p0, (r0, r0, r0), col)
            return
        cone(self.sc, tuple((a + b) / 2 * K), tuple(d), L / 2 * K, r0 * K,
             r1 * K, col, spec=spec, rim=rim)
        if caps:
            self.ball(p0, (r0, r0, r0), col, spec=spec, rim=rim)
            self.ball(p1, (r1, r1, r1), col, spec=spec, rim=rim)

    def finish(self, outline=True, shadow=False):
        """Filter down to final size: the colour of each pixel is the mean
        of the solid beneath it, never of the background, so no edge pixel
        leans toward the magenta; the shaded edges are darkened for a firm
        silhouette, and the rifle is made good wherever it ran thin."""
        s = K * scene3d.SS
        drawn = np.isfinite(self.sc.z)
        x = 1.0 - np.exp(-self.sc.rgb * 1.6)
        x = np.where(drawn[..., None], x, 0)
        h, w = self.h, self.w
        cov = drawn.reshape(h, s, w, s).mean((1, 3))
        col = x.reshape(h, s, w, s, 3).sum((1, 3)) / \
            np.maximum(drawn.reshape(h, s, w, s).sum((1, 3)), 1)[..., None]
        img = np.asarray(dither565(Image.fromarray(
            np.clip(col * 255 + 0.5, 0, 255).astype(np.uint8))), np.float32)
        hit = cov > 0.42
        for p0, p1, c in self.lines:
            n = int(max(abs(p1[0] - p0[0]), abs(p1[1] - p0[1])) * 4) + 1
            for t in np.linspace(0, 1, n):
                px = int(p0[0] + (p1[0] - p0[0]) * t)
                py = int(p0[1] + (p1[1] - p0[1]) * t)
                if 0 <= px < w and 0 <= py < h and not hit[py, px]:
                    hit[py, px] = True
                    img[py, px] = c
        if outline:
            pad = np.pad(hit, 1)
            below = ~pad[2:, 1:-1]
            right = ~pad[1:-1, 2:]
            above = ~pad[:-2, 1:-1]
            left = ~pad[1:-1, :-2]
            img[hit & (below | right)] *= 0.62 if outline is True else outline
            img[hit & ~(below | right) & (above | left)] *= 0.9
        for p0, p1, c in self.bars:
            n = int(max(abs(p1[0] - p0[0]), abs(p1[1] - p0[1])) * 4) + 1
            for t in np.linspace(0, 1, n):
                px = int(p0[0] + (p1[0] - p0[0]) * t)
                py = int(p0[1] + (p1[1] - p0[1]) * t)
                if 0 <= px < w and 0 <= py < h:
                    hit[py, px] = True
                    img[py, px] = c
        out = np.full((h, w, 3), KEY, np.uint8)
        if shadow:
            sh = np.zeros_like(hit)
            sh[1:, 1:] = hit[:-1, :-1]
            out[sh & ~hit] = SHADOW
        out[hit] = np.clip(img[hit], 0, 255).astype(np.uint8)
        # Nothing drawn may be mistaken for the key.
        m = hit & (out[..., 0] > 200) & (out[..., 2] > 200) & (out[..., 1] < 60)
        out[m] = (200, 60, 200)
        return Image.fromarray(out)


def rot2(v, a):
    """Turn (x, y) clockwise on the screen by a radians."""
    c, s = math.cos(a), math.sin(a)
    return (c * v[0] - s * v[1], s * v[0] + c * v[1])


# ---- side on -----------------------------------------------------------------
def helmet_side(m, head, tilt, kind):
    """The head and its helmet, facing left; 'tilt' turns it clockwise."""
    hx, hy, hz = head
    m.ball(head, (1.6, 1.65, 1.6), SKIN, rim=0.3)

    def at(lx, ly):
        ox, oy = rot2((lx, ly), tilt)
        return (hx + ox, hy + oy, hz)
    R = rot_z(tilt)
    if kind == 1914:
        # A tall rounded dome, the spike's bump on its crown, the black
        # peak over the eyes and the short neck guard behind.
        m.ball(at(0.2, -1.0), (1.75, 2.05, 1.7), COVER, R=R, clip_y=0.42,
               rim=0.5)
        m.ball(at(0.2, -3.1), (0.55, 1.0, 0.55), COVER, R=R, rim=0.4)
        m.ball(at(-1.55, -0.1), (1.05, 0.38, 1.3), LEATHER,
               R=R @ rot_z(0.4), spec=0.3)                  # the front peak
        m.ball(at(1.55, -0.05), (0.9, 0.38, 1.3), COVER,
               R=R @ rot_z(0.45))                            # the neck guard
    else:
        # A deep dome coming down over the brow, the skirt flaring out
        # and down over the ears and the back of the neck.
        m.ball(at(0.3, -0.75), (2.15, 2.1, 2.05), STEEL_HELM, R=R,
               clip_y=0.42, spec=0.25, rim=0.5)
        m.ball(at(1.15, 0.55), (2.0, 0.55, 2.1), STEEL_HELM,
               R=R @ rot_z(0.62), spec=0.2, rim=0.5)         # the flared skirt
        m.ball(at(-1.7, 0.1), (0.8, 0.35, 1.5), STEEL_HELM,
               R=R @ rot_z(0.15), spec=0.2)                  # the short brim
        m.ball(at(0.45, -0.55), (0.38, 0.38, 0.3),
               (104, 108, 94), spec=0.4)                     # the side lug


def rifle(m, butt, muzzle, z):
    """The rifle from butt to muzzle: stock, fore-end, barrel, bolt."""
    b, mz = np.array(butt, float), np.array(muzzle, float)
    u = (mz - b) / np.linalg.norm(mz - b)
    L = np.linalg.norm(mz - b)
    perp = np.array([u[1], -u[0]])
    if perp[1] > 0:
        perp = -perp                         # the side toward the sky
    P = lambda t, o=0.0: (b[0] + u[0] * t + perp[0] * o,         # noqa: E731
                          b[1] + u[1] * t + perp[1] * o, z)
    m.limb(P(0.3, -0.15), P(4.2, 0.1), 0.85, 0.42, WOOD, spec=0.2,
           caps=False)                                     # the butt stock
    m.ball(P(0.35, -0.2), (0.4, 0.95, 0.5), (90, 58, 34),
           R=rot_z(math.atan2(u[1], u[0])))                 # the butt plate
    m.limb(P(4.0, 0.1), P(L * 0.78, 0.1), 0.45, 0.4, WOOD, spec=0.2)
    m.limb(P(4.0, 0.35), P(L, 0.35), 0.36, 0.32, STEEL, spec=0.5)
    m.ball(P(4.6, 0.75), (0.32, 0.32, 0.32), STEEL, spec=0.5)  # the bolt
    m.lines.append(((b[0] + perp[0] * 0.2, b[1] + perp[1] * 0.2),
                    (mz[0] + perp[0] * 0.3, mz[1] + perp[1] * 0.3), STEEL))


def body_side(m, j, kind):
    """A man from his joints.  Near limbs (his left side) toward the
    viewer, far limbs behind and a shade darker."""
    dark = lambda c, f=0.8: tuple(v * f for v in c)          # noqa: E731
    z = lambda p, zz: (p[0], p[1], zz)                       # noqa: E731
    # The far leg and arm.
    m.limb(z(j["hip"], FAR - 1), z(j["kf"], FAR), 1.5, 1.2, dark(TROUSER))
    m.limb(z(j["kf"], FAR), z(j["af"], FAR), 1.15, 1.0, dark(TROUSER))
    m.limb(z(j["af"], FAR), z(j["ff"], FAR), 1.0, 0.85, BOOT)
    m.ball(z(j["tf"], FAR), (0.95, 0.7, 0.8), BOOT)
    m.limb(z(j["sh"], FAR - 1), z(j["ef"], FAR), 0.95, 0.85, dark(GREY))
    m.limb(z(j["ef"], FAR), z(j["hf"], FAR), 0.85, 0.65, dark(GREY))
    m.ball(z(j["hf"], FAR - 0.5), (0.65, 0.65, 0.65), dark(SKIN))
    # The body: hips, chest, belt with its pouches, the collar.
    hip, sh = np.array(j["hip"]), np.array(j["sh"])
    m.limb(z(hip, MID), z(sh + (sh - hip) * -0.12, MID), 2.0, 2.25, GREY,
           rim=0.5)
    w = hip + (sh - hip) * 0.22
    m.limb(z(w - (sh - hip) * 0.05, MID - 0.2), z(w + (sh - hip) * 0.06, MID - 0.2),
           2.1, 2.18, LEATHER, spec=0.3, caps=False)
    fwd = np.array(j.get("fwd", (-1.0, 0.0)))
    for k in (0.0, 1.0):
        p = w + fwd * (1.5 + k * 0.1) + (sh - hip) * (0.05 + 0.1 * k) + \
            np.array([k * 0.7, 0])
        m.ball((p[0], p[1], MID - 1.6 - k), (0.9, 0.7, 0.7), LEATHER,
               spec=0.3)
    m.ball(z(sh + (sh - hip) * 0.02, MID), (1.2, 0.9, 1.1), dark(GREY, 0.9))
    helmet_side(m, (j["head"][0], j["head"][1], MID - 0.3), j["tilt"], kind)
    # The rifle, then the near leg and arm over it.
    rifle(m, j["butt"], j["muzzle"], j.get("rifle_z", NEAR + 1.4))
    m.limb(z(j["hip"], NEAR + 1.5), z(j["kn"], NEAR), 1.5, 1.2, TROUSER)
    m.limb(z(j["kn"], NEAR), z(j["an"], NEAR), 1.15, 1.0, TROUSER)
    m.limb(z(j["an"], NEAR), z(j["fn"], NEAR), 1.0, 0.85, BOOT)
    m.ball(z(j["tn"], NEAR), (0.95, 0.7, 0.8), BOOT)
    m.limb(z(j["sh"], NEAR + 1.5), z(j["en"], NEAR - 1), 0.95, 0.85, GREY)
    m.limb(z(j["en"], NEAR - 1), z(j["hn"], NEAR - 1), 0.85, 0.65, GREY)
    m.ball(z(j["hn"], NEAR - 1.3), (0.65, 0.65, 0.65), SKIN)


def aim(sh_pocket, deg, L=14.0):
    """Butt, muzzle, cheek and the two hands for a rifle shouldered at
    'sh_pocket' and aimed up to the left at 'deg' above the horizontal."""
    a = math.radians(deg)
    u = np.array([-math.cos(a), -math.sin(a)])
    up = np.array([math.sin(a), -math.cos(a)])          # above the line
    b = np.array(sh_pocket)
    return dict(butt=tuple(b), muzzle=tuple(b + u * L),
                head=tuple(b + u * 2.4 + up * 1.8),
                hf=tuple(b + u * 2.6 - up * 0.55),
                hn=tuple(b + u * 7.0 - up * 0.55))


def leg(hip, knee, foot_heel, toe_dir):
    """Ankle and toe of a leg whose heel is at foot_heel."""
    f = np.array(foot_heel)
    t = np.array(toe_dir) / np.linalg.norm(toe_dir)
    return tuple(f + (np.array(knee) - f) * 0.35), tuple(f), tuple(f + t * 0.9)


def side_poses():
    P = {}
    # CROUCH: squatting low, head down, the rifle clutched upright across him.
    a = math.radians(62)
    butt = (15.2, 28.6)
    mz = (butt[0] - math.cos(a) * 14, butt[1] - math.sin(a) * 14)
    u = ((mz[0] - butt[0]) / 14, (mz[1] - butt[1]) / 14)
    P["CROUCH"] = dict(
        hip=(14.6, 25.0), sh=(11.0, 20.2), head=(8.6, 20.4), tilt=-0.75,
        kn=(8.2, 23.6), kf=(10.0, 23.2), fn=(10.0, 28.6), ff=(13.4, 28.6),
        tn=(9.0, 28.8), tf=(12.4, 28.8),
        en=(9.4, 24.0), ef=(11.4, 24.4), fwd=(-0.6, 0.6),
        hn=(butt[0] + u[0] * 8.0, butt[1] + u[1] * 8.0),
        hf=(butt[0] + u[0] * 5.0, butt[1] + u[1] * 5.0),
        butt=butt, muzzle=mz, rifle_z=NEAR - 0.5)
    # RISE: rising onto one knee, the rifle coming up to 20 degrees.
    a = math.radians(20)
    butt = (15.0, 21.8)
    mz = (butt[0] - math.cos(a) * 14, butt[1] - math.sin(a) * 14)
    P["RISE"] = dict(
        hip=(15.6, 24.2), sh=(12.0, 18.6), head=(9.8, 16.4), tilt=-0.35,
        kn=(10.6, 23.4), kf=(14.2, 28.2), fn=(10.0, 28.6), ff=(18.6, 28.2),
        tn=(9.0, 28.8), tf=(19.4, 28.9),
        en=(11.6, 22.2), ef=(14.4, 20.6), fwd=(-0.9, 0.3),
        hn=(butt[0] - math.cos(a) * 7.2, butt[1] - math.sin(a) * 7.2 + 0.4),
        hf=(butt[0] - math.cos(a) * 2.6, butt[1] - math.sin(a) * 2.6 + 0.5),
        butt=butt, muzzle=mz)
    # KNEEL_AIM: on the right knee, the rifle up at 40 degrees.
    k = aim((14.4, 17.6), 40)
    P["KNEEL_AIM"] = dict(
        hip=(16.6, 23.8), sh=(15.0, 17.4), tilt=-0.15,
        kn=(11.0, 23.0), kf=(13.2, 28.2), fn=(9.8, 28.6), ff=(18.4, 28.0),
        tn=(8.8, 28.8), tf=(19.0, 28.9),
        en=(10.6, 16.6), ef=(16.6, 19.2), **k)
    # STAND_AIM: on his feet, the rifle up at 50 degrees.
    k = aim((11.8, 12.4), 50)
    P["STAND_AIM"] = dict(
        hip=(12.6, 18.6), sh=(12.4, 12.2), tilt=0.1,
        kn=(9.8, 23.4), kf=(14.2, 23.6), fn=(8.6, 28.0), ff=(15.2, 28.0),
        tn=(7.4, 28.6), tf=(14.2, 28.6),
        en=(8.6, 12.6), ef=(14.4, 13.6), **k)
    return P


def side_fallen(m, kind):
    """Lying where he fell, face down, the rifle on the ground beside him."""
    mz, butt = (3.0, 28.2), (16.8, 28.9)
    m.limb((16.0, 27.0, FAR), (19.6, 27.4, FAR), 1.0, 0.85, (80, 82, 67))
    m.limb((19.6, 27.4, FAR), (21.0, 26.6, FAR), 0.75, 0.7, BOOT)
    m.limb((6.2, 26.6, FAR), (6.0, 24.6, FAR), 0.75, 0.6, (88, 90, 74))
    m.limb((6.0, 24.6, FAR), (3.6, 23.6, FAR), 0.6, 0.5, (88, 90, 74))
    m.limb((6.4, 27.2, MID), (13.2, 27.4, MID), 1.75, 1.55, GREY, rim=0.5)
    m.limb((11.6, 27.3, MID - 0.2), (12.4, 27.3, MID - 0.2), 1.62, 1.62,
           LEATHER, caps=False)
    m.ball((10.6, 28.5, MID - 1.4), (0.7, 0.5, 0.6), LEATHER)
    helmet_side(m, (3.6, 27.3, MID - 0.3), -1.45, kind)
    rifle(m, butt, mz, NEAR)
    m.limb((13.0, 27.6, NEAR + 1), (16.6, 26.4, NEAR + 0.5), 1.2, 1.0, TROUSER)
    m.limb((16.6, 26.4, NEAR + 0.5), (19.8, 28.4, NEAR), 0.95, 0.8, TROUSER)
    m.limb((19.8, 28.4, NEAR), (21.2, 28.6, NEAR), 0.8, 0.7, BOOT)
    m.limb((7.0, 27.6, NEAR + 1), (8.6, 28.6, NEAR - 0.5), 0.75, 0.65, GREY)
    m.limb((8.6, 28.6, NEAR - 0.5), (11.2, 28.6, NEAR - 0.5), 0.65, 0.55, GREY)
    m.ball((11.6, 28.6, NEAR - 0.8), (0.55, 0.5, 0.5), SKIN)
    return mz


def side_frames(kind):
    out = []
    poses = side_poses()
    for name in ("CROUCH", "RISE", "KNEEL_AIM", "STAND_AIM"):
        j = poses[name]
        for key in ("n", "f"):
            j["a" + key], _, _ = leg(j["hip"], j["k" + key], j["f" + key],
                                     (-1, 0.15))
        m = Man(SIDE_W, SIDE_H)
        body_side(m, j, kind)
        out.append((name, m.finish(), j["muzzle"]))
    m = Man(SIDE_W, SIDE_H)
    mz = side_fallen(m, kind)
    out.append(("FALLEN", m.finish(), mz))
    return out


# ---- from above ----------------------------------------------------------------
G = 60.0                    # the ground's depth below the eye


def top_man(m, a, kind, fallen=False):
    """A man seen from directly above, lying prone and aiming along the
    direction 'a' (clockwise from up the screen); or sprawled where he
    fell.  Everything is laid out pointing up, then turned."""
    c = np.array([8.0, 8.0])
    P = lambda x, y, hgt: (*(c + rot2((x - 8.0, y - 8.0), a)), G - hgt)  # noqa: E731
    R = rot_z(a)
    dark = (80, 82, 67)
    if fallen:
        # On his back, arms flung out, one knee drawn up, the rifle apart.
        m.limb(P(7.2, 10.8, 1.0), P(6.6, 15.0, 0.9), 1.0, 0.85, TROUSER)
        m.limb(P(8.8, 10.8, 1.0), P(11.4, 13.0, 1.6), 1.0, 0.9, TROUSER)
        m.limb(P(11.4, 13.0, 1.6), P(11.2, 15.0, 0.8), 0.9, 0.8, TROUSER)
        m.limb(P(6.6, 14.2, 0.9), P(6.5, 15.4, 0.8), 0.85, 0.8, BOOT)
        m.limb(P(11.2, 14.4, 0.8), P(11.0, 15.4, 0.8), 0.85, 0.8, BOOT)
        m.ball(P(8.0, 9.0, 1.3), (2.3, 3.0, 1.3), GREY, R=R, rim=0.5)
        m.ball(P(8.0, 10.8, 1.5), (2.25, 0.5, 1.3), LEATHER, R=R)
        for sx in (6.6, 9.4):
            m.ball(P(sx, 10.0, 2.2), (0.75, 0.6, 0.5), LEATHER, R=R)
        m.ball(P(8.0, 6.6, 1.4), (2.9, 1.4, 1.3), GREY, R=R, rim=0.5)
        m.limb(P(5.4, 6.6, 1.2), P(2.6, 5.4, 0.8), 0.75, 0.6, GREY)
        m.limb(P(2.6, 5.4, 0.8), P(1.6, 3.4, 0.6), 0.6, 0.55, GREY)
        m.limb(P(10.6, 6.6, 1.2), P(13.2, 8.0, 0.8), 0.75, 0.6, GREY)
        m.limb(P(13.2, 8.0, 0.8), P(14.4, 10.2, 0.6), 0.6, 0.55, GREY)
        for hx, hy in ((1.4, 3.0), (14.6, 10.6)):
            m.ball(P(hx, hy, 0.6), (0.55, 0.55, 0.5), SKIN)
        top_helmet(m, P, R, kind, 8.0, 4.4, face_up=True)
        butt, mz = P(13.6, 3.6, 0.5), P(4.8, -1.0, 0.5)
        if not (0 <= mz[0] < 16 and 0 <= mz[1] < 16):
            mz = P(5.4, 0.2, 0.5)
        top_rifle(m, butt, mz)
        return mz
    # Prone: the legs spread behind, the boots.
    m.limb(P(7.0, 11.6, 1.0), P(5.4, 14.4, 0.9), 1.15, 0.95, dark)
    m.limb(P(9.0, 11.6, 1.0), P(10.6, 14.4, 0.9), 1.15, 0.95, dark)
    m.limb(P(5.4, 14.0, 0.9), P(5.1, 15.1, 0.7), 0.95, 0.8, BOOT)
    m.limb(P(10.6, 14.0, 0.9), P(10.9, 15.1, 0.7), 0.95, 0.8, BOOT)
    # The back, the belt across it, the shoulders.
    m.ball(P(8.0, 10.0, 1.4), (2.4, 2.7, 1.4), GREY, R=R, rim=0.5)
    m.ball(P(8.0, 11.5, 1.6), (2.3, 0.55, 1.3), LEATHER, R=R)
    m.ball(P(8.0, 8.0, 1.7), (3.1, 1.5, 1.5), GREY, R=R, rim=0.5)
    # The rifle: butt in the right shoulder, the muzzle well out in front.
    butt, mz = P(9.9, 8.2, 2.6), P(9.2, 0.4, 2.4)
    top_rifle(m, butt, mz)
    # The arms forward to the rifle: left elbow out on the ground, the
    # left hand under the fore-end, the right at the wrist of the stock.
    m.limb(P(5.4, 7.8, 1.6), P(5.0, 5.0, 1.2), 0.95, 0.8, GREY)
    m.limb(P(5.0, 5.0, 1.2), P(8.6, 3.6, 2.0), 0.8, 0.6, GREY)
    m.ball(P(9.1, 3.5, 2.8), (0.65, 0.65, 0.5), SKIN)
    m.limb(P(10.8, 7.8, 1.6), P(11.6, 6.4, 1.6), 0.95, 0.8, GREY)
    m.limb(P(11.6, 6.4, 1.6), P(10.2, 5.4, 2.4), 0.8, 0.6, GREY)
    m.ball(P(9.8, 5.5, 3.0), (0.6, 0.6, 0.5), SKIN)
    top_helmet(m, P, R, kind, 6.9, 6.0)
    return mz


def top_helmet(m, P, R, kind, x, y, face_up=False):
    """The helmet from above: the pale cloth cover of the spiked helmet,
    long from peak to neck guard, with the bump of its spike; or the steel
    helmet's broad dark rim round its dome."""
    if kind == 1914:
        m.ball(P(x, y + 1.8, 3.0), (1.5, 1.0, 0.4), COVER, R=R)   # neck guard
        m.ball(P(x, y - 1.9, 3.0), (1.45, 0.95, 0.4), LEATHER, R=R,
               spec=0.3)                                         # the peak
        m.ball(P(x, y, 3.6), (2.0, 2.2, 1.6), COVER_TOP, R=R, rim=0.3)
        m.ball(P(x, y, 5.3), (0.65, 0.65, 0.5), (190, 182, 146), R=R)
    else:
        m.ball(P(x, y + 0.35, 3.0), (2.6, 2.75, 0.5), (50, 54, 46), R=R,
               spec=0.2, rim=0.6)                                # the skirt
        m.ball(P(x, y, 4.0), (1.7, 1.8, 1.5), (100, 104, 92), R=R,
               spec=0.35, rim=0.4)
        for s in (-1, 1):
            m.ball(P(x + s * 1.55, y + 0.1, 3.8), (0.3, 0.3, 0.3),
                   (120, 124, 110), spec=0.4)                    # the lugs


def top_rifle(m, butt, mz):
    b, e = np.array(butt), np.array(mz)
    d = e - b
    m.limb(tuple(b), tuple(b + d * 0.35), 0.62, 0.42, WOOD, spec=0.2)
    m.limb(tuple(b + d * 0.33), tuple(b + d * 0.8), 0.42, 0.38, WOOD,
           spec=0.2)
    m.limb(tuple(b + d * 0.4 - (0, 0, 0.3)), tuple(e - (0, 0, 0.3)), 0.33,
           0.3, STEEL, spec=0.5)
    m.lines.append(((b[0], b[1]), (e[0], e[1]), STEEL))
    f = b + d * 0.62
    m.bars.append(((f[0], f[1]), (e[0], e[1]), STEEL))


def top_frames(kind):
    out = []
    for k in range(8):
        m = Man(TOP_W, TOP_H)
        mz = top_man(m, k * math.pi / 4, kind)
        out.append((f"AIM_{k}", m.finish(outline=0.8, shadow=True), mz))
    m = Man(TOP_W, TOP_H)
    mz = top_man(m, math.radians(200), kind, fallen=True)
    out.append(("FALLEN", m.finish(outline=0.8, shadow=True), mz))
    return out


# ---- the atlas -----------------------------------------------------------------
def main():
    sprites = []
    for kind, frames in ((1914, side_frames(1914)), (1916, side_frames(1916))):
        sprites += [(f"{kind} side {n.lower()}", img, mz) for n, img, mz in frames]
    for kind in (1914, 1916):
        sprites += [(f"{kind} top {n.lower()}", img, mz)
                    for n, img, mz in top_frames(kind)]
    places, x, y, row_h = [], 0, 0, 0
    for name, img, mz in sprites:
        if x + img.width > ATLAS_W:
            x, y, row_h = 0, y + row_h + 1, 0
        places.append((name, x, y, img.width, img.height, mz))
        x += img.width + 1
        row_h = max(row_h, img.height)
    atlas = Image.new("RGB", (ATLAS_W, y + row_h), KEY)
    for (name, px, py, *_), (_, img, _) in zip(places, sprites):
        atlas.paste(img, (px, py))
    atlas.save(ATLAS)
    out = ["/* Generated by ports/amigaos3/make_riflemen.py: do not edit. */",
           "#ifndef CANVAS_RIFLEMEN_H", "#define CANVAS_RIFLEMEN_H", "",
           "#define RF_SIDE_FRAMES 5", "#define RF_TOP_FRAMES 9", "",
           "typedef enum {", "    RF_SIDE_1914 = 0,", "    RF_SIDE_1916 = 5,",
           "    RF_TOP_1914 = 10,", "    RF_TOP_1916 = 19,",
           "    RF_SPRITE_COUNT = 28", "} RiflemanSprite;", "",
           "static const short rifleman_rect[RF_SPRITE_COUNT][4] = {"]
    out += [f"    {{{px}, {py}, {w}, {h}}}, /* {name} */"
            for name, px, py, w, h, _ in places]
    out += ["};", "", "/* The rifle's muzzle in each cell, from its top left. */",
            "static const signed char rifleman_muzzle[RF_SPRITE_COUNT][2] = {"]
    for name, px, py, w, h, mz in places:
        mx = min(w - 1, max(0, int(round(mz[0]))))
        my = min(h - 1, max(0, int(round(mz[1]))))
        out.append(f"    {{{mx}, {my}}}, /* {name} */")
    out += ["};", "", "#endif", ""]
    HEADER.write_text("\n".join(out))


if __name__ == "__main__":
    main()
