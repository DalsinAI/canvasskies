#!/usr/bin/env python3
"""The soundtracks of the aerodrome films, made from nothing but sums.

Every sound here is our own synthesis, read from the same account of each
film that the pictures are drawn from (airfield.py), so a wheel thumps on
the frame where it touches.  At 11025 samples a second, mono:

  the engine   each firing of a cylinder is a short bark of shaped noise
               over a dying low note, the barks coming as fast as the
               revolutions call for them: the rotary's sharp and ragged
               through its open ports, the in-line engines deeper through
               their pipes.  Under them runs the triangle-wave drone the
               game's own engine makes (audio_callback in canvas_skies.c),
               and the whup of the airscrew's blades.  The blip switch
               cuts the barks in bursts; a hit engine misses and backfires.
  the wheels   a low rumble that grows with the speed over the grass, a
               thump for each arrival on the ground as hard as the
               machine came down, the creak of the undercarriage.
  the tail skid  a gritty scrape while it drags.
  the air      the rush of the glide and the wires singing in it; the
               field's own breeze; rain hissing and pattering; birds at
               dawn.
  the print    the optical track's hiss and crackle, its band cut off at
               the top, a little wow, and a pop at a splice.

The sound travels: it is quieter and duller with distance, and a machine
passing the camera drops in pitch as it goes.

  python3 sound.py OUTDIR [film ...]

writes OUTDIR/<film>.wav, 16-bit, as long as the finished film.
"""

import math
import sys
import wave
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))
import airfield as A  # noqa: E402

SR = 11025
CTRL = 200                 # the film's account is read this often a second
C = 343.0                  # the speed of sound, metres a second


def bandnoise(n, lo, hi, rng, tilt=0.0):
    """White noise kept between lo and hi Hz, with soft shoulders."""
    x = rng.standard_normal(n)
    X = np.fft.rfft(x)
    f = np.fft.rfftfreq(n, 1 / SR)
    m = 1 / (1 + (lo / np.maximum(f, 1e-3)) ** 4) / (1 + (f / hi) ** 4)
    if tilt:
        m = m * (np.maximum(f, 20) / 200.0) ** tilt
    y = np.fft.irfft(X * m, n)
    return y / (np.std(y) + 1e-9)


def lowpass(x, hz):
    X = np.fft.rfft(x)
    f = np.fft.rfftfreq(len(x), 1 / SR)
    return np.fft.irfft(X / (1 + (f / hz) ** 4), len(x))


def env(tc, values, n):
    """A control-rate curve spread over the samples."""
    ts = np.arange(n) / SR
    return np.interp(ts, tc, values)


# -- the pieces -------------------------------------------------------------------

def bark_bank(kind, rng, count=8):
    """Variations on one cylinder's firing, for a rotary or an in-line."""
    bank = []
    L = int(0.045 * SR)
    t = np.arange(L) / SR
    for i in range(count):
        if kind == "rotary":
            crack = rng.standard_normal(L) * np.exp(-t / 0.0035)
            body = np.sin(2 * math.pi * rng.uniform(150, 210) * t) * np.exp(-t / 0.010)
            low = np.sin(2 * math.pi * rng.uniform(70, 90) * t) * np.exp(-t / 0.016)
            k = 0.55 * crack + 0.6 * body + 0.7 * low
        else:
            crack = rng.standard_normal(L) * np.exp(-t / 0.0025)
            body = np.sin(2 * math.pi * rng.uniform(110, 140) * t) * np.exp(-t / 0.014)
            low = np.sin(2 * math.pi * rng.uniform(55, 70) * t) * np.exp(-t / 0.022)
            k = 0.30 * crack + 0.6 * body + 0.9 * low
        k[:3] *= np.linspace(0.3, 1, 3)
        bank.append(k / np.abs(k).max())
    return bank


def triangle(phase):
    v = phase - np.floor(phase)
    return 1.0 - 4.0 * np.abs(v - 0.5)


