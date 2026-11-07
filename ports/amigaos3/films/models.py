"""The machines, the men and the aerodrome, as solids for render.py.

Three machines, after the ones flown in the game:

  scout    1915-17, the Sopwith type: a short rotary-engined single-seater
           with a round open-bottomed cowl, equal wings in a single bay,
           olive drab above and clear doped linen beneath.
  twoseat  1914, the reconnaissance two-seater of the B.E. type: all clear
           doped linen, an air-cooled inline engine with its tall exhaust
           stack, two bays of struts, and an upper plane of longer span
           than the lower whose overhang is braced by sloping struts out
           towards the tips; skids ahead of the wheels against nosing over.
  late     1918, the flat-nosed scout of the S.E. type: a square radiator
           with shutters, flat sides, long exhaust pipes, a Lewis gun on
           the top plane and a Vickers on the decking, a four-bladed
           airscrew.

Each is built about the middle of its lower plane: x along the span to the
right, y up, z to the nose, in metres.  The men are built on a skeleton
whose hands and feet are put where they must go and whose elbows and
knees are found to suit.
"""

import math

import numpy as np

from render import (Box, Capsule, Convex, Cylinder, Ellipsoid, FINE, GRASS,
                    bilinear, ry, rz, rx, unit)

# -- colours ---------------------------------------------------------------------

PC10 = np.array([0.30, 0.29, 0.17])          # olive drab dope
CLEAR = np.array([0.74, 0.69, 0.55])         # clear doped linen
CLEAR_UNDER = np.array([0.78, 0.73, 0.60])
ALU = np.array([0.56, 0.56, 0.54])
STEEL = np.array([0.30, 0.30, 0.31])
BLACK = np.array([0.05, 0.05, 0.05])
WOOD = np.array([0.42, 0.28, 0.15])
VARNISH = np.array([0.46, 0.30, 0.15])
TYRE = np.array([0.07, 0.07, 0.07])
LEATHER = np.array([0.28, 0.18, 0.11])
COAT = np.array([0.34, 0.24, 0.15])
KHAKI = np.array([0.40, 0.36, 0.24])
SKIN = np.array([0.70, 0.52, 0.42])
BLUE = np.array([0.13, 0.16, 0.36])
WHITE = np.array([0.86, 0.84, 0.78])
RED = np.array([0.56, 0.11, 0.08])
CANVAS = np.array([0.66, 0.62, 0.52])
ROPE = np.array([0.62, 0.58, 0.46])

OVERALLS = [np.array(c) for c in ((0.30, 0.29, 0.25), (0.36, 0.33, 0.24),
                                  (0.26, 0.27, 0.27), (0.40, 0.37, 0.28))]


def attitude(yaw, pitch=0.0, roll=0.0):
    """Rows: span, up, nose, for a machine heading 'yaw' (about y; 0 = along
    +z), nose up by 'pitch', right wing down by 'roll'."""
    M = ry(yaw) @ rx(-pitch) @ rz(-roll)
    return M.T


# -- the machines ----------------------------------------------------------------

class Kind:
    pass


def _kinds():
    k = {}
    s = Kind()                      # the Sopwith type
    s.name = "scout"
    s.span_u, s.span_l = 4.05, 4.05
    s.chord, s.gap, s.stagger, s.dihedral = 1.55, 1.48, 0.42, math.radians(3)
    s.lower_y, s.lower_z = 0.0, 0.0
    s.struts = (2.55,)
    s.overhang_strut = None
    s.stations = [  # z, half width, bottom, top
        (1.38, 0.43, -0.12, 0.78), (0.70, 0.42, -0.14, 0.80),
        (-0.60, 0.39, -0.10, 0.80), (-1.10, 0.36, -0.06, 0.74),
        (-2.60, 0.24, 0.04, 0.60), (-4.30, 0.04, 0.26, 0.46)]
    s.cowl = ("rotary", 1.38, 1.86, 0.50)
    s.prop_z, s.prop_r, s.blades = 2.0, 1.30, 2
    s.wheel = (0.86, -0.86, 0.68, 0.33)        # x, y, z of the axle; radius
    s.skid = (-4.15, -0.22)
    s.tail = (-4.0, 0.36, 1.45)                # tailplane z, y, half span
    s.cockpits = (-0.85,)
    s.body = "olive"
    s.length_tail = -4.35
    k["scout"] = s

    b = Kind()                      # the B.E. type two-seater
    b.name = "twoseat"
    b.span_u, b.span_l = 6.25, 5.35
    b.chord, b.gap, b.stagger, b.dihedral = 1.70, 1.80, 0.55, math.radians(3.5)
    b.lower_y, b.lower_z = 0.0, 0.0
    b.struts = (2.35, 4.95)
    b.overhang_strut = (4.95, 5.95)
    b.stations = [
        (1.85, 0.44, -0.20, 0.72), (1.10, 0.46, -0.24, 0.82),
        (-0.40, 0.44, -0.22, 0.86), (-1.90, 0.38, -0.12, 0.80),
        (-3.60, 0.24, 0.04, 0.62), (-5.70, 0.04, 0.30, 0.46)]
    b.cowl = ("inline", 1.85, 2.30, 0.40)
    b.prop_z, b.prop_r, b.blades = 2.42, 1.45, 2
    b.wheel = (0.95, -0.95, 0.85, 0.36)
    b.skid = (-5.55, -0.20)
    b.tail = (-5.30, 0.40, 2.05)
    b.cockpits = (-0.25, -1.55)                 # pilot in front, observer aft
    b.body = "clear"
    b.length_tail = -5.75
    k["twoseat"] = b

    e = Kind()                      # the S.E. type late scout
    e.name = "late"
    e.span_u, e.span_l = 4.10, 3.95
    e.chord, e.gap, e.stagger, e.dihedral = 1.55, 1.50, 0.35, math.radians(5)
    e.lower_y, e.lower_z = 0.0, 0.0
    e.struts = (1.75, 3.45)
    e.overhang_strut = None
    e.stations = [
        (1.95, 0.40, -0.10, 0.90), (1.30, 0.42, -0.16, 0.92),
        (-0.70, 0.40, -0.14, 0.88), (-2.40, 0.28, -0.02, 0.70),
        (-4.75, 0.04, 0.26, 0.48)]
    e.cowl = ("radiator", 1.95, 2.12, 0.0)
    e.prop_z, e.prop_r, e.blades = 2.25, 1.30, 4
    e.wheel = (0.82, -0.92, 0.95, 0.34)
    e.skid = (-4.55, -0.20)
    e.tail = (-4.35, 0.40, 1.55)
    e.cockpits = (-0.95,)
    e.body = "olive"
    e.length_tail = -4.80
    k["late"] = e
    return k


KINDS = _kinds()


def three_point(kind):
    """The pitch at which wheels and tail skid both touch."""
    k = KINDS[kind]
    wy = k.wheel[1] - k.wheel[3]
    return math.atan2(k.skid[1] - wy, k.wheel[2] - k.skid[0])


