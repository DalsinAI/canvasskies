#!/usr/bin/env python3
"""Paint the wing cellule seen from the pilot's seat.

The same geometry the game used to draw at start-up (a Camel's wings
around the pilot's eye, projected 112 degrees wide), painted here at four
times size: the top wing's underside in clear-doped linen with stitched
rib tapes, the low sun glowing through it between the ribs and a varnished
leading edge; streamlined spruce struts with grain, a lit edge and steel
fittings at both ends; stranded bracing wires with their turnbuckles.
The cockpit painting is laid over it in the game, hiding the struts' feet.
Writes assets/canvas-skies/cockpit-cellule.bmp (magenta is clear).
"""

import math
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFilter

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "canvas-skies" / "cockpit-cellule.bmp"
SS = 4
W, H = 640, 400
F, CY = 216.0, 184.0
NEAR = 0.06
UP_LE, UP_TE, UP_Z, SPAN = 1.10, -0.27, -0.53, 4.27
CUT_Y, CUT_TO = 0.38, 0.28
LO_LE, LO_TE, LO_Z, LO_ROOT, DIH = 0.64, -0.73, 1.02, 0.42, 0.0875
FRONT, REAR = 0.25, 0.70
CAB_Y, CAB_FOOT = 0.36, (0.46, 0.77)
STRUT_Y = (1.25, 2.75)
RNG = np.random.default_rng(1917)


def chord(le, te, frac):
    return le - (le - te) * frac


def low_z(y):
    return LO_Z - abs(y) * DIH


def project(p):
    x, y, z = p
    return (320.0 + y * F / x) * SS, (CY + z * F / x) * SS


def clip(a, b):
    a, b = np.array(a, float), np.array(b, float)
    if a[0] < NEAR and b[0] < NEAR:
        return None
    if a[0] < NEAR:
        a = a + (b - a) * (NEAR - a[0]) / (b[0] - a[0])
    if b[0] < NEAR:
        b = b + (a - b) * (NEAR - b[0]) / (a[0] - b[0])
    return a, b


def wings():
    """Ray-cast both wings' visible surfaces at full resolution."""
    yy, xx = np.mgrid[0:H * SS, 0:W * SS].astype(np.float32)
    dx = (xx / SS - 320.0) / F
    dz = (yy / SS - CY) / F
    rgb = np.zeros((H * SS, W * SS, 3), np.float32)
    hit = np.zeros((H * SS, W * SS), bool)
    # Upper wing underside.
    up = dz < -0.001
    x = np.where(up, UP_Z / np.where(up, dz, -1), 0)
    y = x * dx
    m = up & (x <= UP_LE) & (np.abs(y) <= SPAN) & \
        ~((np.abs(y) < CUT_Y) & (x < CUT_TO))
    c = (x - UP_TE) / (UP_LE - UP_TE)
    rib = np.abs(y) / (SPAN / 17.0)
    ribf = rib - np.floor(rib)
    tape = np.clip(1 - np.abs(ribf - 0.5) * 2 / 0.08, 0, 1) ** 0.5   # 0..1
    tape = np.where((ribf < 0.05) | (ribf > 0.95), 1.0, 0.0)
    # Light through the linen between the ribs, brighter to the sun side.
    between = np.sin(np.pi * ribf) ** 0.6
    sun = 0.55 + 0.45 * (y / SPAN)
    glow = between * (0.55 + 0.45 * sun)
    stitch = tape * (np.sin(x * 260.0) > 0.4)
    base = np.stack([128 + glow * 70, 110 + glow * 52, 86 + glow * 30], -1)
    base += tape[..., None] * np.array([6, 4, 2])          # tapes stand proud
    edge_t = np.where((np.abs(ribf - 0.05) < 0.012) | (np.abs(ribf - 0.95) < 0.012), 1.0, 0.0)
    base -= edge_t[..., None] * np.array([30, 28, 24])     # and cast a hairline
    base -= stitch[..., None] * np.array([10, 9, 8])
    base -= ((c > 0.93) * (c - 0.93) * 300)[..., None] * np.array([0.2, 0.25, 0.3])
    lead = (c > 0.975)
    base = np.where(lead[..., None], np.array([196, 160, 110]), base)  # varnished LE
    trail = c < 0.03
    base = np.where(trail[..., None], base * 0.75, base)
    rim = m & (np.abs(np.abs(y) - CUT_Y) < 0.02) & (x < CUT_TO)
    base = np.where(rim[..., None], base * 0.5, base)
    rgb[m] = base[m]
    hit |= m
    # Lower wing top, PC10 khaki.
    lo = dz > 0.001
    k = np.abs(dx) * DIH
    x2 = np.where(lo, LO_Z / np.where(lo, dz + k, 1), 0)
    y2 = x2 * dx
    m2 = lo & (x2 <= LO_LE) & (np.abs(y2) >= LO_ROOT) & (np.abs(y2) <= SPAN * 0.95)
    c2 = (x2 - LO_TE) / (LO_LE - LO_TE)
    r2 = np.abs(y2) / (SPAN / 17.0)
    t2 = np.where((r2 - np.floor(r2) < 0.06) | (r2 - np.floor(r2) > 0.94), 1.0, 0.0)
    fade = (np.abs(y2) - LO_ROOT) / (SPAN - LO_ROOT)
    light = 0.6 + 0.4 * (1 - c2)
    lo_rgb = np.stack([92 + light * 40 + t2 * 18 - fade * 30,
                       80 + light * 26 + t2 * 14 - fade * 20,
                       48 + light * 14 + t2 * 8 - fade * 6], -1)
    rgb[m2 & ~hit] = lo_rgb[m2 & ~hit]
    hit |= m2
    rgb += RNG.normal(0, 3.0, rgb.shape[:2])[..., None] * hit[..., None]
    return rgb, hit


