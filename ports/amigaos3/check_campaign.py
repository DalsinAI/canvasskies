#!/usr/bin/env python3
"""Check the campaign table before the game is packaged.

Every chapter's twelve missions must be four side-on, four top-down, two
cockpit dogfights and two from the rear gunner's seat (see the comment on
campaign_missions in src/modern/canvas_campaign.h).  This reads the table
from the header (or the file named on the command line) and fails,
naming the chapter, if any chapter is off.

It also checks the side-on orders against what the sorties offer: the
order must not exceed the gun pits laid out (side_layout) and an armed
sortie must carry the order and two bombs more.  The rules here mirror
current_mission_goal and side_layout in canvas_skies.c; the game's own
GOALS test entry checks every flat sortie, top-down too, as laid out.
"""

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HEADER = ROOT / "src" / "modern" / "canvas_campaign.h"
SOURCE = ROOT / "src" / "modern" / "canvas_skies.c"
WANT = {"MISSION_SIDE": 4, "MISSION_TOP_DOWN": 4,
        "MISSION_DOGFIGHT_3D": 2, "MISSION_GUNNER_3D": 2}
CHAPTERS, PER_CHAPTER = 8, 12


def chapters(text):
    """The modes of each chapter's missions, in order."""
    start = text.index("campaign_missions[")
    body = text[text.index("{", start) + 1:]
    found, depth, current = [], 0, None
    for token in re.finditer(r"\{|\}|MISSION_[A-Z_0-9]+", body):
        t = token.group(0)
        if t == "{":
            depth += 1
            if depth == 1:
                current = []
        elif t == "}":
            if depth == 1:
                found.append(current)
            elif depth == 0:
                break
            depth -= 1
        elif depth == 2 and current is not None:
            current.append(t)
    return found


def side_orders(source):
    """Problems with the side-on orders, from the chapters table."""
    rows = re.findall(r"\}, (\d+), (\d+), (\d+), (\d+), [\d.]+f\}", source)
    bad = []
    for ch, (goal, targets, _enemies, armed) in enumerate(rows):
        goal, targets, armed = int(goal), int(targets), int(armed)
        bombs = max(12 - ch // 2, 8)
        if ch == 0:
            order, offered = 3, 4          # four tasks, three to do
        else:
            order = goal + 1
            first = max(order - 1, 1)
            offered = min(max(targets, first + 3), 14)
            bombs = max(bombs, order + 2)
        if order > offered:
            bad.append(f"chapter {ch + 1}: side-on order {order} but only "
                       f"{offered} targets")
        if armed and bombs < order + 2:
            bad.append(f"chapter {ch + 1}: side-on order {order} but only "
                       f"{bombs} bombs")
    if len(rows) != CHAPTERS:
        bad.append(f"chapters table: {len(rows)} rows, not {CHAPTERS}")
    return bad


def main():
    text = (Path(sys.argv[1]) if len(sys.argv) > 1 else HEADER).read_text()
    table = chapters(text)
    bad = []
    if len(table) != CHAPTERS:
        bad.append(f"{len(table)} chapters, not {CHAPTERS}")
    for n, modes in enumerate(table, 1):
        counts = {m: modes.count(m) for m in WANT}
        if len(modes) != PER_CHAPTER or counts != WANT:
            bad.append(f"chapter {n}: {len(modes)} missions, " +
                       ", ".join(f"{m[8:].lower()} {counts[m]}" for m in WANT))
    bad += side_orders(SOURCE.read_text())
    if bad:
        print("check_campaign: the campaign table is not as it should be:")
        for line in bad:
            print("  " + line)
        return 1
    print(f"check_campaign: {len(table)} chapters, each 4 side-on, "
          "4 top-down, 2 dogfight, 2 rear-seat; side-on orders within "
          "the targets and bombs on offer")
    return 0


if __name__ == "__main__":
    sys.exit(main())
