#!/usr/bin/env python3
"""Render the picture house: an army hut behind the lines, run as a cinema.

Built and lit with the small renderer in scene3d.py rather than painted
in flat shapes: rough-sawn boards each with its own width, tone, grain,
knots and nails; corrugated iron overhead on king-post trusses; the
moulded surround, pelmet and red serge curtains catching the light of
the screen; hand-lettered posters; the stove, the lamp and the
pianist's candle as real lights; the projector's beam through tobacco
haze; and three benches of soldiers seen from behind (figures3d.py),
each man his own, rim-lit by the screen.  Rendered at twice the size,
filtered down and dithered to RGB565.  The screen is left dark for the
reels (x 176..464, y 70..250).  Writes assets/canvas-skies/cinema-hut.bmp.
"""

import math
import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFilter, ImageFont

sys.path.insert(0, str(Path(__file__).resolve().parent))
from figures3d import bench_row  # noqa: E402
from scene3d import SS, Scene, corrugated, fbm, normalize, planks  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "canvas-skies" / "cinema-hut.bmp"
FONTS = ROOT / "assets" / "fonts"
W, H = 640, 400
SCREEN = (176, 70, 464, 250)
EAVE = 96


def mask(pts):
    m = Image.new("L", (W * SS, H * SS), 0)
    ImageDraw.Draw(m).polygon([(x * SS, y * SS) for x, y in pts], fill=255)
    return np.asarray(m) > 127


def rect(x0, y0, x1, y1):
    return mask([(x0, y0), (x1, y0), (x1, y1), (x0, y1)])


def beam_mask(p0, p1, w):
    dx, dy = p1[0] - p0[0], p1[1] - p0[1]
    n = math.hypot(dx, dy)
    ox, oy = -dy / n * w / 2, dx / n * w / 2
    return mask([(p0[0] + ox, p0[1] + oy), (p1[0] + ox, p1[1] + oy),
                 (p1[0] - ox, p1[1] - oy), (p0[0] - ox, p0[1] - oy)])


def lights(sc):
    x0, y0, x1, y1 = SCREEN
    sc.point(((x0 + x1) / 2, (y0 + y1) / 2, 470), (0.95, 0.95, 0.92), 150)
    sc.point((18, 122, 480), (1.3, 0.85, 0.42), 70)           # the lamp
    sc.point((594, 268, 430), (1.5, 0.7, 0.26), 90)           # the stove
    sc.point((21, 236, 300), (0.8, 0.52, 0.26), 40)           # the candle
    sc.direction((0, -0.45, 1), (1.1, 1.08, 1.02))            # the screen's glow
    sc.direction((0.25, -0.5, -1), (0.12, 0.1, 0.08))         # spill behind us


def walls(sc):
    a, n, _ = planks(H * SS, W * SS, 11, base=(112, 82, 58), widths=(16, 30),
                     soot_top=0.6)
    sc.surface(~rect(0, 0, W, EAVE), 520, a, normals=n)
    ca, cn = corrugated(H * SS, W * SS, 12)
    sc.surface(rect(0, 0, W, EAVE + 1), 530, ca, normals=cn, spec=0.15)
    wood, _, _ = planks(H * SS, W * SS, 13, base=(118, 80, 48),
                        widths=(400, 401), vertical=False)
    sc.surface(rect(0, EAVE - 7, W, EAVE + 4), 495, wood, normal=(0, 0.4, -1))
    for y, s, z in ((2, 1.0, 508), (30, 0.72, 511), (52, 0.52, 514)):
        half, tie = 340 * s, y + 64 * s
        apex, left, right = (320, y), (320 - half, tie), (320 + half, tie)
        w = 11 * s
        m = (beam_mask(left, apex, w) | beam_mask(apex, right, w) |
             beam_mask(left, right, w) | beam_mask(apex, (320, tie), w * 0.9))
        for side in (-1, 1):
            m |= beam_mask((320 + side * half * 0.5, tie),
                           (320 + side * half * 0.22, y + 30 * s), w * 0.7)
        sc.surface(m, z, wood, normal=(0, 0.6, -1))


