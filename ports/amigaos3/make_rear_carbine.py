#!/usr/bin/env python3
"""Render Neddy's carbine in the rear seat, aimed anywhere in the aft arc.

The rifle alone, held in the air where he would hold it, with no arms or
hands: it is a solid in the observer's own view: the eye at the origin looking aft over the tail, x to
the right, y up, z away.  Every pixel is ray-cast through a pinhole for its
true depth and surface normal, then lit by the low sun beyond the tail, a
fill from the sky behind the observer and a rim of sunlight round the edges.
Parallel lines meet at their vanishing point, so a rifle aimed at a point
on the screen visibly runs towards it: back into the picture when aimed
astern, across it when swung onto a flank, up it when raised.

One frame is rendered for each aim point of a grid over the crosshair's
range.  The butt sits where his shoulder would be, turning and lifting
partly with the aim.

  assets/canvas-skies/rear-carbine.bmp   the frames, cropped and packed,
                                         256 colours with magenta as key,
                                         run-length encoded
  src/modern/canvas_rear_carbine.h       where each frame sits on the
                                         screen, and its muzzle
"""

import math
from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"
HEADER = ROOT / "src" / "modern" / "canvas_rear_carbine.h"

W, H = 640, 400
CX, CY = 320.0, 200.0
FOCAL = 330.0                  # pixels: about 88 degrees across the screen
SS = 2
KEY = (255, 0, 255)

# The carbine's aim range in the game (update_perspective_sortie), kept
# above the signs, and the grid of aims rendered over it.
AIM_X = np.linspace(28, 612, 13)
AIM_Y = np.linspace(40, 262, 6)

WOOD = np.array([0.40, 0.24, 0.13])
WOOD_LIGHT = np.array([0.50, 0.32, 0.18])
WOOD_DARK = np.array([0.28, 0.16, 0.09])
STEEL = np.array([0.17, 0.18, 0.20])
STEEL_DARK = np.array([0.10, 0.10, 0.12])

SUN = np.array([0.10, 0.32, 1.0]); SUN /= np.linalg.norm(SUN)
SUN_COL = np.array([1.35, 1.00, 0.66])
FILL = np.array([-0.45, 0.65, -0.62]); FILL /= np.linalg.norm(FILL)
FILL_COL = np.array([0.78, 0.74, 0.68])
AMBIENT = np.array([0.20, 0.21, 0.25])


def unit(v):
    v = np.asarray(v, np.float64)
    return v / np.linalg.norm(v)


