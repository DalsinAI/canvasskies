#!/usr/bin/env python3
"""Render the squadron mess at night, in three moods.

Built and lit with the small renderer in scene3d.py, the officers made
and posed with the rigged figure kit (figurekit.py).  We sit at the near end of a table whose
edge runs across the foot of the picture, with somebody's flying helmet
and goggles on a map, his tankard beside them; the evening's lines are
typed over the dark boards of that table.  Beyond it the long mess table
under one hanging oil lamp, its cloth, bottles and glasses, the officers
seated round it face-on at the far side and back-on at the near; the
Squadron Commander standing at the head of it; the stove glowing in the
corner with a man in a chair beside it and the squadron's dog asleep on
the boards; the piano and its gramophone against the back wall, the
propeller and a trophy cross above them; bunting under the eaves; and
over all of it the tobacco smoke, lit gold round the lamp.  Each man is
told by the set of his head and shoulders and by his hands, since a face
here is a dozen pixels.

The squadron board hangs on the back wall at the left, its slate left
clean: the game chalks the heading and the tally on it at run time, so
keep BOARD in step with draw_mess() in src/modern/canvas_skies.c.

Three evenings, one picture each:

  quiet   mess.bmp        an ordinary good evening: drinks, talk, the
                          piano played softly, the CO with a glass.
  rowdy   mess-rowdy.bmp  after a victory: the lamp turned up and candles
                          lit in bottles, glasses raised all round, a man
                          on the table making a speech, the piano going
                          and a singer at it.
  wake    mess-wake.bmp   after a loss: the lamp turned low, fewer men,
                          heads down, an empty chair pushed back with his
                          cap on the seat and his glass full on the
                          table, the CO alone at the board, the piano shut.

Rendered at four times the size (so that a face of a dozen pixels still
has its brow, eyes, nose and mouth in it), filtered down and dithered to
RGB565.  The men are drawn in a scene of their own and laid into the
room (lamplit_men): lit as a painter would light them, mostly dark
shapes with a warm rim from the lamp and the stove, their edges softened
a touch, under the room's smoke; pipe and cigarette smoke drifts up
through the lamp's glow and the corners of the room fall into shadow.
Each evening takes about a minute and a half and some 850 MB.
Writes assets/canvas-skies/mess.bmp, mess-rowdy.bmp and mess-wake.bmp.

  make_mess.py [quiet|rowdy|wake ...] [--preview DIR]

With --preview, also writes DIR/mess-<mood>.png: the picture with the
evening's lines and the tally laid over it as draw_mess() lays them, in
the game's own glyph atlas, for looking at on the host.
"""

import argparse
import math
import re
import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw

sys.path.insert(0, str(Path(__file__).resolve().parent))
from figurekit import Figure  # noqa: E402
from figures3d import Body, bentwood, dog  # noqa: E402
from scene3d import Scene, fbm, normalize, planks, rot_z  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"
OUT = {"quiet": "mess.bmp", "rowdy": "mess-rowdy.bmp",
       "wake": "mess-wake.bmp"}
W, H = 640, 400
SS = 4                              # samples a side: the men need them
BOARD = (52, 40, 222, 160)          # the frame; the slate is 7 inside it
LAMP = (372, 92)
TABLE = (286, 518)                  # the long table's ends
TOP = (219, 236)                    # its top, far edge to near edge
NEAR = 300                          # the edge of our own table
TEXT = (30, 300, 520, 394)          # where the evening's lines are typed

# How each evening is lit: the lamp's colour and reach, the stove's, the
# smoke, the exposure.
MOODS = {
    "quiet": dict(lamp=(1.9, 1.45, 0.9), reach=220, stove=(1.3, 0.6, 0.22),
                  smoke=1.0, exposure=1.0, seed=1),
    "rowdy": dict(lamp=(2.4, 1.85, 1.15), reach=250, stove=(1.4, 0.66, 0.24),
                  smoke=1.7, exposure=1.08, seed=2),
    "wake":  dict(lamp=(1.05, 0.7, 0.36), reach=180, stove=(1.25, 0.52, 0.18),
                  smoke=0.8, exposure=0.95, seed=3),
}


def floor_y(z):
    """Where the floor is on screen at depth z: the boards meet the back
    wall at y 252 and come toward us a quarter of a pixel per unit."""
    return 252 + 0.25 * (520 - z)


def mask(pts):
    m = Image.new("L", (W * SS, H * SS), 0)
    ImageDraw.Draw(m).polygon([(x * SS, y * SS) for x, y in pts], fill=255)
    return np.asarray(m) > 127


def rect(x0, y0, x1, y1):
    return mask([(x0, y0), (x1, y0), (x1, y1), (x0, y1)])


def boards(seed, **kw):
    """Planks the size of the picture (scene3d.planks)."""
    return planks(H * SS, W * SS, seed, ss=SS, **kw)


def yramp(y0, y1, z0, z1):
    """A depth that runs from z0 at screen row y0 to z1 at row y1."""
    yy = np.arange(H * SS, dtype=np.float32)[:, None] / SS
    t = np.clip((yy - y0) / max(1e-6, y1 - y0), 0, 1)
    return np.broadcast_to(z0 + (z1 - z0) * t, (H * SS, W * SS))


