#!/usr/bin/env python3
"""Paint the pilot's Webley in hand for 1914.

Each part is painted on the host at four times its size: a shaded fill
lit from the low sun ahead and to the right, a soft rim of shadow inside
its edge, a fine grain, and highlights along the lit edges; the result is
filtered down, so edges come out soft and the palette matches the
cockpit painting.  Magenta is the transparent key, as in the other atlases.

  assets/canvas-skies/cockpit-revolver.bmp  two frames, 220x170: at rest
                                            and kicking up with the flash

The observer's carbine is rendered in perspective by make_rear_carbine.py.
"""

import math
from pathlib import Path
import sys

from PIL import Image

sys.path.insert(0, str(Path(__file__).resolve().parent))
from paint import Canvas, KEY, rot  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"

LEATHER = (104, 66, 38)
LEATHER_DARK = (70, 44, 26)
STEEL = (44, 46, 52)
GRIP = (52, 34, 24)
WOOD = (142, 86, 44)
GLOVE = (58, 46, 38)
SLEEVE = (86, 78, 52)


def gloved_fingers(c, R, cap, tips, w, colour=LEATHER):
    """Fingers curled round something: each a capsule with a knuckle crease
    and a lit top edge."""
    for p0, p1 in tips:
        cap(p0, p1, w, colour, gloss=0.2, round_=1.6)
        mx, my = (p0[0] * 0.55 + p1[0] * 0.45), (p0[1] * 0.55 + p1[1] * 0.45)
        q = R([(mx, my - w * 0.32), (mx, my + w * 0.32)])
        c.line(q[0], q[1], LEATHER_DARK, 0.8)