class View:
    """A colour, depth and coverage buffer at SS times the screen."""

    def __init__(self):
        self.w, self.h = W * SS, H * SS
        self.rgb = np.zeros((self.h, self.w, 3))
        self.z = np.full((self.h, self.w), np.inf)

    def rays(self, box):
        x0, y0, x1, y1 = box
        yy, xx = np.mgrid[y0:y1, x0:x1].astype(np.float64)
        d = np.stack([((xx + 0.5) / SS - CX) / FOCAL,
                      (CY - (yy + 0.5) / SS) / FOCAL,
                      np.ones_like(xx)], -1)
        return d

    def bbox(self, points, pad):
        """Screen box (at SS) round some points padded by 'pad' metres."""
        pts = np.asarray(points, np.float64)
        if (pts[:, 2] - pad < 0.05).any():
            return 0, 0, self.w, self.h
        xs, ys = [], []
        for p in pts:
            for dx in (-pad, pad):
                for dy in (-pad, pad):
                    z = p[2] - pad
                    xs.append(CX + FOCAL * (p[0] + dx) / z)
                    ys.append(CY - FOCAL * (p[1] + dy) / z)
        x0 = max(0, int(math.floor(min(xs) * SS)) - 2)
        x1 = min(self.w, int(math.ceil(max(xs) * SS)) + 2)
        y0 = max(0, int(math.floor(min(ys) * SS)) - 2)
        y1 = min(self.h, int(math.ceil(max(ys) * SS)) + 2)
        return x0, y0, x1, y1

    def shade(self, box, t, n, albedo, spec, local=None, texture=None):
        x0, y0, x1, y1 = box
        hit = np.isfinite(t) & (t > 0.30) & (t < self.z[y0:y1, x0:x1])
        if not hit.any():
            return
        n = n / np.maximum(np.linalg.norm(n, axis=-1, keepdims=True), 1e-9)
        alb = np.broadcast_to(albedo, n.shape).copy()
        if texture is not None:
            alb *= texture(local)[..., None]
        view = -self.rays(box)
        view /= np.linalg.norm(view, axis=-1, keepdims=True)
        sun = np.clip((n * SUN).sum(-1), 0, 1)[..., None]
        fill = np.clip((n * FILL).sum(-1), 0, 1)[..., None]
        sky = (0.6 + 0.4 * n[..., 1:2])
        col = alb * (AMBIENT * sky + SUN_COL * sun * 0.75 + FILL_COL * fill)
        # Sunlight wraps round the silhouette from behind: a warm rim.
        edge = (1 - np.clip((n * view).sum(-1), 0, 1)) ** 3
        col += SUN_COL * 0.32 * (edge * np.clip((n * SUN).sum(-1) + 0.35,
                                                0, 1))[..., None]
        if spec:
            for L, C in ((SUN, SUN_COL), (FILL, FILL_COL)):
                hv = view + L
                hv /= np.linalg.norm(hv, axis=-1, keepdims=True)
                col += C * spec * (np.clip((n * hv).sum(-1), 0, 1) ** 24)[..., None]
        self.rgb[y0:y1, x0:x1][hit] = col[hit]
        self.z[y0:y1, x0:x1][hit] = t[hit]

    def capsule(self, a, b, r, albedo, spec=0.0, texture=None):
        a, b = np.asarray(a, np.float64), np.asarray(b, np.float64)
        box = self.bbox([a, b], r)
        if box[0] >= box[2] or box[1] >= box[3]:
            return
        d = self.rays(box)
        ba = b - a
        baba = ba @ ba
        dd = (d * d).sum(-1)
        oa = -a
        bard = d @ ba
        baoa = oa @ ba
        rdoa = d @ oa
        oaoa = oa @ oa
        A = baba * dd - bard * bard
        B = baba * rdoa - baoa * bard
        C = baba * oaoa - baoa * baoa - r * r * baba
        h = B * B - A * C
        with np.errstate(invalid="ignore", divide="ignore"):
            t = (-B - np.sqrt(h)) / A
            y = baoa + t * bard
        t = np.where((h >= 0) & (y > 0) & (y < baba), t, np.inf)
        best = t.copy()
        for c in (a, b):
            oc = -c
            bb = d @ oc
            cc = oc @ oc - r * r
            hh = bb * bb - dd * cc
            with np.errstate(invalid="ignore"):
                ts = (-bb - np.sqrt(hh)) / dd
            ts = np.where(hh >= 0, ts, np.inf)
            best = np.minimum(best, ts)
        pos = d * best[..., None]
        s = np.clip(((pos - a) @ ba) / baba, 0, 1)
        axis = a + s[..., None] * ba
        n = pos - axis
        self.shade(box, best, n, albedo, spec,
                   local=(s, n), texture=texture)

    def box(self, centre, axes, half, albedo, spec=0.0, texture=None,
            bevel=0.25):
        """An oriented box: 'axes' are its three unit axes (rows), 'half'
        its half sizes along them.  Edges darken a little, as worn wood
        and steel are rounded off."""
        c = np.asarray(centre, np.float64)
        R = np.asarray(axes, np.float64)
        hs = np.asarray(half, np.float64)
        corners = [c + R.T @ (hs * np.array(s)) for s in
                   ((1, 1, 1), (1, 1, -1), (1, -1, 1), (1, -1, -1),
                    (-1, 1, 1), (-1, 1, -1), (-1, -1, 1), (-1, -1, -1))]
        box = self.bbox(corners, 0.0)
        if box[0] >= box[2] or box[1] >= box[3]:
            return
        d = self.rays(box)
        o = R @ (-c)
        dl = d @ R.T
        with np.errstate(divide="ignore", invalid="ignore"):
            t1 = (-hs - o) / dl
            t2 = (hs - o) / dl
        tn = np.minimum(t1, t2)
        tf = np.maximum(t1, t2)
        tmin = np.nanmax(tn, -1)
        tmax = np.nanmin(tf, -1)
        ok = (tmax >= tmin) & (tmax > 0)
        t = np.where(ok, tmin, np.inf)
        face = np.argmax(np.nan_to_num(tn, nan=-np.inf), -1)
        q = o + dl * np.where(np.isfinite(t), t, 0)[..., None]
        nl = np.zeros_like(q)
        idx = np.indices(face.shape)
        nl[idx[0], idx[1], face] = np.sign(q[idx[0], idx[1], face])
        # Rounded edges: bend the normal towards the nearer side faces.
        u = q / hs
        for k in range(3):
            near = np.clip((np.abs(u[..., k]) - (1 - bevel)) / bevel, 0, 1)
            nl[..., k] += np.where(face == k, 0, np.sign(u[..., k]) * near)
        n = nl @ R
        self.shade(box, t, n, albedo, spec, local=(u, None),
                   texture=texture)

    def ellipsoid(self, centre, axes, radii, albedo, spec=0.0):
        c = np.asarray(centre, np.float64)
        R = np.asarray(axes, np.float64)
        r = np.asarray(radii, np.float64)
        box = self.bbox([c], r.max())
        if box[0] >= box[2] or box[1] >= box[3]:
            return
        d = self.rays(box)
        o = (R @ (-c)) / r
        dl = (d @ R.T) / r
        A = (dl * dl).sum(-1)
        B = (dl * o).sum(-1)
        C = o @ o - 1
        h = B * B - A * C
        with np.errstate(invalid="ignore"):
            t = (-B - np.sqrt(h)) / A
        t = np.where(h >= 0, t, np.inf)
        q = o + dl * np.where(np.isfinite(t), t, 0)[..., None]
        n = (q / r) @ R
        self.shade(box, t, n, albedo, spec)

    def finish(self):
        """Down to the screen size: colour where at least half a pixel is
        covered, magenta elsewhere.  No colour is blended with the key, so
        no purple fringe is left for the game's loose key to fight."""
        cov = np.isfinite(self.z).astype(np.float64)
        col = self.rgb * cov[..., None]
        cov = cov.reshape(H, SS, W, SS).mean((1, 3))
        col = col.reshape(H, SS, W, SS, 3).sum((1, 3)) / (SS * SS)
        col = col / np.maximum(cov, 1e-6)[..., None]
        col = col / (1 + col * 0.35) * 1.18           # soft shoulder
        col = np.clip(col, 0, 1) ** (1 / 1.25)
        out = (col * 255 + 0.5).astype(np.uint8)
        out[cov < 0.5] = KEY
        return out, cov >= 0.5