def rest_height(kind, pitch, roll=0.0):
    """Height of the machine's origin with its lowest wheel or its skid on
    the ground, at this pitch and roll."""
    k = KINDS[kind]
    R = attitude(0.0, pitch, roll)
    pts = [np.array([sx * k.wheel[0], k.wheel[1] - k.wheel[3], k.wheel[2]])
           for sx in (-1, 1)]
    # The bottom of a wheel rolled over is near enough its lowest point.
    pts.append(np.array([0.0, k.skid[1], k.skid[0]]))
    ys = [(p @ R)[1] for p in pts]
    return -min(ys)


def contacts(kind, pos, R):
    """Heights above the ground of the left wheel, right wheel and skid."""
    k = KINDS[kind]
    out = []
    for p in ([-k.wheel[0], k.wheel[1] - k.wheel[3], k.wheel[2]],
              [k.wheel[0], k.wheel[1] - k.wheel[3], k.wheel[2]],
              [0.0, k.skid[1], k.skid[0]]):
        out.append(float((np.asarray(p) @ R + pos)[1]))
    return out


def _roundel(x, z, cx, cz, r):
    """Blue, white and red rings: returns weights for each colour."""
    d = np.sqrt((x - cx) ** 2 + (z - cz) ** 2) / r
    edge = 0.06
    blue = np.clip((1 - d) / edge, 0, 1)
    white = np.clip((0.66 - d) / edge, 0, 1)
    red = np.clip((0.33 - d) / edge, 0, 1)
    return blue, white, red


def _apply_roundel(col, w):
    blue, white, red = w
    col = col * (1 - blue[..., None]) + BLUE * blue[..., None]
    col = col * (1 - white[..., None]) + WHITE * white[..., None]
    col = col * (1 - red[..., None]) + RED * red[..., None]
    return col


def _holes(seed, amount, region):
    """Bullet holes and rents in a plane's fabric, in model x, z.  Where
    the fabric has gone the ribs still cross the hole."""
    rng = np.random.default_rng(seed)
    x0, x1, z0, z1 = region
    spots = []
    for i in range(int(22 * amount)):
        spots.append((rng.uniform(x0, x1), rng.uniform(z0, z1),
                      rng.uniform(0.04, 0.09)))
    for i in range(int(4 * amount)):        # torn patches and long rents
        x, z = rng.uniform(x0, x1), rng.uniform(z0, z1)
        a = rng.uniform(-0.5, 0.5)
        spots.append((x, z, rng.uniform(0.25, 0.45), a))

    def keep(q):
        x, z = q[..., 0], q[..., 2]
        ok = np.ones(x.shape, bool)
        for s in spots:
            if len(s) == 3:
                ok &= (x - s[0]) ** 2 + (z - s[1]) ** 2 > s[2] ** 2
            else:
                c, sn = math.cos(s[3]), math.sin(s[3])
                u = (x - s[0]) * c - (z - s[1]) * sn
                v = (x - s[0]) * sn + (z - s[1]) * c
                rag = 1 + 0.25 * np.sin(u * 37 + s[0] * 9) * np.sin(v * 23)
                ok &= (u / s[2]) ** 2 + (v / (s[2] * 0.5)) ** 2 > rag
        rib = np.abs(np.mod(x / 0.30, 1.0) - 0.5) < 0.05
        spar = (np.abs(z - (z0 + (z1 - z0) * 0.72)) < 0.03) | (np.abs(z - (z0 + (z1 - z0) * 0.3)) < 0.03)
        return ok | rib | spar
    return keep, spots