# ---- the room ----------------------------------------------------------------
def room(sc):
    a, n, _ = boards(71, base=(116, 84, 56), widths=(18, 32),
                     soot_top=0.55)
    sc.surface(np.ones((H * SS, W * SS), bool), 520, a, normals=n)
    floor, _, _ = boards(72, base=(80, 58, 38), widths=(5, 8),
                         vertical=False, nails=False)
    yy = np.arange(H * SS, dtype=np.float32)[:, None] / SS
    fz = np.broadcast_to(np.maximum(520 - (yy - 252) * 4, 300),
                         (H * SS, W * SS))
    sc.surface(rect(0, 252, W, H), fz, floor * 0.75, normal=(0, -0.9, -0.4))
    # The skirting where the wall meets the boards.
    sc.surface(rect(0, 247, W, 253), 516, (64, 46, 30), normal=(0, -0.3, -1))
    # The eaves beam and the bunting strung beneath it.
    beam, _, _ = boards(73, base=(100, 70, 44),
                        widths=(400, 401), vertical=False)
    sc.surface(rect(0, 0, W, 9), 500, beam, normal=(0, 0.5, -1))
    cols = [(176, 40, 34), (226, 220, 204), (40, 60, 128)]
    cloth = 0.85 + 0.15 * fbm(H * SS, W * SS, 74, 3, 60)
    for k in range(22):
        x = k * 30 - 4
        sag = 7 * math.sin(math.pi * ((x % 320) / 320))
        m = mask([(x, 11 + sag), (x + 22, 11 + sag), (x + 11, 29 + sag)])
        alb = np.array(cols[k % 3], np.float32)[None, None, :] / 255.0 * \
            cloth[..., None]
        sc.surface(m, 495 - k * 0.01, alb, normal=(0.1 * math.sin(k), 0.3, -1))
    sc.surface(mask([(0, 10), (640, 10), (640, 12), (0, 12)]), 494,
               (60, 50, 38))


def board(sc):
    x0, y0, x1, y1 = BOARD
    wood, _, _ = boards(75, base=(120, 80, 46),
                        widths=(400, 401), vertical=False)
    yy, xx = np.mgrid[0:H * SS, 0:W * SS].astype(np.float32) / SS
    n = np.zeros((H * SS, W * SS, 3), np.float32)
    n[..., 2] = -1
    n[..., 0] += np.where(xx < x0 + 3, -0.7, np.where(xx > x1 - 3, 0.7, 0))
    n[..., 1] += np.where(yy < y0 + 3, -0.7, np.where(yy > y1 - 3, 0.7, 0))
    # Its shadow on the boards, then the frame and the slate.
    sc.rgb[rect(x0 + 3, y0 + 4, x1 + 3, y1 + 5)] *= 0.5
    sc.surface(rect(x0, y0, x1, y1) & ~rect(x0 + 7, y0 + 7, x1 - 7, y1 - 7),
               505, wood, normals=normalize(n), spec=0.25)
    slate = 0.85 + 0.15 * fbm(H * SS, W * SS, 76, 5, 8)
    smudge = np.clip((fbm(H * SS, W * SS, 77, 4, 4) - 0.55) * 3, 0, 1)
    alb = np.stack([0.11 + 0.08 * smudge, 0.15 + 0.08 * smudge,
                    0.12 + 0.07 * smudge], -1) * slate[..., None]
    sc.surface(rect(x0 + 7, y0 + 7, x1 - 7, y1 - 7), 506, alb, spec=0.05)
    sc.surface(rect(x0 + 9, y1 - 5, x1 - 9, y1 - 1), 503, wood,
               normal=(0, -0.8, -0.6))                    # the chalk ledge
    for k in range(3):
        sc.cylinder(x0 + 30 + k * 10, y1 - 7, y1 - 5, 1.1, 501,
                    (230, 226, 210))


def wall_things(sc):
    """Photographs pinned up beside the board, and the black cross cut
    from a machine brought down, over the piano."""
    rng = np.random.default_rng(81)
    for k, (x, y, w, h) in enumerate(((252, 52, 18, 13), (274, 47, 14, 18),
                                      (256, 70, 15, 11), (474, 58, 16, 12))):
        sc.rgb[rect(x + 1, y + 2, x + w + 1, y + h + 2)] *= 0.55
        tone = fbm(H * SS, W * SS, 82 + k, 3, 30)
        sepia = np.stack([0.62 * tone + 0.2, 0.52 * tone + 0.17,
                          0.38 * tone + 0.12], -1)
        inner = rect(x + 1.5, y + 1.5, x + w - 1.5, y + h - 2.5)
        sc.surface(rect(x, y, x + w, y + h) & ~inner, 514, (226, 216, 190),
                   normal=(rng.normal(0, 0.1), 0, -1))
        sc.surface(inner, 514, sepia * 0.7, normal=(rng.normal(0, 0.1), 0, -1))
        sc.ellipsoid((x + w / 2, y + 1, 512), (0.9, 0.9, 0.7), (180, 40, 30),
                     spec=0.8)
    # The trophy: a square of doped linen with its cross, nailed up.
    cx, cy = 512, 62
    lin = 0.85 + 0.15 * fbm(H * SS, W * SS, 86, 4, 20)
    sc.rgb[mask([(cx - 17, cy - 13), (cx + 21, cy - 10), (cx + 19, cy + 18),
                 (cx - 15, cy + 16)])] *= 0.5
    sheet = mask([(cx - 19, cy - 15), (cx + 18, cy - 12), (cx + 16, cy + 15),
                  (cx - 17, cy + 13)])
    sc.surface(sheet, 513, np.stack([0.74 * lin, 0.7 * lin, 0.56 * lin], -1),
               normal=(0.1, -0.05, -1))
    arm = 4.5
    cross = (rect(cx - arm, cy - 11, cx + arm, cy + 11) |
             rect(cx - 13, cy - arm, cx + 13, cy + arm))
    edge = (rect(cx - arm - 1.5, cy - 12.5, cx + arm + 1.5, cy + 12.5) |
            rect(cx - 14.5, cy - arm - 1.5, cx + 14.5, cy + arm + 1.5))
    sc.surface(edge & ~cross, 512.5, (214, 210, 196), normal=(0.1, -0.05, -1))
    sc.surface(cross, 512.5, (22, 20, 20), normal=(0.1, -0.05, -1))


def propeller(sc):
    def lam(q, n):
        return 0.82 + 0.18 * (np.sin(q[..., 1] * 14) > 0)
    for side in (-1, 1):
        sc.ellipsoid((572 + side * 30, 30 - side * 3, 500), (4.6, 30, 2.6),
                     (128, 84, 44), R=rot_z(math.pi / 2 - 0.1), spec=0.6,
                     shine=24, texture=lam)
    sc.ellipsoid((572, 30, 497), (6, 6, 4), (60, 44, 30), spec=0.5)


