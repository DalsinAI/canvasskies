#!/usr/bin/env python3
"""make_airfield_films.py FRAMES_DIR SOUND_DIR: pack the aerodrome films.

The eight take-off and landing films are our own, rendered on the host by
ports/amigaos3/films/airfield.py (the pictures, finished as old newsreel,
kept as grey frames at 24 a second in FRAMES_DIR/<film>.npy) and
ports/amigaos3/films/sound.py (their soundtracks, SOUND_DIR/<film>.wav,
11025 Hz mono 16-bit).  Each film is scaled to the picture size below,
toned to sixteen greys for the picture-house lamp table (the game shows
level x as 14 + 241 * (x / 15) ^ 0.8 before its sepia tint), with an
ordered dither that holds still from frame to frame, and stored as the
newsreels are: a key frame and then, for each later frame, the 8x8 blocks
that changed.  The game loads one film at a time from
assets/canvas-skies/airfield-films.bin, all numbers big-endian:

  header, 16 bytes:
    "CSAF", u16 version (1), u16 width, u16 height, u16 frames a second,
    u16 sound samples a second, u16 count
  then per film, 56 bytes:
    u32 picture offset, u32 picture length,
    u32 sound offset, u32 sound length in bytes, 40-byte title
  the picture at its offset, exactly as a newsreel's:
    u16 frames, then frame 0 as 4-bit pixels (width * height / 2 bytes,
    two pixels a byte, the left one in the high nibble), and for each
    later frame a map of its (width / 8) * (height / 8) blocks, a bit
    each, most significant first, rounded up to whole bytes, followed by
    32 bytes (8 rows of 4) for each block whose bit is set
  the sound at its offset:
    signed 16-bit mono samples, big-endian (as the 68040 holds them), at
    the rate in the header; it runs exactly frames / fps seconds, the
    first sample with the first frame.

  python3 make_airfield_films.py out/master/frames out/sound [--size 320x200]
      [--fps 12] [--out FILE]
"""

import argparse
import struct
import sys
import wave
from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "canvas-skies" / "airfield-films.bin"
MASTER_FPS = 24
RATE = 11025
BAYER = np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9],
                  [15, 7, 13, 5]], np.float32) / 16.0 - 0.5
DRIFT = 0.30         # a block is sent again when its mean level has drifted
SHARP = 4            # ... or this many of its pixels moved 3 levels or more
THRESHOLD = 1.3      # ... or its pixels moved this much on average

FILMS = [
    ("takeoff-1-dawn-prop-swing", "DAWN: SWINGING THE AIRSCREW"),
    ("takeoff-2-flight-of-three", "A FLIGHT OF THREE GOES UP"),
    ("takeoff-3-muddy-field", "OFF A FIELD OF MUD"),
    ("takeoff-4-1914-observer-climbs-in", "1914: THE OBSERVER CLIMBS IN"),
    ("landing-1-dusk-three-point", "HOME AT DUSK"),
    ("landing-2-shot-up", "SHOT ABOUT, BUT HOME"),
    ("landing-3-dead-stick", "ENGINE GONE: A DEAD-STICK LANDING"),
    ("landing-4-rain", "LANDING IN THE RAIN"),
]


def levels(grey, w, h):
    """A master frame (0..255 grey) to sixteen dithered levels, so that the
    game's lamp table shows it as near as may be to the master."""
    im = Image.fromarray(grey).resize((w, h), Image.LANCZOS)
    a = np.asarray(im, np.float32)
    x = 15.0 * np.clip((a - 14.0) / 241.0, 0, 1) ** (1 / 0.8)
    d = np.tile(BAYER, (h // 4 + 1, w // 4 + 1))[:h, :w]
    return np.clip(np.floor(x + 0.5 + d), 0, 15).astype(np.uint8)


def pack(block):
    flat = block.reshape(-1)
    return bytes((flat[0::2] << 4) | flat[1::2])


def encode(frames, w, h):
    bw, bh = w // 8, h // 8
    mapsize = (bw * bh + 7) // 8
    out = bytearray(struct.pack(">H", len(frames)))
    shown = frames[0].copy()
    out += pack(shown)
    for f in frames[1:]:
        bits = bytearray(mapsize)
        blocks = bytearray()
        cur = f.reshape(bh, 8, bw, 8).astype(np.int16)
        old = shown.reshape(bh, 8, bw, 8).astype(np.int16)
        # The grain is as often up as down, so it does not shift a block's
        # mean; a slow change of light does, and left alone it would show
        # as a patchwork of blocks a level apart.
        signed = cur - old
        diff = np.abs(signed)
        drift = np.abs(signed.mean(axis=(1, 3)))
        mean = diff.mean(axis=(1, 3))
        sharp = (diff >= 3).sum(axis=(1, 3))
        change = (drift >= DRIFT) | (sharp >= SHARP) | (mean >= THRESHOLD)
        for by, bx in zip(*np.nonzero(change)):
            k = by * bw + bx
            bits[k >> 3] |= 0x80 >> (k & 7)
            blk = f[by * 8:by * 8 + 8, bx * 8:bx * 8 + 8]
            blocks += pack(blk)
            shown[by * 8:by * 8 + 8, bx * 8:bx * 8 + 8] = blk
        out += bits + blocks
    return bytes(out)


def read_wav(path):
    with wave.open(str(path)) as wv:
        assert wv.getnchannels() == 1 and wv.getsampwidth() == 2
        assert wv.getframerate() == RATE
        return np.frombuffer(wv.readframes(wv.getnframes()), "<i2")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("frames")
    ap.add_argument("sound")
    ap.add_argument("--size", default="320x200")
    ap.add_argument("--fps", type=int, default=12)
    ap.add_argument("--out", default=str(OUT))
    args = ap.parse_args()
    w, h = (int(v) for v in args.size.split("x"))
    assert w % 8 == 0 and h % 8 == 0
    step = MASTER_FPS // args.fps
    assert step * args.fps == MASTER_FPS
    films = []
    for name, title in FILMS:
        master = np.load(Path(args.frames) / f"{name}.npy")
        frames = [levels(g, w, h) for g in master[::step]]
        pic = encode(frames, w, h)
        pcm = read_wav(Path(args.sound) / f"{name}.wav")
        n = len(frames) * RATE // args.fps
        pcm = np.pad(pcm, (0, max(0, n - len(pcm))))[:n]
        snd = pcm.astype(">i2").tobytes()
        films.append((title, pic, snd))
        ram = w * h + len(pic) + len(snd)
        print(f"{name:36} {len(frames):3} frames  picture {len(pic) // 1024:5} KB  "
              f"sound {len(snd) // 1024:4} KB  in RAM {ram // 1024:5} KB", flush=True)
    head = bytearray(b"CSAF" + struct.pack(">HHHHHH", 1, w, h, args.fps, RATE, len(films)))
    offset = len(head) + len(films) * 56
    body = bytearray()
    for title, pic, snd in films:
        name = title.encode("ascii", "replace")[:39].ljust(40, b"\0")
        po = offset + len(body)
        body += pic
        so = offset + len(body)
        body += snd
        head += struct.pack(">IIII", po, len(pic), so, len(snd)) + name
    Path(args.out).write_bytes(bytes(head + body))
    print(f"{Path(args.out).name}: {(len(head) + len(body)) // 1024} KB", file=sys.stderr)


if __name__ == "__main__":
    main()