class Machine:
    """One aeroplane, put together each frame where it stands."""

    def __init__(self, kind, seed=0, damage=0.0, weathered=0.0, letter=None):
        self.k = KINDS[kind]
        self.kind = kind
        self.seed = seed
        self.damage = damage
        self.weathered = weathered
        self.holes = {}
        if damage:
            k = self.k
            for i, (side, part) in enumerate(((1, "upper"), (-1, "upper"),
                                               (1, "lower"), (-1, "lower"))):
                span = k.span_u if part == "upper" else k.span_l
                zc = k.stagger if part == "upper" else 0.0
                lo = 0.5 if side > 0 else -span + 0.3
                hi = span - 0.3 if side > 0 else -0.5
                self.holes[(side, part)] = _holes(
                    seed * 13 + i, damage * (1.3 if side > 0 else 0.7),
                    (lo, hi, zc - k.chord / 2 + 0.1, zc + k.chord / 2 - 0.1))

    # -- painting ------------------------------------------------------------------
    def _dope(self, R, part, side=0, zc=0.0, span=0.0):
        """A painter for a fabric surface in model coordinates."""
        k = self.k
        olive = k.body == "olive"
        weathered = self.weathered
        dmg = self.damage
        seed = self.seed

        def paint(q, n):
            nm = n @ R.T
            up = nm[..., 1]
            x, y, z = q[..., 0], q[..., 1], q[..., 2]
            top_col = PC10 if olive else CLEAR
            under_col = CLEAR_UNDER if olive else CLEAR * 1.04
            if part in ("upper", "lower", "tailplane"):
                w = np.clip(up * 3, -1, 1) * 0.5 + 0.5
                col = top_col * w[..., None] + under_col * (1 - w[..., None])
                # Rib tapes across the chord, a little paler.
                rib = np.exp(-((np.mod(x / 0.30, 1.0) - 0.5) / 0.07) ** 2)
                col = col * (1 + 0.07 * rib)[..., None]
                # The leading edge's plywood and the trailing edge's wire
                # scalloping the fabric.
                le = np.clip((z - (zc + k.chord / 2 - 0.12)) / 0.1, 0, 1)
                col = col * (1 - 0.06 * le)[..., None]
                if part != "tailplane" and abs(span) > 0:
                    rx_ = abs(span) - 0.95
                    rr = k.chord * 0.36
                    if part == "upper":
                        on = up > 0
                    else:
                        on = up < 0
                    for cx in (rx_, -rx_):
                        wts = _roundel(x, z, cx, zc, rr)
                        wts = tuple(c * on for c in wts)
                        col = _apply_roundel(col, wts)
                    # Ailerons, outboard on the trailing edge.
                    ail = (np.abs(x) > abs(span) * 0.55) & \
                        (np.abs(z - (zc - k.chord / 2 + 0.36)) < 0.012)
                    col = np.where(ail[..., None], col * 0.7, col)
            elif part == "fuselage":
                w = np.clip(up * 2.5 + 1.2, 0, 1)
                col = top_col * w[..., None] + under_col * (1 - w[..., None])
                # Stringers and the longeron lines.
                col = col * (1 - 0.05 * np.exp(-((y - 0.62) / 0.02) ** 2))[..., None]
                # Oil from the engine thrown back along the belly and sides.
                oil = np.clip((z + 1.2) / 2.4, 0, 1) * np.clip((0.4 - y) / 0.5, 0, 1)
                streak = 0.6 + 0.4 * np.sin(z * 9 + y * 40) ** 2
                col = col * (1 - (0.30 + weathered * 0.3) * oil * streak)[..., None]
                # The roundel on the side, aft of the cockpit.
                rz_ = k.length_tail * 0.47
                side_on = np.abs(nm[..., 0]) > 0.6
                wts = _roundel(z, y, rz_, 0.36, 0.30 if k.name != "twoseat" else 0.36)
                col = _apply_roundel(col, tuple(c * side_on for c in wts))
            elif part == "rudder":
                # Blue at the hinge, white, red at the trailing edge.
                t0 = k.length_tail
                f = np.clip((t0 + 0.02 - z) / 0.55, 0, 1)
                col = np.where((f < 0.33)[..., None], BLUE,
                               np.where((f < 0.66)[..., None], WHITE, RED))
            elif part == "fin":
                col = np.broadcast_to(top_col, q.shape).copy()
            else:
                col = np.broadcast_to(top_col, q.shape).copy()
            if dmg and part in ("upper", "lower", "fuselage"):
                # Scorching and oil about the holes.
                g = bilinear(GRASS, x * 40 + seed * 17, z * 40)
                col = col * (1 - dmg * 0.35 * np.clip((g - 0.55) * 4, 0, 1))[..., None]
            if weathered:
                g = bilinear(FINE, x * 60 + 3, z * 60 + y * 30)
                col = col * (1 - weathered * 0.18 * g)[..., None]
            return col
        return paint

    def hub(self):
        """Where the airscrew's boss is, in the machine's own frame."""
        k = self.k
        s0 = k.stations[0]
        y = (s0[2] + s0[3]) / 2
        if k.cowl[0] == "inline":
            y += 0.05
        elif k.cowl[0] == "radiator":
            y += 0.05
        return np.array([0.0, y, k.prop_z])

    def world(self, pos, R, v):
        return np.asarray(pos, np.float64) + np.asarray(v, np.float64) @ R

    # -- building ------------------------------------------------------------------
    def build(self, f, pos, R, prop=None, prop_spin=0.0, t=0.0, crew=("pilot",),
              tatters=None, reflect=True):
        """Put the machine in the frame.  'prop' is the angle of a still or
        slow airscrew; prop_spin above 0.3 draws it as a turning disc."""
        k = self.k
        pos = np.asarray(pos, np.float64)

        def P(v):
            return pos + np.asarray(v, np.float64) @ R

        def hull(verts, alb, **kw):
            c = Convex(verts, pos, R, alb, half=1.0, reflect=reflect, **kw)
            c.mid = np.zeros(3)
            return f.add(c)

        def cap(a, b, r, alb, **kw):
            return f.add(Capsule(P(a), P(b), r, alb, reflect=reflect, **kw))

        def cyl(a, b, r, alb, **kw):
            return f.add(Cylinder(P(a), P(b), r, alb, reflect=reflect, **kw))

        def ell(c, Rl, radii, alb, **kw):
            return f.add(Ellipsoid(P(c), Rl @ R, radii, alb, reflect=reflect, **kw))

        # The fuselage, section by section.
        fus = self._dope(R, "fuselage")
        st = k.stations
        for a, b in zip(st[:-1], st[1:]):
            verts = []
            for z, hw, yb, yt in (a, b):
                hgt = yt - yb
                for sx in (-1, 1):
                    verts += [(sx * hw * 0.92, yb, z), (sx * hw, yb + 0.5 * hgt, z),
                              (sx * hw * 0.82, yt - 0.10 * hgt, z),
                              (sx * hw * 0.38, yt, z)]
            hull(verts, CLEAR, paint=fus, glow=0.0)
        nose_z = st[0][0]
        # The engine.
        kind = k.cowl[0]
        if kind == "rotary":
            _, z0, z1, r = k.cowl
            cy_ = (st[0][2] + st[0][3]) / 2
            cyl((0, cy_, z0 - 0.05), (0, cy_, z1), r, ALU * 0.95, spec=0.5)
            # The open bottom of the horseshoe cowl, and the cylinders in it.
            cyl((0, cy_ - r * 0.62, z1 - 0.02), (0, cy_ - r * 0.62, z1 + 0.01),
                r * 0.42, BLACK, shadow=False)
            cyl((0, cy_, z1), (0, cy_, z1 + 0.02), r * 0.86, STEEL * 0.4, shadow=False)
            for i in range(5):
                a = i * 2 * math.pi / 5 + t * 0.0
                cap((math.cos(a) * 0.30, cy_ + math.sin(a) * 0.30, z1 + 0.02),
                    (math.cos(a) * 0.42, cy_ + math.sin(a) * 0.42, z1 + 0.02),
                    0.06, STEEL * 0.5, shadow=False)
            hub_y = cy_
        elif kind == "inline":
            _, z0, z1, r = k.cowl
            cy_ = (st[0][2] + st[0][3]) / 2 - 0.05
            # Metal panels round the crankcase, the cylinders on top.
            hull([(sx * 0.40, yy, zz) for sx in (-1, 1)
                  for yy in (-0.16, 0.60) for zz in (z0 - 0.02, z1 - 0.10)] +
                 [(sx * 0.20, -0.05, z1 + 0.05) for sx in (-1, 1)] +
                 [(sx * 0.20, 0.45, z1 + 0.05) for sx in (-1, 1)],
                 ALU, spec=0.4)
            hull([(sx * 0.20, yy, zz) for sx in (-1, 1)
                  for yy in (0.60, 0.92) for zz in (z0 + 0.05, z1 - 0.25)],
                 STEEL * 0.8, spec=0.3)                 # the air scoop
            # The exhaust stack up over the top plane.
            sy = k.lower_y + k.gap + 0.55
            cap((0.0, 0.88, z0 + 0.15), (0.0, sy, z0 + 0.05), 0.07, STEEL * 0.55, spec=0.3)
            cap((0.0, sy, z0 + 0.05), (0.0, sy + 0.08, z0 - 0.02), 0.075, STEEL * 0.4)
            hub_y = cy_ + 0.1
        else:                                          # radiator nose
            _, z0, z1, _ = k.cowl
            s0 = st[0]
            hull([(sx * 0.42, yy, zz) for sx in (-1, 1)
                  for yy in (s0[2], s0[3]) for zz in (z0 - 0.3, z1)], ALU * 0.9, spec=0.3)

            def shutters(q, n):
                nm = n @ R.T
                front = nm[..., 2] > 0.7
                lines = np.abs(np.mod(q[..., 0] / 0.07, 1.0) - 0.5) < 0.18
                c = np.where((front & lines)[..., None], STEEL * 0.35, ALU * 0.85)
                return c
            hull([(sx * 0.36, yy, zz) for sx in (-1, 1)
                  for yy in (s0[2] + 0.06, s0[3] - 0.06) for zz in (z1 - 0.02, z1 + 0.03)],
                 ALU, paint=shutters)
            hub_y = (s0[2] + s0[3]) / 2 + 0.05
            # Long exhaust pipes down each side, back to the cockpit.
            for sx in (-1, 1):
                cap((sx * 0.45, 0.30, z0 - 0.1), (sx * 0.47, 0.30, -0.9), 0.045,
                    STEEL * 0.5, spec=0.3)
            # The head fairing behind the cockpit.
            ell((0, 0.88, k.cockpits[0] - 0.9), np.eye(3), (0.14, 0.12, 0.75), PC10)
        # Cockpits and the men in them.
        for i, cz in enumerate(k.cockpits):
            ztop = np.interp(cz, [s[0] for s in st][::-1], [s[3] for s in st][::-1])
            ell((0, ztop + 0.005, cz), np.eye(3), (0.30, 0.03, 0.42), BLACK * 0.6,
                shadow=False)
            cap((-0.30, ztop + 0.01, cz), (0.30, ztop + 0.01, cz), 0.03, LEATHER,
                shadow=False)
            who = crew[i] if i < len(crew) else None
            if who:
                airman_seated(f, P((0, ztop, cz)), R, t=t, seed=self.seed * 3 + i)
        # Guns.
        if k.name == "scout":
            ztop = 0.80
            cyl((0.0, ztop + 0.10, -0.15), (0.0, ztop + 0.10, 0.95), 0.06, BLACK * 1.5,
                spec=0.4)
            hull([(sx * 0.07, yy, zz) for sx in (-1, 1) for yy in (ztop, ztop + 0.2)
                  for zz in (-0.40, -0.10)], BLACK * 1.3)
            cap((-0.25, ztop, -0.42), (0.25, ztop, -0.42), 0.035, BLACK)  # screen
        if k.name == "late":
            cyl((-0.15, 0.95, -0.4), (-0.15, 0.95, 1.2), 0.055, BLACK * 1.5, spec=0.4)
            uy = k.lower_y + k.gap
            cyl((0, uy + 0.18, k.stagger - 0.6), (0, uy + 0.18, k.stagger + 0.8), 0.05,
                BLACK * 1.4, spec=0.4)                      # the Lewis
            cyl((0, uy + 0.24, k.stagger - 0.25), (0, uy + 0.32, k.stagger - 0.25),
                0.16, BLACK * 1.8)                          # its drum
            cap((0, uy + 0.05, k.stagger - 0.6), (0, 0.95, k.cockpits[0] + 0.3), 0.025,
                STEEL)                                      # the Foster rail
        # The planes.
        for part, half_span, y0, zc in (("lower", k.span_l, k.lower_y, k.lower_z),
                                        ("upper", k.span_u, k.lower_y + k.gap,
                                         k.lower_z + k.stagger)):
            paint = self._dope(R, part, zc=zc, span=half_span)
            for side in (-1, 1):
                root = 0.42 if part == "lower" else 0.0
                verts = _wing_half(root, half_span, zc, k.chord, 0.09, side,
                                   k.dihedral, y0)
                holes = None
                if (side, part) in self.holes:
                    holes = self.holes[(side, part)][0]
                hull(verts, CLEAR, paint=paint, holes=holes,
                     glow=0.55 if k.body == "clear" else 0.38)
        # Interplane struts and the wires.
        dz = k.chord * 0.32
        for side in (-1, 1):
            for xs in k.struts:
                ylo = k.lower_y + math.tan(k.dihedral) * xs
                yhi = ylo + k.gap
                for zz in (dz, -dz):
                    cap((side * xs, ylo, zz), (side * xs, yhi, zz + k.stagger), 0.035,
                        VARNISH, spec=0.3)
            if k.overhang_strut:
                xa, xb = k.overhang_strut
                ylo = k.lower_y + math.tan(k.dihedral) * xa
                yhi = k.lower_y + k.gap + math.tan(k.dihedral) * xb
                for zz in (dz, -dz):
                    cap((side * xa, ylo, zz), (side * xb, yhi, zz + k.stagger), 0.03,
                        VARNISH)
            # Flying and landing wires across each bay.
            xs_all = (0.4,) + tuple(k.struts)
            for xa, xb in zip(xs_all[:-1], xs_all[1:]):
                ya = k.lower_y + math.tan(k.dihedral) * xa
                yb = k.lower_y + math.tan(k.dihedral) * xb
                for zz in (dz * 0.9, -dz * 0.9):
                    cap((side * xa, ya + (0.75 if xa < 0.5 else 0.0), zz),
                        (side * xb, yb + k.gap, zz + k.stagger), 0.0065, STEEL * 1.6,
                        shadow=False)
                    cap((side * xa, ya + k.gap, zz + k.stagger),
                        (side * xb, yb, zz), 0.0065, STEEL * 1.6, shadow=False)
            # Cabane struts to the centre section.
            for zz in (dz, -dz):
                cap((side * 0.30, 0.75, zz * 0.9), (side * 0.42, k.lower_y + k.gap, zz + k.stagger),
                    0.03, STEEL * 1.2)
        # The tail: tailplane and elevators, fin and rudder, skid.
        tz, ty, ths = k.tail
        tpaint = self._dope(R, "tailplane", zc=tz)
        for side in (-1, 1):
            verts = []
            for yy in (ty - 0.03, ty + 0.03):
                for x, z in ((0.05, tz + 0.55), (ths * 0.7, tz + 0.35), (ths, tz),
                             (ths * 0.9, tz - 0.45), (0.05, tz - 0.55)):
                    verts.append((side * x, yy, z))
            hull(verts, CLEAR, paint=tpaint, glow=0.4)
        tail_end = k.length_tail
        hgt = 1.0 if k.name != "twoseat" else 1.15
        fin = []
        rud = []
        for xx in (-0.03, 0.03):
            fin += [(xx, ty, tail_end + 0.9), (xx, ty, tail_end + 0.1),
                    (xx, ty + hgt * 0.75, tail_end + 0.12), (xx, ty + hgt * 0.45, tail_end + 0.6)]
            rud += [(xx, ty - 0.35, tail_end + 0.08), (xx, ty + hgt * 0.80, tail_end + 0.08),
                    (xx, ty + hgt * 0.72, tail_end - 0.30), (xx, ty + hgt * 0.25, tail_end - 0.50),
                    (xx, ty - 0.25, tail_end - 0.40)]
        hull(fin, CLEAR, paint=self._dope(R, "fin"))
        hull(rud, CLEAR, paint=self._dope(R, "rudder"))
        cap((0, st[-1][2] + 0.02, k.skid[0] + 0.35), (0, k.skid[1], k.skid[0]), 0.035, WOOD)
        # Undercarriage: the vee struts, the axle, and the wheels.
        wx, wy, wz, wr = k.wheel
        for side in (-1, 1):
            cap((side * 0.30, -0.10, wz + 0.45), (side * wx * 0.9, wy, wz), 0.035, VARNISH)
            cap((side * 0.30, -0.06, wz - 0.75), (side * wx * 0.9, wy, wz), 0.035, VARNISH)
            cyl((side * (wx - 0.05), wy, wz), (side * (wx + 0.05), wy, wz), wr, TYRE, spec=0.15)
            cyl((side * (wx - 0.058), wy, wz), (side * (wx + 0.058), wy, wz), wr * 0.80,
                CLEAR * 0.92 if k.body == "clear" else CLEAR * 0.85)
            cyl((side * (wx - 0.075), wy, wz), (side * (wx + 0.075), wy, wz), 0.06, STEEL)
        cyl((-wx, wy, wz), (wx, wy, wz), 0.025, STEEL, shadow=False)
        if k.name == "twoseat":
            # The skids out in front against nosing over.
            for side in (-1, 1):
                cap((side * 0.42, wy - 0.05, wz - 0.9), (side * 0.42, wy + 0.05, wz + 0.95),
                    0.04, WOOD)
                cap((side * 0.42, wy + 0.05, wz + 0.95), (side * 0.42, wy + 0.45, wz + 1.15),
                    0.04, WOOD)
        # The airscrew.
        hub = (0, hub_y, k.prop_z)
        ell(hub, np.eye(3), (0.11, 0.11, 0.16), ALU, spec=0.5)
        if prop_spin > 0.3:
            f.disc(P(hub), R[2], k.prop_r, VARNISH * 0.8, 0.10 + 0.08 * (1 - prop_spin),
                   angle=prop or 0.0, blades=k.blades)
        else:
            a = prop or 0.0
            for i in range(k.blades):
                aa = a + i * 2 * math.pi / k.blades
                _blade(f, P(hub), R, aa, k.prop_r, reflect)
        # Torn fabric streaming from the rents.
        if tatters:
            for (x, y, z, ln, wd, ph) in tatters:
                flap = math.sin(t * 23 + ph) * 0.6 + math.sin(t * 41 + ph * 2) * 0.25
                d = np.array([0.0, math.sin(flap) * 0.6 - 0.1, -1.0])
                d = d / np.linalg.norm(d)
                c = np.array([x, y, z]) + d * ln / 2
                Rl = _frame_from(d, np.array([1.0, 0, 0]))
                f.add(Box(P(c), Rl @ R, (wd / 2, 0.006, ln / 2),
                          CLEAR * 0.9 if k.body == "clear" else PC10 * 1.1,
                          glow=0.3, reflect=reflect))
        return P(hub)