def grain(scale, amount):
    """Wood grain running along the stock: bands across its width that
    wander slowly down its length."""
    def tex(local):
        s, n = local
        if n is None:          # a box, in its own -1..1 coordinates
            across = s[..., 0] * 0.8 + s[..., 1] * 0.5
            along = s[..., 2]
        else:                  # a capsule: round it, and along it
            across = np.arctan2(n[..., 1], n[..., 0]) * 0.6
            along = s
        wobble = np.sin(along * 7.0 + across * 3.0) * 0.9
        return 1 + amount * np.sin(across * scale * 0.06 + wobble)
    return tex


def rot_y(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def rot_x(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])


def aim_direction(ax, ay):
    return unit([(ax - CX) / FOCAL, (CY - ay) / FOCAL, 1.0])


def render(ax, ay):
    v = View()
    d = aim_direction(ax, ay)
    yaw = math.atan2(d[0], d[2])
    pitch = math.asin(d[1])
    # The shoulders turn and lift part of the way with the aim; the head
    # turns the rest, so the butt stays in the shoulder without coming
    # through the lens.
    body = rot_y(yaw * 0.55) @ rot_x(-pitch * 0.35)
    # Aiming down, he leans out over the coaming and brings the butt up,
    # so the rifle stays in sight above the signs.
    lean = np.array([0.11, 0.085, 0.0]) * min(1.0, max(0.0, -pitch - 0.02) / 0.28)
    butt = body @ np.array([0.075, -0.215, 0.44]) + lean
    a = d
    up = np.array([0.0, 1.0, 0.0])
    u = unit(up - (up @ a) * a)
    r = np.cross(u, a)
    # A little cant: the rifle rolls with the observer's turn.
    cant = -yaw * 0.18
    u, r = (u * math.cos(cant) + r * math.sin(cant),
            r * math.cos(cant) - u * math.sin(cant))
    axes = np.stack([r, u, a])

    def at(s, du=0.0, dr=0.0):
        return butt + a * s + u * du + r * dr

    def piece(s0, s1, w, h, du, albedo, **kw):
        v.box(at((s0 + s1) / 2, du), axes, (w / 2, h / 2, (s1 - s0) / 2),
              albedo, **kw)

    # The rifle, butt plate at s = 0, muzzle at s = 1.0.
    wood = grain(260.0, 0.10)
    piece(0.0, 0.15, 0.044, 0.118, -0.036, WOOD, spec=0.10, texture=wood)
    piece(0.15, 0.27, 0.040, 0.082, -0.026, WOOD, spec=0.10, texture=wood)
    piece(0.0, 0.012, 0.046, 0.122, -0.036, STEEL, spec=0.4)   # butt plate
    v.capsule(at(0.26, -0.016), at(0.34, -0.010), 0.019, WOOD_DARK,
              spec=0.10, texture=grain(200.0, 0.08))           # wrist
    v.capsule(at(0.315, -0.050), at(0.36, -0.048), 0.005, STEEL, spec=0.4)
    piece(0.33, 0.47, 0.030, 0.036, 0.000, STEEL, spec=0.45)    # receiver
    piece(0.37, 0.445, 0.024, 0.048, -0.040, STEEL_DARK, spec=0.3)  # magazine
    v.capsule(at(0.36, 0.008, 0.012), at(0.37, -0.012, 0.060), 0.0055, STEEL,
              spec=0.5)                                         # bolt
    v.ellipsoid(at(0.37, -0.014, 0.064), axes, (0.011, 0.011, 0.011),
                STEEL, spec=0.6)                                # bolt knob
    piece(0.455, 0.49, 0.016, 0.022, 0.026, STEEL, spec=0.5)    # rear sight
    piece(0.46, 0.86, 0.034, 0.038, -0.006, WOOD, spec=0.10, texture=wood)
    piece(0.48, 0.84, 0.022, 0.012, 0.018, WOOD_LIGHT, spec=0.12,
          texture=grain(300.0, 0.08))                           # handguard
    for s in (0.62, 0.80):
        piece(s, s + 0.016, 0.038, 0.050, 0.000, STEEL, spec=0.45)
    piece(0.855, 0.925, 0.028, 0.034, 0.002, STEEL, spec=0.5)   # nose cap
    v.capsule(at(0.86, 0.004), at(1.0, 0.004), 0.0085, STEEL, spec=0.55)
    for side in (-1, 1):
        piece(0.90, 0.935, 0.004, 0.026, 0.026, STEEL, spec=0.4,
              bevel=0.4)                                        # sight ears
        v.box(at(0.917, 0.026, side * 0.009), axes, (0.002, 0.013, 0.017),
              STEEL, spec=0.4)
    piece(0.910, 0.925, 0.003, 0.016, 0.022, STEEL)             # blade

    muzzle = at(1.0, 0.004)
    mx = CX + FOCAL * muzzle[0] / muzzle[2]
    my = CY - FOCAL * muzzle[1] / muzzle[2]
    img, mask = v.finish()
    return img, mask, (mx, my)


