"""A small software renderer for the painted rooms (mess, picture house).

Not a general 3D package: just enough to light the rooms properly.

  Scene        a colour buffer, a depth buffer and a set of lights, at
               twice the final size; screen x and y are pixels, z runs
               away from the viewer.
  ellipsoid    a shaded solid: each pixel of it is ray-cast for its true
               depth and surface normal, so heads, shoulders, caps,
               bottles and stoves take the light as round forms do.
  surface      a flat, textured face (walls, boards, posters) lit by the
               same lights from its own normal.
  planks       individual boards: varying widths and tones, grain that
               flows round knots, nail heads, dark gaps, wear and soot.
  haze         volumetric: noise-modulated density inside a beam.
  finish       downsample to the final size and dither to RGB565 so the
               Amiga's 16-bit screen shows no banding.

Lights are points (lamp, stove, candle) or directions (the cinema
screen's glow, which from behind a seated man is a rim light).
"""

import math

import numpy as np
from PIL import Image

SS = 2


def fbm(h, w, seed, octaves=5, base=8):
    """Smooth fractal noise in 0..1."""
    rng = np.random.default_rng(seed)
    out = np.zeros((h, w), np.float32)
    amp, total = 1.0, 0.0
    for o in range(octaves):
        n = base * 2 ** o
        g = rng.random((n + 1, int(n * w / h) + 2)).astype(np.float32)
        img = Image.fromarray((g * 255).astype(np.uint8)).resize(
            (w, h), Image.BICUBIC)
        out += np.asarray(img, np.float32) / 255.0 * amp
        total += amp
        amp *= 0.5
    return out / total


def normalize(v):
    n = np.linalg.norm(v, axis=-1, keepdims=True)
    return v / np.maximum(n, 1e-6)


def rot_z(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]], np.float32)


def rot_x(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[1, 0, 0], [0, c, -s], [0, s, c]], np.float32)


def rot_y(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]], np.float32)