def _frame_from(fwd, side):
    """Rows: across, up, along 'fwd'."""
    z = unit(fwd)
    x = unit(side - z * (side @ z))
    y = np.cross(z, x)
    return np.stack([x, y, z])


def _blade(f, hub, R, a, r, reflect=True):
    """One wooden blade, broad near the boss and narrowing to the tip."""
    c, s = math.cos(a), math.sin(a)
    along = np.array([c, s, 0.0])
    across = np.array([-s, c, 0.0])
    verts = []
    for t, w in ((0.08, 0.09), (0.55, 0.11), (0.97, 0.06)):
        for k in (-1, 1):
            for th in (-0.025, 0.025):
                v = along * (r * t) + across * (w * k) + np.array([0, 0, th - k * 0.02])
                verts.append(v)
    verts = np.array(verts)
    f.add(Convex(verts, hub, R, VARNISH, spec=0.4, reflect=reflect))


def _wing_half(root, span, zc, chord, th, side, dihedral, y0):
    """One half of a plane: raked, rounded tips, a little dihedral."""
    le, te = zc + chord / 2, zc - chord / 2
    plan = [(root, le), (span - 0.30, le), (span - 0.08, le - 0.12),
            (span, le - 0.40), (span - 0.05, te + 0.25), (span - 0.40, te),
            (root, te)]
    verts = []
    for x, z in plan:
        yy = y0 + math.tan(dihedral) * x
        # Thicker at the front third, thin at the trailing edge.
        f = 1.0 if z > te + chord * 0.4 else 0.45
        for k in (-1, 1):
            verts.append((side * x, yy + k * th * f / 2 + 0.02 * f, z))
    return verts