def engine(n, tc, plane_rows, kind, rng, extras):
    """One engine at the machine, before the journey to the camera."""
    rpm = env(tc, [r["rpm"] for r in plane_rows], n)
    power = env(tc, [r["power"] for r in plane_rows], n)
    running = env(tc, [float(r["running"]) for r in plane_rows], n) > 0.5
    sputter = env(tc, [r["sputter"] for r in plane_rows], n)
    rotary = kind == "scout"
    per_rev = 4.5 if rotary else 4.0
    f_fire = rpm * A.FULL_REVS * per_rev
    phase = np.cumsum(f_fire / SR)
    fires = np.nonzero(np.diff(np.floor(phase)) > 0)[0] + 1
    bank = bark_bank("rotary" if rotary else "inline", rng)
    out = np.zeros(n + 1000)
    catch = extras.get("catch")
    ts = np.arange(n) / SR
    for i in fires:
        if not running[i] or power[i] < 0.02:
            continue
        miss = 0.04 + 0.18 * (1 - min(1.0, power[i] * 2.5)) * rotary
        if catch is not None and ts[i] < catch + 1.0:
            miss = max(miss, 0.55 * (1 - (ts[i] - catch) / 1.0))
        miss = max(miss, 0.35 * sputter[i])
        if rng.random() < miss:
            continue
        amp = (0.35 + 0.65 * power[i]) * rng.uniform(0.75, 1.15)
        k = bank[rng.integers(len(bank))]
        if sputter[i] > 0.5 and rng.random() < 0.025:
            k = backfire(rng)              # a hit engine bangs in its pipes
            amp *= 1.6
        out[i:i + len(k)] += amp * k
    out = out[:n]
    # The game's own triangle-wave drone, and its sine an octave down.
    live = (running & (power > 0.02)).astype(float)
    live = np.convolve(live, np.ones(200) / 200, mode="same")
    ph = np.cumsum(f_fire * 0.5 / SR)
    drone = (triangle(ph) * 0.55 + np.sin(2 * math.pi * ph * 0.5) * 0.35) * live * \
        (0.4 + 0.6 * power)
    # The airscrew: the whup of its blades going round, turning or not.
    blades = 2 if kind != "late" else 4
    bp = np.cumsum(rpm * A.FULL_REVS * blades / SR)
    whup = (0.5 + 0.5 * np.cos(2 * math.pi * bp)) ** 3
    swish = bandnoise(n, 180, 1400, rng) * whup * (rpm ** 1.5) * 0.12
    return out * 0.9 + drone * 0.35 + swish


def backfire(rng):
    L = int(0.12 * SR)
    t = np.arange(L) / SR
    k = rng.standard_normal(L) * np.exp(-t / 0.012) + \
        np.sin(2 * math.pi * 60 * t) * np.exp(-t / 0.05) * 1.5
    return k / np.abs(k).max()


def thump(strength, rng, heavy=False):
    """The machine arriving on its wheels: the tyres and the shock cord
    taking the weight, the ash struts creaking."""
    L = int((0.45 if heavy else 0.30) * SR)
    t = np.arange(L) / SR
    low = np.sin(2 * math.pi * (52 + 30 * np.exp(-t / 0.05)) * t) * np.exp(-t / (0.09 if heavy else 0.06))
    knock = rng.standard_normal(L) * np.exp(-t / 0.006)
    knock = np.convolve(knock, np.ones(6) / 6, mode="same")
    creak_t = t - 0.04
    creak = np.sin(2 * math.pi * (310 + 60 * np.sin(creak_t * 40)) * creak_t) * \
        np.exp(-np.maximum(creak_t, 0) / 0.05) * (creak_t > 0)
    k = 1.0 * low + 0.6 * knock + 0.15 * creak
    if heavy:
        crunch = rng.standard_normal(L) * np.exp(-t / 0.08) * (0.5 + 0.5 * np.sin(t * 190))
        rattle = np.zeros(L)
        for j in range(6):
            p = int(rng.uniform(0.03, 0.3) * SR)
            q = min(L, p + int(0.02 * SR))
            rattle[p:q] += rng.standard_normal(q - p) * np.exp(-np.arange(q - p) / 30)
        k = k * 1.3 + 0.35 * crunch + 0.4 * rattle
    return strength * k / np.abs(k).max()


def birds(n, rng, until):
    """A lark or two in the half-light before the engine starts."""
    out = np.zeros(n)
    t = 0.2
    while t < until:
        L = int(rng.uniform(0.04, 0.12) * SR)
        s = int(t * SR)
        if s + L >= n:
            break
        tt = np.arange(L) / SR
        f0 = rng.uniform(2600, 3800)
        sweep = f0 * (1 + rng.uniform(-0.25, 0.25) * tt / tt[-1]) + 300 * np.sin(tt * 120)
        ph = np.cumsum(sweep / SR)
        out[s:s + L] += np.sin(2 * math.pi * ph) * np.sin(np.pi * tt / tt[-1]) * rng.uniform(0.3, 1)
        t += rng.uniform(0.05, 0.35)
    return out * 0.05


