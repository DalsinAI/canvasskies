#!/usr/bin/env python3
"""Colonel Bogey on the mess gramophone: a military band, played for this game.

Kenneth J. Alford's march (1914; Alford died in 1945, so the music is in
the public domain) played by a band of our own synthesis and then put on
a worn 78: cornets and clarinets on the tune, a piccolo over the loud
strains, horns on the off-beats, the bombardon on the beats, the bass drum
with its cymbal and a side drum, then the acoustic horn's narrow band and
resonances, the turntable's wow and flutter, and the shellac's hiss and
crackle.  No recording, sample or outside sound of any kind is used.

The tune is the march's own (Hawkes & Son, London, 1914), read note by
note from a modern piano typesetting of it on Wikimedia Commons
(File:Colonel_Bogey_March_piano_solo.pdf); only the melody was taken.
The harmony under it, the scoring for the band and everything heard are
written and made here.  The first part is played twice, as a long
side might hold it: the introduction, the first
strain, the second strain repeated with its two endings, the first strain
again, then the second and first strains once more, louder, to the end.

The file is made to loop in the officers' mess: it opens with a quarter
second of needle on the run-in groove and closes the same way after the
last chord, so the game's 186 ms crossfade from the end into the start
joins surface noise to surface noise.

  python3 make_colonel_bogey.py [OUT.wav]

writes assets/canvas-skies/colonel-bogey-gramophone.wav (11025 Hz mono,
16-bit) by default.  The output is the same on every run.
"""

import math
import sys
import wave
from pathlib import Path

import numpy as np

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "canvas-skies" / "colonel-bogey-gramophone.wav"
RATE = 11025
NYQ = RATE / 2
PACE = 116                      # quick march: two paces a bar, cut time
BEAT = 60.0 / PACE / 2          # a crotchet
BAR = 4 * BEAT
RUN_IN = 0.30                   # needle on the lead-in groove, seconds
RUN_OUT = 1.60                  # the last chord's ring and the run-out groove
RNG = np.random.default_rng(1914)

NOTE = {"C": 0, "D": 2, "E": 4, "F": 5, "G": 7, "A": 9, "B": 11}


def midi(name):
    """'A4', 'C#5', 'Bb3' -> MIDI number."""
    letter, rest = name[0], name[1:]
    shift = 0
    while rest and rest[0] in "#b":
        shift += 1 if rest[0] == "#" else -1
        rest = rest[1:]
    return 12 * (int(rest) + 1) + NOTE[letter] + shift


def hz(m):
    return 440.0 * 2 ** ((m - 69) / 12.0)


# --- The march --------------------------------------------------------------
# Each bar of the tune: (note, crotchets), "r" a rest; and the bar's
# harmony, one chord or two (one for each half).  D major, cut time.

INTRO = [
    ([("D5", 1), ("r", 1), ("C#5", 1.5), ("B4", 0.5)], ["D", "C#"]),
    ([("A4", 1), ("F#4", 1), ("D4", 1), ("F#4", 1)], ["D"]),
    ([("A4", 1), ("A4", .5), ("B4", .5), ("A4", 1), ("A4", .5), ("B4", .5)],
     ["D", "B7"]),
    ([("A4", 4)], ["A7"]),
]