# -- men ---------------------------------------------------------------------------

def ik(root, target, l1, l2, pole):
    """Two bones from 'root' reaching for 'target', bending towards 'pole'."""
    v = target - root
    d = np.linalg.norm(v)
    dirn = v / max(d, 1e-6)
    d = min(max(d, abs(l1 - l2) + 1e-3), l1 + l2 - 1e-4)
    a = (l1 * l1 + d * d - l2 * l2) / (2 * d)
    h = math.sqrt(max(l1 * l1 - a * a, 0.0))
    p = pole - dirn * (pole @ dirn)
    if np.linalg.norm(p) < 1e-6:
        p = np.array([0, 0, 1.0])
    p = p / np.linalg.norm(p)
    return root + dirn * a + p * h, root + dirn * d


def _cloth(seed):
    def paint(q, n):
        g = bilinear(FINE, q[..., 0] * 20 + seed * 7, q[..., 1] * 20 + q[..., 2] * 10)
        return 0.9 + 0.2 * g
    return paint


class Man:
    """A man of the ground crew, or an airman, posed by his hands and
    feet.  All positions are in the world; 'facing' is the yaw of his
    chest."""

    def __init__(self, seed=0, cloth=None, cap="cap", coat=False, height=1.0,
                 puttees=False, helmet=False):
        rng = np.random.default_rng(seed)
        self.seed = seed
        self.cloth = OVERALLS[seed % len(OVERALLS)] if cloth is None else np.asarray(cloth)
        self.cloth = self.cloth * rng.normal(1, 0.04)
        self.skin = SKIN * rng.normal(1, 0.05)
        self.cap = cap
        self.coat = coat
        self.s = height * rng.normal(1.0, 0.025)
        self.puttees = puttees
        self.helmet = helmet

    def draw(self, f, pelvis, facing, feet, hands=None, lean=0.0, head_turn=0.0,
             head_tilt=0.0, reflect=True):
        s = self.s
        Ry = ry(facing)
        fwd = Ry @ np.array([0, 0, 1.0])
        right = Ry @ np.array([1.0, 0, 0])
        up_ = np.array([0, 1.0, 0])
        spine = unit(up_ * math.cos(lean) + fwd * math.sin(lean))
        pelvis = np.asarray(pelvis, np.float64)
        neck = pelvis + spine * 0.50 * s
        cloth = self.cloth
        paint = _cloth(self.seed)

        def cap_(a, b, r, alb, **kw):
            f.add(Capsule(a, b, r, alb, reflect=reflect, paint=paint, **kw))

        # Legs.
        for i, sx in enumerate((-1, 1)):
            hip = pelvis + right * sx * 0.095 * s
            ankle = np.asarray(feet[i], np.float64) + np.array([0, 0.08 * s, 0])
            knee, ankle = ik(hip, ankle, 0.44 * s, 0.44 * s, fwd + up_ * 0.1)
            cap_(hip, knee, 0.078 * s, cloth)
            shin_col = cloth * (0.75 if self.puttees else 1.0)
            cap_(knee, ankle, 0.064 * s, shin_col)
            # The boot, along the foot's line.
            toe_dir = unit(fwd - up_ * (fwd @ up_))
            f.add(Ellipsoid(ankle + toe_dir * 0.06 * s - up_ * 0.035 * s,
                            np.stack([right, up_, toe_dir]), (0.055 * s, 0.05 * s, 0.13 * s),
                            LEATHER * 0.6, spec=0.2, reflect=reflect))
        # Body: hips, the trunk of the overalls, square shoulders.
        Rb = np.stack([right, spine, unit(np.cross(right, spine))])
        f.add(Ellipsoid(pelvis + spine * 0.06 * s, Rb, (0.175 * s, 0.15 * s, 0.12 * s),
                        cloth, reflect=reflect, paint=paint))
        f.add(Ellipsoid(pelvis + spine * 0.30 * s, Rb, (0.19 * s, 0.25 * s, 0.125 * s),
                        cloth, reflect=reflect, paint=paint))
        if self.coat:
            # The skirts of a long coat down to the knees.
            f.add(Ellipsoid(pelvis - up_ * 0.12 * s, Rb, (0.20 * s, 0.30 * s, 0.15 * s),
                            cloth * 0.95, reflect=reflect, paint=paint))
        sh_l = neck + right * -0.18 * s - spine * 0.04 * s
        sh_r = neck + right * 0.18 * s - spine * 0.04 * s
        cap_(sh_l, sh_r, 0.075 * s, cloth)
        # Belt.
        f.add(Ellipsoid(pelvis + spine * 0.14 * s, Rb, (0.18 * s, 0.025 * s, 0.128 * s),
                        LEATHER * 0.8, reflect=reflect))
        # Arms.
        if hands is None:
            hands = [sh_l - up_ * 0.55 * s + fwd * 0.05 * s,
                     sh_r - up_ * 0.55 * s + fwd * 0.05 * s]
        for i, (sh, sx) in enumerate(((sh_l, -1), (sh_r, 1))):
            target = np.asarray(hands[i], np.float64)
            elbow, wrist = ik(sh, target, 0.30 * s, 0.27 * s,
                              -fwd * 0.6 + right * sx * 0.5 - up_ * 0.3)
            cap_(sh, elbow, 0.058 * s, cloth)
            cap_(elbow, wrist, 0.048 * s, cloth)
            f.add(Ellipsoid(wrist + unit(wrist - elbow) * 0.05 * s, Rb,
                            (0.045 * s, 0.06 * s, 0.035 * s), self.skin, reflect=reflect))
        # Neck and head.
        head_c = neck + spine * 0.17 * s + fwd * 0.01
        Rh = ry(facing + head_turn) @ rx(-head_tilt)
        hfwd = Rh @ np.array([0, 0, 1.0])
        hright = Rh @ np.array([1.0, 0, 0])
        hup = Rh @ np.array([0, 1.0, 0])
        RH = np.stack([hright, hup, hfwd])
        f.add(Capsule(neck, head_c - spine * 0.08 * s, 0.05 * s, self.skin * 0.95,
                      reflect=reflect))
        f.add(Ellipsoid(head_c, RH, (0.088 * s, 0.115 * s, 0.10 * s), self.skin,
                        reflect=reflect))
        f.add(Ellipsoid(head_c + hfwd * 0.095 * s - hup * 0.01 * s, RH,
                        (0.018 * s, 0.03 * s, 0.025 * s), self.skin * 0.95,
                        shadow=False, reflect=False))                 # nose
        for sx in (-1, 1):
            f.add(Ellipsoid(head_c + hright * sx * 0.088 * s, RH,
                            (0.012 * s, 0.03 * s, 0.022 * s), self.skin * 0.9,
                            shadow=False, reflect=False))            # ears
        if self.helmet:
            f.add(Ellipsoid(head_c + hup * 0.025 * s - hfwd * 0.005, RH,
                            (0.098 * s, 0.11 * s, 0.11 * s), LEATHER, spec=0.35,
                            reflect=reflect))
            for sx in (-1, 1):
                f.add(Ellipsoid(head_c + hup * 0.075 * s + hfwd * 0.07 * s +
                                hright * sx * 0.04 * s, RH,
                                (0.03 * s, 0.022 * s, 0.02 * s), np.array([0.5, 0.52, 0.55]),
                                spec=0.8, shadow=False, reflect=False))
        elif self.cap == "cap":
            # A soft service cap: the crown, the band and the peak.
            top = head_c + hup * 0.085 * s
            f.add(Ellipsoid(top + hup * 0.01 * s, RH, (0.115 * s, 0.04 * s, 0.12 * s),
                            cloth * 0.8, reflect=reflect))
            f.add(Cylinder(top - hup * 0.035 * s, top + hup * 0.005 * s, 0.098 * s,
                           cloth * 0.7, reflect=reflect))
            f.add(Ellipsoid(top + hfwd * 0.10 * s - hup * 0.04 * s,
                            np.stack([hright, unit(hup + hfwd * 0.4), unit(hfwd - hup * 0.4)]),
                            (0.08 * s, 0.008 * s, 0.05 * s), BLACK * 2, spec=0.4,
                            reflect=False))
        elif self.cap == "cap_comforter":
            f.add(Ellipsoid(head_c + hup * 0.07 * s, RH, (0.10 * s, 0.07 * s, 0.105 * s),
                            cloth * 0.7, reflect=reflect))
        else:
            f.add(Ellipsoid(head_c + hup * 0.04 * s - hfwd * 0.01 * s, RH,
                            (0.092 * s, 0.09 * s, 0.10 * s), np.array([0.14, 0.11, 0.08]),
                            reflect=reflect))                      # hair

    # Ready-made postures ------------------------------------------------------------
    def stand(self, f, x, z, facing, t=0.0, arms="hang", hands=None, lean=0.02,
              head_turn=0.0, ground=0.0, wide=0.13):
        s = self.s
        Ry = ry(facing)
        right = Ry @ np.array([1.0, 0, 0])
        fwd = Ry @ np.array([0, 0, 1.0])
        base = np.array([x, ground, z])
        sway = math.sin(t * 1.3 + self.seed) * 0.01
        feet = [base - right * wide * s + fwd * 0.02, base + right * wide * s - fwd * 0.03]
        pelvis = base + np.array([0, 0.93 * s, 0]) + right * sway
        if arms == "hips":
            hands = [pelvis - right * 0.22 * s + fwd * 0.04, pelvis + right * 0.22 * s + fwd * 0.04]
        elif arms == "behind":
            hands = [pelvis - right * 0.08 * s - fwd * 0.16, pelvis + right * 0.08 * s - fwd * 0.16]
        self.draw(f, pelvis, facing, feet, hands, lean=lean, head_turn=head_turn)

    def walk(self, f, x, z, facing, t, speed=1.4, run=False, head_turn=0.0, hands=None):
        """Walking (or running) with the stride matched to 'speed'."""
        s = self.s
        Ry = ry(facing)
        right = Ry @ np.array([1.0, 0, 0])
        fwd = Ry @ np.array([0, 0, 1.0])
        cadence = (2.8 if run else 1.9) * max(speed, 0.6) / (3.8 if run else 1.4) ** 0.5
        stride = speed / cadence / 2
        ph = t * cadence * 2 * math.pi + self.seed
        base = np.array([x, 0.0, z])
        feet = []
        for i, sx in enumerate((-1, 1)):
            p = ph + i * math.pi
            along = math.sin(p) * stride
            lift = max(0.0, math.cos(p)) * (0.22 if run else 0.10) * s
            feet.append(base + right * sx * 0.10 * s + fwd * along + np.array([0, lift, 0]))
        bob = abs(math.sin(ph)) * (0.05 if run else 0.025)
        pelvis = base + np.array([0, (0.86 if run else 0.91) * s + bob, 0])
        lean = 0.28 if run else 0.06
        if hands is None:
            hands = []
            for i, sx in enumerate((-1, 1)):
                p = ph + i * math.pi
                sw = -math.sin(p) * (0.30 if run else 0.18)
                sh = pelvis + np.array([0, 0.45 * s, 0]) + fwd * (0.12 if run else 0.02)
                hy = 0.25 if run else 0.52
                hands.append(sh + right * sx * 0.22 * s + fwd * (sw + lean * 0.3) -
                             np.array([0, hy * s, 0]))
        self.draw(f, pelvis, facing, feet, hands, lean=lean, head_turn=head_turn)