def crop(img, mask):
    ys, xs = np.nonzero(mask)
    x0, x1, y0, y1 = xs.min(), xs.max() + 1, ys.min(), ys.max() + 1
    return img[y0:y1, x0:x1], (int(x0), int(y0))


def pack(frames, width=1024):
    """Shelf-pack the cropped frames, tallest first."""
    order = sorted(range(len(frames)), key=lambda i: -frames[i][0].shape[0])
    x = y = shelf = 0
    place = [None] * len(frames)
    for i in order:
        h, w = frames[i][0].shape[:2]
        if x + w > width:
            x, y, shelf = 0, y + shelf, 0
        place[i] = (x, y)
        x += w
        shelf = max(shelf, h)
    sheet = np.zeros((y + shelf, width, 3), np.uint8)
    sheet[:] = KEY
    for i, (fx, fy) in enumerate(place):
        f = frames[i][0]
        sheet[fy:fy + f.shape[0], fx:fx + f.shape[1]] = f
    return sheet, place


def rle8_rows(idx):
    """BI_RLE8 for one bottom-up bitmap: runs of a colour as (count, index),
    stretches of differing pixels in absolute mode, an end of line after
    each row and an end of bitmap after the last."""
    out = bytearray()
    for row in idx[::-1]:
        row = row.tolist()
        i, n = 0, len(row)
        literal = []

        def flush():
            while literal:
                chunk = literal[:255]
                del literal[:255]
                if len(chunk) < 3:
                    for c in chunk:
                        out.extend((1, c))
                else:
                    out.extend((0, len(chunk)))
                    out.extend(chunk)
                    if len(chunk) & 1:
                        out.append(0)
        while i < n:
            j = i
            while j < n and row[j] == row[i] and j - i < 255:
                j += 1
            if j - i >= 3:
                flush()
                out.extend((j - i, row[i]))
            else:
                literal.extend(row[i:j])
            i = j
        flush()
        out.extend((0, 0))
    out[-2:] = b"\x00\x01"
    return bytes(out)