FIRST = [
    # "the descending minor third that begins each line"
    ([("A4", 1), ("F#4", .5), ("r", .5), ("r", 2)], ["D"]),
    ([("r", 1), ("F#4", 1), ("G4", 1), ("A4", 1)], ["D"]),
    ([("F#5", 1), ("r", 1), ("F#5", 1), ("r", 1)], ["D"]),
    ([("D5", 4)], ["D"]),
    ([("A4", 1), ("F#4", .5), ("r", .5), ("r", 2)], ["D"]),
    ([("r", 1), ("F#4", 1), ("G4", 1), ("F#4", 1)], ["D"]),
    ([("A4", 1), ("r", 1), ("A4", 1), ("r", 1)], ["D"]),
    ([("G4", 4)], ["A7"]),
    ([("G4", 1), ("E4", .5), ("r", .5), ("r", 2)], ["A7"]),
    ([("r", 1), ("E4", 1), ("F#4", 1), ("G4", 1)], ["A7"]),
    ([("A4", 1), ("F#4", .5), ("r", .5), ("r", 2)], ["D"]),
    ([("r", 1), ("F#4", 1), ("G#4", 1.5), ("F#4", .5)], ["E7"]),
    ([("E4", 1), ("A4", 1), ("r", 1), ("F#4", 1)], ["A"]),
    ([("G#4", 1), ("E4", 1), ("r", 1), ("B4", 1)], ["E7"]),
    ([("A4", 4)], ["A"]),
    ([("A4", 1), ("A4", .5), ("A4", .5), ("A4", 1), ("A4", 1)], ["A7"]),
    ([("A5", 1), ("F#5", .5), ("r", .5), ("r", 2)], ["D"]),
    ([("r", 1), ("F#5", 1), ("G5", 1), ("A5", 1)], ["D"]),
    ([("F#6", 1), ("r", 1), ("F#6", 1), ("r", 1)], ["D"]),
    ([("D6", 4)], ["D"]),
    ([("A5", 1), ("F#5", .5), ("r", .5), ("r", 2)], ["D"]),
    ([("r", 1), ("F#5", 1), ("G5", 1), ("F#5", 1)], ["D"]),
    ([("A5", 1), ("r", 1), ("A5", 1), ("r", 1)], ["D"]),
    ([("G5", 4)], ["A7"]),
    ([("G5", 1), ("E5", .5), ("r", .5), ("r", 2)], ["A7"]),
    ([("r", 1), ("B5", 1), ("C#6", 1), ("B5", 1)], ["A7"]),
    ([("D6", 1), ("A5", 1), ("r", 2)], ["D"]),
    ([("r", 1), ("A5", 1), ("G5", 1), ("F#5", 1)], ["D"]),
    ([("E5", 1), ("B5", .5), ("r", .5), ("r", 1), ("D5", 1)], ["Em", "G"]),
    ([("C#5", 1), ("A5", .5), ("r", .5), ("r", 1), ("A4", 1)], ["A7"]),
    ([("D5", 4)], ["D"]),
    ([("D5", 1), ("r", 1), ("r", 2)], ["D"]),
]

SECOND = [
    ([("r", 1), ("D5", .5), ("E5", .5), ("F#5", 1), ("F#5", 1)], ["Bm"]),
    ([("F#5", 4)], ["Bm"]),
    ([("F#5", 1), ("B4", .5), ("C#5", .5), ("D5", .5), ("C#5", .5),
      ("B4", .5), ("A#4", .5)], ["Bm", "F#7"]),
    ([("B4", 1), ("D5", .5), ("D5", .5), ("D5", 1), ("D5", 1)], ["Bm"]),
    ([("r", 1), ("F#5", 1), ("r", 1), ("F#5", 1)], ["Bm"]),
    ([("r", 1), ("F#5", 1), ("r", 1), ("F#5", 1)], ["Bm"]),
    ([("r", 1), ("G5", .5), ("G5", .5), ("G5", 1), ("G5", 1)], ["G"]),
    ([("G5", 1), ("r", 1), ("G5", 2)], ["G"]),
    ([("F#5", 1), ("D5", .5), ("C#5", .5), ("D5", 1), ("F#5", 1)], ["Bm"]),
    ([("B5", 1), ("r", 1), ("A5", 2)], ["Bm", "B7"]),
    ([("G5", 1), ("G5", .5), ("A5", .5), ("B5", .5), ("A5", .5),
      ("G5", .5), ("F#5", .5)], ["Em"]),
    ([("E5", 1), ("r", 1), ("G5", 2)], ["Em"]),
    ([("F#5", 1), ("D5", .5), ("E5", .5), ("F#5", 1), ("B5", 1)], ["Bm"]),
    ([("F#5", 1), ("C#5", .5), ("D5", .5), ("E5", 1), ("F#5", 1)], ["F#7"]),
    ([("r", 1), ("B5", .5), ("B5", .5), ("B5", 1), ("B5", 1)], ["Bm"]),
]
FIRST_TIME = ([("B5", 1), ("r", 1), ("r", 2)], ["Bm"])
SECOND_TIME = ([("B5", 1), ("r", 1), ("A5", 2)], ["Bm", "A7"])