def airman_seated(f, seat, R, t=0.0, seed=0):
    """Head and shoulders of an airman over the coaming: leather helmet,
    goggles, the fur collar of his coat."""
    up = R[1]
    fwd = R[2]
    right = R[0]
    seat = np.asarray(seat, np.float64)
    nod = math.sin(t * 2.3 + seed) * 0.02
    Rh = np.stack([right, up, fwd])
    head = seat + up * (0.30 + nod) + fwd * 0.02
    f.add(Ellipsoid(seat + up * 0.02, Rh, (0.25, 0.14, 0.17), COAT))     # shoulders
    f.add(Ellipsoid(seat + up * 0.12, Rh, (0.16, 0.06, 0.13), np.array([0.40, 0.33, 0.24])))
    f.add(Ellipsoid(head, Rh, (0.10, 0.125, 0.115), LEATHER * 1.1, spec=0.4))
    f.add(Ellipsoid(head + fwd * 0.095 - up * 0.02, Rh, (0.06, 0.05, 0.03), SKIN))
    for sx in (-1, 1):
        f.add(Ellipsoid(head + fwd * 0.10 + up * 0.03 + right * sx * 0.04, Rh,
                        (0.032, 0.024, 0.02), np.array([0.55, 0.58, 0.62]), spec=0.9,
                        shadow=False))


