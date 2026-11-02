#!/usr/bin/env python3
"""Build original Canvas Skies GlowIcons with AmiCradle's MIT icon writer."""

from __future__ import annotations

import argparse
import importlib.util
import sys
from pathlib import Path

from PIL import Image, ImageDraw, ImageFilter

SIZE = 64
SCALE = 4
HI = SIZE * SCALE
HERE = Path(__file__).resolve().parent


def load_writer(path: Path):
    spec = importlib.util.spec_from_file_location("amigaicon", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"could not load icon writer: {path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def plane(draw: ImageDraw.ImageDraw, ox: int, oy: int, selected: bool) -> None:
    gold = (245, 204, 113, 255) if selected else (218, 174, 85, 255)
    dark = (26, 24, 25, 255)
    draw.rounded_rectangle((ox, oy + 8, ox + 37, oy + 14), radius=3,
                           fill=(73, 86, 56, 255), outline=dark, width=2)
    draw.polygon([(ox + 6, oy + 3), (ox + 35, oy + 1),
                  (ox + 37, oy + 6), (ox + 8, oy + 8)], fill=gold, outline=dark)
    draw.polygon([(ox + 5, oy + 14), (ox + 35, oy + 16),
                  (ox + 34, oy + 21), (ox + 8, oy + 19)], fill=gold, outline=dark)
    draw.polygon([(ox, oy + 5), (ox + 6, oy + 9), (ox + 4, oy + 16),
                  (ox - 3, oy + 15)], fill=(48, 69, 45, 255), outline=dark)
    draw.ellipse((ox + 37, oy + 7, ox + 45, oy + 15), fill=dark)
    draw.line((ox + 13, oy + 6, ox + 13, oy + 17), fill=dark, width=2)
    draw.line((ox + 30, oy + 5, ox + 30, oy + 17), fill=dark, width=2)


def start_art(selected: bool) -> Image.Image:
    img = Image.new("RGBA", (HI, HI), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img, "RGBA")
    glow = Image.new("RGBA", img.size, (0, 0, 0, 0))
    gd = ImageDraw.Draw(glow, "RGBA")
    gd.ellipse((32, 25, 226, 219), fill=(255, 174, 55, 120 if selected else 65))
    img.alpha_composite(glow.filter(ImageFilter.GaussianBlur(20)))
    draw.ellipse((35, 29, 221, 215), fill=(25, 42, 62, 255),
                 outline=(236, 192, 105, 255), width=7)
    for y in range(42, 177):
        t = (y - 42) / 135.0
        colour = (int(25 + 140 * t), int(43 + 88 * t), int(66 + 46 * t), 255)
        draw.line((49, y, 207, y), fill=colour, width=1)
    draw.ellipse((154, 59, 185, 90), fill=(255, 204, 91, 245))
    draw.polygon([(48, 174), (87, 139), (124, 159), (164, 126),
                  (208, 173)], fill=(29, 42, 39, 255))
    plane(draw, 76, 102, selected)
    draw.rounded_rectangle((66, 190, 194, 224), radius=9,
                           fill=(14, 19, 27, 244), outline=(228, 187, 93, 255), width=4)
    for x in (83, 99, 115, 139, 155, 171):
        draw.rectangle((x, 201, x + 7, 213), fill=(242, 216, 154, 255))
    return img.resize((SIZE, SIZE), Image.Resampling.LANCZOS)


def drawer_art(selected: bool) -> Image.Image:
    img = Image.new("RGBA", (HI, HI), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img, "RGBA")
    glow = Image.new("RGBA", img.size, (0, 0, 0, 0))
    gd = ImageDraw.Draw(glow, "RGBA")
    gd.rounded_rectangle((22, 70, 234, 226), radius=24,
                         fill=(255, 170, 55, 105 if selected else 48))
    img.alpha_composite(glow.filter(ImageFilter.GaussianBlur(18)))
    draw.polygon([(22, 76), (88, 76), (108, 98), (234, 98),
                  (219, 218), (34, 218)], fill=(123, 77, 37, 255),
                 outline=(238, 189, 91, 255))
    draw.polygon([(31, 108), (226, 108), (211, 207), (42, 207)],
                 fill=(24, 45, 66, 255))
    draw.ellipse((157, 121, 191, 155), fill=(250, 196, 82, 245))
    draw.polygon([(40, 190), (85, 153), (122, 177), (168, 146),
                  (214, 190)], fill=(36, 53, 42, 255))
    plane(draw, 73, 132, selected)
    return img.resize((SIZE, SIZE), Image.Resampling.LANCZOS)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--writer", type=Path, required=True,
                        help="path to AmiCradle tools/icons/amigaicon.py")
    parser.add_argument("--preview", type=Path)
    args = parser.parse_args()
    ai = load_writer(args.writer)
    out = HERE / "package"
    out.mkdir(exist_ok=True)

    start = ai.Icon(type=ai.WBPROJECT, width=SIZE, height=SIZE,
                    default_tool="C:IconX", tool_types=["DONOTWAIT"],
                    stack=262144, normal=start_art(False), selected=start_art(True))
    drawer = ai.Icon(type=ai.WBDRAWER, width=SIZE, height=SIZE,
                     normal=drawer_art(False), selected=drawer_art(True))
    (out / "Start Canvas Skies.info").write_bytes(ai.glowicon(start))
    (out / "Drawer.info").write_bytes(ai.glowicon(drawer))

    for icon, expected in ((start, ai.WBPROJECT), (drawer, ai.WBDRAWER)):
        data = ai.read(ai.glowicon(icon))
        assert data["type"] == expected and data["face"] == (SIZE, SIZE)
    if args.preview:
        preview = Image.new("RGBA", (SIZE * 4, SIZE), (170, 170, 170, 255))
        for n, art in enumerate((drawer.normal, drawer.selected,
                                 start.normal, start.selected)):
            preview.alpha_composite(art, (n * SIZE, 0))
        preview.save(args.preview)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