def stove(sc, mood):
    sc.cylinder(28, 192, 262, 19, 478, (46, 44, 42), spec=0.4, shine=14,
                texture=lambda u, v: 0.85 + 0.15 * (np.sin(v * 40) > 0.6))
    sc.ellipsoid((28, 192, 478), (19, 5, 11), (56, 54, 50), spec=0.4)
    sc.cylinder(28, 12, 192, 5.5, 480, (52, 50, 48), spec=0.3)
    sc.cylinder(28, 255, 266, 22, 476, (40, 38, 36), spec=0.3)
    fire = (255, 150, 60) if mood != "wake" else (240, 120, 44)
    sc.surface(rect(18, 222, 38, 246), 455, fire)
    sc.glow((28, 234), 11, (255, 140, 50), 1.0)
    # The coal scuttle beside it.
    sc.cylinder(58, 254, 270, 7, 470, (34, 32, 30), spec=0.5, shine=20)
    sc.ellipsoid((58, 254, 470), (7, 2, 5), (20, 18, 16))


def piano(sc, mood):
    case, _, _ = boards(87, base=(66, 36, 22),
                        widths=(400, 401), vertical=False)
    x0, x1 = 518, 610
    sc.surface(rect(x0, 158, x1, 238), 492, case, spec=0.55, shine=26)
    sc.surface(rect(x0 - 2, 154, x1 + 2, 159), 490, case,
               normal=(0, -0.9, -0.4), spec=0.5, shine=26)   # the lid
    sc.surface(rect(x0 + 6, 168, x1 - 6, 200), 491, case * 0.8, spec=0.4)
    for cx in (x0 + 16, x1 - 16):                    # the brass sconces
        sc.ellipsoid((cx, 176, 488), (2.2, 2.2, 2), (190, 150, 70), spec=1.0)
    # The keyboard, open or shut, on its shelf.
    sc.surface(rect(x0 - 2, 226, x1 + 2, 232), 472, case,
               normal=(0, -0.9, -0.4), spec=0.4)
    if mood == "wake":
        sc.surface(rect(x0, 218, x1, 227), 474, case, normal=(0, -0.5, -0.8),
                   spec=0.6, shine=26)                       # fall shut
    else:
        sc.surface(rect(x0 + 2, 222, x1 - 2, 227), 474, (226, 218, 196),
                   normal=(0, -0.6, -0.8), spec=0.3)
        for k in range(26):
            sc.surface(rect(x0 + 4 + k * 3.4, 222, x0 + 5.4 + k * 3.4, 225),
                       473.5, (24, 20, 16))
        sc.surface(rect(x0 + 30, 206, x1 - 30, 218), 486, (214, 206, 180),
                   normal=(0, -0.2, -1))                     # the music
    sc.surface(rect(x0, 232, x1, 252), 480, case * 0.7, spec=0.3)
    for lx in (x0 + 4, x1 - 4):
        sc.cylinder(lx, 232, 260, 2.5, 470, (54, 30, 18), spec=0.5)
    # The gramophone on top, its brass horn.
    gcase, _, _ = boards(80, base=(86, 52, 30),
                         widths=(400, 401), vertical=False)
    sc.surface(rect(530, 138, 566, 154), 488, gcase, spec=0.45, shine=24)
    sc.surface(rect(528, 135, 568, 138), 487, gcase, normal=(0, -0.9, -0.4),
               spec=0.4)
    if mood == "wake":
        sc.surface(rect(529, 128, 567, 135), 486.5, gcase,
                   normal=(0, -0.6, -0.8), spec=0.5)          # its lid down
    else:
        sc.ellipsoid((548, 134, 486), (15, 1.6, 9), (28, 26, 24), spec=0.6)
    n = 40
    for k in range(n):
        t = k / (n - 1)
        cx, cy = 556 + 24 * t, 132 - 32 * t * t - 8 * t
        r = 1.8 + 13 * t ** 2.2
        sc.ellipsoid((cx, cy, 484 - 6 * t), (r * 0.55, r, r), (188, 140, 62),
                     R=rot_z(-0.5 - 0.6 * t), spec=1.0, shine=30)
    sc.ellipsoid((582, 94, 476), (4, 10, 9), (70, 48, 22), R=rot_z(-1.1))


def lamp(sc, mood):
    lx, ly = LAMP
    sc.surface(rect(lx - 0.6, 0, lx + 0.6, ly - 10), 400, (40, 36, 30))
    sc.ellipsoid((lx, ly - 10, 398), (8, 3, 8), (150, 120, 60), spec=0.8)
    chimney = (250, 220, 150) if mood != "wake" else (220, 170, 100)
    sc.ellipsoid((lx, ly + 1, 398), (7, 12, 7), chimney, spec=1.0)
    sc.ellipsoid((lx, ly + 14, 398), (10, 5, 10), (176, 136, 64),
                 spec=0.9, shine=28)                      # the brass font
    strength = {"quiet": 1.6, "rowdy": 2.1, "wake": 0.8}[mood]
    sc.glow((lx, ly), 15, (255, 214, 140), strength)


# ---- the long table and what is on it ----------------------------------------
def top_z(y):
    """The long table's top: depth 444 at its far edge, 400 at the near."""
    return 444 - 44 * (y - TOP[0]) / (TOP[1] - TOP[0])


