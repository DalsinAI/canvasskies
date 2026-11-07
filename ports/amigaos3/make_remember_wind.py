#!/usr/bin/env python3
"""The wind over the old front, for the roll call at the end.

A minute of gentle wind, synthesised here and nowhere else: a soft body
of air that rises and falls in slow gusts, brightening a little as each
gust comes on, and the dry rustle of the long grass and the seed heads
stirring in it, with now and then a stalk ticking against another.  No
music and no birds.

Every part of it is made to repeat exactly once a minute: the noise is
shaped in the frequency domain over the whole minute, the gusts rise and
fall a whole number of times in it, and the ticks wrap round from the end
to the start, so the sound loops without a seam or a click.

  python3 make_remember_wind.py [OUT.wav]

writes assets/canvas-skies/remember-wind.wav (11025 Hz mono, 16-bit) by
default.
"""

import math
import sys
import wave
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "canvas-skies" / "remember-wind.wav"
SR = 11025
SECONDS = 60
N = SR * SECONDS


def band(rng, lo, hi, tilt=0.0):
    """Noise between lo and hi Hz with soft shoulders, periodic in N."""
    X = np.fft.rfft(rng.standard_normal(N))
    f = np.fft.rfftfreq(N, 1 / SR)
    m = 1 / (1 + (lo / np.maximum(f, 1e-3)) ** 4) / (1 + (f / hi) ** 4)
    if tilt:
        m *= (np.maximum(f, 10) / 200.0) ** tilt
    y = np.fft.irfft(X * m, N)
    return y / (np.std(y) + 1e-12)


def slow(rng, cycles, depth):
    """A slow wave of whole cycles a minute, for the gusts."""
    t = np.arange(N) / N
    out = np.zeros(N)
    for c, d in zip(cycles, depth):
        out += d * np.sin(2 * math.pi * c * t + rng.uniform(0, 2 * math.pi))
    return out


def wind():
    rng = np.random.default_rng(1919)
    # The gusts: a long swell with lesser ones riding on it.
    g = slow(rng, (2, 3, 5, 8, 13), (0.35, 0.25, 0.18, 0.12, 0.07))
    g = np.clip(0.55 + g, 0.12, 1.3)
    g = np.convolve(np.concatenate([g[-2000:], g, g[:2000]]), np.ones(2001) / 2001,
                    mode="same")[2000:-2000]
    # The body of the air, dark and soft.
    body = band(rng, 50, 700, tilt=-0.6) * (0.35 + 0.65 * g)
    # The gust's breath, brighter as it comes on.
    breath = band(rng, 250, 2200, tilt=-0.3) * np.clip(g - 0.35, 0, None) ** 1.6
    # The grass: a dry hiss, fluttering, stronger in the gusts.
    flutter = np.abs(band(rng, 6, 28))
    flutter = 0.4 + 0.6 * flutter / flutter.max()
    hiss = band(rng, 2200, 5200) * flutter * np.clip(g - 0.2, 0, None) ** 1.4
    # Stalks and seed heads ticking against each other, now and then.
    ticks = np.zeros(N)
    L = 90
    k = np.arange(L)
    for pos in rng.integers(0, N, 1400):
        if rng.random() > 0.25 + 0.6 * min(1.0, g[pos]):
            continue
        tick = rng.standard_normal(L) * np.exp(-k / rng.uniform(6, 18))
        idx = (pos + k) % N
        ticks[idx] += tick * rng.uniform(0.2, 1.0)
    T = np.fft.rfft(ticks)
    f = np.fft.rfftfreq(N, 1 / SR)
    T *= 1 / (1 + (1800 / np.maximum(f, 1)) ** 2) / (1 + (f / 4800) ** 4)
    ticks = np.fft.irfft(T, N)
    ticks /= np.std(ticks) + 1e-12
    x = 0.110 * body + 0.045 * breath + 0.022 * hiss + 0.010 * ticks
    # Keep the loudest gust well inside the range; no limiter needed.
    return x * (0.55 / np.abs(x).max())


def main():
    out = Path(sys.argv[1]) if len(sys.argv) > 1 else OUT
    x = wind()
    data = (np.clip(x, -1, 1) * 32767).round().astype("<i2")
    with wave.open(str(out), "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(SR)
        w.writeframes(data.tobytes())
    step = np.abs(np.diff(data.astype(int)))
    print(f"{out.name}: {len(data) / SR:.1f} s, peak {np.abs(x).max():.2f}, "
          f"rms {np.sqrt((x ** 2).mean()):.3f}, loop step {abs(int(data[0]) - int(data[-1]))} "
          f"(typical {int(np.percentile(step, 50))}, 99th {int(np.percentile(step, 99))})")


if __name__ == "__main__":
    main()