# Chords: the bombardon's root and fifth, and the horns' notes.
CHORDS = {
    "D": ("D2", "A2", ["F#3", "A3", "D4"]),
    "C#": ("C#2", "G#2", ["E#3", "G#3", "C#4"]),
    "B7": ("B1", "F#2", ["D#3", "A3", "B3"]),
    "A": ("A1", "E2", ["E3", "A3", "C#4"]),
    "A7": ("A1", "E2", ["G3", "A3", "C#4"]),
    "E7": ("E2", "B1", ["G#3", "B3", "D4"]),
    "Em": ("E2", "B1", ["G3", "B3", "E4"]),
    "G": ("G1", "D2", ["G3", "B3", "D4"]),
    "Bm": ("B1", "F#2", ["F#3", "B3", "D4"]),
    "F#7": ("F#2", "C#2", ["E3", "A#3", "C#4"]),
}

# The side: (bars, loudness, full band?).  The second time through is the
# loud one, with the piccolo up top and the drums on every beat.
SIDE = [
    (INTRO, 0.95, True),
    (FIRST, 0.62, False),
    (SECOND + [FIRST_TIME], 0.92, True),
    (SECOND + [SECOND_TIME], 0.92, True),
    (FIRST, 0.75, False),
    (SECOND + [FIRST_TIME], 1.0, True),
    (SECOND + [SECOND_TIME], 1.0, True),
    (FIRST, 0.95, True),
]


# --- The band ---------------------------------------------------------------

def envelope(n, attack, release):
    env = np.ones(n)
    a = min(n, max(1, int(attack * RATE)))
    r = min(n - a, max(1, int(release * RATE)))
    env[:a] = np.linspace(0, 1, a) ** 1.5
    if r > 0:
        env[n - r:] *= np.linspace(1, 0, r) ** 2
    return env


def voice(f, dur, kind):
    """One note of one instrument, as a sum of partials below Nyquist."""
    n = int(dur * RATE)
    if n < 8:
        return np.zeros(0)
    t = np.arange(n) / RATE
    out = np.zeros(n)
    if kind == "cornet":
        # Bright brass: the upper partials bloom a little after the attack.
        bloom = 1 - np.exp(-t * 18)
        vib = 1 + 0.0025 * np.sin(2 * math.pi * 5.2 * t) * (1 - np.exp(-t * 3))
        for k in range(1, 16):
            if f * k > NYQ * 0.92:
                break
            amp = k ** -0.9 * (1.0 if k < 3 else 0.35 + 0.65 * bloom)
            out += amp * np.sin(2 * math.pi * f * k * np.cumsum(vib) / RATE)
        env = envelope(n, 0.025, 0.06)
    elif kind == "clarinet":
        for k in range(1, 16, 2):
            if f * k > NYQ * 0.92:
                break
            out += k ** -1.1 * np.sin(2 * math.pi * f * k * t +
                                      RNG.uniform(0, 6.3))
        out += 0.08 * np.sin(2 * math.pi * f * 2 * t)
        env = envelope(n, 0.03, 0.05)
    elif kind == "piccolo":
        out = np.sin(2 * math.pi * f * t) + 0.12 * np.sin(4 * math.pi * f * t)
        out += 0.03 * RNG.standard_normal(n)              # the breath
        env = envelope(n, 0.02, 0.04)
    elif kind == "horn":
        for k, amp in ((1, 1.0), (2, 0.5), (3, 0.28), (4, 0.12), (5, 0.06)):
            out += amp * np.sin(2 * math.pi * f * k * t)
        env = envelope(n, 0.035, 0.07)
    elif kind == "bass":
        for k, amp in ((1, 1.0), (2, 0.7), (3, 0.38), (4, 0.2), (5, 0.1),
                       (6, 0.05)):
            out += amp * np.sin(2 * math.pi * f * k * t)
        env = envelope(n, 0.03, 0.08) * np.exp(-t * 1.2)
    else:
        raise ValueError(kind)
    return out * env


