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


def side_frame(kind, frame):
    """One side-on frame, facing right, nose up by the frame's pitch."""
    # Painted on a slightly larger canvas and filtered down to 96x48, with
    # the body drawn a little shorter and deeper than its sketch so the
    # machine has the chunky profile of the existing Sopwith and Albatros.
    c = Canvas(110, 55, light=(0.2, -1.0))
    pitch = (2 - frame) * STEP
    o = (55, 27.5)

    def R(pts):
        return rot([(55 + (x - 48) * 0.92, 27.5 + (y - 21) * 1.3)
                    for x, y in pts], -pitch, o)

    if kind == "twoseat":
        body, wing = CLEAR_DOPE, CLEAR_DOPE
        c.part(R([(8, 19), (24, 16.5), (60, 13.5), (80, 14.5), (86, 19),
                  (86, 27), (60, 29.5), (24, 24.5), (8, 21.5)]), body,
               gloss=0.2, round_=1.2)
        c.part(R([(78, 16), (90, 18), (90, 27), (78, 27)]), METAL, gloss=0.4,
               round_=0.8)                         # engine cowling
        for x in (50, 64):                         # two cockpits
            c.part(R([(x - 4, 14), (x + 4, 14), (x + 3, 16), (x - 3, 16)]), DARK,
                   round_=0.5)
        c.part(R([(30, 6.5), (88, 5.5), (88, 9.5), (30, 10.5)]), wing,
               gloss=0.25, round_=0.8)             # upper wing, seen edge on
        c.part(R([(30, 28.5), (84, 28.5), (84, 32), (30, 32)]), wing,
               gloss=0.25, round_=0.8)             # lower wing
        # Two bays each side, as on the B.E.2: the near struts stand in two
        # front-and-rear pairs, the far pair a shade behind them; short
        # cabane struts carry the centre section over the front cockpit.
        for x in (40, 50, 66, 76):
            c.line(R([(x + 1.5, 9.5)])[0], R([(x + 0.5, 28.5)])[0], (60, 44, 30), 1.2)
        for x in (38, 48, 64, 74):
            c.line(R([(x, 9.5)])[0], R([(x - 1, 28.5)])[0], WOOD, 1.7)
        for x in (70, 78):
            c.line(R([(x, 9.5)])[0], R([(x + 1, 15)])[0], WOOD, 1.4)
        c.part(R([(8, 9), (16, 9), (20, 17), (8, 18)]), wing, round_=0.8)  # fin
        for k, col in enumerate((BLUE, WHITE, RED)):          # rudder stripes
            c.part(R([(2 + k * 2.4, 10), (4.4 + k * 2.4, 10), (4.4 + k * 2.4, 22),
                      (2 + k * 2.4, 22)]), col, round_=0.2, rim=0.1)
        roundel(c, *R([(38, 21.5)])[0], 5.0)
        wheel_x = (74, 64)
    elif kind == "late":
        body, wing = PC10, PC10
        c.part(R([(8, 19.5), (26, 16.5), (66, 12.5), (86, 13.5), (88, 20),
                  (86, 28.5), (60, 30), (26, 24.5), (8, 21.5)]), body,
               gloss=0.22, round_=1.4)
        c.part(R([(82, 13), (90, 14), (90, 28), (82, 28)]), METAL, gloss=0.45,
               round_=0.6)                         # the flat radiator nose
        c.part(R([(56, 12.5), (62, 12.5), (61, 15), (57, 15)]), DARK,
               round_=0.4)
        c.part(R([(60, 9.5), (72, 9.5), (72, 11), (60, 11)]), DARK,
               round_=0.2)                         # the wing gun
        c.part(R([(36, 5.5), (84, 5), (84, 9), (36, 9.5)]), wing, gloss=0.25,
               round_=0.8)
        c.part(R([(40, 28.5), (82, 28.5), (82, 32), (40, 32)]), wing,
               gloss=0.25, round_=0.8)
        # One bay, a front and rear strut each side, and the cabanes.
        for x in (54, 76):
            c.line(R([(x + 1.5, 9)])[0], R([(x - 0.5, 28.5)])[0], (60, 44, 30), 1.2)
        for x in (52, 74):
            c.line(R([(x, 9)])[0], R([(x - 2, 28.5)])[0], WOOD, 1.8)
        for x in (72, 80):
            c.line(R([(x, 9)])[0], R([(x + 1, 13.5)])[0], WOOD, 1.4)
        c.part(R([(6, 8), (16, 8), (22, 17), (6, 19)]), body, round_=1.0)
        for k, col in enumerate((BLUE, WHITE, RED)):
            c.part(R([(2 + k * 2.4, 9), (4.4 + k * 2.4, 9), (4.4 + k * 2.4, 22),
                      (2 + k * 2.4, 22)]), col, round_=0.2, rim=0.1)
        roundel(c, *R([(40, 21.5)])[0], 5.2)
        wheel_x = (78, 70)
    elif kind == "mono":
        body, wing = STREAKED, STREAKED
        c.part(R([(8, 19.5), (26, 17), (66, 13.5), (82, 14.5), (86, 20),
                  (84, 27.5), (62, 29), (26, 24.5), (8, 21.5)]), body,
               gloss=0.2, round_=1.4)
        c.part(R([(80, 14), (90, 17), (90, 26), (80, 28)]), METAL, gloss=0.5,
               round_=2.0)                         # the round rotary cowl
        c.part(R([(58, 12.5), (64, 12.5), (63, 15), (59, 15)]), DARK,
               round_=0.4)
        c.part(R([(50, 14), (80, 14), (80, 16.5), (50, 16.5)]), wing,
               gloss=0.25, round_=0.6)             # the shoulder wing
        c.line(R([(62, 13)])[0], R([(62, 4)])[0], DARK, 1.6)   # the pylon
        c.line(R([(62, 4)])[0], R([(44, 14)])[0], (60, 56, 50), 0.6)
        c.line(R([(62, 4)])[0], R([(82, 14)])[0], (60, 56, 50), 0.6)
        c.line(R([(68, 36)])[0], R([(50, 16.5)])[0], (60, 56, 50), 0.6)  # landing
        c.line(R([(68, 36)])[0], R([(80, 16.5)])[0], (60, 56, 50), 0.6)
        c.line(R([(62, 5)])[0], R([(52, 14)])[0], (60, 56, 50), 0.5)
        c.line(R([(62, 5)])[0], R([(76, 14)])[0], (60, 56, 50), 0.5)
        c.part(R([(6, 10), (14, 9), (20, 18), (6, 20)]), WHITE, round_=1.2)
        cross(c, *R([(12, 14.5)])[0], 4.2)
        cross(c, *R([(38, 21.5)])[0], 4.6)
        wheel_x = (74, 68)
    else:                                          # "late" enemy biplane
        body = LOZENGE[0]
        c.part(R([(8, 19.5), (26, 16.5), (66, 12.5), (84, 13.5), (88, 20),
                  (86, 28.5), (60, 30), (26, 24.5), (8, 21.5)]),
               (150, 140, 108), gloss=0.2, round_=1.2)
        c.part(R([(78, 13), (90, 14), (90, 28), (78, 28)]), METAL, gloss=0.45,
               round_=0.6)
        c.part(R([(56, 12), (62, 12), (61, 15), (57, 15)]), DARK, round_=0.4)
        c.part(R([(34, 6), (82, 6), (82, 9), (34, 9)]), body, gloss=0.2,
               round_=0.8)                         # thick square upper wing
        c.part(R([(42, 28.5), (78, 28.5), (78, 31.5), (42, 31.5)]),
               LOZENGE[1], gloss=0.2, round_=0.8)
        for x in (74, 80):                         # cabanes
            c.line(R([(x, 9)])[0], R([(x + 1, 13.5)])[0], DARK, 1.4)
        for x in (52, 72):                         # N-struts
            c.line(R([(x, 9)])[0], R([(x - 3, 28.5)])[0], DARK, 1.7)
        c.line(R([(52, 9)])[0], R([(69, 28.5)])[0], DARK, 1.3)
        c.part(R([(6, 9), (16, 9), (22, 18), (6, 20)]), WHITE, round_=1.2)
        cross(c, *R([(12, 14.5)])[0], 4.2)
        cross(c, *R([(40, 21.5)])[0], 4.6)
        wheel_x = (74, 68)
    # Detail common to every type: panel lines down the fuselage, a lit
    # longeron, the pilot's leather helmet and goggles in the cockpit,
    # louvres and an exhaust on the cowling, rigging wires between the wings.
    body_dark = tuple(max(0, v - 34) for v in body)
    body_lit = tuple(min(255, v + 30) for v in body)
    # The belly in shadow, the sun on the top decking.
    c.part(R([(26, 23.5), (60, 26.5), (84, 25.5), (86, 27.5), (60, 29.6),
              (26, 24.4)]), tuple(int(v * 0.62) for v in body), gloss=0.0,
           grain=5, rim=0.2, round_=0.6)
    c.part(R([(26, 16.8), (60, 13.9), (80, 14.8), (80, 15.8), (60, 15.2),
              (26, 17.6)]), body_lit, gloss=0.1, grain=4, rim=0.1, round_=0.4)
    for x in range(22, 80, 9):
        c.line(R([(x, 16)])[0], R([(x, 26)])[0], body_dark, 0.45)
    c.line(R([(18, 17.5)])[0], R([(78, 15)])[0], body_lit, 0.6)
    c.line(R([(18, 24.5)])[0], R([(76, 27)])[0], body_dark, 0.6)
    cockpit = {"twoseat": 64, "late": 59, "mono": 61, "enemy_late": 59}[kind]
    hx, hy = R([(cockpit, 12.2)])[0]
    c.part([(hx - 2.4, hy - 2.6), (hx + 2.4, hy + 1.8)], (84, 56, 34),
           shape="ellipse", gloss=0.3, grain=3, rim=0.25, round_=0.2)
    gx, gy = R([(cockpit + 1.6, 11.6)])[0]
    c.part([(gx - 0.9, gy - 0.7), (gx + 0.9, gy + 0.7)], (190, 200, 210),
           shape="ellipse", gloss=0.6, round_=0.1)
    if kind == "twoseat":                          # the observer behind him
        ox, oy = R([(50, 12.2)])[0]
        c.part([(ox - 2.2, oy - 2.4), (ox + 2.2, oy + 1.8)], (74, 52, 34),
               shape="ellipse", gloss=0.3, round_=0.2)
    for k in range(3):                             # cowling louvres
        x = 80 + k * 2.4
        c.line(R([(x, 17)])[0], R([(x, 25)])[0], (70, 68, 64), 0.4)
    c.line(R([(76, 26.5)])[0], R([(68, 28.5)])[0], (54, 46, 40), 1.0)  # exhaust
    if kind != "mono":
        top_y, low_y = 8.0, 30.0
        c.line(R([(40, top_y)])[0], R([(74, low_y)])[0], (64, 60, 56), 0.35)
        c.line(R([(74, top_y)])[0], R([(40, low_y)])[0], (64, 60, 56), 0.35)
    # Weathering: oil streaks back from the cowling, scuffs on the fabric.
    for k in range(4):
        y = 18 + k * 2.0
        c.line(R([(74, y)])[0], R([(56 - k * 3, y + 0.6)])[0],
               tuple(max(0, v - 50) for v in body), 0.35)
    if kind != "mono":
        for x in range(36, 84, 5):                 # rib tapes on the wings
            c.line(R([(x, 6.4)])[0], R([(x, 8.6)])[0], (220, 210, 180), 0.35)
            c.line(R([(x, 28.8)])[0], R([(x, 31.2)])[0], (220, 210, 180), 0.35)
    # Undercarriage and wheels.
    a, b = wheel_x
    c.line(R([(a, 27)])[0], R([(b, 38)])[0], DARK, 0.9)
    c.line(R([(a - 8, 27)])[0], R([(b, 38)])[0], DARK, 0.9)
    wx, wy = R([(b, 38)])[0]
    c.part([(wx - 5.5, wy - 5.5), (wx + 5.5, wy + 5.5)], (40, 38, 36), shape="ellipse",
           gloss=0.15, round_=0.3)
    c.part([(wx - 1.6, wy - 1.6), (wx + 1.6, wy + 1.6)], METAL,
           shape="ellipse", round_=0.2)
    # The tailskid and the spinning propeller, a blur of brown.
    sx, sy = R([(10, 22)])[0]
    c.line((sx, sy), (sx - 3, sy + 4), DARK, 0.8)
    px, py = R([(92, 21)])[0]
    c.part([(px - 1.4, py - 12), (px + 1.4, py + 12)], (140, 110, 70),
           shape="ellipse", gloss=0.0, grain=4, rim=0.0, round_=0.2)
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
        x, y = 55 + (x - 48) * 0.92, 27.5 + (y - 21) * 1.3
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
    """From directly above, nose to the top."""
    c = Canvas(78, 79, light=(-0.6, -0.8))
    if kind == "twoseat":
        wing, body, span, chord = CLEAR_DOPE, CLEAR_DOPE, 37, 12
    else:
        wing, body, span, chord = PC10, PC10, 34, 11
    cx = 39
    c.part([(cx - span, 20), (cx + span, 20), (cx + span, 20 + chord),
            (cx - span, 20 + chord)], wing, gloss=0.2, round_=2.0)
    for k in range(1, 12):                         # rib tapes
        x = cx - span + k * span * 2 / 12
        c.line((x, 21), (x, 19 + chord), tuple(max(0, v - 18) for v in wing),
               0.5)
    c.part([(cx - 4, 8), (cx + 4, 8), (cx + 3, 66), (cx - 3, 66)], body,
           gloss=0.25, round_=2.0)
    c.part([(cx - 4.5, 4), (cx + 4.5, 4), (cx + 4.5, 11), (cx - 4.5, 11)],
           METAL, gloss=0.45, round_=1.5)
    c.part([(cx - 14, 62), (cx + 14, 62), (cx + 12, 70), (cx - 12, 70)], wing,
           gloss=0.2, round_=1.5)                  # tailplane
    c.part([(cx - 3, 26), (cx + 3, 26), (cx + 3, 31), (cx - 3, 31)], DARK,
           round_=1.0)                             # cockpit
    if kind == "twoseat":
        c.part([(cx - 3, 36), (cx + 3, 36), (cx + 3, 41), (cx - 3, 41)], DARK,
               round_=1.0)
    # Strut stations: the tops of the front and rear interplane struts (and,
    # on the two-seater, the outer bay too), with the shadow they cast.
    stations = (0.38, 0.72) if kind == "twoseat" else (0.6,)
    for f in stations:
        for side in (-1, 1):
            for y in (20 + chord * 0.25, 20 + chord * 0.72):
                x = cx + side * span * f
                c.part([(x + 1, y + 1), (x + 3.4, y + 3.4)], (40, 34, 28),
                       shape="ellipse", gloss=0.0, rim=0.0, round_=0.2)
                c.part([(x - 1.2, y - 1.2), (x + 1.2, y + 1.2)], (84, 56, 36),
                       shape="ellipse", gloss=0.4, round_=0.1)
    for x in (cx - span * 0.82, cx + span * 0.82):
        roundel(c, x, 20 + chord / 2, chord * 0.42)
    c.part([(cx - 16, 1), (cx + 16, 3.5)], (140, 110, 70), shape="ellipse",
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