class Scene:
    def __init__(self, w, h, ambient=(0.05, 0.045, 0.04)):
        self.w, self.h = w * SS, h * SS
        self.rgb = np.zeros((self.h, self.w, 3), np.float32)
        self.z = np.full((self.h, self.w), np.inf, np.float32)
        self.ambient = np.array(ambient, np.float32)
        self.points = []        # (pos (x,y,z) at 1x, colour, reach)
        self.dirs = []          # (direction toward the light, colour)

    # -- lights ------------------------------------------------------------
    def point(self, pos, colour, reach):
        self.points.append((np.array(pos, np.float32) * SS,
                            np.array(colour, np.float32), reach * SS))

    def direction(self, d, colour):
        self.dirs.append((normalize(np.array(d, np.float32)),
                          np.array(colour, np.float32)))

    def light(self, pos, n, albedo, spec=0.0, shine=16, rim=0.0):
        """Light points at 'pos' (N,3) with normals 'n' (N,3)."""
        out = albedo * self.ambient
        view = np.array([0, 0, -1], np.float32)
        for d, col in self.dirs:
            lam = np.clip((n * d).sum(-1), 0, 1)[:, None]
            out = out + albedo * col * lam
            if rim:
                edge = (1 - np.clip((n * view).sum(-1), 0, 1)) ** 3
                out = out + col * rim * (edge * np.clip((n * d).sum(-1) + 0.1,
                                                        0, 1))[:, None]
            if spec:
                hv = normalize(d + view)
                out = out + col * spec * (np.clip((n * hv).sum(-1), 0, 1) **
                                          shine)[:, None]
        for p, col, reach in self.points:
            l = p - pos
            dist = np.linalg.norm(l, axis=-1, keepdims=True)
            l = l / np.maximum(dist, 1e-6)
            fall = 1.0 / (1.0 + (dist / reach) ** 2)
            lam = np.clip((n * l).sum(-1, keepdims=True), 0, 1)
            out = out + albedo * col * lam * fall
            if spec:
                hv = normalize(l + view)
                out = out + col * spec * fall * (
                    np.clip((n * hv).sum(-1, keepdims=True), 0, 1) ** shine)
        return out

    # -- solids --------------------------------------------------------------
    def ellipsoid(self, centre, radii, albedo, R=None, spec=0.0, shine=16,
                  rim=0.0, texture=None, clip_y=None):
        """A lit ellipsoid.  'texture(q, n)' may return an albedo multiplier
        (HxW) from the local position q (in radii units) and normal.
        'clip_y' cuts away whatever lies below that local y (radii units,
        -1..1, y downward), so a helmet can be a dome."""
        c = np.array(centre, np.float32) * SS
        r = np.array(radii, np.float32) * SS
        R = np.eye(3, dtype=np.float32) if R is None else R
        ext = r.max() * 1.02
        x0, x1 = int(max(0, c[0] - ext)), int(min(self.w, c[0] + ext + 1))
        y0, y1 = int(max(0, c[1] - ext)), int(min(self.h, c[1] + ext + 1))
        if x0 >= x1 or y0 >= y1:
            return
        yy, xx = np.mgrid[y0:y1, x0:x1].astype(np.float32)
        o = np.stack([xx - c[0], yy - c[1], -c[2] + 0 * xx], -1) @ R
        d = np.array([0, 0, 1], np.float32) @ R
        A = ((d / r) ** 2).sum()
        B = 2 * (o * d / r ** 2).sum(-1)
        C = ((o / r) ** 2).sum(-1) - 1
        disc = B * B - 4 * A * C
        hit = disc >= 0
        t = (-B - np.sqrt(np.maximum(disc, 0))) / (2 * A)
        q = (o + t[..., None] * d) / r
        if clip_y is not None:
            hit &= q[..., 1] <= clip_y        # screen y grows downward
        hit &= t < self.z[y0:y1, x0:x1]
        if not hit.any():
            return
        nl = q / r
        n = normalize(nl @ R.T)
        pos = np.stack([xx, yy, t], -1)
        alb = np.broadcast_to(np.array(albedo, np.float32) / 255.0,
                              q.shape).copy()
        if texture is not None:
            alb *= texture(q, n)[..., None]
        col = self.light(pos[hit], n[hit], alb[hit], spec, shine, rim)
        self.rgb[y0:y1, x0:x1][hit] = col
        self.z[y0:y1, x0:x1][hit] = t[hit]

    def surface(self, mask, z, albedo, normal=(0, 0, -1), spec=0.0,
                shine=12, normals=None):
        """A face over 'mask' (full-size bool array) at depth 'z' (scalar
        or array), albedo (H,W,3 in 0..1 or a colour), lit from its
        normal (or per-pixel 'normals')."""
        zz = np.broadcast_to(np.asarray(z, np.float32) * SS, mask.shape)
        m = mask & (zz < self.z)
        if not m.any():
            return
        yy, xx = np.nonzero(m)
        pos = np.stack([xx, yy, zz[m]], -1).astype(np.float32)
        if normals is None:
            n = np.broadcast_to(normalize(np.array(normal, np.float32)),
                                pos.shape)
        else:
            n = normals[m]
        a = np.asarray(albedo, np.float32)
        a = a[m] if a.ndim == 3 else np.broadcast_to(a / 255.0, pos.shape)
        self.rgb[m] = self.light(pos, n, a, spec, shine)
        self.z[m] = zz[m]

    def cylinder(self, x, y0, y1, radius, z, albedo, spec=0.0, shine=16,
                 rim=0.0, texture=None):
        """An upright cylinder (a stove, a flue, a bottle's body) from y0
        to y1 at 1x, its front face lit round its curve."""
        cx, r = x * SS, radius * SS
        x0, x1 = int(max(0, cx - r)), int(min(self.w, cx + r + 1))
        ya, yb = int(max(0, y0 * SS)), int(min(self.h, y1 * SS))
        if x0 >= x1 or ya >= yb:
            return
        yy, xx = np.mgrid[ya:yb, x0:x1].astype(np.float32)
        u = (xx - cx) / r
        hit = np.abs(u) <= 1
        nz = -np.sqrt(np.clip(1 - u * u, 0, 1))
        t = z * SS + nz * r
        hit &= t < self.z[ya:yb, x0:x1]
        if not hit.any():
            return
        n = np.stack([u, 0 * u, nz], -1)
        pos = np.stack([xx, yy, t], -1)
        alb = np.broadcast_to(np.array(albedo, np.float32) / 255.0,
                              n.shape).copy()
        if texture is not None:
            alb *= texture(u, (yy / SS - y0) / max(1e-6, y1 - y0))[..., None]
        self.rgb[ya:yb, x0:x1][hit] = self.light(pos[hit], n[hit], alb[hit],
                                                 spec, shine, rim)
        self.z[ya:yb, x0:x1][hit] = t[hit]

    def glow(self, centre, radius, colour, strength=1.0):
        """Additive light: a flame, a lamp mantle, a fire door."""
        cx, cy = centre[0] * SS, centre[1] * SS
        r = radius * SS
        x0, x1 = int(max(0, cx - r * 3)), int(min(self.w, cx + r * 3))
        y0, y1 = int(max(0, cy - r * 3)), int(min(self.h, cy + r * 3))
        yy, xx = np.mgrid[y0:y1, x0:x1].astype(np.float32)
        g = np.exp(-(((xx - cx) ** 2 + (yy - cy) ** 2) / (r * r)))
        self.rgb[y0:y1, x0:x1] += g[..., None] * np.array(colour, np.float32) \
            / 255.0 * strength

    # -- out -----------------------------------------------------------------
    def finish(self, exposure=1.0):
        """Tone-map, filter down to 1x and dither to RGB565."""
        x = 1.0 - np.exp(-self.rgb * exposure * 1.6)
        img = Image.fromarray(np.clip(x * 255 + 0.5, 0, 255).astype(np.uint8))
        img = img.resize((self.w // SS, self.h // SS), Image.LANCZOS)
        return dither565(img)


def dither565(img):
    """Ordered dither to the 5-6-5 levels the Amiga screen shows."""
    a = np.asarray(img, np.float32)
    bayer = np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9],
                      [15, 7, 13, 5]], np.float32) / 16.0 - 0.5
    h, w = a.shape[:2]
    t = np.tile(bayer, (h // 4 + 1, w // 4 + 1))[:h, :w]
    out = np.empty_like(a)
    for c, bits in enumerate((5, 6, 5)):
        step = 255.0 / (2 ** bits - 1)
        out[..., c] = np.round(a[..., c] / step + t) * step
    return Image.fromarray(np.clip(out, 0, 255).astype(np.uint8))


# ---- materials ---------------------------------------------------------------
def planks(h, w, seed, base=(118, 82, 50), widths=(18, 30), vertical=True,
           soot_top=0.0, wear=None, nails=True):
    """Boards of rough-sawn timber: returns (albedo 0..1 HxWx3, normals
    HxWx3, gap mask).  Each board has its own width and tone, grain lines
    that swirl round its knots, nail heads in rows, and a dark gap with a
    lit edge on one side and a shadowed edge on the other."""
    rng = np.random.default_rng(seed)
    if not vertical:
        a, n, g = planks(w, h, seed, base, widths, True, 0.0, None, nails)
        a, n, g = a.transpose(1, 0, 2), n.transpose(1, 0, 2)[..., [1, 0, 2]], g.T
    else:
        yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
        a = np.zeros((h, w, 3), np.float32)
        n = np.zeros((h, w, 3), np.float32)
        n[..., 2] = -1
        g = np.zeros((h, w), bool)
        warp = fbm(h, w, seed + 1, 4, 4) - 0.5
        fine = fbm(h, w, seed + 2, 3, 24)
        x = 0
        while x < w:
            bw = int(rng.integers(widths[0], widths[1]) * SS)
            sl = slice(x, min(w, x + bw))
            u = (xx[:, sl] - x) / bw              # 0..1 across the board
            tone = rng.normal(1.0, 0.09)
            hue = np.array([rng.normal(1, 0.03), rng.normal(1, 0.03),
                            rng.normal(1, 0.05)])
            # Grain: lines along the board, pushed round the knots.
            phase = u * rng.uniform(9, 16) + warp[:, sl] * 3.0
            for _ in range(rng.integers(0, 3) if bw > 12 * SS else 0):
                ky = rng.uniform(0, h)
                kx = rng.uniform(0.2, 0.8)
                d = np.sqrt(((u - kx) * bw / (5 * SS)) ** 2 +
                            ((yy[:, sl] - ky) / (9 * SS)) ** 2)
                phase += 2.4 * np.exp(-d * d) * np.sign(u - kx)
                knot = np.exp(-(d * 1.6) ** 2)
                a[:, sl] -= knot[..., None] * 0.35
            grain = 0.5 + 0.5 * np.sin(phase * math.pi * 2)
            grain = grain ** 3
            v = tone * (0.86 + 0.14 * fine[:, sl] - 0.18 * grain)
            a[:, sl] += v[..., None] * np.array(base) / 255.0 * hue
            # Bevelled edges: the left lit, the right in shade, and the gap.
            n[:, sl, 0] += np.where(u < 0.06, -0.6, np.where(u > 0.94, 0.6, 0))
            gap = (u > 0.97)
            g[:, sl] |= gap
            # Nail heads in rows.
            for ny in (np.arange(rng.uniform(20, 60), h / SS,
                                 rng.uniform(80, 120)) if nails else ()):
                for nx in (0.18, 0.82):
                    cy, cx = ny * SS, x + nx * bw
                    d2 = (xx[:, sl] - cx) ** 2 + (yy[:, sl] - cy) ** 2
                    head = d2 < (1.6 * SS) ** 2
                    a[:, sl][head] = np.array([0.16, 0.15, 0.14])
                    shine = d2 < (0.6 * SS) ** 2
                    a[:, sl][shine & head] = np.array([0.4, 0.38, 0.34])
            x += bw
        a[g] *= 0.12
        n = normalize(n)
        if soot_top:
            s = np.clip(1 - yy / (h * 0.45), 0, 1) ** 2 * soot_top
            a *= (1 - s * (0.6 + 0.4 * fine))[..., None]
    if wear is not None:
        y0, y1 = wear
        band = np.exp(-((np.arange(a.shape[0]) - (y0 + y1) / 2) /
                        ((y1 - y0) / 2)) ** 2)[:, None, None]
        a = a * (1 + 0.25 * band)
    return np.clip(a, 0, 1), n, g


def corrugated(h, w, seed, base=(96, 92, 86), pitch=7.0):
    """Corrugated iron seen from beneath: normals from the ripple, rust
    bleeding down from the laps and nail lines."""
    yy, xx = np.mgrid[0:h, 0:w].astype(np.float32)
    ph = xx / (pitch * SS) * 2 * math.pi
    n = np.stack([np.sin(ph) * 0.55, 0 * ph, -np.ones_like(ph)], -1)
    n = normalize(n)
    rust = fbm(h, w, seed, 5, 6)
    streak = fbm(h, w, seed + 3, 3, 30)
    rust = np.clip((rust - 0.45) * 2.4 + (streak - 0.5) * 0.6, 0, 1)
    a = np.array(base, np.float32) / 255.0 * (0.85 + 0.15 * streak[..., None])
    a = a * (1 - rust[..., None]) + np.array([0.42, 0.22, 0.12]) * rust[..., None]
    return a, n
