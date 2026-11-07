"""The little ray-caster behind the aerodrome films.

A pinhole camera looks out over a flat field.  Each thing in the picture is
a simple solid (an oriented box, a convex block cut from a handful of
points, a capsule, a capped cylinder or an ellipsoid) that a ray either
strikes or misses.  The picture is drawn in two goes: first every solid
leaves its colour, its facing and its distance in a set of buffers, then
the sun is sent back from every point the camera sees to find what stands
in its way, and only then is the whole frame lit, hazed with distance and
given its smoke, dust and spinning airscrews.  The ground and the sky are
painted by functions of where the ray falls.

Distances are in metres, y is up, and a machine on the ground runs along +z.
"""

import math

import numpy as np
from PIL import Image

W, H = 640, 400
SS = 2                 # rays per pixel along each side
FOCAL = 520.0


def unit(v):
    v = np.asarray(v, np.float64)
    return v / np.linalg.norm(v)


def rx(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])


def ry(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def rz(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]])


def smooth(t):
    t = min(1.0, max(0.0, t))
    return t * t * (3 - 2 * t)


def lerp(a, b, t):
    return a + (b - a) * t


def dot(a, b):
    a, b = np.broadcast_arrays(a, b)
    return np.einsum("...i,...i->...", a, b)


def noise2(h, w, seed, base=6, octaves=5):
    """Smooth value noise, several octaves, wrapping at its edges."""
    rng = np.random.default_rng(seed)
    out = np.zeros((h, w), np.float32)
    amp = 1.0
    tot = 0.0
    for o in range(octaves):
        n = base * 2 ** o
        g = rng.random((n, n)).astype(np.float32)
        g = np.pad(g, ((0, 1), (0, 1)), mode="wrap")
        im = Image.fromarray(g, "F").resize((w + w // n, h + h // n),
                                           Image.BICUBIC)
        out += np.asarray(im, np.float32)[:h, :w] * amp
        tot += amp
        amp *= 0.5
    return out / tot


GRASS = noise2(512, 512, 7, base=8)
CLOUD = noise2(1024, 1024, 13, base=6, octaves=6)
MUD = noise2(512, 512, 23, base=10)
FINE = noise2(256, 256, 31, base=32, octaves=3)


def sample(tex, u, v):
    """Nearest sample of a wrapping texture at texel coordinates u, v."""
    h, w = tex.shape
    return tex[v.astype(np.int64) % h, u.astype(np.int64) % w]


def bilinear(tex, u, v):
    h, w = tex.shape
    u0 = np.floor(u)
    v0 = np.floor(v)
    fu = (u - u0).astype(np.float32)
    fv = (v - v0).astype(np.float32)
    u0 = u0.astype(np.int64) % w
    v0 = v0.astype(np.int64) % h
    u1 = (u0 + 1) % w
    v1 = (v0 + 1) % h
    return (tex[v0, u0] * (1 - fu) + tex[v0, u1] * fu) * (1 - fv) + \
        (tex[v1, u0] * (1 - fu) + tex[v1, u1] * fu) * fv


# -- the camera ----------------------------------------------------------------

class Camera:
    def __init__(self, pos, target, roll=0.0, focal=FOCAL):
        self.pos = np.asarray(pos, np.float64)
        self.focal = focal
        f = unit(np.asarray(target, np.float64) - self.pos)
        r = unit(np.cross([0, 1, 0], f))
        u = np.cross(f, r)
        self.R = rz(roll) @ np.stack([r, u, f])        # world -> camera
        w, h = W * SS, H * SS
        yy, xx = np.mgrid[0:h, 0:w].astype(np.float64)
        d = np.stack([((xx + 0.5) / SS - W / 2) / focal,
                      (H / 2 - (yy + 0.5) / SS) / focal,
                      np.ones_like(xx)], -1)
        d = d @ self.R
        self.dirs = d / np.linalg.norm(d, axis=-1, keepdims=True)

    def view(self, pts):
        return (np.asarray(pts, np.float64) - self.pos) @ self.R.T

    def project(self, p):
        q = self.view(p)
        if q[2] <= 0.05:
            return None
        return (W / 2 + self.focal * q[0] / q[2],
                H / 2 - self.focal * q[1] / q[2], q[2])

    def screen_box(self, pts, pad=0.0):
        """The pixel box (at SS) holding these points grown by 'pad'
        metres, or None when none of it can be seen.  Points behind the
        camera are clipped against the near plane first."""
        near = 0.05
        q = self.view(np.atleast_2d(pts))
        if pad:
            # Grow along the view axis so the clip sees the whole radius.
            q = np.concatenate([q + [0, 0, pad], q - [0, 0, pad]])
        front = q[:, 2] > near
        if not front.any():
            return None
        if not front.all():
            a = q[front]
            b = q[~front]
            k = (near - a[:, None, 2]) / (b[None, :, 2] - a[:, None, 2])
            cut = a[:, None, :] + (b[None, :, :] - a[:, None, :]) * k[..., None]
            q = np.concatenate([a, cut.reshape(-1, 3)])
            q[:, 2] = np.maximum(q[:, 2], near)
        sx = W / 2 + self.focal * q[:, 0] / q[:, 2]
        sy = H / 2 - self.focal * q[:, 1] / q[:, 2]
        grow = self.focal * pad / max(q[:, 2].min(), near) + 1.5
        x0 = max(0, int((sx.min() - grow) * SS))
        x1 = min(W * SS, int((sx.max() + grow) * SS) + 2)
        y0 = max(0, int((sy.min() - grow) * SS))
        y1 = min(H * SS, int((sy.max() + grow) * SS) + 2)
        if x0 >= x1 or y0 >= y1:
            return None
        return x0, y0, x1, y1


class Light:
    def __init__(self, sun, sun_col, sky_col, ambient, bounce=(0.10, 0.10, 0.07)):
        self.sun = unit(sun)
        self.sun_col = np.array(sun_col, np.float64)
        self.sky_col = np.array(sky_col, np.float64)
        self.ambient = np.array(ambient, np.float64)
        self.bounce = np.array(bounce, np.float64)


# -- solids ----------------------------------------------------------------------
#
# Each solid answers hit(o, d): for rays from o (one point or an array of
# them) along unit directions d it returns the distance to the near face
# (inf where it misses), the outward normal there, and the point in the
# solid's own frame scaled to -1..1, for painting.

INF = np.inf


class Solid:
    _next = 1

    def __init__(self, albedo, spec=0.0, paint=None, holes=None,
                 shadow=True, glow=0.0, reflect=False, rough=0.0):
        self.albedo = np.asarray(albedo, np.float64)
        self.spec = spec
        self.paint = paint          # f(local, normal) -> albedo multiplier or rgb
        self.holes = holes          # f(local) -> True where there is fabric
        self.shadow = shadow
        self.glow = glow            # sunlight let through doped linen
        self.reflect = reflect
        self.rough = rough
        self.id = Solid._next
        Solid._next += 1

    def colour(self, local, n):
        """The albedo where the rays fell: the solid's own colour, or what
        its painter makes of it (a shade to multiply, or colours)."""
        alb = np.broadcast_to(self.albedo, n.shape).astype(np.float64)
        if self.paint is not None:
            p = self.paint(local, n)
            if p.ndim == alb.ndim:
                alb = p
            else:
                alb = alb * p[..., None]
        return alb


class Box(Solid):
    def __init__(self, centre, R, half, albedo, **kw):
        super().__init__(albedo, **kw)
        self.c = np.asarray(centre, np.float64)
        self.R = np.asarray(R, np.float64)
        self.hs = np.asarray(half, np.float64)
        s = np.array([[i, j, k] for i in (-1, 1) for j in (-1, 1) for k in (-1, 1)])
        self.pts = self.c + (s * self.hs) @ self.R
        self.pad = 0.0

    def hit(self, o, d):
        R = self.R
        ol = (o - self.c) @ R.T
        dl = d @ R.T
        with np.errstate(divide="ignore", invalid="ignore"):
            inv = 1.0 / dl
            t1 = (-self.hs - ol) * inv
            t2 = (self.hs - ol) * inv
        tn = np.nan_to_num(np.minimum(t1, t2), nan=-INF)
        tf = np.nan_to_num(np.maximum(t1, t2), nan=INF)
        tmin = tn.max(-1)
        tmax = tf.min(-1)
        ok = (tmax >= tmin) & (tmax > 0)
        face = np.argmax(tn, -1)
        t = np.where(ok, tmin, INF)
        tt = np.where(np.isfinite(t), t, 0.0)
        q = ol + dl * tt[..., None]
        if self.holes is not None:
            u = q / self.hs
            keep = self.holes(u)
            if not keep.all():
                # Through the hole to the far face, which is holed too.
                tb = np.where(ok, tmax, INF)
                qb = ol + dl * np.where(np.isfinite(tb), tb, 0.0)[..., None]
                keepb = self.holes(qb / self.hs)
                facb = np.argmin(tf, -1)
                t = np.where(keep, t, np.where(keepb, tb, INF))
                face = np.where(keep, face, facb)
                q = np.where(keep[..., None], q, qb)
        nl = np.zeros(q.shape)
        np.put_along_axis(nl, face[..., None],
                          np.sign(np.take_along_axis(q, face[..., None], -1)), -1)
        return t, nl @ R, q / self.hs


_HULLS = {}


def hull_planes(v):
    """Face planes (n, d with n.x <= d inside) of the convex hull of v."""
    key = np.round(v, 5).tobytes()
    if key in _HULLS:
        return _HULLS[key]
    n = len(v)
    idx = np.array([(i, j, k) for i in range(n) for j in range(i + 1, n)
                    for k in range(j + 1, n)])
    a, b, c = v[idx[:, 0]], v[idx[:, 1]], v[idx[:, 2]]
    nrm = np.cross(b - a, c - a)
    ln = np.linalg.norm(nrm, axis=1)
    good = ln > 1e-9
    nrm = nrm[good] / ln[good][:, None]
    a = a[good]
    d = (nrm * a).sum(1)
    side = v @ nrm.T - d                                 # (n, planes)
    tol = 1e-6 * (1 + np.abs(v).max())
    out_ = (side > tol).any(0)
    in_ = (side < -tol).any(0)
    keep = ~(out_ & in_)
    nrm, d = nrm[keep], d[keep]
    flip = (side[:, keep] > tol).any(0)
    nrm[flip] *= -1
    d[flip] *= -1
    planes = {}
    for nn, dd in zip(nrm, d):
        k = tuple(np.round(nn, 4)) + (round(dd, 4),)
        planes[k] = (nn, dd)
    N = np.array([p[0] for p in planes.values()])
    D = np.array([p[1] for p in planes.values()])
    _HULLS[key] = (N, D)
    return N, D


class Convex(Solid):
    """The convex hull of a few points given in a frame (centre, R) and
    painted in that frame scaled by 'half'."""

    def __init__(self, verts, centre, R, albedo, half=None, **kw):
        super().__init__(albedo, **kw)
        v = np.asarray(verts, np.float64)
        N, D = hull_planes(v)
        self.c = np.asarray(centre, np.float64)
        self.R = np.asarray(R, np.float64)
        self.N = N @ self.R                              # to the world
        self.D = D + self.N @ self.c
        self.pts = self.c + v @ self.R
        self.pad = 0.0
        lo, hi = v.min(0), v.max(0)
        self.mid = (lo + hi) / 2
        self.hs = np.maximum((hi - lo) / 2, 1e-6) if half is None else \
            np.asarray(half, np.float64)

    def hit(self, o, d):
        den = d @ self.N.T
        num = self.D - o @ self.N.T
        with np.errstate(divide="ignore", invalid="ignore"):
            t = num / den
        enter = den < 0
        leave = den > 0
        outside = (den == 0) & (num < 0)
        tn = np.where(enter, t, -INF)
        tf = np.where(leave, t, INF)
        tmin = tn.max(-1)
        tmax = tf.min(-1)
        ok = (tmax >= tmin) & (tmax > 0) & ~outside.any(-1)
        face = np.argmax(tn, -1)
        tt = np.where(ok, tmin, INF)
        p = o + d * np.where(ok, tmin, 0.0)[..., None]
        local = ((p - self.c) @ self.R.T - self.mid) / self.hs
        if self.holes is not None:
            keep = self.holes(local)
            tb = np.where(ok, tmax, INF)
            pb = o + d * np.where(ok, tmax, 0.0)[..., None]
            lb = ((pb - self.c) @ self.R.T - self.mid) / self.hs
            keepb = self.holes(lb)
            facb = np.argmin(tf, -1)
            tt = np.where(keep, tt, np.where(keepb, tb, INF))
            face = np.where(keep, face, facb)
            local = np.where(keep[..., None], local, lb)
        return tt, self.N[face], local


class Capsule(Solid):
    def __init__(self, a, b, r, albedo, **kw):
        super().__init__(albedo, **kw)
        self.a = np.asarray(a, np.float64)
        self.b = np.asarray(b, np.float64)
        self.r = r
        self.pts = np.stack([self.a, self.b])
        self.pad = r

    def hit(self, o, d):
        a, b, r = self.a, self.b, self.r
        ba = b - a
        oa = o - a
        baba = ba @ ba
        bard = d @ ba
        baoa = oa @ ba
        rdoa = dot(d, oa)
        oaoa = dot(oa, oa)
        best = np.full(np.broadcast_shapes(np.shape(bard), np.shape(baoa)), INF)
        if baba > 1e-12:
            A = baba - bard * bard
            B = baba * rdoa - baoa * bard
            C = baba * oaoa - baoa * baoa - r * r * baba
            h = B * B - A * C
            with np.errstate(invalid="ignore", divide="ignore"):
                t = (-B - np.sqrt(h)) / A
                y = baoa + t * bard
            best = np.where((h >= 0) & (y > 0) & (y < baba) & (t > 0), t, INF)
        for c in (a, b):
            oc = o - c
            bb = dot(d, oc)
            cc = dot(oc, oc) - r * r
            hh = bb * bb - cc
            with np.errstate(invalid="ignore"):
                ts = -bb - np.sqrt(hh)
            best = np.minimum(best, np.where((hh >= 0) & (ts > 0), ts, INF))
        pos = o + d * np.where(np.isfinite(best), best, 0)[..., None]
        s = np.clip(((pos - a) @ ba) / max(baba, 1e-12), 0, 1)
        n = pos - (a + s[..., None] * ba)
        n = n / np.maximum(np.linalg.norm(n, axis=-1, keepdims=True), 1e-9)
        # Paint frame: along the axis 0..1, round it by the angle.
        local = np.stack([s * 2 - 1, n[..., 1], np.arctan2(n[..., 0], n[..., 2]) / math.pi], -1)
        return best, n, local


class Cylinder(Solid):
    """A capped cylinder from a to b."""

    def __init__(self, a, b, r, albedo, **kw):
        super().__init__(albedo, **kw)
        self.a = np.asarray(a, np.float64)
        self.b = np.asarray(b, np.float64)
        self.r = r
        self.pts = np.stack([self.a, self.b])
        self.pad = r

    def hit(self, o, d):
        a, b, r = self.a, self.b, self.r
        ba = b - a
        oc = o - a
        baba = ba @ ba
        bard = d @ ba
        baoc = oc @ ba
        k2 = baba - bard * bard
        k1 = baba * dot(oc, d) - baoc * bard
        k0 = baba * dot(oc, oc) - baoc * baoc - r * r * baba
        h = k1 * k1 - k2 * k0
        with np.errstate(invalid="ignore", divide="ignore"):
            sh = np.sqrt(h)
            t = (-k1 - sh) / k2
            y = baoc + t * bard
            body = (h >= 0) & (y > 0) & (y < baba) & (t > 0)
            tc = (np.where(y < 0, 0.0, baba) - baoc) / bard
            cap = (h >= 0) & ~body & (np.abs(k1 + k2 * tc) < sh) & (tc > 0)
        tt = np.where(body, t, np.where(cap, tc, INF))
        p = o + d * np.where(np.isfinite(tt), tt, 0)[..., None]
        s = ((p - a) @ ba) / baba
        nb = p - (a + s[..., None] * ba)
        nb = nb / np.maximum(np.linalg.norm(nb, axis=-1, keepdims=True), 1e-9)
        ncap = ba / math.sqrt(baba) * np.where(s > 0.5, 1.0, -1.0)[..., None]
        n = np.where(body[..., None], nb, ncap)
        rad = np.linalg.norm(p - (a + np.clip(s, 0, 1)[..., None] * ba), axis=-1) / r
        local = np.stack([s * 2 - 1, rad, body.astype(np.float64)], -1)
        return tt, n, local


class Ellipsoid(Solid):
    def __init__(self, centre, R, radii, albedo, **kw):
        super().__init__(albedo, **kw)
        self.c = np.asarray(centre, np.float64)
        self.R = np.asarray(R, np.float64)
        self.r = np.asarray(radii, np.float64)
        s = np.array([[i, j, k] for i in (-1, 1) for j in (-1, 1) for k in (-1, 1)])
        self.pts = self.c + (s * self.r) @ self.R
        self.pad = 0.0

    def hit(self, o, d):
        ol = ((o - self.c) @ self.R.T) / self.r
        dl = (d @ self.R.T) / self.r
        A = dot(dl, dl)
        B = dot(dl, ol)
        C = dot(ol, ol) - 1
        h = B * B - A * C
        with np.errstate(invalid="ignore"):
            sh = np.sqrt(h)
            t = (-B - sh) / A
            t2 = (-B + sh) / A
        ok = (h >= 0) & (t2 > 0)
        t = np.where(ok, np.maximum(t, 0), INF)
        q = ol + dl * np.where(ok, t, 0)[..., None]
        n = (q / self.r) @ self.R
        return t, n, q


# -- see-through things drawn over the lit picture ---------------------------------

class Puff:
    """A soft round puff: smoke, dust, spray, steam."""

    def __init__(self, p, radius, colour, alpha, lit=True):
        self.p = np.asarray(p, np.float64)
        self.radius = radius
        self.colour = np.asarray(colour, np.float64)
        self.alpha = alpha
        self.lit = lit


class Disc:
    """A spinning airscrew seen as a faint disc, with the ghost of its
    blades and a darker band where the tips run."""

    def __init__(self, centre, axis, radius, colour, alpha, angle=0.0,
                 blades=2):
        self.c = np.asarray(centre, np.float64)
        self.n = unit(axis)
        self.radius = radius
        self.colour = np.asarray(colour, np.float64)
        self.alpha = alpha
        self.angle = angle
        self.blades = blades


# -- a frame ---------------------------------------------------------------------

class Sky:
    """Colours of the sky and the haze, and the clouds over them."""

    def __init__(self, top, low, light, glow=0.0, cloud=0.5, seed=0,
                 haze=260.0, cloud_dark=0.55, sun_disc=0.0, cloud_lit=0.6):
        self.top = np.asarray(top, np.float64)
        self.low = np.asarray(low, np.float64)
        self.light = light
        self.glow = glow
        self.cloud = cloud
        self.seed = seed
        self.haze = haze
        self.cloud_dark = cloud_dark
        self.sun_disc = sun_disc
        self.cloud_lit = cloud_lit

    def haze_colour(self, d):
        """The colour things fade into far off: the sky at the horizon
        behind them, so a distant tree goes grey into the murk and does
        not stand out against it."""
        h = d.copy()
        h[..., 1] = 0.004
        h /= np.linalg.norm(h, axis=-1, keepdims=True)
        return self.colour(h, disc=False)

    def colour(self, d, disc=True):
        L = self.light
        el = d[..., 1]
        t = np.clip(el / 0.45, 0, 1) ** 0.7
        sky = self.low * (1 - t[..., None]) + self.top * t[..., None]
        g = np.clip(dot(d, L.sun), 0, 1)
        if self.glow:
            sky = sky + L.sun_col * self.glow * (0.55 * g ** 8 + 0.6 * g ** 60)[..., None] \
                * np.clip(1 - el * 2.5, 0.3, 1)[..., None]
        if self.sun_disc and disc:
            sky = sky + L.sun_col * self.sun_disc * \
                np.clip((g - 0.99965) * 6000, 0, 1)[..., None]
        # Cloud on a flat ceiling: no seam, and it runs into the horizon.
        e = np.maximum(el, 0.012)
        u = (d[..., 0] / e * 0.9 + self.seed * 0.37) * 170
        v = (d[..., 2] / e * 0.9 + self.seed * 0.11) * 170
        c = bilinear(CLOUD, u, v)
        # Near the horizon the ceiling is seen so edge-on that its pattern
        # would streak: there it thins into an even bank.
        k = np.clip((el - 0.01) / 0.10, 0, 1)
        c = c * k + (1 - self.cloud * 0.5) * (1 - k)
        cover = np.clip((c - (1 - self.cloud)) * 3.0, 0, 1) * np.clip(el / 0.02, 0, 1)
        thick = np.clip((c - (1 - self.cloud)) * 1.6, 0, 1)
        # Lit from beneath and at the edge by a low sun; shaded in the body.
        edge = (1 - thick) * (0.4 + 1.4 * g ** 4)
        ccol = self.top * 1.25 * (1 - self.cloud_dark * thick)[..., None] + \
            L.sun_col[None, None, :] * (self.cloud_lit * edge)[..., None] * 0.45
        sky = sky * (1 - cover[..., None] * 0.85) + ccol * cover[..., None] * 0.85
        return sky


class Frame:
    def __init__(self, cam, sky, ground):
        self.cam, self.sky, self.ground_fn = cam, sky, ground
        self.light = sky.light
        self.h, self.w = H * SS, W * SS
        self.solids = []
        self.overlays = []
        self.blobs = []
        self.shadow_range = 110.0     # beyond this the ground painter shades

    def add(self, s):
        self.solids.append(s)
        return s

    def puff(self, p, radius, colour, alpha, lit=True):
        self.overlays.append(Puff(p, radius, colour, alpha, lit))

    def disc(self, *a, **kw):
        self.overlays.append(Disc(*a, **kw))

    def blob(self, x, z, rx_, rz_, strength=0.35, yaw=0.0):
        """Darkening on the ground under a thing: the light it keeps from
        the sky, soft, whatever the sun."""
        self.blobs.append((x, z, rx_, rz_, strength, yaw))

    # -- the render -------------------------------------------------------------
    def render(self, t=0.0, post=None):
        cam, L = self.cam, self.light
        h, w = self.h, self.w
        d = cam.dirs
        alb = np.zeros((h, w, 3), np.float32)
        nrm = np.zeros((h, w, 3), np.float32)
        z = np.full((h, w), INF)
        spec = np.zeros((h, w), np.float32)
        glow = np.zeros((h, w), np.float32)
        ids = np.zeros((h, w), np.int32)
        wet = np.zeros((h, w), np.float32)
        # The ground: the plane y = 0.
        o = cam.pos
        with np.errstate(divide="ignore", invalid="ignore"):
            tg = -o[1] / d[..., 1]
        gnd = (d[..., 1] < -1e-5) & (tg > 0)
        tg = np.where(gnd, tg, INF)
        px = o[0] + d[..., 0] * np.where(gnd, tg, 0)
        pz = o[2] + d[..., 2] * np.where(gnd, tg, 0)
        g = self.ground_fn(px[gnd], pz[gnd], tg[gnd], t, self.light.sun)
        alb[gnd] = g["albedo"]
        nrm[gnd] = g.get("normal", np.array([0, 1, 0], np.float32))
        z[gnd] = tg[gnd]
        if "wet" in g:
            wet[gnd] = g["wet"]
        if "spec" in g:
            spec[gnd] = g["spec"]
        sheen = None
        if "sheen" in g:
            sheen = np.zeros((h, w), np.float32)
            sheen[gnd] = g["sheen"]
        self.ground = (gnd, px, pz)
        # Everything standing on it.
        by_id = {}
        for s in self.solids:
            by_id[s.id] = s
            box = cam.screen_box(s.pts, s.pad)
            if box is None:
                continue
            x0, y0, x1, y1 = box
            dd = d[y0:y1, x0:x1]
            ts, ns, local = s.hit(o, dd)
            near = np.isfinite(ts) & (ts < z[y0:y1, x0:x1]) & (ts > 0.05)
            if not near.any():
                continue
            ns = ns / np.maximum(np.linalg.norm(ns, axis=-1, keepdims=True), 1e-9)
            facing = dot(ns, dd) > 0
            ns = np.where(facing[..., None], -ns, ns)       # both faces lit
            col = s.colour(local[near], ns[near])
            r = (slice(y0, y1), slice(x0, x1))
            alb[r][near] = col
            nrm[r][near] = ns[near]
            z[r][near] = ts[near]
            spec[r][near] = s.spec
            glow[r][near] = s.glow
            ids[r][near] = s.id
            wet[r][near] = 0
        # The sun, and what stands in its way.
        vis = np.where(np.isfinite(z), 1.0, 0.0)
        sun = L.sun
        hit_p = None
        if L.sun_col.max() > 0.05 and sun[1] > 0.01:
            fin = np.isfinite(z)
            hit_p = np.where(fin[..., None], o + d * np.where(fin, z, 0)[..., None], 0)
            hit_p = hit_p + nrm * 0.02
            reach = min(80.0, 14.0 / max(sun[1], 0.12))
            for s in self.solids:
                if not s.shadow:
                    continue
                if np.linalg.norm(s.pts - o, axis=1).min() > self.shadow_range:
                    continue
                top = s.pts.copy()
                lo = top - sun[None, :] * np.clip(top[:, 1:2] / sun[1], 0, reach)
                box = cam.screen_box(np.concatenate([top, lo]), s.pad)
                if box is None:
                    continue
                x0, y0, x1, y1 = box
                r = (slice(y0, y1), slice(x0, x1))
                m = (vis[r] > 0) & (ids[r] != s.id)
                if not m.any():
                    continue
                pts = hit_p[r][m]
                ts, _, _ = s.hit(pts, sun)
                blocked = np.isfinite(ts)
                if blocked.any():
                    sub = vis[r]
                    mm = np.where(m)
                    sub[mm[0][blocked], mm[1][blocked]] = 0.0
            vis = _soften(vis)
        # Light it all.
        n = nrm.astype(np.float64)
        lam = np.clip(dot(n, sun), 0, 1) * vis
        skyl = (0.55 + 0.45 * n[..., 1])[..., None] * L.sky_col
        under = np.clip(-n[..., 1], 0, 1)[..., None] * L.bounce
        a64 = alb.astype(np.float64)
        col = a64 * (L.ambient + skyl + under + L.sun_col * lam[..., None])
        # Doped linen glows with the sun behind it.
        tr = glow * np.clip(-dot(n, sun), 0, 1) * vis
        col += a64 * L.sun_col * tr[..., None] * 0.9
        view = -d
        hv = sun[None, None, :] + view
        hv /= np.linalg.norm(hv, axis=-1, keepdims=True)
        sp = spec * np.clip(dot(n, hv), 0, 1) ** 24 * vis
        col += L.sun_col * sp[..., None]
        # Rim of light on the edges turned from us, towards the sun.
        rim = (1 - np.clip(dot(n, view), 0, 1)) ** 4 * np.clip(dot(view, -sun) + 0.15, 0, 1)
        col += L.sun_col * 0.12 * (rim * vis * (ids > 0))[..., None]
        # Wet ground mirrors the sky, and puddles the things over them.
        if wet.any():
            m = wet > 0.01
            rd = d[m].copy()
            rd[:, 1] *= -1
            refl = self.sky.colour(rd[None])[0]
            if hit_p is None:
                hp = o + d[m] * z[m][:, None]
            else:
                hp = hit_p[m]
            deep = wet[m] > 0.3
            if deep.any():
                rc, rt = self._trace(hp[deep] + np.array([0, 0.01, 0]), rd[deep])
                sub = refl[deep]
                refl[deep] = np.where(np.isfinite(rt)[:, None], rc, sub)
            k = wet[m][:, None].astype(np.float64)
            col[m] = col[m] * (1 - k) + refl * k
        # The ground's sheen: the sky caught at a slant.
        if sheen is not None:
            fres = (1 - np.clip(-d[..., 1], 0, 1)) ** 5
            k = (sheen * fres * (ids == 0))[..., None]
            rd = d.copy()
            rd[..., 1] = np.abs(rd[..., 1])
            col = col + self.sky.colour(rd, disc=False) * k
        # Soft darkening under things.
        if self.blobs:
            gm, gx, gz = self.ground
            for x, zz, a_, b_, st, yaw in self.blobs:
                c, s_ = math.cos(yaw), math.sin(yaw)
                u = ((gx - x) * c - (gz - zz) * s_) / a_
                v = ((gx - x) * s_ + (gz - zz) * c) / b_
                k = np.clip(1.15 - (u * u + v * v), 0, 1) ** 1.5 * st
                k = np.where(gm & (ids == 0), k, 0)
                col *= (1 - k)[..., None]
        # Haze with distance, the sky where nothing is.
        fin = np.isfinite(z)
        f = 1 - np.exp(-np.where(fin, z, 0) / self.sky.haze)
        hz = self.sky.haze_colour(d)
        col = col * (1 - f[..., None]) + hz * f[..., None]
        skyc = self.sky.colour(d)
        col = np.where(fin[..., None], col, skyc)
        self.z = z
        self.ids = ids
        for ov in self.overlays:
            if isinstance(ov, Puff):
                self._puff(col, ov)
            else:
                self._disc(col, ov)
        if post is not None:
            col = post(self, col)
        return col.reshape(H, SS, W, SS, 3).mean((1, 3))

    def _trace(self, o, d):
        """The nearest reflecting solid along each ray, lit simply."""
        L = self.light
        best = np.full(len(o), INF)
        col = np.zeros((len(o), 3))
        for s in self.solids:
            if not s.reflect:
                continue
            if np.linalg.norm(s.pts - self.cam.pos, axis=1).min() > 70:
                continue
            ts, ns, local = s.hit(o, d)
            m = np.isfinite(ts) & (ts < best) & (ts > 0.02)
            if not m.any():
                continue
            nn = ns[m] / np.maximum(np.linalg.norm(ns[m], axis=-1, keepdims=True), 1e-9)
            nn = np.where((dot(nn, d[m]) > 0)[:, None], -nn, nn)
            a = s.colour(local[m], nn)
            lam = np.clip(nn @ L.sun, 0, 1)[:, None]
            col[m] = a * (L.ambient + L.sky_col * (0.55 + 0.45 * nn[:, 1:2]) + L.sun_col * lam * 0.6)
            best[m] = ts[m]
        return col, best

    def _puff(self, col, p):
        q = self.cam.project(p.p)
        if q is None:
            return
        cx, cy, depth = q
        r = p.radius * self.cam.focal / depth * SS
        x0, x1 = int(max(0, cx * SS - r)), int(min(self.w, cx * SS + r + 1))
        y0, y1 = int(max(0, cy * SS - r)), int(min(self.h, cy * SS + r + 1))
        if x0 >= x1 or y0 >= y1 or r < 0.5:
            return
        yy, xx = np.mgrid[y0:y1, x0:x1]
        d2 = ((xx - cx * SS) ** 2 + (yy - cy * SS) ** 2) / (r * r)
        # Soft, and broken up so it does not read as a disc.
        lump = bilinear(FINE, (xx - cx * SS) / r * 9 + p.p[0] * 7,
                        (yy - cy * SS) / r * 9 + p.p[2] * 7)
        a = np.exp(-3.2 * d2) * (d2 < 1) * np.clip(0.55 + 0.9 * (lump - 0.5) + (1 - d2) * 0.5, 0, 1)
        a = a * p.alpha
        # Fades out where it meets something just in front or behind.
        zz = self.z[y0:y1, x0:x1]
        a = a * np.clip((zz - depth) / max(p.radius, 0.2) + 0.5, 0, 1)
        c = p.colour
        if p.lit:
            L = self.light
            view = unit(p.p - self.cam.pos)
            g = max(0.0, float(view @ L.sun))
            c = c * (L.ambient + L.sky_col * 0.9 + L.sun_col * (0.22 + 0.5 * g ** 6))
        f = 1 - math.exp(-depth / self.sky.haze)
        c = c * (1 - f) + self.sky.low * f
        reg = col[y0:y1, x0:x1]
        reg[:] = reg * (1 - a[..., None]) + c * a[..., None]

    def _disc(self, col, dsc):
        cam = self.cam
        ax = dsc.n
        u = unit(np.cross(ax, [0, 1, 0]) if abs(ax[1]) < 0.9 else np.cross(ax, [1, 0, 0]))
        v = np.cross(ax, u)
        ring = [dsc.c + dsc.radius * (math.cos(a) * u + math.sin(a) * v)
                for a in np.linspace(0, 2 * math.pi, 12, endpoint=False)]
        box = cam.screen_box(np.array(ring), dsc.radius * 0.3)
        if box is None:
            return
        x0, y0, x1, y1 = box
        d = cam.dirs[y0:y1, x0:x1]
        den = d @ ax
        with np.errstate(divide="ignore", invalid="ignore"):
            t = ((dsc.c - cam.pos) @ ax) / den
        p = cam.pos + d * np.where(np.isfinite(t), t, 0)[..., None]
        rel = p - dsc.c
        r = np.linalg.norm(rel, axis=-1) / dsc.radius
        ang = np.arctan2(rel @ v, rel @ u)
        inside = (r < 1) & (t > 0) & (t < self.z[y0:y1, x0:x1])
        # The blur of the blades: two faint bars smeared round, a band at
        # the tips and a little more density near the hub.
        ghost = np.cos((ang - dsc.angle) * dsc.blades) * 0.5 + 0.5
        ghost = ghost ** 6
        band = np.exp(-((r - 0.9) / 0.06) ** 2)
        a = dsc.alpha * (0.45 + 0.9 * ghost * (1 - r * 0.3) + 0.9 * band) * inside
        a *= np.clip((1 - r) * 25, 0, 1)
        a = np.clip(a, 0, 0.85)
        reg = col[y0:y1, x0:x1]
        reg[:] = reg * (1 - a[..., None]) + dsc.colour * a[..., None]


def _soften(v):
    """Soften the edges of the shadows by a pixel or so."""
    out = v.copy()
    out[1:-1, 1:-1] = (v[:-2, 1:-1] + v[2:, 1:-1] + v[1:-1, :-2] + v[1:-1, 2:] +
                       v[1:-1, 1:-1] * 2) / 6
    return out
