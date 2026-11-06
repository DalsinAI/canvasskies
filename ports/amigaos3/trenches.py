"""Western Front trenches seen from the air, built as height fields and lit
by the low sun (scene3d.shade_height), for the top-down trench runs.

A tile is a stretch of fire trench running up the screen: fire bays and
traverses in the crenellated plan, the floor laid with duckboards and
water standing in the sumps, the sides revetted, a sandbagged parapet on
the enemy side and a lower parados behind, chalk or clay spoil thrown
along the lip, a belt of wire on screw pickets out in front, and shell
holes broken into it.  One variant carries a communication trench back
from the line.  Tiles repeat vertically without a seam; everything
outside the works is left clear (magenta) so the painted battlefield
shows through.
"""

import math

import numpy as np
from PIL import Image

from scene3d import fbm, seg_distance, shade_height

K = 4                      # internal oversampling
TW, TH = 160, 96           # tile size at 1x
KEY = (255, 0, 255)


def periodic_noise(h, w, seed, octaves=4, base=4):
    """fbm that wraps top to bottom, so the tiles meet without a seam."""
    n = fbm(h * 2, w, seed, octaves, base)
    a, b = n[:h], n[h:]
    t = (np.arange(h, dtype=np.float32) / h)[:, None]
    return a * (1 - t) + b * t