def paint_strut(rgb, hit, a, b, width):
    """A streamlined spruce strut, teardrop in section: seen end-on it is
    narrow, lit softly along its rounded leading edge and shaded down the
    long tapering trailing edge.  It narrows into the distance toward the
    wing, where a flat steel socket plate with a wire lug sits flush on the
    linen and casts a little shadow on it."""
    seg = clip(a, b)
    if seg is None:
        return
    a, b = seg
    (ax, ay), (bx, by) = project(a), project(b)
    wa, wb = width * F / a[0] * SS * 0.62, width * F / b[0] * SS * 0.62
    wa, wb = min(wa, 40 * SS), min(wb, 40 * SS)
    wa = min(wa, wb * 0.72)                    # taper away from the eye
    length = math.hypot(bx - ax, by - ay)
    if length < 1:
        return
    ux, uy = (bx - ax) / length, (by - ay) / length
    nx, ny = -uy, ux
    pad = max(wa, wb) * 2
    x0 = int(max(0, min(ax, bx) - pad)); x1 = int(min(W * SS, max(ax, bx) + pad))
    y0 = int(max(0, min(ay, by) - pad)); y1 = int(min(H * SS, max(ay, by) + pad))
    if x1 <= x0 or y1 <= y0:
        return
    yy, xx = np.mgrid[y0:y1, x0:x1].astype(np.float32)
    t = ((xx - ax) * ux + (yy - ay) * uy) / length
    s = (xx - ax) * nx + (yy - ay) * ny
    half = (wa + (wb - wa) * np.clip(t, 0, 1)) / 2
    region = rgb[y0:y1, x0:x1]
    # The socket's shadow on the linen, beside the strut's head.
    d = t * length                              # along, in pixels
    shadow = (d > -1.0 * SS) & (d < 3.0 * SS) & (s > half * 0.4) & \
        (s < half * 2.4) & hit[y0:y1, x0:x1]
    region[shadow] *= 0.72
    inside = (t >= 0) & (t <= 1) & (np.abs(s) <= half)
    u = np.clip(s / np.maximum(half, 1e-3), -1, 1)
    # Teardrop: the rounded nose on the lit side (u < 0), the long tail
    # falling away into shade.
    nose = np.clip(1 - np.abs(u + 0.55) / 0.45, 0, 1)
    tail = np.clip((u + 0.1) / 1.1, 0, 1)
    shade = 0.62 + 0.42 * nose - 0.42 * tail
    grain = np.sin(t * length * 0.11 + np.sin(t * length * 0.017) * 2.2) * 0.05
    col = np.stack([156 * (shade + grain), 104 * (shade + grain),
                    58 * (shade + grain)], -1)
    spec = np.clip(1 - np.abs(u + 0.6) / 0.1, 0, 1) * 0.8
    col += spec[..., None] * np.array([60, 48, 34])
    # The socket plate at the head: flat steel, just wider than the strut,
    # and the wire lug standing off it.
    plate = (d >= -0.5 * SS) & (d < 2.2 * SS) & (np.abs(s) <= half * 1.25)
    col_plate = np.array([84, 84, 88]) * (0.85 + 0.3 * np.clip(-u, 0, 1))[..., None]
    lug = (d >= 0.5 * SS) & (d < 3.5 * SS) & (np.abs(s - half * 1.45) < 0.9 * SS)
    region[inside] = np.clip(col[inside], 0, 255)
    region[plate] = np.clip(col_plate[plate], 0, 255)
    region[lug] = (70, 70, 74)
    hit[y0:y1, x0:x1] |= inside | plate | lug