def table(sc, mood):
    x0, x1 = TABLE
    y0, y1 = TOP
    wood, _, _ = boards(78, base=(122, 82, 46), widths=(6, 8),
                        vertical=False)
    for lx in (x0 + 10, x1 - 10):
        sc.cylinder(lx, y1, floor_y(400), 3.5, 400, (70, 46, 28), spec=0.2)
    # The cloth over the top and hanging a hand's depth down the front.
    cloth = 0.88 + 0.12 * fbm(H * SS, W * SS, 79, 3, 40)
    stain = np.clip((fbm(H * SS, W * SS, 88, 4, 6) - 0.6) * 2.5, 0, 1)
    if mood != "rowdy":
        stain *= 0.3
    yy, xx = np.mgrid[0:H * SS, 0:W * SS].astype(np.float32) / SS
    drape = 0.9 + 0.1 * np.sin(xx * 0.45 + 3 * fbm(H * SS, W * SS, 89, 3, 6))
    cloth = cloth * np.where(yy > y1, drape, 1.0)
    alb = np.stack([0.74 - 0.2 * stain, 0.71 - 0.26 * stain,
                    0.63 - 0.3 * stain], -1) * cloth[..., None]
    sc.surface(rect(x0 - 2, y0, x1 + 2, y1), yramp(y0, y1, 444, 400), alb,
               normal=(0, -0.95, -0.3), spec=0.1)
    sc.surface(rect(x0 - 2, y1, x1 + 2, y1 + 20), 398, alb * 0.72,
               normal=(0, -0.15, -1))
    sc.surface(rect(x0 - 2, y1, x1 + 2, y1 + 2), 397.5, alb * 0.95,
               normal=(0, -0.7, -0.7))                     # the fold


def bottle(sc, x, yb, z, full=True):
    """A dark green bottle standing with its base at row yb."""
    green = (36, 64, 40)
    sc.cylinder(x, yb - 20, yb, 3.6, z, green, spec=1.2, shine=40)
    sc.ellipsoid((x, yb - 20, z), (3.6, 2.5, 3.6), green, spec=1.0, shine=40)
    sc.cylinder(x, yb - 31, yb - 21, 1.3, z - 0.5, green, spec=1.0, shine=40)
    sc.cylinder(x, yb - 33, yb - 30, 1.6, z - 1, (150, 120, 70) if full
                else green)
    sc.surface(rect(x - 3, yb - 14, x + 3, yb - 7), z - 3.7, (210, 196, 160),
               normal=(0, 0, -1))                          # its label


def pot(sc, x, yb, z, full=1.0):
    """A pint pot on the table, its base at row yb."""
    sc.cylinder(x, yb - 11, yb, 3.4, z, (214, 150, 58) if full else
                (120, 110, 96), spec=1.0, shine=36)
    if full:
        sc.ellipsoid((x, yb - 11, z), (3.4, 1.4, 3.4), (236, 228, 206),
                     spec=0.3)
        sc.glow((x - 1, yb - 5), 2.4, (255, 190, 90), 0.3)


def candle(sc, x, yb, z, lit=True):
    """A candle stuck in the neck of an empty bottle."""
    bottle(sc, x, yb, z, full=False)
    sc.cylinder(x, yb - 42, yb - 33, 1.5, z - 1.5, (232, 224, 200))
    sc.ellipsoid((x, yb - 33, z - 1.5), (3, 1.5, 3), (230, 220, 196))
    if lit:
        sc.ellipsoid((x, yb - 45, z - 2), (1.1, 2.4, 1.1), (255, 236, 170))
        sc.glow((x, yb - 45), 4, (255, 200, 110), 1.1)


def ashtray(sc, x, yb, z):
    sc.ellipsoid((x, yb - 1, z), (6, 1.6, 4), (120, 100, 70), spec=0.5)
    sc.ellipsoid((x + 3, yb - 2.5, z - 1), (3, 0.6, 0.6), (220, 214, 200),
                 R=rot_z(-0.2))                            # a cigarette


def on_table(sc, mood):
    """Bottles in the gaps between the men, a pot in front of each."""
    y = TOP[0] + 3
    yz = top_z(y) - 6
    py = y + 9
    pz = top_z(py) - 5
    if mood == "rowdy":
        for x in (300, 348, 452, 504):
            bottle(sc, x, y + 1, yz)
        for x in (400, 494):
            candle(sc, x, y + 2, yz - 2)
        for x in (336, 360, 414, 490):
            pot(sc, x, py, pz, full=0.0 if x == 360 else 1.0)
        ashtray(sc, 440, py + 2, pz + 1)
    elif mood == "quiet":
        for x in (348, 452):
            bottle(sc, x, y + 1, yz)
        candle(sc, 400, y + 2, yz - 2)
        for x in (310, 386, 466):
            pot(sc, x, py, pz)
        ashtray(sc, 424, py + 2, pz + 1)
    else:
        bottle(sc, 430, y + 1, yz)
        candle(sc, 404, y + 2, yz - 2)
        pot(sc, 384, py + 3, pz - 3)          # his, kept full
        pot(sc, 352, py, pz)
        ashtray(sc, 448, py + 2, pz + 1)