def bass_drum(n_out):
    n = int(0.45 * RATE)
    t = np.arange(n) / RATE
    body = np.sin(2 * math.pi * (52 * t + 18 * (1 - np.exp(-t * 30)) / 30))
    hit = RNG.standard_normal(n) * np.exp(-t * 60) * 0.3
    return (body * np.exp(-t * 7) + hit)[:n_out]


def cymbal():
    n = int(0.9 * RATE)
    t = np.arange(n) / RATE
    x = RNG.standard_normal(n)
    x = np.diff(x, prepend=0)                     # the brassy top
    return x * np.exp(-t * 5.5) * 0.22


def side_drum():
    n = int(0.16 * RATE)
    t = np.arange(n) / RATE
    snare = RNG.standard_normal(n) * np.exp(-t * 32)
    shell = np.sin(2 * math.pi * 190 * t) * np.exp(-t * 45)
    return (snare * 0.55 + shell * 0.45) * 0.5


def play(mix, start, sound, gain):
    s = max(0, int(start * RATE + RNG.normal(0, 0.005) * RATE))
    end = min(len(mix), s + len(sound))
    if end > s:
        mix[s:end] += sound[:end - s] * gain


def band(total):
    mix = np.zeros(total)
    t0 = RUN_IN
    bars = [(bar, loud, full) for part, loud, full in SIDE for bar in part]
    for i, ((tune, harmony), loud, full) in enumerate(bars):
        last = i == len(bars) - 1
        # The tune: cornets and clarinets together, the piccolo an octave up
        # on the loud strains when it is not too high to sing.
        t = t0
        for name, beats in tune:
            if name != "r":
                m = midi(name)
                dur = beats * BEAT * (0.92 if beats >= 2 else 0.78)
                if last:
                    dur = 2.2
                cornet = m - 12 if m > midi("B5") else m
                play(mix, t, voice(hz(cornet), dur, "cornet"), 0.30 * loud)
                play(mix, t, voice(hz(cornet) * 1.003, dur, "clarinet"),
                     0.16 * loud)
                if full and m + 12 <= midi("D7") and m >= midi("D5"):
                    play(mix, t, voice(hz(m + 12) if m < midi("A5")
                                       else hz(m), dur, "piccolo"),
                         0.09 * loud)
            t += beats * BEAT
        # The harmony: bombardon on the beats, horns off them.
        for beat in range(4):
            chord = harmony[0] if len(harmony) == 1 or beat < 2 else harmony[1]
            root, fifth, notes = CHORDS[chord]
            when = t0 + beat * BEAT
            if last and beat > 0:
                break
            if beat % 2 == 0:
                b = root if beat == 0 else fifth
                play(mix, when, voice(hz(midi(b)), 2.4 if last
                                      else BEAT * 0.8, "bass"), 0.34 * loud)
                if full or beat == 0:
                    play(mix, when, bass_drum(len(mix)), 0.30 * loud)
                    play(mix, when, cymbal(), 0.55 * loud if full else 0.3)
            else:
                for nn in notes:
                    play(mix, when, voice(hz(midi(nn)), BEAT * 0.55, "horn"),
                         0.085 * loud)
                if full:
                    play(mix, when, side_drum(), 0.32 * loud)
            if last:
                for nn in notes:
                    play(mix, when, voice(hz(midi(nn)), 2.2, "horn"),
                         0.10 * loud)
        t0 += BAR
    return mix