def save_rle8(path, idx, palette):
    """An 8-bit BMP, run-length encoded: the sheet is mostly the key, so
    this is a small fraction of its raw size.  SDL_LoadBMP reads it."""
    import struct
    h, w = idx.shape
    data = rle8_rows(idx)
    pal = b"".join(bytes((palette[i * 3 + 2], palette[i * 3 + 1],
                          palette[i * 3], 0)) for i in range(256))
    offset = 14 + 40 + len(pal)
    head = struct.pack("<2sIHHI", b"BM", offset + len(data), 0, 0, offset)
    info = struct.pack("<IiiHHIIiiII", 40, w, h, 1, 8, 1, len(data),
                       2835, 2835, 256, 256)
    Path(path).write_bytes(head + info + pal + data)


def main():
    frames, rects = [], []
    for ay in AIM_Y:
        for ax in AIM_X:
            img, mask, muzzle = render(ax, ay)
            part, origin = crop(img, mask)
            frames.append((part, origin, muzzle))
            print(f"aim {ax:5.0f},{ay:5.0f}: {part.shape[1]}x{part.shape[0]}"
                  f" at {origin}, muzzle {muzzle[0]:.0f},{muzzle[1]:.0f}")
    sheet, place = pack(frames)
    # 255 colours plus the key, so the sheet is a quarter of the size.
    rgb = Image.fromarray(sheet)
    keyed = (sheet == KEY).all(-1)
    q = rgb.quantize(colors=255, method=Image.Quantize.MEDIANCUT,
                     dither=Image.Dither.FLOYDSTEINBERG)
    pal = q.getpalette()[:255 * 3] + list(KEY)
    idx = np.asarray(q).copy()
    idx[keyed] = 255
    save_rle8(ASSETS / "rear-carbine.bmp", idx.astype(np.uint8), pal)
    rows, cols = len(AIM_Y), len(AIM_X)
    with open(HEADER, "w") as f:
        f.write("/* Generated by ports/amigaos3/make_rear_carbine.py: "
                "do not edit. */\n")
        f.write("#ifndef CANVAS_REAR_CARBINE_H\n#define CANVAS_REAR_CARBINE_H\n\n")
        f.write(f"#define CARBINE_AIM_COLUMNS {cols}\n")
        f.write(f"#define CARBINE_AIM_ROWS {rows}\n")
        f.write(f"#define CARBINE_AIM_X0 {AIM_X[0]:.0f}\n")
        f.write(f"#define CARBINE_AIM_X1 {AIM_X[-1]:.0f}\n")
        f.write(f"#define CARBINE_AIM_Y0 {AIM_Y[0]:.0f}\n")
        f.write(f"#define CARBINE_AIM_Y1 {AIM_Y[-1]:.0f}\n\n")
        f.write("/* Per aim, row by row: where the frame lies in the sheet "
                "(x, y, w, h),\n * where it is drawn on the screen (x, y) and "
                "the muzzle on the screen. */\n")
        f.write("static const short carbine_frames[CARBINE_AIM_ROWS * "
                "CARBINE_AIM_COLUMNS][8] = {\n")
        for i, (part, origin, muzzle) in enumerate(frames):
            h, w = part.shape[:2]
            f.write(f"    {{{place[i][0]}, {place[i][1]}, {w}, {h}, "
                    f"{origin[0]}, {origin[1]}, {muzzle[0]:.0f}, "
                    f"{muzzle[1]:.0f}}},\n")
        f.write("};\n\n#endif\n")
    print(f"sheet {sheet.shape[1]}x{sheet.shape[0]}")


if __name__ == "__main__":
    main()