# ---- the officers ------------------------------------------------------------
def officers(sc, room_sc, mood):
    """The squadron, each man made and posed with the figure kit
    (figurekit.py): his own build, face and kit from his seed.  The men
    go into their own scene 'sc', lit for lamplight (see lamplit_men);
    the chair and the dog belong to the room."""
    far, near = 446, 372
    fy, ny = floor_y(far), 294            # the near side sit a little lower
    S, SN = 1.25 * 13, 1.4 * 13           # pixels to a head-height
    co = dict(grey=True, moustache="walrus", style="sd", hair=(70, 58, 44),
              skin=(196, 146, 116), build=1.08)

    def man(seed, pose, x, floor, z, px, yaw, make=None, **over):
        f = Figure(seed=seed, **(make or {}))
        f.pose(pose, **over)
        f.draw(sc, x, floor, z, px, yaw)
        return f
    stove = (92, floor_y(448), 448, S)
    if mood == "quiet":
        # Round the far side, face-on: one with his glass, one talking,
        # one drinking, one with his pipe.
        man(21, "glass", 322, fy, far, S, 0.15, turn=18)
        man(22, "talk", 374, fy, far, S, -0.1)
        man(23, "drink", 426, fy, far, S, 0.0)
        man(24, "pipe", 478, fy, far, S, -0.3, dict(moustache="full"),
            turn=-14)
        # The CO with a glass at the head of the table.
        man(20, "sglass", 256, floor_y(430), 430, S, -0.5, co, turn=-10)
        # Back-on at the near side, turned to his neighbour.
        man(25, "table", 300, ny, near, SN, math.pi + 0.35, turn=25)
        # At the piano, playing softly.
        man(26, "piano", 572, floor_y(452), 452, S, math.pi)
        # By the stove, his hands to it.
        man(27, "warm", *stove[:3], S, math.pi / 2 - 0.25)
    elif mood == "rowdy":
        man(31, "toast", 322, fy, far, S, 0.2)
        man(32, "cheer", 374, fy, far, S, 0.0, dict(style="shirt"))
        man(33, "drink", 426, fy, far, S, -0.1)
        man(34, "toast", 478, fy, far, S, -0.2, turn=-12)
        # On the table, making his speech to the lamp.
        man(35, "sraise", 452, TOP[0] + 5, top_z(TOP[0] + 5), S, -0.25)
        man(20, "sraise", 256, floor_y(430), 430, S, -0.4, co)
        man(36, "toast", 300, ny, near, SN, math.pi + 0.3)
        man(37, "toast", 502, ny, near, SN, math.pi - 0.3, turn=-20)
        # The piano going, and a man leaning on it to sing.
        man(38, "piano", 566, floor_y(452), 452, S, math.pi - 0.2, lift=8)
        man(39, "lean", 620, floor_y(458), 458, S, -0.6)
        man(40, "toast", *stove[:3], S, -0.9)
    else:
        # Fewer of us.  Heads down; nobody at the piano.
        man(41, "clasp", 340, fy, far, S, 0.1, dict(hair=(36, 28, 22)))
        man(42, "head", 466, fy, far, S, -0.15)
        man(43, "clasp", 502, ny, near, SN, math.pi - 0.2)
        # His chair, pushed back from the table, his cap on its back.
        b = Body(room_sc, (390, ny - 1.9 * 13 * 1.4, near - 4), 1.4,
                 math.pi + 0.3)
        bentwood(b, cap=True)
        # The CO alone at the board, his back half to us, hands behind.
        man(20, "behind", 240, floor_y(436), 436, S, math.pi / 2 + 0.3, co)
        man(44, "warm", *stove[:3], S, math.pi / 2 - 0.1, bow=22)
    dog(room_sc, 74, floor_y(404) - 7, 404, 1.2)


# ---- our own table, nearest -------------------------------------------------
def near_table(sc, mood):
    top, _, _ = boards(90, base=(104, 70, 42), widths=(16, 26),
                       vertical=False)
    sc.surface(rect(0, NEAR + 3, W, H), yramp(NEAR, H, 260, 140), top * 0.85,
               normal=(0, -0.92, -0.4), spec=0.3, shine=18)
    sc.surface(rect(0, NEAR, W, NEAR + 3), 262, top, normal=(0, -0.6, -0.8),
               spec=0.5, shine=20)                         # its far edge
    # A map, folded open, the lines in coloured pencil.
    m = mask([(512, 336), (636, 326), (646, 404), (522, 410)])
    paper = np.zeros((H * SS, W * SS, 3), np.float32)
    paper[:] = (0.8, 0.76, 0.64)
    paper *= (0.85 + 0.15 * fbm(H * SS, W * SS, 91, 4, 20))[..., None]
    sheet = Image.new("RGB", (W * SS, H * SS), (0, 0, 0))
    d = ImageDraw.Draw(sheet)
    for k in range(9):                             # the squares of the sheet
        x = (512 + k * 16) * SS
        d.line([(x, 320 * SS), (x + 10 * SS, 410 * SS)], fill=(60, 60, 60),
               width=1)
        y = (336 + k * 12) * SS
        d.line([(500 * SS, y), (640 * SS, y - 10 * SS)], fill=(60, 60, 60),
               width=1)
    d.line([(c * SS, r * SS) for c, r in ((514, 380), (540, 366), (566, 372),
            (590, 352), (620, 356), (644, 340))], fill=(40, 70, 160),
           width=2 * SS)                                    # a river
    d.line([(c * SS, r * SS) for c, r in ((530, 404), (556, 386), (600, 380),
            (640, 392))], fill=(170, 40, 30), width=SS)    # the lines
    d.ellipse([588 * SS, 358 * SS, 604 * SS, 370 * SS], outline=(170, 40, 30),
              width=SS)                                     # a ringed target
    ink = np.asarray(sheet, np.float32) / 255.0
    inked = ink.sum(-1) > 0
    paper[inked] = paper[inked] * 0.4 + ink[inked] * 0.6
    yy, xx = np.mgrid[0:H * SS, 0:W * SS].astype(np.float32) / SS
    fold = np.abs(xx - (576 + (yy - 330) * 0.12)) < 1.0
    nrm = np.zeros((H * SS, W * SS, 3), np.float32)
    nrm[..., 1], nrm[..., 2] = -0.9, -0.42
    nrm[..., 0] = np.where(xx < 576 + (yy - 330) * 0.12, 0.12, -0.12)
    paper[fold] *= 0.7
    sc.surface(m, yramp(NEAR, H, 255, 136) - 1, paper, normals=normalize(nrm),
               spec=0.1)
    # The flying helmet: brown leather, its ear flaps out, the goggles
    # pushed up on the crown with their lenses toward the lamp.
    hx, hy, hz = 568, 338, 188
    leather = (122, 76, 42)

    def stitch(q, n):
        return 0.86 + 0.14 * (np.abs(np.sin(q[..., 0] * 7)) > 0.25)
    sc.ellipsoid((hx, hy, hz), (25, 18, 20), leather, clip_y=0.15,
                 spec=0.9, shine=22, rim=0.4, texture=stitch)
    for side in (-1, 1):
        sc.ellipsoid((hx + side * 24, hy + 6, hz - 2), (7, 12, 5), leather,
                     R=rot_z(side * 0.35), spec=0.7, shine=20)
        sc.ellipsoid((hx + side * 26, hy + 18, hz - 6), (3, 7, 1.6),
                     (70, 42, 24), R=rot_z(side * 0.2))     # the chin strap
    sc.ellipsoid((hx, hy - 4, hz - 14), (24, 3.6, 7), (60, 52, 46),
                 R=rot_z(0.04))                             # goggle strap
    for side in (-1, 1):
        gx = hx + side * 9
        sc.ellipsoid((gx, hy - 6, hz - 22), (7.5, 6.5, 4), (170, 146, 86),
                     spec=1.2, shine=30)                    # brass rim
        sc.ellipsoid((gx, hy - 6, hz - 24.5), (5.5, 4.8, 2.5), (70, 82, 78),
                     spec=1.8, shine=60)                    # the glass
    sc.ellipsoid((hx, hy - 5, hz - 23), (3, 1.4, 1.5), (170, 146, 86),
                 spec=1.0)                                  # the bridge
    # A pewter tankard, and a candle in its saucer that lights our end.
    sc.cylinder(526, 288, 322, 8, 214, (150, 140, 128), spec=1.4, shine=40)
    sc.ellipsoid((526, 288, 214), (8, 2.6, 7), (170, 160, 146), spec=1.0)
    sc.ellipsoid((536, 304, 216), (3, 8, 2.5), (150, 140, 128), spec=1.2,
                 R=rot_z(0.2))
    sc.ellipsoid((616, 318, 214), (13, 3, 9), (190, 180, 160), spec=0.8)
    stub = 286 if mood != "wake" else 298
    sc.cylinder(616, stub, 316, 4, 210, (232, 224, 200), rim=0.2)
    sc.ellipsoid((616, stub, 210), (4, 1.6, 4), (240, 232, 210))
    sc.ellipsoid((616, stub - 4, 209), (1.4, 3.2, 1.4), (255, 236, 170))
    sc.glow((616, stub - 4), 5, (255, 200, 110), 1.2)


