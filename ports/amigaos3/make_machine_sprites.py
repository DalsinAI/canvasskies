#!/usr/bin/env python3
"""Paint the side-on and top-down sprites for the machines of each year.

The existing painted Sopwith-type (ours, 1915-1917) and Albatros-type
(theirs, 1916-1917) atlases stay.  This adds, in the same layout:

  aircraft-player-twoseat.bmp   1914: the two-seat reconnaissance machine
  aircraft-player-late.bmp      1918: the late-war scout, flat-nosed
  aircraft-enemy-mono.bmp       1915: their shoulder-wing monoplane
  aircraft-enemy-late.bmp       1918: their square-winged biplane
  topdown-player-twoseat.bmp    the two-seater from above
  topdown-player-late.bmp       the late scout from above

Side-on atlases are five 96x48 frames, pitch frame 0 to 4; ours face right
with frame 0 nose up, theirs face left with frame 0 nose down, as the game
expects.  Everything is painted with the shared painter (paint.py).
"""

import math
import sys
from pathlib import Path

from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parent))
from paint import Canvas, KEY, rot  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"
STEP = math.radians(3.2)

CLEAR_DOPE = (196, 184, 150)
PC10 = (104, 98, 62)
LOZENGE = [(116, 84, 132), (84, 112, 76), (140, 108, 70), (70, 80, 128)]
STREAKED = (150, 128, 90)
METAL = (120, 118, 112)
DARK = (40, 36, 32)
WOOD = (120, 80, 44)
RED, WHITE, BLUE = (176, 40, 34), (230, 226, 214), (40, 60, 128)
BLACK = (24, 22, 22)


def roundel(c, x, y, r):
    for rr, col in ((r, BLUE), (r * 0.66, WHITE), (r * 0.34, RED)):
        c.part([(x - rr, y - rr), (x + rr, y + rr)], col, shape="ellipse",
               gloss=0.1, grain=3, rim=0.15, round_=0.2)


def cross(c, x, y, r, upright=True):
    w = r * 0.42
    c.part([(x - r, y - r), (x + r, y + r)], WHITE, round_=0.2, rim=0.1,
           gloss=0.05)
    c.part([(x - w, y - r), (x + w, y - r), (x + w, y + r), (x - w, y + r)],
           BLACK, round_=0.1, rim=0.1)
    c.part([(x - r, y - w), (x + r, y - w), (x + r, y + w), (x - r, y + w)],
           BLACK, round_=0.1, rim=0.1)


def aerofoil(c, R, le, chord, y, thick, colour, under=None, **kw):
    """A wing seen end-on along its span: a rounded leading edge, the
    camber rising over the front third, a fine trailing edge."""
    top, bottom = [], []
    for k in range(9):
        t = k / 8
        x = le - chord * t
        hump = thick * (1.0 - t) ** 0.5 * (0.7 + 1.2 * t * (1 - t))
        top.append((x, y - hump * 0.75))
        bottom.append((x, y + hump * 0.35))
    if under:
        # Seen from a little below: the doped underside shows as a pale band.
        belly = [(x, yb + thick * 0.55 * (1 - k / 9)) for k, (x, yb) in
                 enumerate(bottom)]
        c.part(R(bottom + belly[::-1]), under, gloss=0.2, grain=3, round_=0.3)
    c.part(R(top + bottom[::-1]), colour, gloss=0.45, round_=0.4, **kw)
    lit = tuple(min(255, v + 36) for v in colour)
    c.line(R([top[1]])[0], R([top[-2]])[0], lit, 0.5)    # sun on the top


def fuselage(c, R, top, bottom, colour):
    c.part(R(top + bottom[::-1]), colour, gloss=0.22, grain=4, round_=1.0)


def strut(c, R, p0, p1, colour=WOOD, w=2.2):
    c.part([R([p0])[0], R([p1])[0], w], colour, shape="capsule", gloss=0.35,
           round_=0.3)


