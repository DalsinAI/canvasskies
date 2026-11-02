#!/usr/bin/env python3
"""Rasterise the two distributable period faces used by Canvas Skies."""

from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[2]
ASSETS = ROOT / "assets" / "canvas-skies"


def atlas(path: Path, font_path: str, font_size: int,
          cell_w: int, cell_h: int) -> None:
    image = Image.new("RGB", (cell_w * 16, cell_h * 4), "black")
    draw = ImageDraw.Draw(image)
    font = ImageFont.truetype(font_path, font_size)
    for code in range(32, 96):
        ch = chr(code)
        box = draw.textbbox((0, 0), ch, font=font)
        width, height = box[2] - box[0], box[3] - box[1]
        col, row = (code - 32) & 15, (code - 32) >> 4
        x = col * cell_w + (cell_w - width) // 2 - box[0]
        y = row * cell_h + (cell_h - height) // 2 - box[1]
        draw.text((x, y), ch, font=font, fill="white")
    image.save(path, format="BMP")


def main() -> None:
    serif = "/usr/share/fonts/truetype/noto/NotoSerifDisplay-Regular.ttf"
    journal = "/usr/share/fonts/truetype/noto/NotoSerif-Italic.ttf"
    mono = "/usr/share/fonts/truetype/liberation/LiberationMono-Bold.ttf"
    # Each atlas is rendered at its final on-screen cell size. The game never
    # rescales these glyphs, avoiding soft edges and broken thin strokes.
    for name, font_size, width, height in (
        ("font-display-36x42.bmp", 40, 36, 42),
        ("font-display-22x28.bmp", 27, 22, 28),
        ("font-display-16x23.bmp", 21, 16, 23),
        ("font-display-12x18.bmp", 16, 12, 18),
    ):
        atlas(ASSETS / name, serif, font_size, width, height)
    for name, font_size, width, height in (
        ("font-typewriter-10x15.bmp", 14, 10, 15),
        ("font-typewriter-7x11.bmp", 10, 7, 11),
    ):
        atlas(ASSETS / name, mono, font_size, width, height)
    atlas(ASSETS / "font-journal-8x12.bmp", journal, 11, 8, 12)


if __name__ == "__main__":
    main()