def travel(sig, tc, dist, n):
    """The sound as heard at the camera: later by the distance (which bends
    the pitch of a passing machine), quieter, and duller far off."""
    ts = np.arange(n) / SR
    d = np.interp(ts, tc, dist)
    src_t = ts - d / C
    heard = np.interp(src_t, ts, sig, left=0.0)
    dull = lowpass(heard, 900)
    k = np.clip(d / 70, 0, 1)
    heard = heard * (1 - k) + dull * k
    return heard / (1 + d / 14.0)


# -- a whole film ---------------------------------------------------------------------

EXTRAS = {
    "takeoff-1-dawn-prop-swing": {"catch": 1.0, "coughs": (0.82,), "birds": 1.0, "swing": 0.66},
}


def account(name):
    """Read the film at the control rate: the camera and each machine."""
    fn, length, intro = A.FILMS[name]
    tc = np.arange(0, length + 1 / CTRL, 1 / CTRL)
    rows = []
    for t in tc:
        shot = fn(t)
        cam = np.asarray(shot.cam[0], np.float64)
        planes = []
        for p in shot.planes:
            l, r, sk = p.contacts()
            planes.append(dict(kind=p.kind, pos=p.pos.copy(), rpm=p.rpm, power=p.power,
                               running=p.running, sputter=p.sputter, speed=p.speed,
                               wheel=min(l, r), skid=sk))
        rows.append(dict(cam=cam, planes=planes, weather=shot.weather, rain=shot.rain,
                         events=shot.events))
    return tc, rows


def soundtrack(name, seed=0):
    fn, length, intro = A.FILMS[name]
    pre = A.intro_length(intro)
    n_frames = len(A.intro_frames(intro)) + int(round(length * A.FPS))
    total = n_frames / A.FPS
    n = int(round(length * SR))
    rng = np.random.default_rng(seed + 1917)
    tc, rows = account(name)
    extras = EXTRAS.get(name, {})
    mix = np.zeros(n)
    weather = rows[0]["weather"]
    n_planes = len(rows[0]["planes"])
    for k in range(n_planes):
        pr = [r["planes"][k] for r in rows]
        kind = pr[0]["kind"]
        dist = np.array([np.linalg.norm(p["pos"] - r["cam"]) for p, r in zip(pr, rows)])
        sig = engine(n, tc, pr, kind, rng, extras) * 0.9
        speed = np.array([p["speed"] for p in pr])
        wheel = np.array([p["wheel"] for p in pr])
        skid = np.array([p["skid"] for p in pr])
        on = wheel < 0.04
        # Rumbling over the grass, rougher with speed.
        sp = env(tc, speed, n)
        ground = env(tc, on.astype(float), n)
        bumps = np.abs(bandnoise(n, 5, 18, rng)) * 0.7 + 0.5
        mud = 1.0 if name.startswith("takeoff-3") else 0.0
        rumble = bandnoise(n, 25, 220 + 200 * mud, rng) * bumps * ground * np.clip(sp / 14, 0, 1.2)
        sig += rumble * 0.55
        if mud:
            slosh = bandnoise(n, 300, 1800, rng) * np.abs(bandnoise(n, 3, 12, rng)) * ground * \
                np.clip(sp / 10, 0, 1)
            sig += slosh * 0.12
        # The tail skid dragging.
        sk_on = env(tc, (skid < 0.05).astype(float), n)
        grit = np.abs(bandnoise(n, 12, 45, rng))
        scrape = bandnoise(n, 280, 1500, rng) * grit * sk_on * np.clip(sp / 8, 0, 1) * \
            (sp > 0.5)
        sig += scrape * 0.22
        # The rush of air past it and the wires singing, in the glide.
        air = env(tc, (~on).astype(float), n)
        v2 = np.clip(sp / 20, 0, 1.3) ** 2
        rush = bandnoise(n, 90, 1100, rng, tilt=-0.3) * v2 * (0.4 + 0.6 * air)
        ts = np.arange(n) / SR
        wires = np.zeros(n)
        for f0, a in ((640, 1.0), (955, 0.7), (1330, 0.45), (1810, 0.25)):
            drift = 1 + 0.025 * np.sin(2 * math.pi * 0.31 * ts + f0) + 0.01 * bandnoise(n, 0.5, 3, rng)
            wires += a * np.sin(2 * math.pi * np.cumsum(f0 * drift * np.clip(0.6 + sp / 40, 0.5, 1.2) / SR))
        wires *= (0.6 + 0.4 * np.abs(bandnoise(n, 1, 6, rng))) * np.clip((sp - 7) / 10, 0, 1) ** 2
        quiet = 1.0 if not any(p["running"] for p in pr) else 0.25
        sig += rush * (0.04 + 0.32 * quiet) + wires * 0.15 * quiet
        # Thumps: wherever the wheels come down from the air.
        for i in range(1, len(tc)):
            if on[i] and not on[i - 1]:
                vy = max(0.3, (wheel[i - 2] - wheel[i]) * CTRL / 2 if i > 1 else 0.5)
                heavy = vy > 2.0
                s = int(tc[i] * SR)
                th = thump(min(1.5, 0.35 + vy * 0.35), rng, heavy=heavy)
                e = min(n, s + len(th))
                sig[s:e] += th[:e - s]
        mix += travel(sig, tc, dist, n) * 2.2 / math.sqrt(n_planes)
    # The shot-up landing's second arrival, on one wheel, with a lurch.
    ev = rows[-1]["events"]
    if "lurch" in ev:
        s = int(ev["lurch"] * SR)
        th = thump(1.2, rng, heavy=True)
        mix[s:s + len(th)] += th[:n - s] * 2.0
    # Swinging the airscrew and the coughs before it catches.
    if "swing" in extras:
        s = int(extras["swing"] * SR)
        L = int(0.25 * SR)
        sw = bandnoise(L, 200, 1500, rng) * np.sin(np.linspace(0, math.pi, L)) * 0.18
        mix[s:s + L] += sw
    for c in extras.get("coughs", ()):
        s = int(c * SR)
        k = backfire(rng) * 0.6
        mix[s:s + len(k)] += k
    # The field: a breeze, birds at dawn, rain.
    mix += bandnoise(n, 40, 500, rng, tilt=-0.5) * 0.010 * (1 + np.abs(bandnoise(n, 0.2, 1, rng)))
    if "birds" in extras:
        mix += birds(n, rng, 3.5)
    if rows[0]["rain"]:
        hiss = bandnoise(n, 1200, 5200, rng) * 0.05
        patter = np.zeros(n)
        for s in rng.integers(0, n - 60, int(length * 160)):
            L = 40
            patter[s:s + L] += rng.standard_normal(L) * np.exp(-np.arange(L) / 6) * rng.uniform(0.2, 1)
        mix += hiss + lowpass(patter, 4000) * 0.04
        mix += bandnoise(n, 60, 400, rng) * 0.015
    if weather in ("overcast",) and n_planes and not rows[0]["planes"][0]["running"]:
        mix += bandnoise(n, 60, 700, rng, tilt=-0.4) * 0.02
    # Cuts in the film are splices in the track too.
    if "cut" in rows[-1]["events"]:
        s = int(rows[-1]["events"]["cut"] * SR)
        mix[s:s + 30] += np.linspace(0.25, 0, 30) * rng.choice([-1, 1])
    # The head of the reel: silence on the leader, a pop at a splice.
    head = np.zeros(int(round(pre * SR)))
    if intro == "cut":
        head[-40:] += np.linspace(0.3, 0, 40)
    full = np.concatenate([head, mix])
    want = int(round(total * SR))
    full = np.pad(full, (0, max(0, want - len(full))))[:want]
    return optical(full, rng)