def paint_wire(img, a, b, tension=1.0):
    """Stranded steel wire: a dark core with a fine lit strand."""
    seg = clip(a, b)
    if seg is None:
        return
    a, b = seg
    pa, pb = project(a), project(b)
    d = ImageDraw.Draw(img)
    d.line([pa, pb], fill=(36, 34, 32, 255), width=int(SS * 1.6))
    off = SS * 0.5
    d.line([(pa[0] - off, pa[1] - off), (pb[0] - off, pb[1] - off)],
           fill=(150, 146, 136, 255), width=max(1, SS // 2))
    # A turnbuckle a fifth of the way along.
    tx, ty = pa[0] + (pb[0] - pa[0]) * 0.2, pa[1] + (pb[1] - pa[1]) * 0.2
    d.ellipse((tx - SS * 1.8, ty - SS * 1.8, tx + SS * 1.8, ty + SS * 1.8),
              fill=(70, 68, 64, 255))


def main():
    rgb, hit = wings()
    layer = Image.new("RGBA", (W * SS, H * SS), (0, 0, 0, 0))
    for side in (-1, 1):
        root, inner, outer = side * LO_ROOT, side * STRUT_Y[0], side * STRUT_Y[1]
        for frac in (FRONT, REAR):
            ux, lx = chord(UP_LE, UP_TE, frac), chord(LO_LE, LO_TE, frac)
            paint_wire(layer, (lx, root, low_z(root)), (ux, inner, UP_Z))
            paint_wire(layer, (ux, side * CAB_Y, UP_Z), (lx, inner, low_z(inner)))
            paint_wire(layer, (lx, inner, low_z(inner)), (ux, outer, UP_Z))
            paint_wire(layer, (ux, inner, UP_Z), (lx, outer, low_z(outer)))
        for y in (inner, outer):
            fu, ru = chord(UP_LE, UP_TE, FRONT), chord(UP_LE, UP_TE, REAR)
            fl, rl = chord(LO_LE, LO_TE, FRONT), chord(LO_LE, LO_TE, REAR)
            paint_wire(layer, (fu, y, UP_Z), (rl, y, low_z(y)))
            paint_wire(layer, (ru, y, UP_Z), (fl, y, low_z(y)))
    wire = np.asarray(layer).astype(np.float32)
    a = wire[..., 3:4] / 255.0
    rgb = rgb * (1 - a) + wire[..., :3] * a
    hit |= a[..., 0] > 0.5
    for side in (-1, 1):
        for y in (side * STRUT_Y[1], side * STRUT_Y[0]):
            for frac in (REAR, FRONT):
                paint_strut(rgb, hit, (chord(UP_LE, UP_TE, frac), y, UP_Z),
                            (chord(LO_LE, LO_TE, frac), y, low_z(y)), 0.11)
        for frac in (REAR, FRONT):
            cx = chord(UP_LE, UP_TE, frac)
            paint_strut(rgb, hit, (cx, side * CAB_Y, UP_Z),
                        (cx - 0.15, side * CAB_FOOT[0], CAB_FOOT[1]), 0.10)
    img = Image.fromarray(np.clip(rgb, 0, 255).astype(np.uint8))
    alpha = Image.fromarray((hit * 255).astype(np.uint8))
    img = img.resize((W, H), Image.LANCZOS)
    alpha = alpha.resize((W, H), Image.LANCZOS)
    out = Image.new("RGB", (W, H), (255, 0, 255))
    out.paste(img, (0, 0), alpha.point(lambda v: 255 if v > 100 else 0))
    out.save(OUT)


if __name__ == "__main__":
    main()