def rudder(c, R, pts, colours=None, colour=None):
    if colours:                                    # the British stripes
        xs = [x for x, _ in pts]
        x0, x1 = min(xs), max(xs)
        c.part(R(pts), colours[2], gloss=0.15, round_=0.8)
        for k, col in enumerate(colours[:2]):
            xa = x0 + (x1 - x0) * k / 3
            xb = x0 + (x1 - x0) * (k + 1) / 3
            clipped = [(min(max(x, xa), xb), y) for x, y in pts]
            c.part(R(clipped), col, gloss=0.15, round_=0.3)
    else:
        c.part(R(pts), colour, gloss=0.15, round_=0.8)


def side_frame(kind, frame):
    """One side-on frame, facing right, nose up by the frame's pitch.

    Proportions follow the real machines: seen from the side a wing shows
    only its chord, a fifth or so of the fuselage, with the upper plane
    staggered forward of the lower."""
    c = Canvas(110, 55, light=(0.2, -1.0))
    pitch = (2 - frame) * STEP
    o = (55, 27.5)

    def R(pts):
        return rot([(55 + (x - 48) * 0.92, 27.5 + (y - 19) * 1.2)
                    for x, y in pts], -pitch, o)

    stripes = (BLUE, WHITE, RED)
    if kind == "twoseat":                          # a B.E.2 type
        body = CLEAR_DOPE
        rudder(c, R, [(2, 8), (8, 7), (10, 12), (10, 22), (3, 22), (1, 15)],
               stripes)
        c.part(R([(8, 10), (12, 9.5), (18, 17), (8, 17)]), body, gloss=0.2,
               round_=0.8)                         # fin
        aerofoil(c, R, 20, 9, 19.5, 1.6, body)     # tailplane
        fuselage(c, R, [(8, 18), (30, 16), (56, 14), (76, 14), (86, 16)],
                 [(8, 22.5), (30, 26.5), (56, 30), (76, 30), (86, 26)], body)
        c.part(R([(82, 15), (90, 16.5), (90, 24), (82, 25)]), METAL,
               gloss=0.45, round_=1.0)             # engine and its cowl
        c.part(R([(84, 10), (86, 10), (86.5, 15), (84, 15)]), DARK,
               round_=0.3)                         # exhaust stack
        for x in (50, 64):
            c.part(R([(x - 4, 13.6), (x + 4, 13.6), (x + 3, 15.6),
                      (x - 3, 15.6)]), DARK, round_=0.5)
        upper, lower = (78, 26, 2.5), (76, 26, 31.0)
        struts = ((74, 3.4, 72, 30.4), (57, 3.4, 55, 30.4))
        cabanes = ((78, 3.4, 80, 14.2), (68, 3.4, 68, 14.0))
        roundel(c, *R([(36, 20.5)])[0], 4.6)
        wheel_x = (74, 66)
    elif kind == "late":                           # an S.E.5 type
        body = PC10
        rudder(c, R, [(3, 8), (9, 7.5), (11, 12), (11, 22), (4, 22), (2, 15)],
               stripes)
        c.part(R([(9, 9), (14, 9), (20, 16.5), (9, 16.5)]), body, gloss=0.2,
               round_=0.8)
        aerofoil(c, R, 22, 10, 19.0, 1.6, body)
        fuselage(c, R, [(9, 17.5), (30, 15.5), (58, 12.5), (80, 12.5), (88, 14)],
                 [(9, 22.5), (30, 27), (58, 30.5), (80, 30.5), (88, 28.5)], body)
        c.part(R([(84, 13), (90, 13.5), (90, 27.5), (84, 28)]), METAL,
               gloss=0.45, round_=0.6)             # the flat radiator nose
        for k in range(4):
            c.line(R([(89.4, 15 + k * 3.5)])[0], R([(85, 15 + k * 3.5)])[0],
                   (70, 68, 64), 0.4)              # shutters
        c.part(R([(56, 12.3), (62, 12.3), (61, 14.6), (57, 14.6)]), DARK,
               round_=0.4)
        c.part(R([(58, -0.6), (73, -0.6), (73, 0.8), (58, 0.8)]), DARK,
               round_=0.2)                         # Lewis on the top plane
        c.part(R([(62, -2.4), (66, -2.4), (66, -0.6), (62, -0.6)]), DARK,
               round_=0.3)                         # its drum
        upper, lower = (76, 24, 2.2), (72, 22, 31.0)
        struts = ((72, 3.0, 69, 30.4), (56, 3.0, 53, 30.4))
        cabanes = ((76, 3.0, 79, 12.8), (66, 3.0, 67, 12.6))
        roundel(c, *R([(38, 21)])[0], 4.8)
        wheel_x = (78, 72)
    elif kind == "mono":                           # a Fokker E type
        body = STREAKED
        rudder(c, R, [(2, 14), (4, 8.5), (9, 8), (12, 12), (11, 21), (5, 22),
                      (2, 19)], colour=WHITE)      # the comma rudder
        cross(c, *R([(7, 15)])[0], 3.6)
        aerofoil(c, R, 22, 9, 19.0, 1.4, body)
        fuselage(c, R, [(10, 18), (30, 16.5), (58, 14), (78, 14.5), (84, 16)],
                 [(10, 22), (30, 26), (58, 29.5), (78, 29.5), (84, 26.5)], body)
        c.part(R([(80, 14.5), (90, 16.5), (90, 25.5), (80, 27)]), METAL,
               gloss=0.55, round_=2.2)             # the rotary's cowl
        c.part(R([(57, 13), (63, 13), (62, 15.2), (58, 15.2)]), DARK,
               round_=0.4)
        aerofoil(c, R, 80, 24, 14.6, 3.0, body,
                 (150, 176, 196))                  # the shoulder wing
        strut(c, R, (62, 4), (62, 13.5), DARK, 1.6)          # the pylon
        for a_, b_ in (((62, 4), (46, 14.5)), ((62, 4), (78, 14.5)),
                       ((68, 36), (52, 15.5)), ((68, 36), (78, 15.5))):
            c.line(R([a_])[0], R([b_])[0], (60, 56, 50), 0.55)
        cross(c, *R([(36, 20)])[0], 4.2)
        upper = lower = None
        struts = cabanes = ()
        wheel_x = (74, 68)
    else:                                          # a Fokker D.VII type
        body = (150, 140, 108)
        c.part(R([(2, 9), (7, 7), (12, 7.5), (13, 21.5), (4, 22), (2, 16)]),
               WHITE, gloss=0.15, round_=0.6)      # fin and rudder
        cross(c, *R([(8, 14.5)])[0], 3.8)
        aerofoil(c, R, 24, 10, 19.0, 1.8, LOZENGE[1])
        fuselage(c, R, [(10, 17.5), (30, 15.5), (58, 12.5), (80, 13), (88, 14)],
                 [(10, 22.5), (30, 27), (58, 30.5), (80, 30.5), (88, 28)], body)
        c.part(R([(82, 13), (90, 14), (90, 27), (82, 28)]), METAL, gloss=0.45,
               round_=0.8)                         # the car-type radiator
        for k in range(5):
            c.line(R([(89.5, 15 + k * 2.6)])[0], R([(85, 15 + k * 2.6)])[0],
                   (70, 68, 64), 0.35)
        c.part(R([(56, 12.2), (62, 12.2), (61, 14.6), (57, 14.6)]), DARK,
               round_=0.4)
        upper, lower = (78, 26, 1.8), (74, 22, 31.0)
        struts = ((74, 2.6, 69, 30.3), (58, 2.6, 56, 30.3))
        cabanes = ((78, 2.6, 80, 13.2), (68, 2.6, 69, 12.8))
        cross(c, *R([(40, 21)])[0], 4.4)
        wheel_x = (76, 70)
    # The planes: the lower first, then struts, then the upper over them.
    # The gap closes up a little from the true one, and the lower plane
    # hangs just under the belly, as on the painted Sopwith, so both read
    # as wings at sprite size.
    gap = 2.5
    if upper:
        upper = (upper[0], upper[1], upper[2] + gap)
        lower = (lower[0], lower[1], lower[2] + 1.6)
        struts = tuple((a, b + gap, c_, d + 1.6) for a, b, c_, d in struts)
        cabanes = tuple((a, b + gap, c_, d) for a, b, c_, d in cabanes)
    thick = 5.0 if kind == "enemy_late" else 4.0
    wing_colour = LOZENGE[0] if kind == "enemy_late" else body
    under = (150, 176, 196) if kind in ("enemy_late", "mono") else \
        (214, 204, 170)
    if lower:
        aerofoil(c, R, lower[0], lower[1], lower[2], thick * 1.6, under, under)
    for x0, y0, x1, y1 in cabanes:
        strut(c, R, (x0, y0), (x1, y1), DARK if kind == "enemy_late" else WOOD,
              1.3)
    for x0, y0, x1, y1 in struts:
        strut(c, R, (x0, y0), (x1, y1), DARK if kind == "enemy_late" else WOOD)
    if kind == "enemy_late":                       # the N: a diagonal
        strut(c, R, (struts[1][0], struts[1][1]), (struts[0][2], struts[0][3]),
              DARK, 1.3)
    if upper:
        aerofoil(c, R, upper[0], upper[1], upper[2], thick, wing_colour, under)
        # Flying and landing wires across the bay.
        if kind != "enemy_late":
            c.line(R([(struts[1][2], struts[1][3])])[0], R([(struts[0][0], struts[0][1])])[0],
                   (96, 90, 80), 0.32)
            c.line(R([(struts[1][0], struts[1][1])])[0], R([(struts[0][2], struts[0][3])])[0],
                   (96, 90, 80), 0.32)
    body_dark = tuple(max(0, v - 34) for v in body)
    body_lit = tuple(min(255, v + 30) for v in body)
    # The longerons: sun along the top decking, the belly in shade.
    c.line(R([(18, 17.4)])[0], R([(78, 14.6)])[0], body_lit, 0.6)
    c.line(R([(18, 21.6)])[0], R([(76, 26.4)])[0], body_dark, 0.6)
    for x in range(24, 76, 10):
        c.line(R([(x, 17)])[0], R([(x, 24)])[0], body_dark, 0.35)
    cockpit = {"twoseat": 64, "late": 59, "mono": 60, "enemy_late": 59}[kind]
    hx, hy = R([(cockpit, 12.0)])[0]
    c.part([(hx - 2.3, hy - 2.5), (hx + 2.3, hy + 1.8)], (84, 56, 34),
           shape="ellipse", gloss=0.3, grain=3, rim=0.25, round_=0.2)
    gx, gy = R([(cockpit + 1.6, 11.4)])[0]
    c.part([(gx - 0.9, gy - 0.7), (gx + 0.9, gy + 0.7)], (190, 200, 210),
           shape="ellipse", gloss=0.6, round_=0.1)
    if kind == "twoseat":                          # the observer behind him
        ox, oy = R([(50, 12.2)])[0]
        c.part([(ox - 2.2, oy - 2.4), (ox + 2.2, oy + 1.8)], (74, 52, 34),
               shape="ellipse", gloss=0.3, round_=0.2)
    c.line(R([(78, 25.6)])[0], R([(68, 27.4)])[0], (54, 46, 40), 0.9)  # exhaust
    if kind != "twoseat":
        # The fixed gun on the decking, its muzzle where the tracers start.
        mx, my = MUZZLE[kind]
        c.part(R([(mx - 18, my - 1.1), (mx, my - 0.9), (mx, my + 0.9),
                  (mx - 18, my + 1.3)]), (52, 54, 58), gloss=0.5, round_=0.3)
        c.part(R([(mx - 18, my - 1.8), (mx - 9, my - 1.6), (mx - 9, my + 1.6),
                  (mx - 18, my + 1.8)]), (36, 38, 42), gloss=0.4,
               round_=0.3)                         # its breech

    for k in range(3):                             # oil streaks
        y = 18 + k * 2.2
        c.line(R([(76, y)])[0], R([(58 - k * 4, y + 0.6)])[0],
               tuple(max(0, v - 46) for v in body), 0.35)
    # Undercarriage: a V of struts to the axle, the wheel with its hub.
    a, b = wheel_x
    strut(c, R, (a, 28.5), (b, 36.5), DARK, 1.4)
    strut(c, R, (a - 9, 28.5), (b, 36.5), DARK, 1.4)
    wx, wy = R([(b, 37)])[0]
    c.part([(wx - 5.2, wy - 5.2), (wx + 5.2, wy + 5.2)], (40, 38, 36),
           shape="ellipse", gloss=0.2, round_=0.3)
    c.part([(wx - 3.4, wy - 3.4), (wx + 3.4, wy + 3.4)], (150, 140, 116),
           shape="ellipse", gloss=0.3, round_=0.2)     # the doped wheel cover
    c.part([(wx - 1.2, wy - 1.2), (wx + 1.2, wy + 1.2)], METAL,
           shape="ellipse", round_=0.1)
    sx, sy = R([(10, 21.5)])[0]
    c.line((sx, sy), (sx - 3, sy + 4), DARK, 0.8)  # the tailskid
    px, py = R([(92, 20.5)])[0]
    c.part([(px - 1.3, py - 12), (px + 1.3, py + 12)], (140, 110, 70),
           shape="ellipse", gloss=0.0, grain=4, rim=0.0, round_=0.2)
    c.part([(px - 1.4, py - 1.8), (px + 1.6, py + 1.8)], METAL,
           shape="ellipse", gloss=0.5, round_=0.2)     # the boss
    return c.image((96, 48))


