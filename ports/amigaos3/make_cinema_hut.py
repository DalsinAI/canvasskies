#!/usr/bin/env python3
"""Paint the picture house: an army hut behind the lines, run as a cinema.

Weathered planking with knots, nail rows and damp stains under a
corrugated-iron roof on king-post trusses; hand-lettered posters pinned
to the walls; a proscenium of moulded boards with a lettered fascia, a
fringed pelmet and red serge curtains drawn back on their tie-backs;
the projector's beam coming in over our heads through tobacco haze;
benches of soldiers seen from behind, rim-lit by the screen; the
pianist at his upright by a candle and the stove in the corner.
Painted with the shared painter (paint.py) at four times size and
filtered down.  The screen is left dark for the reels
(x 176..464, y 70..250).  Writes assets/canvas-skies/cinema-hut.bmp.
"""

import math
import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFilter, ImageFont

sys.path.insert(0, str(Path(__file__).resolve().parent))
from paint import Canvas, rot  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "canvas-skies" / "cinema-hut.bmp"
FONTS = ROOT / "assets" / "fonts"
RNG = np.random.default_rng(1917)
W, H = 640, 400
SCREEN = (176, 70, 464, 250)
EAVE = 96                       # where the walls meet the roof at the sides
TIMBER, TIMBER_DARK = (92, 64, 40), (52, 36, 24)


def lighting(xx, yy):
    """Warm from the stove and the lamp, cool and strong from the screen."""
    stove = np.exp(-(((xx - 600) / 130) ** 2 + ((yy - 250) / 120) ** 2))
    lamp = np.exp(-(((xx - 18) / 130) ** 2 + ((yy - 110) / 110) ** 2))
    screen = np.exp(-(((xx - 320) / 250) ** 2 + ((yy - 160) / 190) ** 2))
    return 0.22 + stove[..., None] * np.array([1.1, 0.68, 0.32]) + \
        lamp[..., None] * np.array([0.85, 0.66, 0.38]) + \
        screen[..., None] * np.array([0.62, 0.62, 0.6])


