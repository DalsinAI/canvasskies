#!/usr/bin/env python3
"""The picture-house pianist: an original accompaniment for the newsreels.

Sixteen bars in A minor turning to C major, played the way a cinema
pianist kept a reel moving: an oom-pah left hand under a plain tune, a
little rubato, a slightly tired upright piano.  Composed for this game and
synthesised here, so there is nothing to license.  Written to
assets/canvas-skies/cinema-piano.wav, mono 16-bit at 11025 Hz like the
other records.
"""

import math
import wave
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "canvas-skies" / "cinema-piano.wav"
RATE = 11025
BEAT = 60.0 / 96.0
RNG = np.random.default_rng(1916)

NOTE = {"C": 0, "D": 2, "E": 4, "F": 5, "G": 7, "A": 9, "B": 11}


def freq(name):
    """'A4', 'C#5', 'Bb3'."""
    letter, rest = name[0], name[1:]
    shift = 0
    while rest and rest[0] in "#b":
        shift += 1 if rest[0] == "#" else -1
        rest = rest[1:]
    midi = 12 * (int(rest) + 1) + NOTE[letter] + shift
    return 440.0 * 2 ** ((midi - 69) / 12.0)


def piano(f, dur, vel):
    """A struck string: a few detuned partials, a quick decay, a soft thump."""
    n = int(dur * RATE)
    t = np.arange(n) / RATE
    out = np.zeros(n)
    for k, amp in ((1, 1.0), (2, 0.45), (3, 0.22), (4, 0.12), (5, 0.06)):
        detune = 1.0 + RNG.normal(0, 0.0012)
        out += amp * np.sin(2 * math.pi * f * k * detune * t) * \
            np.exp(-t * (2.2 + k * 0.9))
    out *= 1 - np.exp(-t * 400)                       # the hammer's attack
    return out * vel


# Melody (right hand): (note, beats).  A minor, then C major for the end.
TUNE = [
    ("E5", 1), ("A5", 1), ("C6", 1), ("B5", 1), ("A5", 2), ("E5", 2),
    ("F5", 1), ("E5", 1), ("D5", 1), ("C5", 1), ("B4", 2), ("E5", 2),
    ("A4", 1), ("C5", 1), ("E5", 1), ("A5", 1), ("G#5", 2), ("E5", 2),
    ("A5", 1.5), ("B5", 0.5), ("C6", 1), ("B5", 1), ("A5", 4),
    ("G5", 1), ("C6", 1), ("E6", 1), ("D6", 1), ("C6", 2), ("G5", 2),
    ("A5", 1), ("G5", 1), ("F5", 1), ("E5", 1), ("D5", 2), ("G5", 2),
    ("E5", 1), ("G5", 1), ("C6", 1), ("E6", 1), ("D6", 2), ("B5", 2),
    ("C6", 1.5), ("D6", 0.5), ("B5", 1), ("G5", 1), ("C6", 4),
]
# Left hand: a bass note on 1 and 3, a chord on 2 and 4, one chord a bar.
CHORDS = ["Am", "Am", "Dm", "E", "Am", "E", "Am", "Am",
          "C", "C", "F", "G", "C", "G", "C", "C"]
SHAPES = {"Am": ("A2", ["A3", "C4", "E4"]), "Dm": ("D3", ["D4", "F4", "A4"]),
          "E": ("E2", ["E3", "G#3", "B3"]), "C": ("C3", ["C4", "E4", "G4"]),
          "F": ("F2", ["F3", "A3", "C4"]), "G": ("G2", ["G3", "B3", "D4"])}


def main():
    total = int(len(CHORDS) * 4 * BEAT * RATE) + RATE
    mix = np.zeros(total)

    def put(f, start, dur, vel):
        s = int(start * RATE + RNG.normal(0, 0.006) * RATE)   # human time
        s = max(0, s)
        note = piano(f, dur + 0.4, vel)
        mix[s:s + len(note)] += note[:max(0, total - s)]

    t = 0.0
    for name, beats in TUNE:
        put(freq(name), t, beats * BEAT, 0.34)
        t += beats * BEAT
    for bar, chord in enumerate(CHORDS):
        bass, notes = SHAPES[chord]
        b0 = bar * 4 * BEAT
        for beat in range(4):
            if beat % 2 == 0:
                put(freq(bass), b0 + beat * BEAT, BEAT, 0.30)
            else:
                for nn in notes:
                    put(freq(nn), b0 + beat * BEAT, BEAT * 0.8, 0.10)
    # The room: a touch of reverb, and the upright's slightly dull top.
    rev = np.zeros_like(mix)
    for delay, gain in ((0.031, 0.22), (0.047, 0.16), (0.073, 0.1)):
        d = int(delay * RATE)
        rev[d:] += mix[:-d] * gain
    mix = mix + rev
    kernel = np.ones(3) / 3
    mix = np.convolve(mix, kernel, mode="same")
    mix /= np.max(np.abs(mix)) * 1.15
    pcm = (mix * 32767).astype(np.int16)
    with wave.open(str(OUT), "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(RATE)
        w.writeframes(pcm.tobytes())


if __name__ == "__main__":
    main()