def optical(x, rng):
    """The optical track: a little wow, its top cut away, hiss and crackle,
    and a gentle limit on the loudest moments."""
    n = len(x)
    ts = np.arange(n) / SR
    warp = ts + 0.0018 * np.sin(2 * math.pi * 0.7 * ts) / (2 * math.pi * 0.7)
    x = np.interp(warp, ts, x)
    x = lowpass(x, 3600)
    x = x + bandnoise(n, 900, 5000, rng) * 0.006
    for s in rng.integers(0, n - 8, int(n / SR * 4)):
        x[s:s + 3] += rng.uniform(-0.12, 0.12) * np.array([1, -0.6, 0.3])
    x = np.tanh(x * 1.1) / math.tanh(1.1)
    peak = np.abs(x).max()
    if peak > 0.92:
        x = x * 0.92 / peak
    return x


def write_wav(path, x):
    data = (np.clip(x, -1, 1) * 32767).astype("<i2").tobytes()
    with wave.open(str(path), "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(SR)
        w.writeframes(data)


def main():
    out = Path(sys.argv[1])
    out.mkdir(parents=True, exist_ok=True)
    names = sys.argv[2:] or list(A.FILMS)
    for i, name in enumerate(A.FILMS):
        if name not in names:
            continue
        x = soundtrack(name, seed=i)
        write_wav(out / f"{name}.wav", x)
        print(name, f"{len(x) / SR:.2f} s", f"peak {np.abs(x).max():.2f}",
              f"rms {np.sqrt((x ** 2).mean()):.3f}", flush=True)


if __name__ == "__main__":
    main()
