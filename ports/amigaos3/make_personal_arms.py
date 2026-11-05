#!/usr/bin/env python3
"""Paint 1914's weapons in hand: the pilot's Webley and Tom's carbine.

Each part is painted on the host at four times its size: a shaded fill
lit from the low sun ahead and to the right, a soft rim of shadow inside
its edge, a fine grain, and highlights along the lit edges; the result is
filtered down, so edges come out soft and the palette matches the
cockpit painting.  Magenta is the transparent key, as in the other atlases.

  assets/canvas-skies/cockpit-revolver.bmp  two frames, 220x170: at rest
                                            and kicking up with the flash
  assets/canvas-skies/rear-carbine.bmp      nine frames, 200x260: the
                                            carbine raised from the rear seat
                                            at -48..+48 degrees from upright
"""

import math
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFilter

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"
SS = 4
KEY = (255, 0, 255)
RNG = np.random.default_rng(1914)
LIGHT = np.array([0.55, -0.75])        # from the upper right, normalised below
LIGHT /= np.linalg.norm(LIGHT)


class Canvas:
    def __init__(self, w, h):
        self.w, self.h = w * SS, h * SS
        self.rgb = np.zeros((self.h, self.w, 3), np.float32)
        self.alpha = np.zeros((self.h, self.w), np.float32)

    def part(self, points, colour, gloss=0.25, grain=7.0, rim=0.45,
             round_=1.2, shape="poly"):
        """A shaded part: points are in output pixels.  shape "ellipse"
        takes a bounding box, "capsule" two end points and a width."""
        mask_img = Image.new("L", (self.w, self.h), 0)
        d = ImageDraw.Draw(mask_img)
        if shape == "ellipse":
            (x0, y0), (x1, y1) = points
            d.ellipse((x0 * SS, y0 * SS, x1 * SS, y1 * SS), fill=255)
        elif shape == "capsule":
            (x0, y0), (x1, y1), w = points
            d.line([(x0 * SS, y0 * SS), (x1 * SS, y1 * SS)], fill=255,
                   width=int(w * SS))
            for x, y in ((x0, y0), (x1, y1)):
                d.ellipse(((x - w / 2) * SS, (y - w / 2) * SS,
                           (x + w / 2) * SS, (y + w / 2) * SS), fill=255)
        else:
            d.polygon([(x * SS, y * SS) for x, y in points], fill=255)
        if round_ > 0:
            # Soften the corners: blur, then cut back at the half level.
            mask_img = mask_img.filter(ImageFilter.GaussianBlur(round_ * SS))
            mask_img = mask_img.point(lambda v: 255 if v > 127 else 0)
            mask_img = mask_img.filter(ImageFilter.GaussianBlur(SS * 0.5))
        mask = np.asarray(mask_img, np.float32) / 255.0
        if mask.max() == 0:
            return
        # Light falls across the part along LIGHT; the far side is darker.
        ys, xs = np.nonzero(mask > 0.5)
        cx, cy = xs.mean(), ys.mean()
        span = max(xs.max() - xs.min(), ys.max() - ys.min(), 1)
        yy, xx = np.mgrid[0:self.h, 0:self.w]
        along = ((xx - cx) * LIGHT[0] + (yy - cy) * LIGHT[1]) / span
        shade = 0.82 + 0.5 * np.clip(along, -0.6, 0.6)
        # A soft rim of shadow just inside the edge, from a blurred mask.
        inner = np.asarray(mask_img.filter(ImageFilter.GaussianBlur(SS * 2.2)),
                           np.float32) / 255.0
        shade *= 1.0 - rim * (1.0 - np.clip(inner * 1.6, 0, 1))
        # Highlight on the lit edge.
        lit_edge = np.asarray(
            mask_img.transform(mask_img.size, Image.AFFINE,
                               (1, 0, -LIGHT[0] * SS * 2.5, 0, 1,
                                -LIGHT[1] * SS * 2.5)), np.float32) / 255.0
        highlight = np.clip(mask - lit_edge, 0, 1) * gloss
        noise = RNG.normal(0, grain, (self.h // SS + 1, self.w // SS + 1))
        noise = np.kron(noise, np.ones((SS, SS)))[:self.h, :self.w]
        col = np.array(colour, np.float32)
        paint = col[None, None, :] * shade[..., None] + noise[..., None]
        paint += highlight[..., None] * np.array([255, 214, 160], np.float32)
        a = mask[..., None]
        self.rgb = self.rgb * (1 - a) + paint * a
        self.alpha = np.maximum(self.alpha, mask)

    def line(self, p0, p1, colour, width):
        img = Image.new("L", (self.w, self.h), 0)
        ImageDraw.Draw(img).line([(p0[0] * SS, p0[1] * SS),
                                  (p1[0] * SS, p1[1] * SS)],
                                 fill=255, width=int(width * SS))
        m = np.asarray(img, np.float32)[..., None] / 255.0
        self.rgb = self.rgb * (1 - m) + np.array(colour, np.float32) * m
        self.alpha = np.maximum(self.alpha, m[..., 0])

    def glow(self, centre, radius, colour, strength):
        yy, xx = np.mgrid[0:self.h, 0:self.w]
        d = np.hypot(xx - centre[0] * SS, yy - centre[1] * SS) / (radius * SS)
        m = np.clip(1.0 - d, 0, 1) ** 1.6 * strength
        self.rgb = self.rgb * (1 - m[..., None]) + \
            np.array(colour, np.float32) * m[..., None]
        self.alpha = np.maximum(self.alpha, np.clip(m * 2.0, 0, 1))

    def image(self):
        rgb = Image.fromarray(np.clip(self.rgb, 0, 255).astype(np.uint8))
        alpha = Image.fromarray((self.alpha * 255).astype(np.uint8))
        size = (self.w // SS, self.h // SS)
        rgb = rgb.resize(size, Image.LANCZOS)
        alpha = alpha.resize(size, Image.LANCZOS)
        out = Image.new("RGB", size, KEY)
        out.paste(rgb, (0, 0), alpha.point(lambda v: 255 if v > 110 else 0))
        return out


def rot(points, angle, origin):
    ca, sa = math.cos(angle), math.sin(angle)
    ox, oy = origin
    return [(ox + (x - ox) * ca - (y - oy) * sa,
             oy + (x - ox) * sa + (y - oy) * ca) for x, y in points]


LEATHER = (104, 66, 38)
LEATHER_DARK = (70, 44, 26)
STEEL = (44, 46, 52)
GRIP = (52, 34, 24)
WOOD = (142, 86, 44)
GLOVE = (58, 46, 38)
SLEEVE = (86, 78, 52)


def revolver(kick):
    c = Canvas(220, 170)
    lift = -12 * kick
    o = (150, 120 + lift)                 # the wrist, pivot of the kick
    a = -0.18 - 0.22 * kick               # pointing ahead and a little left

    def R(pts):
        return rot([(x, y + lift) for x, y in pts], a, o)

    def cap(p0, p1, w, colour, **kw):
        q = R([p0, p1])
        c.part([q[0], q[1], w], colour, shape="capsule", **kw)

    # Khaki sleeve and the flared leather gauntlet, stitched at the cuff.
    c.part([(222, 172), (222, 116), (190, 98), (150, 124), (164, 172)],
           SLEEVE, gloss=0.06, round_=3)
    c.part(R([(206, 140), (176, 100), (146, 106), (134, 134), (156, 158),
              (204, 164)]), LEATHER_DARK, gloss=0.1, round_=4)
    c.line(R([(176, 104)])[0], R([(150, 156)])[0], (120, 84, 52), 0.7)
    # The revolver: a long barrel with its top rib, the frame and latch,
    # the fluted cylinder, the hammer, a bird's-head grip of dark wood.
    cap((24, 79), (100, 80), 6.5, STEEL, gloss=0.18, round_=0.6)
    cap((24, 75.5), (100, 76), 2.2, (82, 84, 92), gloss=0.1, round_=0.3)
    c.part(R([(23, 74), (27, 74), (27, 70), (24, 70)]), STEEL, round_=0.3)
    c.part(R([(98, 70), (128, 70), (130, 92), (98, 92)]), STEEL, gloss=0.22,
           round_=2.5)
    for k in range(4):
        c.line(R([(101, 74 + k * 4.5)])[0], R([(126, 74 + k * 4.5)])[0],
               (26, 26, 30), 0.7)
    c.part(R([(126, 70), (156, 72), (158, 92), (128, 96)]), STEEL, gloss=0.2,
           round_=2.0)
    c.part(R([(146, 70), (158, 60), (164, 64), (154, 76)]), STEEL,
           round_=1.0)                                   # hammer spur
    c.part(R([(130, 92), (152, 90), (150, 128), (134, 134), (124, 118)]),
           GRIP, gloss=0.12, round_=3)
    c.part([R([(118, 98)])[0], R([(124, 112)])[0], 2.2], STEEL,
           shape="capsule", round_=0.3)                  # trigger guard
    # The gloved fingers curled round the grip, knuckles catching the
    # light, and the thumb laid along the frame.
    for k in range(4):
        y = 100 + k * 8.5
        cap((118, y), (146, y - 2), 9.0, LEATHER, gloss=0.16, round_=1.5)
    c.part(R([(132, 96), (160, 92), (172, 122), (150, 138), (138, 126)]),
           LEATHER, gloss=0.14, round_=4)                # back of the hand
    cap((136, 84), (162, 94), 9.0, LEATHER, gloss=0.18, round_=1.5)
    if kick > 0.5:
        muzzle = R([(18, 78)])[0]
        c.glow(muzzle, 18, (255, 200, 110), 0.85)
        c.glow((muzzle[0] - 4, muzzle[1] - 2), 7, (255, 248, 220), 1.0)
    return c.image()


def carbine(angle):
    c = Canvas(200, 260)
    base = (100, 260)

    def R(pts):
        return rot(pts, angle, base)

    def cap(p0, p1, w, colour, **kw):
        q = R([p0, p1])
        c.part([q[0], q[1], w], colour, shape="capsule", **kw)
    # Pointing straight up in its own frame, butt at the bottom edge.
    c.part(R([(76, 262), (124, 262), (112, 198), (90, 198)]), WOOD,
           gloss=0.14, round_=3)                         # butt
    c.part(R([(90, 200), (110, 200), (106, 150), (94, 150)]), WOOD,
           gloss=0.16, round_=2)                         # wrist of the stock
    c.part(R([(93, 168), (107, 168), (106, 118), (94, 118)]), STEEL,
           gloss=0.2, round_=1.5)                        # receiver
    cap((106, 160), (122, 154), 4.0, STEEL, gloss=0.2, round_=0.6)
    knob = R([(122.5, 154.5)])[0]
    c.part([(knob[0] - 3.5, knob[1] - 3.5), (knob[0] + 3.5, knob[1] + 3.5)],
           STEEL, shape="ellipse", round_=0.5)          # bolt knob
    c.part(R([(93, 150), (107, 150), (104, 40), (96, 40)]), WOOD,
           gloss=0.16, round_=2)                         # fore-end
    cap((100, 62), (100, 6), 4.5, STEEL, gloss=0.2, round_=0.5)
    c.part(R([(98, 10), (102, 10), (102, 2), (98, 2)]), (24, 24, 28),
           round_=0.3)
    # The hands in dark gloves: the right round the wrist of the stock,
    # the left cupping the fore-end from beneath, its thumb along the wood.
    c.part(R([(78, 222), (96, 204), (124, 212), (128, 236), (94, 244)]),
           GLOVE, gloss=0.14, round_=5)
    for k in range(2):
        c.line(R([(100, 218 + k * 8)])[0], R([(124, 222 + k * 7)])[0],
               (34, 26, 22), 0.9)
    c.part(R([(108, 100), (126, 108), (124, 134), (100, 140), (88, 124)]),
           GLOVE, gloss=0.14, round_=5)
    cap((96, 104), (100, 84), 6.5, GLOVE, gloss=0.18, round_=1.0)  # thumb
    for k in range(2):
        c.line(R([(104, 116 + k * 8)])[0], R([(122, 118 + k * 7)])[0],
               (34, 26, 22), 0.9)
    c.part(R([(36, 262), (82, 230), (96, 262)]), SLEEVE, gloss=0.06, round_=4)
    c.part(R([(156, 262), (116, 230), (106, 262)]), SLEEVE, gloss=0.06,
           round_=4)
    return c.image()


def main():
    frames = [revolver(0.0), revolver(1.0)]
    sheet = Image.new("RGB", (440, 170), KEY)
    for i, f in enumerate(frames):
        sheet.paste(f, (i * 220, 0))
    sheet.save(ASSETS / "cockpit-revolver.bmp")
    sheet = Image.new("RGB", (200 * 9, 260), KEY)
    for i in range(9):
        sheet.paste(carbine(math.radians(-48 + 12 * i)), (i * 200, 0))
    sheet.save(ASSETS / "rear-carbine.bmp")


if __name__ == "__main__":
    main()
