"""paint.py: the host-side painter shared by the sprite generators.

Parts are painted at SS times their final size: a shaded fill lit from the
low sun, a soft rim of shadow inside the edge, a fine grain and a warm
highlight on the lit edge; the canvas is filtered down at the end, and
magenta marks what is transparent."""

import math

import numpy as np
from PIL import Image, ImageDraw, ImageFilter

SS = 4
KEY = (255, 0, 255)
RNG = np.random.default_rng(1914)
LIGHT = np.array([0.55, -0.75])        # from the upper right, normalised below
LIGHT /= np.linalg.norm(LIGHT)


class Canvas:
    def __init__(self, w, h, light=None):
        self.w, self.h = w * SS, h * SS
        self.light = LIGHT if light is None else np.array(light) / np.linalg.norm(light)
        self.rgb = np.zeros((self.h, self.w, 3), np.float32)
        self.alpha = np.zeros((self.h, self.w), np.float32)

    def part(self, points, colour, gloss=0.25, grain=7.0, rim=0.45,
             round_=1.2, shape="poly"):
        """A shaded part: points are in output pixels.  shape "ellipse"
        takes a bounding box, "capsule" two end points and a width."""
        mask_img = Image.new("L", (self.w, self.h), 0)
        d = ImageDraw.Draw(mask_img)
        if shape == "ellipse":
            xs_, ys_ = [q[0] for q in points], [q[1] for q in points]
            x0, y0, x1, y1 = min(xs_), min(ys_), max(xs_), max(ys_)
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
        along = ((xx - cx) * self.light[0] + (yy - cy) * self.light[1]) / span
        shade = 0.82 + 0.5 * np.clip(along, -0.6, 0.6)
        # A soft rim of shadow just inside the edge, from a blurred mask.
        inner = np.asarray(mask_img.filter(ImageFilter.GaussianBlur(SS * 2.2)),
                           np.float32) / 255.0
        shade *= 1.0 - rim * (1.0 - np.clip(inner * 1.6, 0, 1))
        # Highlight on the lit edge.
        lit_edge = np.asarray(
            mask_img.transform(mask_img.size, Image.AFFINE,
                               (1, 0, -self.light[0] * SS * 2.5, 0, 1,
                                -self.light[1] * SS * 2.5)), np.float32) / 255.0
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

    def image(self, size=None):
        """The painting filtered down to its size, or to 'size' if given."""
        rgb = Image.fromarray(np.clip(self.rgb, 0, 255).astype(np.uint8))
        alpha = Image.fromarray((self.alpha * 255).astype(np.uint8))
        size = size or (self.w // SS, self.h // SS)
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