def room():
    """Walls of weathered boards and the corrugated roof above them."""
    yy, xx = np.mgrid[0:H, 0:W].astype(np.float32)
    base = np.zeros((H, W, 3), np.float32)
    board = (xx // 24).astype(int)
    tone = RNG.normal(0, 7, board.max() + 1)[board]
    grain = np.sin(yy * 0.11 + board * 2.7 + np.sin(xx * 0.3) * 0.8) * 4
    for c, v in enumerate((70, 50, 34)):
        base[..., c] = v + tone + grain
    base[(xx % 24) < 1.6] *= 0.5                         # the joints
    for b in range(board.max() + 1):                     # nail rows, knots
        for y in (EAVE + 30, 210, 330):
            for dx in (5, 18):
                x = b * 24 + dx
                base[max(0, y - 1):y + 1, x:x + 2] = (30, 26, 22)
        if RNG.random() < 0.35:
            kx, ky = b * 24 + RNG.uniform(6, 18), RNG.uniform(EAVE, H)
            ya, xa = int(ky) - 8, int(kx) - 8
            sub = (slice(max(ya, 0), ya + 16), slice(max(xa, 0), xa + 16))
            knot = np.exp(-(((xx[sub] - kx) / 3.5) ** 2 +
                            ((yy[sub] - ky) / 2.2) ** 2))
            base[sub] *= (1 - 0.45 * knot)[..., None]
    stain = np.zeros((H, W), np.float32)                 # damp runs
    for _ in range(9):
        sx = RNG.uniform(0, W)
        stain += np.exp(-((xx - sx) / RNG.uniform(6, 16)) ** 2) * \
            np.clip((yy - EAVE) / 160, 0, 1) * np.clip(1 - (yy - EAVE) / 240, 0, 1)
    base *= (1 - 0.3 * np.clip(stain, 0, 1))[..., None]
    # The roof: corrugated iron seen from beneath, running up to the ridge.
    roof = yy < EAVE
    flute = 0.5 + 0.5 * np.sin(xx * 0.9 + (yy * 0.15) * np.sign(xx - 320))
    iron = np.stack([52 + flute * 26, 50 + flute * 24, 48 + flute * 22], -1)
    rust = np.clip(RNG.normal(0, 1, (H, W)) * 0.4 + np.sin(xx * 0.05) * 0.4, 0, 1)
    iron += rust[..., None] * np.array([26, 8, -6])
    base[roof] = iron[roof]
    base += RNG.normal(0, 3.5, (H, W, 1))
    return Image.fromarray(np.clip(base * lighting(xx, yy), 0, 255)
                           .astype(np.uint8))


def trusses(c):
    """The wall plate, purlins and king-post trusses under the iron."""
    c.part([(0, EAVE - 6), (W, EAVE - 6), (W, EAVE + 4), (0, EAVE + 4)],
           TIMBER, gloss=0.25, grain=8, round_=1)        # wall plate
    for y in (18, 46, 72):
        c.part([(0, y), (W, y), (W, y + 6), (0, y + 6)], TIMBER_DARK,
               gloss=0.2, grain=6, round_=1)             # purlins
    for k, (y, s) in enumerate(((2, 1.0), (30, 0.72), (52, 0.52))):
        half = 340 * s
        tie_y = y + 64 * s
        apex = (320, y)
        left, right = (320 - half, tie_y), (320 + half, tie_y)
        w = 10 * s
        for a, b in ((left, apex), (apex, right), (left, right),
                     (apex, (320, tie_y))):
            c.part([a, b, w], TIMBER, shape="capsule", gloss=0.3, grain=8,
                   round_=0.5)
        for side in (-1, 1):
            c.part([(320 + side * half * 0.5, tie_y),
                    (320 + side * half * 0.22, y + 30 * s), 6.5 * s],
                   TIMBER_DARK, shape="capsule", gloss=0.2, round_=0.4)
            bolt = (320 + side * half * 0.5, tie_y)
            c.part([(bolt[0] - 2 * s, bolt[1] - 2 * s),
                    (bolt[0] + 2 * s, bolt[1] + 2 * s)], (30, 28, 26),
                   shape="ellipse", round_=0.3)


def lettering(text, font, size, colour, w, h, wobble=0.0, seed=0):
    """Brush lettering: each letter set a little off its line and angle,
    as a sign-writer's hand would leave it."""
    rng = np.random.default_rng(seed)
    f = ImageFont.truetype(str(FONTS / font), size * 4)
    img = Image.new("RGBA", (w * 4, h * 4), (0, 0, 0, 0))
    total = sum(f.getlength(ch) for ch in text)
    x = (w * 4 - total) / 2
    for ch in text:
        g = Image.new("RGBA", (size * 8, size * 8), (0, 0, 0, 0))
        ImageDraw.Draw(g).text((size * 2, size * 2), ch, font=f, fill=colour)
        g = g.rotate(rng.normal(0, 3) * wobble, resample=Image.BICUBIC)
        img.alpha_composite(g, (int(x - size * 2),
                                int(h * 2 - size * 2.6 + rng.normal(0, 2.5) * wobble
                                    - size * 2 + size * 1.4)))
        x += f.getlength(ch)
    return img.resize((w, h), Image.LANCZOS)


def poster(img, box, angle, paper, lines, seed):
    """A poster on the boards: foxed paper, its lines lettered by hand, a
    drawing pin at each top corner, curling a little at one corner."""
    x0, y0, x1, y1 = box
    w, h = x1 - x0, y1 - y0
    rng = np.random.default_rng(seed)
    p = np.zeros((h, w, 4), np.float32)
    p[..., :3] = paper
    yy, xx = np.mgrid[0:h, 0:w]
    p[..., :3] *= (1 - 0.18 * np.exp(-(((xx - rng.uniform(0, w)) / (w * 0.4)) ** 2
                                      + ((yy - rng.uniform(0, h)) / (h * 0.3)) ** 2)))[..., None]
    p[..., :3] += rng.normal(0, 5, (h, w, 1))
    edge = np.minimum(np.minimum(xx, w - 1 - xx), np.minimum(yy, h - 1 - yy))
    p[..., :3] *= (0.82 + 0.18 * np.clip(edge / 4, 0, 1))[..., None]
    p[..., 3] = 255
    p[h - 9:, w - 9:][(xx[h - 9:, w - 9:] - (w - 9)) + (yy[h - 9:, w - 9:] - (h - 9)) > 9, 3] = 0
    sheet = Image.fromarray(np.clip(p, 0, 255).astype(np.uint8), "RGBA")
    y = 4
    for text, font, size, colour, lh in lines:
        lt = lettering(text, font, size, colour + (255,), w - 8, lh, 1.0, seed + y)
        sheet.alpha_composite(lt, (4, y))
        y += lh
    d = ImageDraw.Draw(sheet)
    for px in (4, w - 6):
        d.ellipse((px - 1, 2, px + 2, 5), fill=(180, 170, 150, 255))
    sheet = sheet.rotate(angle, resample=Image.BICUBIC, expand=True)
    shadow = Image.new("RGBA", sheet.size, (0, 0, 0, 0))
    shadow.putalpha(sheet.getchannel("A").point(lambda v: int(v * 0.45)))
    shadow = shadow.filter(ImageFilter.GaussianBlur(2))
    img.alpha_composite(shadow, (x0 + 2, y0 + 3))
    img.alpha_composite(sheet, (x0, y0))


def posters(img):
    ink, red = (34, 26, 22), (150, 32, 26)
    fell, sc = "imfellenglish/IMFeENrm28P.ttf", "imfellenglish/IMFeENsc28P.ttf"
    sten, old = "stardosstencil/StardosStencil-Bold.ttf", "oldstandardtt/OldStandard-Bold.ttf"
    poster(img, (34, 118, 116, 212), 2.5, (212, 200, 168), [
        ("Y.M.C.A.", sc, 13, red, 18),
        ("PICTURES", old, 14, ink, 20),
        ("TO-NIGHT", old, 14, ink, 20),
        ("at 7", fell, 13, red, 18),
        ("ADMISSION FREE", sc, 8, ink, 12)], 3)
    poster(img, (512, 116, 572, 186), -3.0, (226, 216, 186), [
        ("CONCERT", sten, 11, red, 16),
        ("PARTY", sten, 11, red, 16),
        ("THURSDAY", old, 9, ink, 14),
        ("all ranks", fell, 9, ink, 14)], 7)
    poster(img, (78, 214, 118, 264), 4.0, (200, 190, 160), [
        ("NO", old, 9, ink, 12),
        ("SMOKING", old, 7, ink, 11),
        ("during the", fell, 7, ink, 10),
        ("pictures", fell, 7, red, 11)], 11)
    poster(img, (510, 198, 550, 248), -2.0, (196, 204, 182), [
        ("WRITE", sc, 7, ink, 11),
        ("HOME", old, 9, red, 13),
        ("paper at", fell, 7, ink, 10),
        ("the counter", fell, 6, ink, 10)], 13)


def proscenium(c):
    x0, y0, x1, y1 = SCREEN
    # The surround of moulded boards, then the black masking.
    c.part([(x0 - 34, y0 - 34), (x1 + 34, y0 - 34), (x1 + 34, y1 + 24),
            (x0 - 34, y1 + 24)], (86, 54, 32), gloss=0.25, grain=8, round_=2)
    c.part([(x0 - 26, y0 - 12), (x1 + 26, y0 - 12), (x1 + 26, y1 + 16),
            (x0 - 26, y1 + 16)], (132, 88, 50), gloss=0.4, grain=6, round_=1.5)
    c.part([(x0 - 8, y0 - 8), (x1 + 8, y0 - 8), (x1 + 8, y1 + 8),
            (x0 - 8, y1 + 8)], (16, 14, 12), gloss=0.05, round_=0.6)
    # The stage below the screen, its boards' edge catching the light.
    c.part([(x0 - 40, y1 + 22), (x1 + 40, y1 + 22), (x1 + 46, y1 + 34),
            (x0 - 46, y1 + 34)], (104, 70, 42), gloss=0.35, grain=8,
           round_=1)
    # Red serge curtains drawn back in folds and held by tie-backs: each
    # fold runs from the rail, is gathered in at the tie-back and flares
    # out again to the stage.
    waist = y0 + 112
    for side in (-1, 1):
        edge = x0 - 8 if side < 0 else x1 + 8
        for k in range(5, -1, -1):
            def X(u):
                return edge + side * u
            c.part([(X(k * 5.4), y0 - 8), (X(k * 5.4 + 8), y0 - 8),
                    (X(18 + k * 2.0 + 4), waist),
                    (X(k * 5.6 + 11), y1 + 22), (X(k * 5.6), y1 + 22),
                    (X(18 + k * 2.0), waist)],
                   (140 - k * 9, 30 + k, 28), gloss=0.45, grain=3,
                   round_=1.2)
        tx = edge + side * 24
        c.part([(tx - 9, waist - 4), (tx + 9, waist - 4), (tx + 8, waist + 5),
                (tx - 8, waist + 5)], (196, 160, 70), gloss=0.6, round_=1.5)
        c.part([(tx - 3, waist + 5), (tx + 3, waist + 5), (tx + 4, waist + 22),
                (tx - 4, waist + 22)], (176, 140, 60), gloss=0.5,
               round_=1)                                  # the tassel
    # The pelmet across the top, scalloped and fringed in gold.
    c.part([(x0 - 30, y0 - 30), (x1 + 30, y0 - 30), (x1 + 30, y0 - 6),
            (x0 - 30, y0 - 6)], (118, 26, 24), gloss=0.35, grain=3, round_=1)
    n = 14
    sw = (x1 - x0 + 60) / n
    for k in range(n):
        xa = x0 - 30 + k * sw
        pts = [(xa + sw * t, y0 - 6 + 9 * math.sin(math.pi * t))
               for t in np.linspace(0, 1, 9)]
        c.part([(xa, y0 - 7)] + pts + [(xa + sw, y0 - 7)], (118, 26, 24),
               gloss=0.35, round_=0.8)
        for t in np.linspace(0.05, 0.95, 6):
            fx, fy = xa + sw * t, y0 - 6 + 9 * math.sin(math.pi * t)
            c.line((fx, fy), (fx, fy + 4), (210, 172, 80), 0.6)
    c.line((x0 - 30, y0 - 29), (x1 + 30, y0 - 29), (214, 176, 84), 1.0)


def fascia(img):
    """The lettered board above the pelmet."""
    x0, y0, x1, y1 = SCREEN
    lt = lettering("THE  PICTURE  HOUSE", "imfellenglish/IMFeENsc28P.ttf",
                   15, (232, 214, 160, 255), 260, 22, 0.6, 21)
    img.alpha_composite(lt, (320 - 130, y0 - 58))


def stove(c):
    c.part([(574, 214), (616, 214), (618, 304), (572, 304)], (40, 38, 36),
           gloss=0.45, grain=3, round_=4)
    c.part([(586, EAVE - 10), (602, EAVE - 10), (602, 216), (586, 216)],
           (46, 44, 42), gloss=0.4, round_=2)
    c.part([(582, 262), (606, 288)], (255, 150, 60), shape="ellipse",
           gloss=0.0, rim=0.0, round_=1)
    for k in range(4):
        c.line((580, 260 + k * 7), (608, 260 + k * 7), (30, 22, 18), 1.0)
    c.glow((594, 276), 44, (255, 140, 50), 0.55)


def lamp(c):
    c.line((18, EAVE + 4), (18, 112), (40, 36, 30), 1.0)
    c.part([(8, 112), (28, 132)], (200, 168, 90), shape="ellipse", gloss=0.6,
           round_=1)
    c.part([(12, 106), (24, 114)], (70, 60, 40), round_=0.6)
    c.glow((18, 122), 28, (255, 214, 130), 0.85)


def piano(c):
    """An upright in the pit, the pianist on his stool, a candle on top."""
    c.part([(10, 262), (120, 262), (122, 356), (8, 356)], (58, 34, 22),
           gloss=0.45, grain=7, round_=2)
    c.part([(18, 270), (112, 270), (112, 300), (18, 300)], (82, 50, 30),
           gloss=0.35, grain=5, round_=1.5)
    for k in range(2):
        c.part([(28 + k * 44, 276), (60 + k * 44, 294)], (66, 40, 24),
               shape="ellipse", gloss=0.2, round_=1)     # carved panels
    c.part([(12, 304), (118, 304), (120, 314), (10, 314)], (226, 218, 196),
           gloss=0.25, round_=0.5)
    for k in range(18):
        c.line((16 + k * 5.8, 304), (16 + k * 5.8, 309), (20, 16, 14), 0.8)
    c.part([(30, 252), (60, 262)], (220, 210, 186), gloss=0.1,
           round_=0.4)                                   # music on the stand
    c.part([(18, 246), (24, 262)], (232, 224, 200), round_=0.5)   # candle
    c.glow((21, 242), 22, (255, 200, 120), 0.9)
    # The pianist, his back to us, in shirt sleeves and braces.
    c.part([(96, 300), (150, 300), (156, 358), (90, 358)], (164, 150, 120),
           gloss=0.25, grain=3, round_=7)                # shirt
    for bx in (110, 136):
        c.line((bx, 300), (bx - 2, 358), (70, 50, 34), 1.4)   # braces
    c.part([(116, 290), (130, 302)], (150, 110, 84), round_=1.5)  # neck
    c.part([(110, 266), (136, 294)], (156, 116, 88), shape="ellipse",
           gloss=0.3, round_=2)
    c.part([(108, 262), (138, 282)], (40, 30, 24), shape="ellipse", round_=1)
    c.part([(106, 276), (112, 286)], (150, 110, 84), shape="ellipse",
           round_=0.5)                                   # an ear


def soldier(c, x, y, s, kind, coat, rim, lean):
    """A man on a bench seen from behind, rim-lit from the screen ahead."""
    def R(pts):
        return rot(pts, lean, (x, y + 40 * s))
    r = 9 * s
    c.part(R([(x - r * 2.5, y + r * 1.6), (x - r * 1.8, y + r * 0.85),
              (x - r * 0.6, y + r * 0.62), (x + r * 0.6, y + r * 0.62),
              (x + r * 1.8, y + r * 0.85), (x + r * 2.5, y + r * 1.6),
              (x + r * 2.7, H + 12), (x - r * 2.7, H + 12)]), coat,
           gloss=0.18, grain=5, round_=r * 0.5)          # shoulders
    c.part(R([(x - r * 0.45, y + r * 0.2), (x + r * 0.45, y + r * 0.2),
              (x + r * 0.5, y + r * 0.9), (x - r * 0.5, y + r * 0.9)]),
           (34, 26, 22), round_=1)                       # the neck
    c.part(R([(x - r * 0.95, y - r * 0.15), (x - r * 0.62, y + r * 0.45)]),
           (40, 30, 24), shape="ellipse", round_=0.6)    # ears
    c.part(R([(x + r * 0.62, y - r * 0.15), (x + r * 0.95, y + r * 0.45)]),
           (40, 30, 24), shape="ellipse", round_=0.6)
    c.part(R([(x - r * 0.75, y - r * 0.95), (x + r * 0.75, y + r * 0.75)]),
           (30, 24, 20), shape="ellipse", round_=1)      # head
    if kind == 0:                                        # soft trench cap
        c.part(R([(x - r * 0.95, y - r * 1.3), (x + r * 0.95, y - r * 0.25)]),
               (52, 48, 34), shape="ellipse", gloss=0.2, grain=3, round_=1)
        c.part(R([(x - r * 0.8, y - r * 0.5), (x + r * 0.8, y - r * 0.2)]),
               (40, 36, 26), round_=0.5)                 # the band
    elif kind == 1:                                      # steel helmet
        c.part(R([(x - r * 1.55, y - r * 0.62), (x + r * 1.55, y - r * 0.22)]),
               (46, 48, 42), shape="ellipse", gloss=0.45, round_=1)
        c.part(R([(x - r * 0.95, y - r * 1.4), (x + r * 0.95, y - r * 0.3)]),
               (50, 52, 46), shape="ellipse", gloss=0.45, round_=1)
    elif kind == 2:                                      # slouch hat
        c.part(R([(x - r * 1.6, y - r * 0.7), (x + r * 1.6, y - r * 0.28)]),
               (62, 52, 38), shape="ellipse", gloss=0.2, round_=1)
        c.part(R([(x - r * 0.85, y - r * 1.55), (x + r * 0.85, y - r * 0.45)]),
               (66, 54, 40), shape="ellipse", gloss=0.2, round_=1)
        c.part(R([(x - r * 0.85, y - r * 0.75), (x + r * 0.85, y - r * 0.5)]),
               (50, 40, 28), round_=0.3)
    else:                                                # bare, hair parted
        c.part(R([(x - r * 0.78, y - r * 1.0), (x + r * 0.78, y + r * 0.1)]),
               (44, 32, 24), shape="ellipse", gloss=0.25, round_=1)
        c.line(R([(x - r * 0.1, y - r * 1.0)])[0], R([(x - r * 0.2, y - r * 0.4)])[0],
               (70, 56, 44), 0.5)
    top = {0: 1.3, 1: 1.4, 2: 1.55, 3: 1.0}[kind]
    c.line(R([(x - r * 0.6, y - r * top)])[0], R([(x + r * 0.6, y - r * top)])[0],
           rim, 0.9)                                     # the rim light
    c.line(R([(x - r * 1.9, y + r * 0.82)])[0], R([(x - r * 0.7, y + r * 0.6)])[0],
           rim, 0.6)
    c.line(R([(x + r * 0.7, y + r * 0.6)])[0], R([(x + r * 1.9, y + r * 0.82)])[0],
           rim, 0.6)


def audience(c):
    """Benches of men, the near ones big; a bench back between each row."""
    rng = np.random.default_rng(14)
    coats = [(52, 50, 36), (60, 54, 38), (46, 44, 40), (56, 48, 34)]
    rows = ((288, 0.8, 172), (322, 1.04, 196), (362, 1.38, 150))
    for row, (y, s, x) in enumerate(rows):
        c.part([(0, y + 14 * s), (W, y + 14 * s), (W, y + 22 * s),
                (0, y + 22 * s)], (70, 48, 30), gloss=0.35, grain=6,
               round_=1)                                 # the bench in front
        x += int(rng.integers(-8, 8))
        rim = tuple(int(v * (0.8 - row * 0.12)) for v in (208, 200, 178))
        while x < W + 20:
            soldier(c, x, y, s, int(rng.choice(4, p=[0.45, 0.3, 0.1, 0.15])),
                    coats[rng.integers(4)], rim, rng.normal(0, 0.06))
            x += int(9 * s * 4.9 + rng.integers(0, 18))


def beam_and_haze(img):
    """The projector's beam over our heads to the screen, lit up in the
    smoke, with motes of dust drifting in it."""
    x0, y0, x1, y1 = SCREEN
    beam = Image.new("L", (W, H), 0)
    ImageDraw.Draw(beam).polygon([(312, 420), (344, 420), (x1, y0), (x1, y1),
                                  (x0, y1), (x0, y0)], fill=72)
    beam = beam.filter(ImageFilter.GaussianBlur(10))
    yy, xx = np.mgrid[0:H, 0:W]
    smoke = np.zeros((H, W), np.float32)
    for _ in range(22):
        cx, cy = RNG.uniform(140, 500), RNG.uniform(80, 340)
        rx, ry = RNG.uniform(30, 100), RNG.uniform(8, 24)
        smoke += np.exp(-(((xx - cx) / rx) ** 2 + ((yy - cy) / ry) ** 2)) * 0.55
    smoke = np.clip(smoke, 0, 1)
    b = np.asarray(beam, np.float32) / 255.0 * (0.35 + 1.5 * smoke)
    arr = np.asarray(img.convert("RGB")).astype(np.float32)
    arr += b[..., None] * np.array([232, 224, 204])
    arr += (smoke * 10)[..., None] * np.array([1.0, 0.95, 0.85])  # room haze
    inbeam = np.asarray(beam) > 18
    for _ in range(90):
        mx, my = int(RNG.uniform(160, 480)), int(RNG.uniform(60, 330))
        if inbeam[my, mx]:
            arr[my, mx] += 60
    return Image.fromarray(np.clip(arr, 0, 255).astype(np.uint8)).convert("RGBA")


def layer_over(img, c):
    layer = c.image()
    key = np.all(np.asarray(layer) == (255, 0, 255), axis=2)
    arr = np.asarray(img.convert("RGB")).copy()
    arr[~key] = np.asarray(layer)[~key]
    return Image.fromarray(arr).convert("RGBA")


def main():
    img = room().convert("RGBA")
    c = Canvas(W, H, light=(0.0, -1.0))
    trusses(c)
    stove(c)
    lamp(c)
    img = layer_over(img, c)
    posters(img)
    c = Canvas(W, H, light=(0.0, -1.0))
    proscenium(c)
    img = layer_over(img, c)
    fascia(img)
    x0, y0, x1, y1 = SCREEN
    # The screen: unbleached canvas, its weave faintly showing, left dark.
    scr = np.full((y1 - y0, x1 - x0, 3), 22, np.float32)
    scr += RNG.normal(0, 1.5, scr.shape[:2])[..., None]
    scr_img = Image.fromarray(np.clip(scr, 0, 255).astype(np.uint8))
    img.paste(scr_img, (x0, y0))
    img = beam_and_haze(img)
    img.paste(scr_img, (x0, y0))
    c = Canvas(W, H, light=(0.3, -1.0))
    piano(c)
    audience(c)
    img = layer_over(img, c)
    img.convert("RGB").save(OUT)


if __name__ == "__main__":
    main()