def lettering(text, font, size, colour, w, h):
    f = ImageFont.truetype(str(FONTS / font), size * SS)
    img = Image.new("RGBA", (w * SS, h * SS), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    tw = d.textlength(text, font=f)
    rng = np.random.default_rng(len(text) * 31 + size)
    x = (w * SS - tw) / 2
    for ch in text:
        d.text((x, h * SS / 2 + rng.normal(0, 0.6) * SS), ch, font=f,
               fill=colour + (235,), anchor="lm")
        x += d.textlength(ch, font=f)
    return np.asarray(img, np.float32) / 255.0


def poster(sc, box, lines, paper, seed):
    x0, y0, x1, y1 = box
    m = rect(x0, y0, x1, y1)
    a = np.zeros((H * SS, W * SS, 3), np.float32)
    foxing = fbm(H * SS, W * SS, seed, 4, 10)
    a[:] = np.array(paper, np.float32) / 255.0
    a *= (0.82 + 0.25 * foxing)[..., None]
    y = y0 + 3
    for text, font, size, colour, lh in lines:
        t = lettering(text, font, size, colour, x1 - x0 - 4, lh)
        sl = a[int(y * SS):int(y * SS) + t.shape[0],
               int((x0 + 2) * SS):int((x0 + 2) * SS) + t.shape[1]]
        al = t[:sl.shape[0], :sl.shape[1], 3:4]
        sl[:] = sl * (1 - al) + t[:sl.shape[0], :sl.shape[1], :3] * al
        y += lh
    # A drop shadow on the boards, then the sheet a hair proud of them.
    sc.rgb[rect(x0 + 2, y0 + 3, x1 + 2, y1 + 3) & ~m] *= 0.45
    sc.surface(m, 517, a, normal=(0.05, -0.05, -1))
    for px in (x0 + 3, x1 - 3):
        sc.ellipsoid((px, y0 + 3, 515), (1.4, 1.4, 1.0), (200, 190, 170),
                     spec=0.6)


def posters(sc):
    ink, red = (30, 24, 20), (150, 32, 26)
    fell, sc_ = "imfellenglish/IMFeENrm28P.ttf", "imfellenglish/IMFeENsc28P.ttf"
    sten, old = ("stardosstencil/StardosStencil-Bold.ttf",
                 "oldstandardtt/OldStandard-Bold.ttf")
    poster(sc, (34, 132, 112, 222), [
        ("Y.M.C.A.", sc_, 12, red, 17), ("PICTURES", old, 13, ink, 19),
        ("TO-NIGHT", old, 13, ink, 19), ("at 7", fell, 12, red, 16),
        ("ADMISSION FREE", sc_, 7, ink, 12)], (214, 202, 170), 3)
    poster(sc, (504, 118, 566, 188), [
        ("CONCERT", sten, 10, red, 15), ("PARTY", sten, 10, red, 15),
        ("THURSDAY", old, 8, ink, 13), ("all ranks", fell, 8, ink, 13)],
        (226, 216, 186), 7)
    poster(sc, (74, 226, 116, 252), [
        ("NO SMOKING", old, 6, ink, 11), ("during the pictures", fell, 5, red, 10)],
        (200, 190, 160), 11)
    poster(sc, (508, 196, 548, 238), [
        ("WRITE", sc_, 7, ink, 11), ("HOME", old, 9, red, 13),
        ("paper at the", fell, 6, ink, 9)], (196, 204, 182), 13)


def proscenium(sc):
    x0, y0, x1, y1 = SCREEN
    yy, xx = np.mgrid[0:H * SS, 0:W * SS].astype(np.float32) / SS
    # The moulded surround: a bevel round the opening, lit by the screen.
    outer = rect(x0 - 32, y0 - 34, x1 + 32, y1 + 24)
    inner = rect(x0 - 8, y0 - 8, x1 + 8, y1 + 8)
    ring = outer & ~inner
    dl, dr = np.abs(xx - (x0 - 8)), np.abs((x1 + 8) - xx)
    dt, db = np.abs(yy - (y0 - 8)), np.abs((y1 + 8) - yy)
    near = np.minimum(np.minimum(dl, dr), np.minimum(dt, db))
    bevel = near < 8
    n = np.zeros((H * SS, W * SS, 3), np.float32)
    n[..., 2] = -1
    n[..., 0] += np.where(bevel & (dl == near), 0.8, 0)
    n[..., 0] += np.where(bevel & (dr == near), -0.8, 0)
    n[..., 1] += np.where(bevel & (dt == near), 0.8, 0)
    n[..., 1] += np.where(bevel & (db == near), -0.8, 0)
    wood, _, _ = planks(H * SS, W * SS, 21, base=(140, 92, 52),
                        widths=(400, 401), vertical=False)
    sc.surface(ring, 505, wood, normals=normalize(n), spec=0.25, shine=20)
    sc.surface(inner & ~rect(x0, y0, x1, y1), 506, (14, 12, 10))
    sc.surface(rect(x0, y0, x1, y1), 507, (20, 19, 17))
    # The stage's edge below.
    stage, _, _ = planks(H * SS, W * SS, 22, base=(112, 76, 46),
                         widths=(5, 7), vertical=False)
    sc.surface(rect(x0 - 46, y1 + 24, x1 + 46, y1 + 36), 480, stage,
               normal=(0, -0.5, -1))
    # Red serge curtains: deep folds catching the screen light, gathered
    # at gilt tie-backs.
    waist = y0 + 112
    for side in (-1, 1):
        edge = x0 - 8 if side < 0 else x1 + 8
        u = (xx - edge) * side                     # outward from the edge
        top = np.clip((yy - (y0 - 10)) / (waist - y0 + 10), 0, 1)
        bot = np.clip((yy - waist) / (y1 + 22 - waist), 0, 1)
        width = np.where(yy < waist, 30 - 12 * top, 18 + 14 * bot)
        m = (u >= -2) & (u <= width) & (yy > y0 - 12) & (yy < y1 + 22)
        phase = u / np.maximum(width, 1) * 6.5 * math.pi
        cn = np.stack([np.sin(phase) * 0.9, np.zeros_like(phase),
                       -np.ones_like(phase)], -1)
        tex = 0.75 + 0.25 * fbm(H * SS, W * SS, 30 + side, 4, 30)
        alb = np.stack([0.52 * tex, 0.09 * tex, 0.08 * tex], -1)
        sc.surface(m, 500, alb, normals=normalize(cn), spec=0.12, shine=8)
        tx = edge + side * 22
        sc.ellipsoid((tx, waist, 492), (10, 4.5, 5), (200, 156, 66), spec=0.7,
                     shine=24)
        sc.ellipsoid((tx, waist + 12, 492), (3.5, 9, 3.5), (186, 140, 58),
                     spec=0.5)
    # The pelmet, scalloped and fringed in gold, and the fascia above.
    sw = (x1 - x0 + 64) / 14
    scallop = (yy - (y0 - 7)) < 9 * np.abs(np.sin(math.pi * (xx - (x0 - 32)) / sw))
    m = rect(x0 - 32, y0 - 32, x1 + 32, y0 - 6) | (
        rect(x0 - 32, y0 - 7, x1 + 32, y0 + 3) & scallop)
    tex = 0.8 + 0.2 * fbm(H * SS, W * SS, 33, 4, 40)
    sc.surface(m, 498, np.stack([0.48 * tex, 0.08 * tex, 0.07 * tex], -1),
               normal=(0, 0.35, -1), spec=0.1)
    sc.surface(rect(x0 - 32, y0 - 32, x1 + 32, y0 - 30), 497, (214, 176, 86),
               spec=0.5)
    t = lettering("THE  PICTURE  HOUSE", "imfellenglish/IMFeENsc28P.ttf", 15,
                  (236, 216, 160), 260, 22)
    fas = np.zeros((H * SS, W * SS, 4), np.float32)
    fy, fx = int((y0 - 60) * SS), int(190 * SS)
    fas[fy:fy + t.shape[0], fx:fx + t.shape[1]] = t
    sc.surface(fas[..., 3] > 0.35, 496, fas[..., :3], normal=(0, 0.3, -1),
               spec=0.3)


def stove_lamp_piano(sc):
    # The stove: a cast-iron drum on legs, its door open on the fire,
    # the flue rising to the roof.
    sc.cylinder(594, 220, 300, 21, 450, (46, 44, 42), spec=0.4, shine=14,
                texture=lambda u, v: 0.85 + 0.15 * (np.sin(v * 40) > 0.6))
    sc.ellipsoid((594, 220, 450), (21, 5, 12), (56, 54, 50), spec=0.4)
    sc.cylinder(594, EAVE - 6, 220, 6, 452, (52, 50, 48), spec=0.3)
    for lx in (578, 610):
        sc.cylinder(lx, 298, 310, 2.4, 449, (36, 34, 32))
    sc.surface(rect(584, 258, 604, 284), 428, (255, 150, 60))
    sc.glow((594, 271), 12, (255, 140, 50), 1.1)
    sc.ellipsoid((18, 124, 470), (8, 10, 8), (230, 190, 110), spec=0.8)
    sc.glow((18, 122), 14, (255, 214, 140), 1.2)
    wood, _, _ = planks(H * SS, W * SS, 41, base=(70, 42, 26),
                        widths=(400, 401), vertical=False)
    sc.surface(rect(8, 262, 122, 356), 330, wood, spec=0.35, shine=24)
    sc.surface(rect(6, 254, 124, 262), 328, wood, normal=(0, -0.8, -0.6),
               spec=0.4)
    sc.surface(rect(10, 304, 120, 314), 300, (226, 218, 196),
               normal=(0, -0.6, -0.8), spec=0.3)
    for k in range(19):
        sc.surface(rect(12 + k * 5.8, 304, 13.6 + k * 5.8, 309), 299,
                   (24, 20, 16))
    sc.ellipsoid((21, 250, 300), (2.6, 8, 2.6), (232, 224, 200))
    sc.glow((21, 240), 5, (255, 210, 130), 1.2)


def haze(sc):
    """The projector's beam over our heads to the screen, lit up in the
    smoke; a faint pall over the whole room."""
    x0, y0, x1, y1 = SCREEN
    beam = mask([(314, 430), (342, 430), (x1, y0), (x1, y1), (x0, y1), (x0, y0)])
    soft = np.asarray(Image.fromarray(beam.astype(np.uint8) * 255).filter(
        ImageFilter.GaussianBlur(16 * SS)), np.float32) / 255.0
    smoke = fbm(H * SS, W * SS, 50, 6, 3)
    smoke = np.clip((smoke - 0.35) * 2.0, 0, 1)
    yy = np.arange(H * SS, dtype=np.float32)[:, None] / SS
    along = np.clip((yy - y0) / (430 - y0), 0, 1)           # brighter near us
    dens = soft * (0.25 + 0.9 * smoke) * (0.5 + 0.7 * along)
    dens[rect(x0, y0, x1, y1)] = 0
    sc.rgb += dens[..., None] * np.array([0.42, 0.41, 0.38])
    return dens
    sc.rgb += (smoke * 0.035)[..., None] * np.array([1.0, 0.95, 0.85])


def main():
    sc = Scene(W, H, ambient=(0.05, 0.045, 0.04))
    lights(sc)
    walls(sc)
    posters(sc)
    proscenium(sc)
    stove_lamp_piano(sc)
    dens = haze(sc)
    # The floor of the hall and the benches, then four rows of men,
    # nearest last and largest.
    floor, fn, _ = planks(H * SS, W * SS, 61, base=(70, 52, 36),
                          widths=(5, 8), vertical=False)
    sc.surface(rect(126, 262, W, H), 470, floor * 0.6, normal=(0, -0.9, -0.4))
    rows = ((276, 0.66, 300, 182, 6), (292, 0.84, 260, 168, 7),
            (326, 1.08, 200, 150, 8), (368, 1.42, 120, 130, 9))
    bench, _, _ = planks(H * SS, W * SS, 62, base=(84, 58, 36),
                         widths=(400, 401), vertical=False)
    for y, s, z, x0, seed in rows:
        sc.surface(rect(x0 - 30, y + 20 * s, W, y + 30 * s), z + 30, bench,
                   normal=(0, -0.3, -1), spec=0.2)
    for y, s, z, x0, seed in rows:
        bench_row(sc, y, s, z, x0, seed, gap=4.3)
    # The beam passes over the men too: it lights the smoke in front of
    # the heads of the back rows.
    yy = np.arange(H * SS)[:, None]
    sc.rgb += (dens * (yy < 330 * SS))[..., None] * np.array([0.3, 0.29, 0.27])
    img = sc.finish(exposure=1.0)
    x0, y0, x1, y1 = SCREEN
    a = np.asarray(img).copy()
    a[y0:y1, x0:x1] = (22, 20, 18)                  # the screen, left dark
    Image.fromarray(a).save(OUT)


if __name__ == "__main__":
    main()
