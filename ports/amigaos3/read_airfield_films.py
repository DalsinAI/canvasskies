#!/usr/bin/env python3
"""read_airfield_films.py [BIN] OUTDIR: play back airfield-films.bin.

Reads the file written by make_airfield_films.py exactly as the game must
(the format is set out at the head of that script), and for each film
writes what the screen will show: the sixteen greys through the picture
house's lamp table and sepia tint, doubled (or as near as whole pixels
allow) to fill 640x400, as OUTDIR/<n>-<title>.webp, with the sound read
back out of the file as OUTDIR/<n>-<title>.wav and a strip of key frames
as OUTDIR/<n>-<title>-keys.png.
"""

import struct
import sys
import wave
from pathlib import Path

import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]


def lamp_table():
    """The game's lamp table (draw_cinema), without its flicker."""
    x = np.arange(16)
    v = 14 + (241.0 * (x / 15.0) ** 0.8).astype(int)
    return np.stack([v * 236 // 240, v * 214 // 240, v * 172 // 240], -1).astype(np.uint8)


class Films:
    def __init__(self, path):
        self.data = Path(path).read_bytes()
        d = self.data
        assert d[:4] == b"CSAF", "not an airfield films file"
        (self.version, self.w, self.h, self.fps, self.rate,
         self.count) = struct.unpack(">HHHHHH", d[4:16])
        self.entries = []
        for k in range(self.count):
            e = d[16 + k * 56:16 + (k + 1) * 56]
            po, pl, so, sl = struct.unpack(">IIII", e[:16])
            title = e[16:56].split(b"\0")[0].decode("ascii")
            self.entries.append((po, pl, so, sl, title))

    def frames(self, k):
        """Yields each frame of film k as levels 0..15, as the game sees it."""
        po, pl, _, _, _ = self.entries[k]
        d = self.data[po:po + pl]
        w, h = self.w, self.h
        bw, bh = w // 8, h // 8
        mapsize = (bw * bh + 7) // 8
        n = struct.unpack(">H", d[:2])[0]
        cur = 2
        raw = np.frombuffer(d[cur:cur + w * h // 2], np.uint8)
        pix = np.empty(w * h, np.uint8)
        pix[0::2] = raw >> 4
        pix[1::2] = raw & 15
        pix = pix.reshape(h, w)
        cur += w * h // 2
        yield pix.copy()
        for _ in range(n - 1):
            bits = d[cur:cur + mapsize]
            cur += mapsize
            for k_ in range(bw * bh):
                if bits[k_ >> 3] & (0x80 >> (k_ & 7)):
                    b = np.frombuffer(d[cur:cur + 32], np.uint8)
                    cur += 32
                    blk = np.empty(64, np.uint8)
                    blk[0::2] = b >> 4
                    blk[1::2] = b & 15
                    by, bx = (k_ // bw) * 8, (k_ % bw) * 8
                    pix[by:by + 8, bx:bx + 8] = blk.reshape(8, 8)
            yield pix.copy()
        assert cur == len(d), "picture data not used up exactly"

    def sound(self, k):
        _, _, so, sl, _ = self.entries[k]
        return np.frombuffer(self.data[so:so + sl], ">i2").astype(np.int16)


def screen(pix, lut):
    """The frame as shown full-screen: whole-pixel scaling to 640x400."""
    h, w = pix.shape
    ys = (np.arange(400) * h // 400)
    xs = (np.arange(640) * w // 640)
    return Image.fromarray(lut[pix[ys][:, xs]])


def main():
    args = sys.argv[1:]
    src = Path(args[0]) if len(args) > 1 else ROOT / "assets" / "canvas-skies" / "airfield-films.bin"
    out = Path(args[-1])
    out.mkdir(parents=True, exist_ok=True)
    films = Films(src)
    lut = lamp_table()
    print(f"{films.count} films, {films.w}x{films.h} at {films.fps} a second, "
          f"sound at {films.rate}")
    for k, (po, pl, so, sl, title) in enumerate(films.entries):
        shots = [screen(p, lut) for p in films.frames(k)]
        name = f"{k + 1}-" + "".join(c if c.isalnum() else "-" for c in title.lower()).strip("-")
        while "--" in name:
            name = name.replace("--", "-")
        shots[0].save(out / f"{name}.webp", save_all=True, append_images=shots[1:],
                      duration=int(round(1000 / films.fps)), loop=0, quality=70, method=6)
        picks = [int(len(shots) * q) for q in (0.08, 0.3, 0.55, 0.8, 0.98)]
        sheet = Image.new("RGB", (5 * 320, 200))
        for i, p in enumerate(picks):
            sheet.paste(shots[min(p, len(shots) - 1)].resize((320, 200), Image.LANCZOS), (i * 320, 0))
        sheet.save(out / f"{name}-keys.png", optimize=True)
        snd = films.sound(k)
        with wave.open(str(out / f"{name}.wav"), "wb") as wv:
            wv.setnchannels(1)
            wv.setsampwidth(2)
            wv.setframerate(films.rate)
            wv.writeframes(snd.astype("<i2").tobytes())
        secs = len(shots) / films.fps
        ram = films.w * films.h + pl + sl
        print(f"{k + 1}. {title:36} {len(shots):3} frames {secs:5.2f} s  "
              f"sound {len(snd) / films.rate:5.2f} s  file {pl + sl:8} bytes  RAM {ram:8} bytes  "
              f"webp {(out / (name + '.webp')).stat().st_size // 1024} KB")


if __name__ == "__main__":
    main()