# -- the aerodrome -------------------------------------------------------------------

def _canvas_paint(seed, bays, dark_door=None):
    """Canvas over a wooden frame: the frames show as darker lines with the
    canvas sagging between them, and the weathering runs down."""
    def paint(q, n):
        x, y, z = q[..., 0], q[..., 1], q[..., 2]
        bay = np.mod((z + 1) * bays / 2, 1.0)
        frame = np.exp(-((bay - 0.5) / 0.035) ** 2)
        sag = 0.94 + 0.06 * np.sin(bay * math.pi)
        stain = bilinear(GRASS, x * 30 + seed, y * 60 + z * 25)
        streak = 0.95 + 0.05 * np.sin(x * 40 + stain * 6)
        c = sag * (1 - 0.30 * frame) * (0.88 + 0.16 * stain) * streak
        c = c * (1 - 0.18 * np.clip(0.0 - y, 0, 1))
        if dark_door is not None:
            dx, dy = dark_door
            front = (np.abs(z) > 0.985)
            door = front & (np.abs(x) < dx) & (y < dy)
            depth = 0.10 + 0.10 * np.clip((y + 1) / (dy + 1), 0, 1)
            c = np.where(door, depth, c)
        return c
    return paint


def hangar(f, x, z, yaw, w=20.0, d=24.0, wall=3.6, ridge=7.0, seed=0, door=0.72,
           ropes=True):
    """A Bessonneau hangar: canvas on a wooden frame, the front curtains
    drawn back, guy ropes out to pegs down both sides."""
    R = ry(yaw).T
    c = np.array([x, 0.0, z])
    hw, hd = w / 2, d / 2
    prof = [(-hw, 0), (-hw, wall), (-hw * 0.62, wall + (ridge - wall) * 0.72),
            (-hw * 0.25, ridge * 0.985), (0, ridge), (hw * 0.25, ridge * 0.985),
            (hw * 0.62, wall + (ridge - wall) * 0.72), (hw, wall), (hw, 0)]
    verts = [(px_, py_, sz * hd) for px_, py_ in prof for sz in (-1, 1)]
    door_h = (wall * 1.15) / (ridge / 2) - 1
    s = Convex(verts, c, R, CANVAS, paint=_canvas_paint(seed, d / 4.0, (door, door_h)),
               rough=1.0)
    f.add(s)
    P = lambda v: c + np.asarray(v, np.float64) @ R
    # The curtains drawn aside, bunched in folds.
    for sx in (-1, 1):
        for k in range(3):
            xx = sx * (hw * door + 0.25 + k * 0.32)
            f.add(Capsule(P((xx, 0.1, hd + 0.18 + 0.05 * k)), P((xx, wall * 1.1, hd + 0.1)),
                          0.22 - 0.04 * k, CANVAS * (0.82 + 0.05 * k)))
    # A dim tail of a machine inside.
    if ropes:
        for sx in (-1, 1):
            for k in range(int(d / 4) + 1):
                zz = -hd + k * 4.0
                top = P((sx * hw, wall, zz))
                peg = P((sx * (hw + 3.2), 0.0, zz))
                f.add(Capsule(top, peg, 0.03, ROPE, shadow=False))
                f.add(Box(peg + np.array([0, 0.12, 0]), R, (0.05, 0.15, 0.05), WOOD,
                          shadow=False))
        for sx in (-0.5, 0.5):
            top = P((sx * hw, wall + (ridge - wall) * 0.6, hd))
            peg = P((sx * hw * 1.4, 0, hd + 4.5))
            f.add(Capsule(top, peg, 0.03, ROPE, shadow=False))


def tent(f, x, z, yaw, w=3.6, d=5.0, wall=1.4, ridge=2.9, colour=None, seed=0):
    """A ridge tent with its walls and guy ropes."""
    R = ry(yaw).T
    c = np.array([x, 0.0, z])
    hw, hd = w / 2, d / 2
    prof = [(-hw, 0), (-hw, wall), (0, ridge), (hw, wall), (hw, 0)]
    verts = [(px_, py_, sz * hd) for px_, py_ in prof for sz in (-1, 1)]
    col = CANVAS * 0.92 if colour is None else colour
    f.add(Convex(verts, c, R, col, paint=_canvas_paint(seed, 2, (0.3, -0.05))))
    P = lambda v: c + np.asarray(v, np.float64) @ R
    for sx in (-1, 1):
        for zz in (-hd + 0.3, 0, hd - 0.3):
            f.add(Capsule(P((sx * hw, wall, zz)), P((sx * (hw + 1.6), 0, zz)), 0.02, ROPE,
                          shadow=False))
    f.add(Capsule(P((0, ridge, hd)), P((0, 0, hd + 1.8)), 0.02, ROPE, shadow=False))
    f.add(Capsule(P((0, ridge, -hd)), P((0, 0, -hd - 1.8)), 0.02, ROPE, shadow=False))


def bell_tent(f, x, z, r=2.4, h=3.2, seed=0):
    """A bell tent: a low wall and a cone to the pole's top."""
    verts = []
    for k in range(12):
        a = k * math.pi / 6
        verts += [(math.cos(a) * r, 0, math.sin(a) * r),
                  (math.cos(a) * r, 0.5, math.sin(a) * r)]
    verts.append((0, h, 0))
    f.add(Convex(verts, np.array([x, 0, z]), np.eye(3), CANVAS * 0.95,
                 paint=_canvas_paint(seed, 3)))