# --- The record -------------------------------------------------------------

def horn_and_band(x):
    """The acoustic horn: nothing much below 200 Hz or above 3.3 kHz, and
    the horn's own resonances standing up through the middle."""
    n = len(x)
    f = np.fft.rfftfreq(n, 1 / RATE)
    g = 1 / (1 + (220 / np.maximum(f, 1)) ** 6) / (1 + (f / 3300) ** 8)
    for centre, width, boost in ((540, 90, 1.9), (1080, 160, 1.7),
                                 (1750, 220, 1.5), (2550, 260, 1.35)):
        g *= 1 + (boost - 1) * np.exp(-0.5 * ((f - centre) / width) ** 2)
    return np.fft.irfft(np.fft.rfft(x) * g, n)


def wow_and_flutter(x):
    """78 turns a minute, a slightly eccentric hole, a governor that hunts."""
    n = len(x)
    t = np.arange(n) / RATE
    drift = (0.0022 * np.sin(2 * math.pi * (78 / 60) * t) +
             0.0006 * np.sin(2 * math.pi * 6.3 * t + 1.0))
    where = t + np.cumsum(drift) / RATE
    return np.interp(where * RATE, np.arange(n), x)


def shellac(n):
    """The surface: a soft hiss in the horn's band, crackle, and the tick
    of a scratch once a turn."""
    t = np.arange(n) / RATE
    f = np.fft.rfftfreq(n, 1 / RATE)
    g = 1 / (1 + (900 / np.maximum(f, 1)) ** 2) / (1 + (f / 4200) ** 6)
    hiss = np.fft.irfft(np.fft.rfft(RNG.standard_normal(n)) * g, n)
    hiss *= 0.012 / np.std(hiss)
    hiss *= 1 + 0.25 * np.sin(2 * math.pi * (78 / 60) * t)
    crackle = np.zeros(n)
    count = int(n / RATE * 9)
    for pos in RNG.integers(0, n - 64, count):
        size = RNG.exponential(0.035) * RNG.choice((-1, 1))
        crackle[pos:pos + 6] += size * np.exp(-np.arange(6) / 1.5)
    turn = int(RATE * 60 / 78)
    for pos in range(int(RNG.uniform(0, turn)), n - 64, turn):
        crackle[pos:pos + 10] += 0.05 * np.exp(-np.arange(10) / 2.5)
    return hiss + crackle


def main():
    out = Path(sys.argv[1]) if len(sys.argv) > 1 else OUT
    bars = sum(len(part) for part, _, _ in SIDE)
    total = int((RUN_IN + bars * BAR + RUN_OUT) * RATE)
    music = band(total)
    music = np.tanh(music * 1.6) / 1.6            # the cutting stylus
    music = horn_and_band(music)
    music /= np.sqrt(np.mean(music[int(RUN_IN * RATE):] ** 2)) / 0.16
    # The music fades in from the run-in groove and dies into the run-out,
    # so the start and the end of the file are only the needle.
    edge = np.ones(total)
    a = int(RUN_IN * RATE)
    edge[:a] = 0
    edge[a:a + 64] = np.linspace(0, 1, 64)
    tail = int((RUN_IN + bars * BAR + RUN_OUT - 0.35) * RATE)
    edge[tail - 2200:tail] *= np.linspace(1, 0, 2200) ** 2
    edge[tail:] = 0
    music *= edge
    record = wow_and_flutter(music) + shellac(total)
    record = np.clip(record, -0.97, 0.97)
    pcm = np.round(record * 32767).astype("<i2")
    with wave.open(str(out), "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(RATE)
        w.writeframes(pcm.tobytes())
    print(f"{out}: {total / RATE:.1f} s, {bars} bars")


if __name__ == "__main__":
    main()