def revolver(kick):
    """A Webley Mk VI held out at arm's length, pointing ahead and left."""
    c = Canvas(220, 170)
    lift = -12 * kick
    o = (160, 112 + lift)                 # the wrist, pivot of the kick
    a = -0.16 - 0.24 * kick

    def R(pts):
        return rot([(x, y + lift) for x, y in pts], a, o)

    def cap(p0, p1, w, colour, **kw):
        q = R([p0, p1])
        c.part([q[0], q[1], w], colour, shape="capsule", **kw)

    def poly(pts, colour, **kw):
        c.part(R(pts), colour, **kw)

    # Khaki sleeve and the flared leather gauntlet, stitched at the cuff.
    c.part([(224, 174), (224, 108), (196, 94), (160, 120), (170, 174)],
           SLEEVE, gloss=0.08, grain=4, round_=3)
    poly([(214, 140), (186, 100), (156, 108), (150, 136), (170, 158),
          (214, 166)], LEATHER_DARK, gloss=0.12, grain=4, round_=4)
    c.line(R([(186, 104)])[0], R([(164, 154)])[0], (126, 88, 54), 0.7)
    c.line(R([(190, 108)])[0], R([(168, 158)])[0], (40, 26, 16), 0.5)
    # The grip first: a bird's head of dark walnut with its chequering and
    # the lanyard ring, so the fingers can close over it.
    poly([(140, 94), (160, 92), (168, 118), (164, 140), (148, 146),
          (140, 124)], GRIP, gloss=0.18, grain=5, round_=3)
    c.part([R([(156, 148)])[0], R([(156, 148.2)])[0], 3.2], (90, 92, 100),
           shape="capsule", gloss=0.5, round_=0.6)       # lanyard ring
    # The barrel: round, with the raised rib on top, the foresight blade,
    # the lug beneath and the dark mouth.
    poly([(30, 70), (106, 68), (106, 84), (30, 82)], STEEL, gloss=0.35,
         grain=2, round_=1.6)
    poly([(32, 65), (106, 63), (106, 69), (32, 70)], (66, 68, 76),
         gloss=0.45, round_=0.8)                         # the rib
    poly([(33, 64), (40, 63), (39, 57), (35, 57)], STEEL, round_=0.4)
    poly([(80, 82), (106, 82), (106, 92), (90, 89)], STEEL, gloss=0.3,
         round_=1.2)                                     # barrel lug
    c.part(R([(27, 72), (32, 81)]), (14, 14, 16), shape="ellipse", round_=0.4)
    # The fluted cylinder between barrel and frame.
    poly([(106, 64), (132, 64), (132, 96), (106, 96)], STEEL, gloss=0.4,
         grain=2, round_=2.8)
    for k in range(4):
        q = R([(109, 70 + k * 7), (129, 70 + k * 7)])
        c.line(q[0], q[1], (24, 24, 28), 1.4)            # the flutes
    c.line(R([(107, 66)])[0], R([(107, 94)])[0], (120, 122, 132), 0.8)
    # Top strap, the barrel catch, the frame and the hammer spur.
    poly([(104, 58), (152, 58), (152, 64), (104, 64)], STEEL, gloss=0.4,
         round_=1)
    poly([(144, 56), (154, 56), (156, 70), (146, 70)], (60, 62, 70),
         gloss=0.5, round_=1)                            # barrel catch
    poly([(130, 62), (158, 62), (160, 96), (132, 98)], STEEL, gloss=0.3,
         grain=2, round_=2)
    poly([(152, 62), (164, 48), (171, 51), (160, 68)], STEEL, gloss=0.4,
         round_=1)
    for k in range(3):
        q = R([(157 + k * 3, 56 - k * 3), (161 + k * 3, 58 - k * 3)])
        c.line(q[0], q[1], (90, 92, 100), 0.6)           # chequered spur
    # Trigger guard and trigger.
    for p0, p1 in (((130, 98), (124, 108)), ((124, 108), (132, 116)),
                   ((132, 116), (146, 110))):
        cap(p0, p1, 2.6, STEEL, gloss=0.4, round_=0.4)
    poly([(136, 98), (140, 98), (138, 110), (134, 108)], (30, 30, 34),
         round_=0.4)
    # The gloved hand: back of the hand and the thumb laid along the frame,
    # the forefinger through the guard, three fingers round the grip.
    poly([(158, 88), (190, 94), (198, 128), (174, 148), (158, 132)],
         LEATHER, gloss=0.16, grain=4, round_=5)
    cap((176, 92), (140, 84), 10.0, LEATHER, gloss=0.22, round_=2)
    c.line(R([(150, 82)])[0], R([(142, 82)])[0], (150, 112, 74), 0.6)
    gloved_fingers(c, R, cap, [((158, 104), (132, 106)), ((166, 117), (140, 120)),
                         ((168, 128), (142, 131)), ((166, 139), (144, 140))],
                   8.6)
    c.line(R([(176, 98)])[0], R([(170, 140)])[0], LEATHER_DARK, 0.8)
    if kick > 0.5:
        # The flash: a short bright core and spikes thrown forward, a puff
        # of grey smoke rolling out ahead of it.
        m = R([(26, 76)])[0]
        dx, dy = math.cos(a), math.sin(a)
        for k, (ang, ln) in enumerate(((0, 30), (0.5, 16), (-0.5, 18),
                                       (1.3, 10), (-1.3, 9))):
            ca, sa = math.cos(ang), math.sin(ang)
            ux, uy = -(dx * ca - dy * sa), -(dx * sa + dy * ca)
            tip = (m[0] + ux * ln, m[1] + uy * ln)
            side = (-uy * 3, ux * 3)
            c.part([(m[0] + side[0], m[1] + side[1]), tip,
                    (m[0] - side[0], m[1] - side[1])],
                   (255, 214, 120) if k else (255, 236, 170), gloss=0,
                   rim=0, round_=0.4)
        c.glow(m, 20, (255, 190, 100), 0.7)
        c.glow(m, 7, (255, 250, 230), 1.0)
        for k in range(3):
            q = (m[0] - dx * (34 + k * 10), m[1] - dy * (34 + k * 10) - k * 3)
            r = 6 + k * 3
            c.part([(q[0] - r, q[1] - r * 0.7), (q[0] + r, q[1] + r * 0.7)],
                   (150, 146, 138), shape="ellipse", gloss=0, rim=0.1,
                   round_=r * 0.6)
    return c.image()


def main():
    frames = [revolver(0.0), revolver(1.0)]
    sheet = Image.new("RGB", (440, 170), KEY)
    for i, f in enumerate(frames):
        sheet.paste(f, (i * 220, 0))
    sheet.save(ASSETS / "cockpit-revolver.bmp")


if __name__ == "__main__":
    main()
