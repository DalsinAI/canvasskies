# Canvas Skies 0.2.0 RC6: validation receipt

Date: 9 November 2026

This is the record of what RC6 is and how it was checked. It is kept out of the
release archive because it records the archive's own checksum.

## What was built

| | |
|---|---|
| Source | `release/rc6` at `8e198ae`: `main` at `92a79da` (round 6, PRs #20 to #32), the QA pilot from #34, and the RC6 text in `README.md`, the drawer's `ReadMe` and `docs/CANVAS_SKIES_STATUS.md` |
| Compiler | the Open6 stove, GFOpenAmigaGCC 16.2.0-open6 (`os32-gcc16`), on daletop |
| Build | `make -f ports/amigaos3/Makefile.canvas AMIGA_STOVE=<stove> package`: no warnings, campaign check and package link check pass |
| Archive | `CanvasSkies-0.2.0-rc6.lha`, 59,519,850 bytes, the `CanvasSkies` drawer and its icon |
| Archive SHA-256 | `ec9c9051f9f2862ccdf58e4917cc5f289de6a4723d746e42c2888f46cfbeacdb` |
| Program SHA-256 | `7e4f08d5445362251a21e064d3a337b6fc6c46f2e2999f97e1e1eb6b37350909` |

This is the second RC6 build. The first (`46c8a1d`, program `9e66a9c4...`,
archive `fd7000ac...`) was tested in full below. Three sorties in it missed
their orders or ran past the lab's time limit; all three were the QA
autopilot, not the game (see "The rebuild"), and #34 gave the autopilot a
better pilot. Only the autopilot and the sortie log's `hits=` and `wall=`
fields differ between the two programs; no order, enemy or rule changed.

## What RC6 needs

The Open6 build loads `LIBS:GFOpenGPU/SDL2.module` and `LIBS:gfopengpu.library`,
GFOpenUp's GFOpenGPU part. RC5, built with Open4, loaded `LIBS:OpenGPU/`. On a
clean copy of the GFOS instance without the part, RC6 stopped at once with "this
program needs LIBS:GFOpenGPU/SDL2.module (GFOpenGPU), which isn't installed".
The ReadMe and README say so. Every test below had GFOpenUp 0.8.26's GFOpenGPU
part installed with `GFOpenUpTool INSTALL`.

## How it was tested

Lab copies of the live instances, on origimagic: Lab-50 (AmigaOS 3.2.3,
800x600 Workbench) and Lab-51 (GFOS, 1600x900). The game folder and its saves
were deleted and RC6 unpacked fresh from the archive. Sorties were flown by the
game's own autopilot (`RUN FLY`), which writes one line per sortie to
`RAM:canvas-sortie.log`; scenes and key tests were driven by key presses with
screenshots, all of which were looked at.

### Sorties

| Lab | Sortie | Result |
|---|---|---|
| 50 | 1914 side-on, mission 5 | success, 4/3 orders, 266 s |
| 50 | 1918 rear seat, Lewis gun (`GUNNER`) | success, 4 kills, 195 s |
| 50 | 1915 rear seat, Lewis gun ("Tom's Lewis Gun") | success, 4 kills, 161 s |
| 50 | 1914 rear seat, carbine | still flying at 200 s, no log line; screenshots normal |
| 50 | 1918 cockpit dogfight (`DOGFIGHT 6`) | failed, 0/4 kills (the cockpit autopilot rarely downs anything) |
| 50 | 1914 cockpit, "Eyes Above the Marne" | success, observer's tasks 4/3, 203 s |
| 51 | 1918 spring side-on (chapter 6, mission 1) | failed, 3/5 orders, 4 kills |
| 51 | 1915 top-down railway | success, 8/6 |
| 51 | top-down trench run (`TOPMIX 6`) | success, 14/9 |
| 51 | 1917 top-down, the base and ambulances | success, 4/4 |
| 51 | Christmas truce | success, pudding delivered, nobody fired, 188 s |
| 51 | 1914 cockpit, "Horses in the Sky" | success, observer's tasks 4/3, 184 s |
| 51 | slow-frames dogfight (`DOGFIGHT 3 SLOWFRAMES`) | still flying at 600 s, no log line |

The 1918 spring, 1915 railway, trench run and base sorties, and the slow-frames
dogfight, are the #27 runs a pause had cut off.

### The rebuild

The three misses above, investigated:

- The 1918 cockpit dogfight offers 9 enemies and ammunition for many more;
  the old autopilot aimed at where the enemy was, not where it would be, and
  never cleared a gun jam.
- The 1918 spring side-on offers 9 targets for 5 orders and 10 bombs; the old
  autopilot spent half its bombs on machine-gun posts that don't count.
- The slow-frames dogfight ends on the sortie clock, which advances at most
  50 ms a frame, so at about eight frames a second it runs about three times
  slower than real time. The sortie was longer than the lab's 600 s.

With the new pilot, every one of the 16 cockpit dogfights met its order on
three host clocks (48 of 48), and all 32 side-on sorties still do. The rebuilt
archive was installed clean on both labs again:

| Lab | Sortie | Result |
|---|---|---|
| 51 | 1918 cockpit dogfight (`DOGFIGHT 6`) | success, 9/4 kills, 132 shots, 54 hits |
| 51 | 1918 spring side-on | success, 9/5 orders, 9 of 10 bombs |
| 51 | Christmas truce | success, as before |
| 51 | slow-frames dogfight (`DOGFIGHT 3 SLOWFRAMES`) | success, 5/3 kills, 118 s of sortie in 370 s |
| 50 | 1918 cockpit, mission 12 | success, 9/4 kills |
| 50 | 1918 rear seat, Lewis gun | success, 4 kills |
| 50 | 1914 cockpit, "Horses in the Sky" | success, observer's tasks 4/3 |
| 50 | 1914 side-on, mission 5 | success, 4/3 |
| 50 | top-down trench run | success, 15/9 |

Full screen in, out, in and Escape to a clean Workbench, and the wake evening
in the mess, were checked again on both labs.

### Scenes, display and saves (both labs)

- The three mess evenings (`MESS`, `MESS QUIET`, `MESS WAKE`), We Remember,
  the muddy-field film and the Officer's Preferences: all drawn as expected.
- Full screen: from the title, F9 in, out, in again, then Escape: each switch
  clean and the game quit to an intact Workbench (AmigaOS 3.2 and GFOS).
- Wingmen and grounding: `WINGMEN 20 100 60`, then `QASAVE` Continue;
  `GROUNDED` to the memorandum, then `QASAVE` Continue: as expected on both.
- Old save: a real RC5 save (format 5, chapter 1, mission 10) from the RC5
  install on Instance-50 continued in RC6 on Lab-50 into that day's paper.

### The live instances

On 9 November, with the owner's word, Instances 50 and 51 on daletop were
given GFOpenUp 0.8.26's GFOpenGPU part (the 25 files `GFOpenUpTool INSTALL`
wrote on the lab copies) and RC6, the RC5 folder kept as
`Games/CanvasSkies-ca4e16c.bak` and the save carried across byte for byte. A
backup and `undo.sh` are in `~/AmiCradleInstances/backups-20261109-rc6/`.
Fresh lab copies of the updated instances booted straight into RC6 from
WBStartup, switched full screen in and out and quit to a clean Workbench, and
Instance-50's own RC5 save continued.

They run the first build. Bringing them to this one waits for the owner's word.

## Not tested

- A play-through by a person, difficulty, and the joystick.
- Frame times on a real 68040 or PiStorm (the lab is faster).
- Listening: the film soundtracks, including the three re-rendered in round 6,
  and the game by ear for ticks.
- Memory in use with the Lewis gun's new sheet (calculated, not measured).
- Any GFOpenUp before 0.8.26.
- A real 68040 drawing under 20 frames a second plays in slow motion, as the
  slow-frames run shows; whether one does has not been measured.