# ---- the men by lamplight ----------------------------------------------------
# How the men are lit, per evening: the lamp's share of plain light on
# them, its rim, the stove's, the light everywhere else, and how much
# their edges are softened (in pixels of the finished picture).  The
# figures are the kit's (figurekit.py) unchanged; only the light and the
# softness are the painter's.
MEN = {
    "quiet": dict(lamp=0.9, rim=3.6, stove=0.9, ambient=0.014, soft=0.7),
    "rowdy": dict(lamp=1.1, rim=3.8, stove=0.9, ambient=0.018, soft=0.85),
    "wake":  dict(lamp=0.32, rim=3.0, stove=1.0, ambient=0.006, soft=0.75),
}


def soften(a, r, passes=3):
    """A near-Gaussian blur of an (H, W[, C]) array: 'passes' box blurs
    of half-width r, each done along both axes with running sums."""
    if r < 1:
        return a
    out = a.astype(np.float32)
    for _ in range(passes):
        for ax in (0, 1):
            c = np.cumsum(out, axis=ax, dtype=np.float64)
            pad = [(0, 0)] * out.ndim
            pad[ax] = (r + 1, r)
            c = np.pad(c, pad, mode="edge")
            n = out.shape[ax]
            hi = np.take(c, np.arange(n) + 2 * r + 1, axis=ax)
            lo = np.take(c, np.arange(n), axis=ax)
            if ax == 0:
                lo[:r + 1] = 0
            else:
                lo[:, :r + 1] = 0
            out = ((hi - lo) / (2 * r + 1)).astype(np.float32)
    return out


def lamplit_men(sc, mood):
    """Draws the squadron in a scene of their own, lit as a painter
    would light men in a smoky hut: little plain light, so they read
    mostly as dark shapes, a face catching the lamp only on the side
    toward it; a warm rim of light round heads, shoulders and raised
    arms from the lamp (set back over the table, among them) and from
    the stove.  Then their edges are softened a touch and they are laid
    into the room where nothing nearer hides them, before the smoke."""
    k, m = MOODS[mood], MEN[mood]
    amb = m["ambient"]
    men = Scene(W, H, ss=SS, ambient=(amb, amb * 0.85, amb * 0.7))
    men.point_rim = m["rim"] / max(m["lamp"], 0.1)
    lamp_col = np.array(k["lamp"]) * m["lamp"]
    men.point((LAMP[0], LAMP[1] + 30, 438), tuple(lamp_col), k["reach"])
    men.point((28, 234, 440), tuple(np.array(k["stove"]) * m["stove"]), 110)
    officers(men, sc, mood)
    vis = np.isfinite(men.z) & (men.z < sc.z)
    r = int(round(m["soft"] * SS))
    a = soften(vis.astype(np.float32), r)
    rgb = soften(np.where(vis[..., None], men.rgb, 0), r)
    sc.rgb = sc.rgb * (1 - a[..., None]) + rgb
    sc.z = np.where(vis, men.z, sc.z)