def windsock(f, x, z, wind_dir, strength, t, seed=0):
    """A striped sock on its pole, filling and drooping with the wind."""
    top = np.array([x, 7.0, z])
    f.add(Cylinder((x, 0, z), top, 0.07, np.array([0.55, 0.53, 0.50])))
    for k in range(3):
        a = k * 2 * math.pi / 3
        f.add(Capsule(top - np.array([0, 1.0, 0]), (x + math.cos(a) * 3, 0, z + math.sin(a) * 3),
                      0.012, STEEL, shadow=False))
    wd = unit(np.array([math.cos(wind_dir), 0, math.sin(wind_dir)]))
    pos = top + np.array([0, -0.1, 0])
    n = 6
    for k in range(n):
        droop = (1 - strength) * 0.9 + 0.08 * math.sin(t * 3 + k + seed)
        flut = 0.10 * math.sin(t * 7.5 - k * 0.9) * strength
        seg = unit(wd * (0.6 + strength) - np.array([0, droop, 0]) +
                   np.cross(wd, [0, 1, 0]) * flut) * 0.45
        r = 0.30 - k * 0.035
        col = np.array([0.86, 0.82, 0.72]) if k % 2 == 0 else np.array([0.52, 0.12, 0.08])
        f.add(Capsule(pos, pos + seg, r, col, glow=0.3))
        pos = pos + seg


def drums(f, x, z, n, seed=0, standing=True):
    """Petrol and oil drums, a few on their sides, and a crate or two."""
    rng = np.random.default_rng(seed)
    for k in range(n):
        dx, dz = rng.uniform(-1.6, 1.6), rng.uniform(-1.2, 1.2)
        col = np.array([0.25, 0.24, 0.22]) * rng.uniform(0.8, 1.3)
        if rng.random() < 0.75:
            a = np.array([x + dx, 0, z + dz])
            f.add(Cylinder(a, a + [0, 0.88, 0], 0.29, col, spec=0.2))
            f.add(Cylinder(a + [0, 0.86, 0], a + [0, 0.9, 0], 0.30, col * 0.8))
        else:
            ang = rng.uniform(0, math.pi)
            dd = np.array([math.cos(ang), 0, math.sin(ang)]) * 0.44
            c = np.array([x + dx, 0.29, z + dz])
            f.add(Cylinder(c - dd, c + dd, 0.29, col, spec=0.2))
    for k in range(2):
        f.add(Box((x + rng.uniform(-2, 2), 0.22, z + rng.uniform(1.4, 2.2)), ry(rng.uniform(0, 1)).T,
                  (0.30, 0.22, 0.45), WOOD * 1.1))


def tree(f, x, z, h, rng, kind="round"):
    """A tree: a trunk and a crown of leafy masses, dappled."""
    seed = int(rng.integers(0, 999))

    def leaves(q, n):
        g = bilinear(FINE, q[..., 0] * 12 + seed, q[..., 1] * 12 + q[..., 2] * 8)
        g2 = bilinear(GRASS, q[..., 0] * 5 + seed, q[..., 2] * 5 + q[..., 1] * 3)
        return 0.45 + 0.85 * g * g2 + 0.2 * n[..., 1]
    green = np.array([0.17, 0.21, 0.12]) * rng.uniform(0.8, 1.15)
    if kind == "poplar":
        # Tall and narrow, in ragged lumps up the stem.
        f.add(Cylinder((x, 0, z), (x, h * 0.35, z), 0.18, WOOD * 0.45))
        n = 6
        for k in range(n):
            yy = h * (0.22 + 0.72 * k / (n - 1))
            w = h * 0.10 * (1.0 - 0.55 * (k / (n - 1)) ** 1.6) * rng.uniform(0.8, 1.2)
            f.add(Ellipsoid((x + rng.normal(0, 0.25), yy, z + rng.normal(0, 0.25)),
                            ry(rng.uniform(0, 3)).T, (w, h * 0.12, w * 0.9),
                            green * rng.uniform(0.85, 1.1), paint=leaves))
        return
    f.add(Cylinder((x, 0, z), (x, h * 0.5, z), 0.22 + h * 0.012, WOOD * 0.45))
    lean = rng.normal(0, 0.06)
    for k in range(3):                              # boughs
        a = rng.uniform(0, 2 * math.pi)
        f.add(Capsule((x, h * 0.38, z), (x + math.cos(a) * h * 0.2, h * 0.58,
                                         z + math.sin(a) * h * 0.2), 0.10 + h * 0.006,
                      WOOD * 0.45, shadow=False, reflect=False))
    for k in range(9):
        a = rng.uniform(0, 2 * math.pi)
        rr = h * rng.uniform(0.11, 0.19)
        off = h * rng.uniform(0.08, 0.26)
        yy = h * rng.uniform(0.50, 0.86)
        c = (x + math.cos(a) * off + lean * yy, yy, z + math.sin(a) * off)
        shade = rng.uniform(0.8, 1.15) * (0.85 + 0.25 * (yy / h))
        f.add(Ellipsoid(c, ry(a).T, (rr * 1.2, rr * 0.85, rr), green * shade,
                        paint=leaves, reflect=False))


def hedge(f, x0, x1, z, seed, height=1.8, along="x"):
    rng = np.random.default_rng(seed)
    p = x0
    while p < x1:
        r = rng.uniform(1.2, 2.2)
        hh = height * rng.uniform(0.8, 1.2)
        c = (p, hh * 0.5, z) if along == "x" else (z, hh * 0.5, p)
        rad = (r, hh * 0.55, 1.1) if along == "x" else (1.1, hh * 0.55, r)
        f.add(Ellipsoid(c, np.eye(3), rad, np.array([0.15, 0.19, 0.11]) * rng.uniform(0.85, 1.1),
                        reflect=False, shadow=True,
                        paint=lambda q, n: 0.75 + 0.3 * bilinear(FINE, q[..., 0] * 9, q[..., 1] * 9 + q[..., 2] * 9)))
        p += r * 1.4


def farm(f, x, z, yaw, seed=0):
    """A farm beyond the field: the house, its barn and some trees."""
    rng = np.random.default_rng(seed)
    R = ry(yaw).T
    c = np.array([x, 0.0, z])

    def gabled(cx, cz, w, d, wall, ridge, col, roof):
        P0 = c + np.array([cx, 0, cz]) @ R
        verts = [(sx * w / 2, 0, sz * d / 2) for sx in (-1, 1) for sz in (-1, 1)] + \
                [(sx * w / 2, wall, sz * d / 2) for sx in (-1, 1) for sz in (-1, 1)]
        f.add(Convex(verts, P0, R, col, reflect=False))
        rv = [(sx * (w / 2 + 0.4), wall - 0.2, sz * (d / 2 + 0.3)) for sx in (-1, 1) for sz in (-1, 1)] + \
             [(0, ridge, sz * (d / 2 + 0.3)) for sz in (-1, 1)]
        f.add(Convex(rv, P0, R, roof, reflect=False))
    gabled(0, 0, 14, 8, 6, 10, np.array([0.62, 0.58, 0.50]), np.array([0.30, 0.18, 0.13]))
    f.add(Box(c + np.array([3.5, 10.5, 0]) @ R, R, (0.5, 1.5, 0.5), np.array([0.45, 0.30, 0.22])))
    gabled(18, 6, 10, 20, 5, 9, np.array([0.40, 0.33, 0.25]), np.array([0.28, 0.24, 0.20]))
    for k in range(4):
        tree(f, x + rng.uniform(-18, 24), z + rng.uniform(8, 18), rng.uniform(10, 15), rng)
    f.add(Ellipsoid(c + np.array([-12, 2.2, 6]) @ R, np.eye(3), (2.6, 2.6, 2.6),
                    np.array([0.55, 0.47, 0.30]), reflect=False))      # a rick
