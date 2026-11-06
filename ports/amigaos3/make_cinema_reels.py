#!/usr/bin/env python3
"""make_cinema_reels.py CUTS.json SOURCE_DIR: cut the picture-house reels.

Each cut is a few seconds of public-domain film (see the newsreel section
of CANVAS_SKIES_SOURCES.md).  It is cropped to 16:10, scaled to 192x120,
toned to sixteen greys with an ordered dither (stable from frame to frame,
so little changes between them), and stored at 8 frames a second as a key
frame followed by 8x8 blocks that changed.  The game reads one reel at a
time from assets/canvas-skies/newsreels.bin:

  "CSNR" u16 count, then per reel: u32 offset, u32 length, 40-byte title
  per reel at its offset: u16 frames, then frame 0 as 4-bit pixels
  (11520 bytes) and for each later frame a 45-byte map of the 360 blocks
  followed by 32 bytes for each block whose bit is set.

CUTS.json: [{"source": "2", "start": 812.0, "seconds": 5, "title": "..."}]
with sources named <n>.<ext> in SOURCE_DIR.  Needs PyAV.
"""

import json
import struct
import sys
from pathlib import Path

import av
import numpy as np
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "assets" / "canvas-skies" / "newsreels.bin"
W, H, FPS = 192, 120, 8
BAYER = np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9],
                  [15, 7, 13, 5]], np.float32) / 16.0 - 0.5
THRESHOLD = 1.3      # mean level change below which a block is kept


def frames_of(path, start, seconds):
    c = av.open(str(path))
    v = c.streams.video[0]
    c.seek(int(start / v.time_base), stream=v, backward=True)
    want, out = start, []
    for f in c.decode(v):
        t = float(f.pts * v.time_base)
        if t + 1e-3 < want:
            continue
        out.append(f.to_image())
        want += 1.0 / FPS
        if len(out) >= int(seconds * FPS):
            break
    c.close()
    return out


def picture_box(img):
    """The picture inside any black pillarbox or letterbox bars."""
    a = np.asarray(img.convert("L"), np.float32)
    cols = np.where(a.mean(axis=0) > 14)[0]
    rows = np.where(a.mean(axis=1) > 14)[0]
    if len(cols) < 16 or len(rows) < 16:
        return (0, 0, a.shape[1], a.shape[0])
    return (int(cols[0]) + 2, int(rows[0]) + 2, int(cols[-1]) - 1,
            int(rows[-1]) - 1)


def tone(img, box):
    g = img.convert("L").crop(box)
    w, h = g.size
    if w / h > W / H:                              # crop to 16:10
        nw = int(h * W / H)
        g = g.crop(((w - nw) // 2, 0, (w - nw) // 2 + nw, h))
    else:
        nh = int(w * H / W)
        g = g.crop((0, (h - nh) // 2, w, (h - nh) // 2 + nh))
    a = np.asarray(g.resize((W, H), Image.LANCZOS), np.float32) / 255.0
    lo, hi = np.percentile(a, 2), np.percentile(a, 98)
    a = np.clip((a - lo) / max(hi - lo, 0.05), 0, 1)
    d = np.tile(BAYER, (H // 4, W // 4))
    return np.clip(np.floor(a * 15.0 + 0.5 + d), 0, 15).astype(np.uint8)


def pack(block):
    flat = block.reshape(-1)
    return bytes((flat[0::2] << 4) | flat[1::2])


def encode(frames):
    out = bytearray(struct.pack(">H", len(frames)))
    shown = frames[0].copy()
    out += pack(shown)
    for f in frames[1:]:
        bits = bytearray(45)
        blocks = bytearray()
        for by in range(H // 8):
            for bx in range(W // 8):
                cur = f[by * 8:by * 8 + 8, bx * 8:bx * 8 + 8]
                old = shown[by * 8:by * 8 + 8, bx * 8:bx * 8 + 8]
                if np.abs(cur.astype(int) - old).mean() < THRESHOLD:
                    continue
                k = by * (W // 8) + bx
                bits[k >> 3] |= 0x80 >> (k & 7)
                blocks += pack(cur)
                shown[by * 8:by * 8 + 8, bx * 8:bx * 8 + 8] = cur
        out += bits + blocks
    return bytes(out)


def main():
    cuts = json.load(open(sys.argv[1]))
    src = Path(sys.argv[2])
    reels = []
    for cut in cuts:
        path = next(src.glob(cut["source"] + ".*"))
        raw = frames_of(path, cut["start"], cut["seconds"])
        box = picture_box(raw[len(raw) // 2])
        frames = [tone(im, box) for im in raw]
        data = encode(frames)
        reels.append((cut["title"], data))
        print(f'{cut["title"][:34]:34} {len(frames):3} frames {len(data) // 1024:4} KB')
    head = bytearray(b"CSNR" + struct.pack(">H", len(reels)))
    offset = len(head) + len(reels) * 48
    body = bytearray()
    for title, data in reels:
        name = title.encode("ascii", "replace")[:39].ljust(40, b"\0")
        head += struct.pack(">II", offset + len(body), len(data)) + name
        body += data
    OUT.write_bytes(bytes(head + body))
    print(f"{OUT.name}: {(len(head) + len(body)) // 1024} KB")


if __name__ == "__main__":
    main()