def tile(ours, variant, seed):
    """One trench tile.  'ours' faces west (our wire to the left), theirs
    faces east; 'variant' 0 is a plain stretch, 1 has a communication
    trench and heavier shelling."""
    rng = np.random.default_rng(seed)
    nseed = 700 if ours else 900          # one ground for every tile of a line
    H, W = TH * K, TW * K
    yy, xx = np.mgrid[0:H, 0:W].astype(np.float32) / K
    front = -1 if ours else 1
    cx = TW / 2 - front * 14            # leave room for the wire in front
    # The crenellated centreline: bays alternately forward and back,
    # joined by short jogs round the traverses; two bays a tile.
    off = 8.0
    segs = []
    for k in range(4):
        y0, y1 = k * 24, (k + 1) * 24
        x = cx + (off if k % 2 == 0 else -off)
        segs.append(((x, y0), (x, y1)))
        segs.append(((cx - off, y1), (cx + off, y1)))
    segs.append(((cx - off, 0), (cx + off, 0)))
    if variant:
        # A communication trench zigzagging back from the line to a sap.
        bx = cx - front * off
        pts = [(bx, 60), (bx - front * 12, 54), (bx - front * 20, 64),
               (bx - front * 30, 58), (bx - front * 34, 62)]
        segs += list(zip(pts[:-1], pts[1:]))
    d, which = seg_distance(xx, yy, segs)
    centre_x = np.where(((yy // 24) % 2) == 0, cx + off, cx - off)
    side = np.sign(xx - centre_x) * front          # +1 toward the enemy
    comm = which >= 9
    noise = periodic_noise(H, W, nseed, 5, 6)
    fine = periodic_noise(H, W, nseed + 1, 3, 30)

    half = 4.6
    # Nothing on the ground is ruled: wobble every band's edge.
    rough = (periodic_noise(H, W, nseed + 9, 4, 10) - 0.5)
    d_floor = d + rough * 1.2
    d = d + rough * 4.0
    floor = d_floor < half
    wall = (d >= half) & (d < half + 1.2)
    para = (d >= half + 1.2) & (d < half + 8.0) & ((side > 0) & ~comm)
    parados = (d >= half + 1.2) & (d < half + 6.0) & ~para
    spoil_w = 14 + 7 * (noise - 0.5)
    spoil = (d >= half + 6.0) & (d < half + spoil_w) & ~para
    # Height: the floor cut down, the bags raised in courses, the spoil
    # heaped unevenly along the lip.
    h = np.zeros((H, W), np.float32)
    h[floor] = -4.0
    h[wall] = -2.0 + (d[wall] - half) * 1.6
    along = yy + np.where(comm, xx, 0)
    course = np.floor((d - half - 1.2) / 1.7)
    bag_u = (along / 3.0 + course * 0.5) % 1.0
    bag_v = ((d - half - 1.2) / 1.7) % 1.0
    bump = (np.sin(math.pi * bag_u) * np.sin(math.pi * bag_v)) ** 0.6
    top = 2.6 - 0.25 * course
    h[para] = top[para] * 0.75 + 0.6 * bump[para]
    h[parados] = 1.6 * 0.75 + 0.5 * bump[parados]
    lip = np.clip(1 - (d - half - 6.0) / spoil_w, 0, 1)
    h[spoil] = (0.4 + 1.2 * lip[spoil]) * (0.7 + 0.6 * noise[spoil])
    # Albedo for each part.
    clay = np.array([0.50, 0.40, 0.28])
    chalk = np.array([0.74, 0.72, 0.66])
    spoil_col = clay if ours else chalk
    bag = np.array([0.66, 0.58, 0.40]) if ours else np.array([0.46, 0.46, 0.40])
    alb = np.zeros((H, W, 3), np.float32)
    alb[:] = np.array([0.36, 0.30, 0.22])
    alb[spoil] = (spoil_col * (0.75 + 0.35 * fine[..., None]))[spoil]
    bagtone = (0.82 + 0.25 * fine + 0.12 * np.sin(along * 7.0 + course * 2))
    alb[para | parados] = (bag[None, :] * bagtone[..., None])[para | parados]
    seams = (bag_u < 0.08) | (bag_v < 0.1)
    alb[(para | parados) & seams] *= 0.55
    alb[wall] = np.array([0.34, 0.25, 0.16]) * (0.8 + 0.3 * fine[wall])[:, None]
    # Revetting: corrugated sheet or wattle along the walls, in bands.
    alb[wall & (np.sin(along * 6) > 0.3)] *= 0.7
    # The floor: mud, duckboards down the middle, water in the sumps.
    alb[floor] = np.array([0.24, 0.20, 0.15])
    duck = floor & (d < half * 0.62) & (((along * 1.6) % 1.0) < 0.62)
    alb[duck] = np.array([0.58, 0.46, 0.30]) * (0.85 + 0.2 * fine[duck])[:, None]
    wet = floor & (periodic_noise(H, W, nseed + 5, 3, 3) > 0.62) & ~duck
    # Shell holes broken into the works, some holding water.
    craters = []
    for _ in range(1 + variant * 2):
        r = rng.uniform(6, 11)
        ccx = cx + rng.uniform(-18, 18)
        ccy = rng.uniform(r + 6, TH - r - 6)
        craters.append((ccx, ccy, r))
    crater_any = np.zeros((H, W), bool)
    for _ in range(10 + 6 * variant):               # small shell scrapes
        ccx = cx + rng.uniform(-30, 30)
        ccy = rng.uniform(5, TH - 5)
        craters.append((ccx, ccy, rng.uniform(1.6, 3.5)))
    for ccx, ccy, r in craters:
        rr = np.hypot(xx - ccx, yy - ccy) / r
        bowl = rr < 1.0
        rim = (rr >= 1.0) & (rr < 1.45)
        h[bowl] = np.minimum(h[bowl], -3.5 * (1 - rr[bowl] ** 2))
        h[rim] = np.maximum(h[rim], 1.4 * (1 - (rr[rim] - 1) / 0.45))
        alb[bowl | rim] = (clay * 0.8 * (0.8 + 0.4 * fine[..., None]))[bowl | rim]
        if rng.random() < 0.6:
            pool = rr < 0.45
            wet |= pool
            h[pool] = -2.8
        crater_any |= bowl | rim
    # The wire belt in front: rows of screw pickets, the wire tangled
    # between them.
    wire_x0 = cx + front * (half + 13)
    wire_x1 = cx + front * (half + 34)
    lo, hi = min(wire_x0, wire_x1), max(wire_x0, wire_x1)
    wire = np.zeros((H, W), bool)
    stakes = []
    for row in range(3):
        x = lo + (hi - lo) * (row + 0.5) / 3
        for y in np.arange(rng.uniform(0, 6), TH, 7.5):
            stakes.append((x + rng.normal(0, 1.2), y + rng.normal(0, 1)))
    stake = np.zeros((H, W), bool)
    for sx, sy in stakes:
        stake |= np.hypot(xx - sx, yy - sy) < 0.9
    for _ in range(70):
        a, b = stakes[rng.integers(len(stakes))], stakes[rng.integers(len(stakes))]
        if math.hypot(a[0] - b[0], a[1] - b[1]) > 14:
            continue
        t = np.linspace(0, 1, 60)
        sag = rng.normal(0, 1.5)
        px = a[0] + (b[0] - a[0]) * t + np.sin(t * math.pi) * sag
        py = a[1] + (b[1] - a[1]) * t + np.sin(t * 9 + sag) * 0.6
        for x, y in zip(px, py):
            ix, iy = int(x * K), int(y * K) % H
            wire[iy:iy + 3, max(0, ix):ix + 3] = True
    # Light it; the wire and stakes go on top, dark with their shadows.
    out = shade_height(h, alb, float(K), wet=wet, shadow_len=60)
    shadow = np.roll(np.roll(stake | wire, 3, 0), 3, 1) & ~(stake | wire)
    out[shadow] *= 0.6
    out[wire] = np.array([0.20, 0.19, 0.18])
    out[stake] = np.array([0.16, 0.13, 0.10])
    footprint = (d < half + spoil_w) | crater_any | wire | stake | \
        (shadow & (np.abs(xx - (lo + hi) / 2) < (hi - lo) / 2 + 3))
    # Filter down; anything mostly outside the works becomes clear.
    rgb = Image.fromarray(np.clip(out * 255, 0, 255).astype(np.uint8))
    rgb = rgb.resize((TW, TH), Image.LANCZOS)
    a = Image.fromarray((footprint * 255).astype(np.uint8)).resize((TW, TH),
                                                                   Image.BOX)
    arr = np.asarray(rgb).copy()
    arr[np.asarray(a) < 100] = KEY
    return Image.fromarray(arr)