def pipe_smoke(sc, mood, lamp_col):
    """Pipe and cigarette smoke: thin ribbons rising from each pipe and
    each cigarette left in an ashtray, curling and spreading as they go,
    lit where they drift through the lamp's glow."""
    sources = {
        "quiet": [(466, 186, 1.0), (424, 236, 0.8), (262, 168, 0.5)],
        "rowdy": [(440, 236, 0.9), (330, 230, 0.6), (560, 196, 0.6)],
        "wake":  [(448, 236, 1.1), (96, 196, 0.7)],
    }[mood]
    d = np.zeros((H * SS, W * SS), np.float32)
    rng = np.random.default_rng(len(mood))
    for x0, y0, amount in sources:
        ph = rng.uniform(0, 6)
        drift = rng.uniform(-0.25, 0.25)
        for i in range(140):
            t = i / 139
            rise = t * rng.uniform(90, 120)
            x = x0 + drift * rise + 6 * t * math.sin(t * 9 + ph) + \
                3 * math.sin(t * 23 + ph * 2)
            y = y0 - rise
            w = 0.8 + 7 * t ** 1.4                    # spreading as it goes
            dens = amount * (1 - t) ** 1.2 * 0.06 / (0.4 + 0.25 * w)
            cx, cy, R = x * SS, y * SS, int(w * SS * 2.5) + 1
            x0i, x1i = max(0, int(cx) - R), min(W * SS, int(cx) + R)
            y0i, y1i = max(0, int(cy) - R), min(H * SS, int(cy) + R)
            if x0i >= x1i or y0i >= y1i:
                continue
            yy, xx = np.mgrid[y0i:y1i, x0i:x1i].astype(np.float32)
            d[y0i:y1i, x0i:x1i] += dens * np.exp(
                -((xx - cx) ** 2 + (yy - cy) ** 2) / (w * SS) ** 2)
    curl = fbm(H * SS, W * SS, 70 + len(mood), 4, 14)
    d *= 0.5 + curl
    yy, xx = np.mgrid[0:H * SS, 0:W * SS].astype(np.float32) / SS
    glow = 0.35 + 1.4 * np.exp(-((np.hypot(xx - LAMP[0], yy - LAMP[1] - 10)
                                  / 170) ** 2))
    col = np.array(lamp_col, np.float32) * 0.5
    sc.rgb = sc.rgb * (1 - 0.3 * np.clip(d, 0, 1)[..., None]) + \
        (d * glow)[..., None] * col


def dark_edges(sc, mood):
    """Deeper shadow toward the edges and corners of the room, where the
    lamp does not reach; the wake darkest."""
    k = {"quiet": 0.5, "rowdy": 0.4, "wake": 0.62}[mood]
    yy, xx = np.mgrid[0:H * SS, 0:W * SS].astype(np.float32) / SS
    r = np.hypot((xx - LAMP[0]) / 380, (yy - 170) / 300)
    sc.rgb *= (1 - k * np.clip((r - 0.45) / 0.7, 0, 1) ** 1.5)[..., None]


# ---- smoke and the finish ----------------------------------------------------
def shade_under_table(sc):
    """The renderer casts no shadows, so lay the one that matters by hand:
    under the long table, where the cloth keeps the lamp off the legs."""
    x0, x1 = TABLE
    yy, xx = np.mgrid[0:H * SS, 0:W * SS].astype(np.float32) / SS
    under = (np.clip((xx - x0 + 6) / 12, 0, 1) * np.clip((x1 + 6 - xx) / 12, 0, 1)
             * np.clip((yy - TOP[1] - 18) / 4, 0, 1)
             * np.clip((floor_y(380) + 6 - yy) / 10, 0, 1))
    under *= sc.z / SS > 396
    sc.rgb *= (1 - 0.7 * under)[..., None]
def haze(sc, mood, colour):
    """Tobacco smoke: thickest under the roof and round the lamp, where it
    is lit gold, and deeper the further into the room we look."""
    k = MOODS[mood]
    smoke = fbm(H * SS, W * SS, 50 + k["seed"], 6, 3)
    smoke = np.clip((smoke - 0.32) * 1.9, 0, 1)
    wisp = fbm(H * SS, W * SS, 60 + k["seed"], 4, 10)
    smoke = smoke * (0.7 + 0.6 * wisp)
    yy, xx = np.mgrid[0:H * SS, 0:W * SS].astype(np.float32) / SS
    d = np.hypot(xx - LAMP[0], (yy - LAMP[1] - 10) * 1.25)
    glow = np.exp(-(d / 190) ** 2)
    depth = np.clip((np.where(np.isfinite(sc.z), sc.z / SS, 520) - 180) / 340,
                    0, 1)
    rise = 0.55 + 0.65 * np.clip(1 - yy / 320, 0, 1)
    dens = smoke * (0.2 + 0.8 * glow) * (0.25 + 0.75 * depth) * rise * \
        k["smoke"]
    col = np.array(colour, np.float32) * 0.3
    sc.rgb = sc.rgb * (1 - 0.18 * dens[..., None]) + dens[..., None] * col
    # A little bloom straight round the lamp.
    sc.glow((LAMP[0], LAMP[1] + 4), 60, (120, 90, 50),
            {"quiet": 0.18, "rowdy": 0.26, "wake": 0.08}[mood])


def render(mood):
    k = MOODS[mood]
    sc = Scene(W, H, ss=SS, ambient={"quiet": (0.03, 0.026, 0.022),
                              "rowdy": (0.035, 0.03, 0.025),
                              "wake": (0.024, 0.022, 0.024)}[mood])
    sc.point_rim = 0.45
    sc.point((LAMP[0], LAMP[1] + 6, 405), k["lamp"], k["reach"])
    sc.point((28, 234, 440), k["stove"], 85)
    sc.point((616, 282, 160), (1.0, 0.68, 0.32), 60)       # our candle
    # The candles on the long table: two lit for the party, and on the
    # night of a loss the one left burning by his place.
    sc.point((400, 176, 425), {"quiet": (0.3, 0.2, 0.1),
                               "rowdy": (0.5, 0.35, 0.18),
                               "wake": (0.7, 0.46, 0.22)}[mood], 70)
    room(sc)
    board(sc)
    wall_things(sc)
    propeller(sc)
    stove(sc, mood)
    piano(sc, mood)
    table(sc, mood)
    on_table(sc, mood)
    lamp(sc, mood)
    near_table(sc, mood)
    lamplit_men(sc, mood)
    shade_under_table(sc)
    haze(sc, mood, k["lamp"])
    pipe_smoke(sc, mood, k["lamp"])
    dark_edges(sc, mood)
    img = sc.finish(exposure=k["exposure"])
    # Keep the boards under the evening's lines dark enough to read on.
    a = np.asarray(img).astype(np.float32)
    x0, y0, x1, y1 = TEXT
    yy, xx = np.mgrid[0:H, 0:W].astype(np.float32)
    inside = (np.clip((xx - x0) / 16, 0, 1) * np.clip((x1 - xx) / 24, 0, 1) *
              np.clip((yy - y0 + 6) / 10, 0, 1))
    a *= (1 - 0.45 * inside)[..., None]
    return Image.fromarray(np.clip(a, 0, 255).astype(np.uint8))