MUZZLE = {"twoseat": (60, 13), "late": (82, 12.5), "mono": (80, 14.5),
          "enemy_late": (78, 12.5)}


def muzzles(kind, enemy):
    """The gun muzzle in each frame of the finished 96x48 atlas."""
    out = []
    for f in range(5):
        frame = 4 - f if enemy else f
        pitch = (2 - frame) * STEP
        x, y = MUZZLE[kind]
        x, y = 55 + (x - 48) * 0.92, 27.5 + (y - 19) * 1.2
        x, y = rot([(x, y)], -pitch, (55, 27.5))[0]
        x, y = x * 96 / 110, y * 48 / 55
        if enemy:
            x = 96 - x
        out.append((round(x), round(y)))
    return out


def side_atlas(kind, enemy):
    sheet = Image.new("RGB", (480, 48), KEY)
    for f in range(5):
        img = side_frame(kind, 4 - f if enemy else f)
        if enemy:
            img = img.transpose(Image.FLIP_LEFT_RIGHT)
        sheet.paste(img, (f * 96, 0))
    return sheet


def top_view(kind):
    """From directly above, nose to the top: the upper plane over the
    lower, which shows behind it by the stagger; a fuselage swelling at
    the cockpits and tapering to the tail."""
    c = Canvas(78, 79, light=(-0.6, -0.8))
    if kind == "twoseat":
        wing, body, span, chord, lower_span = CLEAR_DOPE, CLEAR_DOPE, 37, 12, 33
    else:
        wing, body, span, chord, lower_span = PC10, PC10, 34, 11, 30
    shade = tuple(int(v * 0.72) for v in wing)
    cx, le = 39, 19

    def plane(y, half, colour, **kw):
        # Raked tips: the leading edge runs out further than the trailing.
        c.part([(cx - half + 3, y), (cx + half - 3, y), (cx + half, y + 2.5),
                (cx + half - 1.5, y + chord), (cx - half + 1.5, y + chord),
                (cx - half, y + 2.5)], colour, **kw)

    plane(le + 4, lower_span, shade, gloss=0.15, grain=4, round_=1.2)
    # The tailplane and elevators, with the fin a thin line along the middle.
    c.part([(cx - 13, 62), (cx + 13, 62), (cx + 14, 65), (cx + 12, 70),
            (cx - 12, 70), (cx - 14, 65)], wing, gloss=0.25, grain=3,
           round_=1.5)
    c.line((cx - 12, 66.5), (cx + 12, 66.5), shade, 0.5)
    # The fuselage: widest at the cockpits, tapering away to the sternpost.
    c.part([(cx - 4.5, 6), (cx + 4.5, 6), (cx + 5, 24), (cx + 4.4, 42),
            (cx + 1.6, 72), (cx - 1.6, 72), (cx - 4.4, 42), (cx - 5, 24)],
           body, gloss=0.35, grain=4, round_=1.6)
    c.line((cx + 2.8, 26), (cx + 1, 70), tuple(int(v * 0.8) for v in body), 0.5)
    c.line((cx, 60), (cx, 73), DARK, 0.7)          # the fin from above
    plane(le, span, wing, gloss=0.3, grain=4, round_=1.2)
    for k in range(1, 12):                         # rib tapes
        x = cx - span + k * span * 2 / 12
        if abs(x - cx) > 5:
            c.line((x, le + 1), (x, le + chord - 1),
                   tuple(min(255, v + 14) for v in wing), 0.35)
    for side in (-1, 1):                           # ailerons
        c.line((cx + side * span * 0.55, le + chord * 0.7),
               (cx + side * (span - 1.5), le + chord * 0.7), shade, 0.5)
    c.part([(cx - 4, le + 1), (cx + 4, le + 1), (cx + 4, le + chord - 1),
            (cx - 4, le + chord - 1)], shade, gloss=0.1,
           round_=0.6)                             # the centre-section cut-out
    c.part([(cx - 4.6, 2), (cx + 4.6, 2), (cx + 5, 9), (cx - 5, 9)], METAL,
           gloss=0.55, round_=2.0)                 # the cowl
    for y in (26, 36) if kind == "twoseat" else (31,):
        c.part([(cx - 3, y), (cx + 3, y + 5)], DARK, shape="ellipse",
               round_=0.6)                         # cockpits
        c.part([(cx - 1.8, y + 0.6), (cx + 1.8, y + 3.6)], (84, 56, 34),
               shape="ellipse", gloss=0.4, round_=0.3)  # a leather helmet
    if kind == "late":
        c.part([(cx - 1, le - 3), (cx + 1, le - 3), (cx + 1, le + 6),
                (cx - 1, le + 6)], DARK, round_=0.2)    # the Lewis gun
    stations = (0.38, 0.72) if kind == "twoseat" else (0.6,)
    for f in stations:
        for side in (-1, 1):
            for y in (le + chord * 0.25, le + chord * 0.72):
                x = cx + side * span * f
                c.part([(x + 1, y + 1), (x + 3.4, y + 3.4)], (40, 34, 28),
                       shape="ellipse", gloss=0.0, rim=0.0, round_=0.2)
                c.part([(x - 1.2, y - 1.2), (x + 1.2, y + 1.2)], (84, 56, 36),
                       shape="ellipse", gloss=0.4, round_=0.1)
    for x in (cx - span * 0.8, cx + span * 0.8):
        roundel(c, x, le + chord / 2, chord * 0.4)
    c.part([(cx - 16, 0.5), (cx + 16, 3)], (140, 110, 70), shape="ellipse",
           gloss=0.0, rim=0.0, round_=0.2)         # propeller disc
    return c.image()