# ---- the preview, as the game draws it --------------------------------------
def load_faces():
    text = (ROOT / "src" / "modern" / "canvas_fonts.h").read_text()
    faces = {}
    for m in re.finditer(r"\{(\d+), (\d+), (\d+), (\d+), (\d+), (\d+), \{ "
                         r"/\* (\w+) \*/([^}]*)\}", text):
        nums = [int(v) for v in m.group(8).replace(",", " ").split()]
        faces[m.group(7)] = dict(top=int(m.group(1)), cw=int(m.group(2)),
                                 ch=int(m.group(3)), ox=int(m.group(4)),
                                 base=int(m.group(5)), adv=nums)
    atlas = np.asarray(Image.open(ASSETS / "fonts.bmp").convert("L"))
    return faces, atlas


def font_text(img, faces, atlas, face, x, baseline, s, colour):
    f = faces[face]
    for ch in s:
        o = ord(ch) if 32 <= ord(ch) <= 126 else 32
        i = o - 32
        if ch != " ":
            sx, sy = (i % 16) * f["cw"], f["top"] + (i // 16) * f["ch"]
            cov = atlas[sy:sy + f["ch"], sx:sx + f["cw"]].astype(np.float32)
            dx, dy = x - f["ox"], baseline - f["base"]
            for py in range(f["ch"]):
                for px in range(f["cw"]):
                    c = cov[py, px]
                    X, Y = dx + px, dy + py
                    if c < 12 or not (0 <= X < W and 0 <= Y < H):
                        continue
                    t = 1.0 if c > 243 else ((int(c) + 4) >> 3) / 32.0
                    img[Y, X] = img[Y, X] * (1 - t) + np.array(colour) * t
        x += f["adv"][i]


def typewriter(img, faces, atlas, x, y, s, h, colour):
    font_text(img, faces, atlas, "FONT_TYPE" if h >= 15 else "FONT_TYPE_SMALL",
              x, y + h * 4 // 5, s, colour)


SAMPLE = {
    "quiet": (7, 0, ["THE MESS, THAT EVENING.",
                     "TOM'S PHOTOGRAPHS GO UP ON THE WALL.",
                     "THE COLONEL LOOKS IN, SAYS WELL DONE, AND LEAVES US TO IT.",
                     "THE GRAMOPHONE PLAYS UNTIL THE NEEDLE GIVES OUT."]),
    "rowdy": (9, 2, ["THE MESS, THAT EVENING.",
                     "BLUEBOTTLE CHALKS 2 MORE ON THE BOARD.",
                     "BLUEBOTTLE STANDS ON A CHAIR AND MAKES A SPEECH NOBODY HEARS.",
                     "WE SING EVERY SONG WE KNOW, AND THEN THE RUDE ONES."]),
    "wake":  (23, 1, ["THE MESS, THAT EVENING.",
                      "ECCLES IS IN THE SICK BAY TONIGHT.",
                      "THE COLONEL LOOKS IN, STANDS A WHILE AT THE BOARD, AND GOES.",
                      "WE KEEP HIS GLASS FULL FOR THE MORNING."]),
}


def chalk_mark(img, x0, y0, x1, y1, colour):
    """line() twice over, a pixel apart, as draw_mess chalks a stroke."""
    n = int(max(abs(x1 - x0), abs(y1 - y0))) + 1
    for t in np.linspace(0, 1, n):
        x, y = int(round(x0 + (x1 - x0) * t)), int(round(y0 + (y1 - y0) * t))
        for X in (x, x + 1):
            if 0 <= X < W and 0 <= y < H:
                img[y, X] = colour


def preview(mood, img, out):
    """The picture with the evening laid over it, as draw_mess() does:
    keep the numbers here in step with it."""
    faces, atlas = load_faces()
    a = np.asarray(img).astype(np.float32).copy()
    victories, kills, lines = SAMPLE[mood]
    bx, by = BOARD[0] + 7, BOARD[1] + 7
    typewriter(a, faces, atlas, bx + 9, by + 5, "SQUADRON BOARD", 11,
               (232, 228, 214))
    typewriter(a, faces, atlas, bx + 9, by + 21, "VICTORIES %d" % victories,
               11, (220, 218, 204))
    for i in range(min(victories, 60)):
        today = i >= victories - kills
        col = (240, 214, 96) if today else (214, 214, 204)
        cx = bx + 12 + ((i % 20) // 5) * 38
        cy = by + 42 + (i // 20) * 18
        k = i % 5
        if k < 4:
            chalk_mark(a, cx + k * 7, cy, cx + k * 7, cy + 12, col)
        else:
            chalk_mark(a, cx - 3, cy + 11, cx + 24, cy + 1, col)
    for i, s in enumerate(lines):
        typewriter(a, faces, atlas, 40, 304 + i * 19, s, 11,
                   (214, 92, 62) if i == 0 else (226, 210, 172))
    typewriter(a, faces, atlas, 300, 381, "ENTER", 11, (200, 180, 140))
    Image.fromarray(np.clip(a, 0, 255).astype(np.uint8)).save(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("moods", nargs="*", default=list(OUT))
    ap.add_argument("--preview", type=Path)
    args = ap.parse_args()
    for mood in args.moods:
        img = render(mood)
        img.save(ASSETS / OUT[mood])
        if args.preview:
            args.preview.mkdir(parents=True, exist_ok=True)
            preview(mood, img, args.preview / ("mess-%s.png" % mood))


if __name__ == "__main__":
    main()