def main():
    header = ["/* Generated by ports/amigaos3/make_machine_sprites.py: do not edit. */",
              "#ifndef CANVAS_MACHINE_SPRITES_H", "#define CANVAS_MACHINE_SPRITES_H",
              "", "/* Gun muzzles per pitch frame: our two-seater (the observer's",
              " * carbine), our late scout, their monoplane, their late biplane. */",
              "static const unsigned char machine_muzzle[4][5][2] = {"]
    for kind, enemy in (("twoseat", False), ("late", False), ("mono", True),
                        ("enemy_late", True)):
        header.append("    {" + ", ".join(f"{{{x}, {y}}}" for x, y in
                                           muzzles(kind, enemy)) + "},")
    header += ["};", "", "#endif", ""]
    (ROOT / "src" / "modern" / "canvas_machine_sprites.h").write_text("\n".join(header))
    side_atlas("twoseat", False).save(ASSETS / "aircraft-player-twoseat.bmp")
    side_atlas("late", False).save(ASSETS / "aircraft-player-late.bmp")
    side_atlas("mono", True).save(ASSETS / "aircraft-enemy-mono.bmp")
    side_atlas("enemy_late", True).save(ASSETS / "aircraft-enemy-late.bmp")
    top_view("twoseat").save(ASSETS / "topdown-player-twoseat.bmp")
    top_view("late").save(ASSETS / "topdown-player-late.bmp")


if __name__ == "__main__":
    main()
