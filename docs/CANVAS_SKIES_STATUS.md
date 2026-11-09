# Canvas Skies: implementation status and handoff

Status date: 9 November 2026

Branch: `main` (round 6), built as 0.2.0 RC6 from `release/rc6`

Code state: **0.2.0 RC6**, a release candidate prepared as a draft
pre-release, `CanvasSkies-0.2.0-rc6.lha`. The archive's SHA-256, the exact
commit and what was and was not tested are in the repository's
`docs/CANVAS_SKIES_RC6_VALIDATION.md` (kept out of the archive, since it
records the archive's own checksum).

Repository: `DalsinAI/canvasskies`

This file is the resumable handoff. Update it when a significant feature is
finished, a test result changes, or a new release blocker is found. Do not rely
on chat history as the only record of a decision.

## Current result

Canvas Skies is a complete eight-chapter, 96-mission campaign for AmigaOS 3 on a
68040, built on SDL2 and GFOpenUp's GFOpenGPU SDL2 runtime. Every scene has finished art,
the four mission views (side-on, top-down, cockpit dogfight and rear gunner)
all play through to a debrief, and the campaign saves, continues and ends at the
Armistice.

RC6 was built with the Open6 stove and installed fresh from its release
archive on two lab copies of the live instances, Lab-50 (AmigaOS 3.2.3) and
Lab-51 (GFOS), each with GFOpenUp 0.8.26's GFOpenGPU part. Every mission view,
the new round-6 work, the scenes, full screen, saves and grounding were run
there; the receipt lists each result. The live Instances 50 and 51 on daletop
run RC6 with the GFOpenGPU part.

A complete play-through by a person has not been done since round 5; the
mission tuning below was done with the game's own autopilot.

## What is in the repository

- Main implementation: `src/modern/canvas_skies.c`
- The 96-mission campaign: `src/modern/canvas_campaign.h`
- Generated tables: `src/modern/canvas_*.h` (fonts, sprites, carbine frames,
  riflemen), each written by its generator in `ports/amigaos3/`
- Art, sound and film assets: `assets/canvas-skies/`, all listed with their
  sources in `ART_PROVENANCE.md` and `MUSIC_LICENSE.md`
- Generators: `ports/amigaos3/make_*.py`, `scene3d.py`, `figurekit.py` and
  `films/` (the aerodrome films and their sound)
- Amiga build and release drawer: `ports/amigaos3/Makefile.canvas` and
  `ports/amigaos3/package/`
- Story and intended behaviour: `docs/CANVAS_SKIES_STORY_AND_DESIGN.md`
- Lab captures for every round: the `review/round-3-captures` branch, under
  `round-5/` for this release

## Build and direct tests

Build the release drawer from the repository root:

```sh
make -f ports/amigaos3/Makefile.canvas AMIGA_STOVE=<os32-gcc16 stove> package
```

It writes `build-canvas-skies/package/CanvasSkies` and its drawer icon. The
release archive also carries the drawer's `ReadMe` and
`CANVAS_SKIES_SOURCES.md`. The build targets AmigaOS 3, 68040 with 68881, with
OpenAmigaGCC's `os32-gcc16` stove and SDL2, and compiles with no warnings under
`-Wall -Wextra`. The internal framebuffer is 640 by 400, 16-bit.

Direct test entries (`CanvasSkies <ENTRY>`):

| Entry | Opens |
|---|---|
| `SIDE n`, `TOPDOWN`, `TOPMIX n`, `DOGFIGHT n`, `GUNNER`, `UNARMED`, `UNARMED_GUNNER` | a sortie of that kind |
| `MISSION c m` | any campaign sortie, numbered from 1 |
| `... RUN` | flies the sortie to its real end, keeping the airframe above 30 |
| `... FLY` | hands the controls to the autopilot |
| `... TIME` | shows the frame's drawing time |
| `TRUCE`, `AIRSHIP`, `GOTHA` | the three special sorties |
| `DAWN`, `MESS [QUIET\|WAKE]`, `CINEMA n`, `NEWS`, `DIARY`, `MACHINES n`, `AWARD n`, `LOSS` | the presentation scenes |
| `DEBRIEF`, `DEBRIEF_SUCCESS`, `CHAPTER_SUCCESS`, `FINAL_SUCCESS`, `HOSPITAL`, `DEATH`, `REPRIMAND`, `COURTMARTIAL` | the outcomes |
| `SETTINGS [ALL]` | the officer's preferences (`ALL` first loads every scene's art) |
| `FILM n` | aerodrome film 0-7 |

A `RUN` sortie appends one line to `RAM:canvas-sortie.log`: chapter, mission,
mode, length, idle seconds, the longest idle stretch, success, objectives,
kills, airframe and fuel.

Controls are in the drawer's `ReadMe` and the README.

## Implemented and observed working

### Campaign and presentation

- **Campaign:** eight chapters of twelve missions, with the diary before each
  sortie, a briefing board, a debrief with a late-edition clipping, and the
  Armistice epilogue.
- **Career:** three lives, field hospital and graveside memorial, victories,
  promotion and medals, and conduct (reprimand, lost rank, court martial).
- **Saves:** versioned `CanvasSkies.save` with a backup and transactional
  replacement. Old saves still load. Preferences are kept apart in
  `CanvasSkies.prefs`.
- **Officers' menu:** the campaign and year entries, the Book of Press Cuttings
  and the Officer's Preferences (keep the art in memory, music and effects
  volume, full screen, memory in use).
- **Scenes:** the morning paper, the picture house with real period film, the
  mess in three evenings (quiet, rowdy after a victory, a wake after a loss) by
  lamplight, dawn at the hangars.
- **Aerodrome films:** a take-off before each sortie and a landing after one
  flown home, on old film with their own sound, chosen by the day. Enter or
  fire skips.

### Missions

- **Pacing:** every sortie is flown out, the task done, then a run home and a
  landing. On the lab autopilot, sorties run about 3 to 4½ minutes, and no
  stretch without something to do or dodge is longer than about 7 seconds.
- **Hazards:** Archie with a ranging puff, ground machine guns, riflemen over
  enemy ground, kite balloons on cables, squalls, and scouts head-on and
  chasing home.
- **1914:** marking columns, message bags to headquarters and photographing
  batteries (side-on and top-down); in the cockpit patrols Tom also spots for
  the guns (S signals each fall of shot), and the debrief and late edition
  count the observer's tasks.
- **Christmas truce:** flown out over men of both armies meeting in no man's
  land, the pudding dropped, and home to land, with nobody firing.
- **Squalls:** in every view, with a blended rain haze when flying into one.
- **Rear seat:** the 1914 carbine swings through the full rear arc. The Lewis
  gun (1915 to 1918) also depresses to 30 degrees below level; the view dips
  with it, and the fuselage and lower wing stop rounds. Attackers come as a
  steady stream, from the flanks and from below too, and break off smoking
  when hit.
  The autopilot downs 2 to 4 each sortie.
- **Memory:** each scene loads only its own art and records. Every scene is
  under 32 MB with the cache off; the worst is the Lewis rear seat, 23 MB in
  round 5 and about 2 MB less since its gun became a 256-colour sheet and the
  unused ring overlay stopped loading (not yet re-measured on the lab).
- **Display:** F9 full screen leaves Workbench intact on leaving and on quitting
  (Lab-50 and Lab-51). The SDL2 module's own fault, behind the garbled
  Workbench seen with modules from before GFOpenUp 0.8.18, is hardened in
  GFOpenUp 0.8.26; the game's own workaround stays and does no harm.
- **Requirement:** the Open6 build loads `LIBS:GFOpenGPU/SDL2.module` and
  `gfopengpu.library`. Without GFOpenUp's GFOpenGPU part the game stops at
  once and says what is missing.

## Known problems and incomplete work

1. **Difficulty:** untested by a person. All tuning used the autopilot.
2. **Timing:** frame times were measured only on the lab, which is faster than
   a real 68040. Squalls, clouds, the films and the Lewis gun's low arc need
   timing on real hardware or PiStorm.
3. **Autopilot results in RC6:** the 1918 spring side-on sortie met 3 of its 5
   orders; the 1918 cockpit dogfight downed none (the cockpit autopilot rarely
   does); the slow-frames dogfight was still flying after 600 seconds. These
   are the autopilot's results, not a person's.
4. **Cache on:** with "keep the art in memory" on, a long campaign grows to
   about 48 MB.
5. **Recordings and film:** every shipped recording and film is listed with
   its SHA-256 in `assets/canvas-skies/MUSIC_LICENSE.md`. Colonel Bogey is
   our own rendition; the six period records were cleared by the project
   owner and Amazing Grace is kept; four of the eight newsreel source films
   await the National Archives' rights confirmation, which the project owner
   is seeking.
6. **Listening:** nobody has listened to the aerodrome films' soundtracks,
   including the three re-rendered in round 6, or re-checked the game by ear
   for ticks.
7. **Mess officers:** they now read as men, but close up the standing men's
   legs are stiff.
8. **Lewis gun memory:** the 2 MB saving is calculated, not measured.

## Acceptance before 0.2.0

- [x] Normal campaign starts, advances, saves, continues and reaches a chapter
      end and the Armistice.
- [x] All four mission views complete and fail correctly (lab autopilot).
- [x] Full screen and windowed preserve aspect and colour, and Workbench is
      intact after both (Lab-50, Lab-51).
- [x] Keyboard controls match the ReadMe. Joystick in port 2 is advertised but
      was not re-tested in round 5.
- [x] No magenta edges, horizon holes, seams or detached effects found in the
      round-5 captures.
- [ ] No recurring audio tick or stutter. Not re-checked by ear since round 5.
- [x] Dogfight banks, turns, climbs, stalls, dives, crashes and completes.
- [x] Rear gun muzzle, crosshair, rail, tail obstruction and approaches agree,
      for the carbine and the Lewis gun's full arc (lab autopilot).
- [ ] 1914 approachable and 1918 materially harder. True on the autopilot; not
      yet confirmed by a person.
- [x] The drawer and start icons are obvious and the game runs from a clean
      install of the release archive.
- [x] Every distributed asset and recording has its source and licence
      recorded.
- [x] The release contains no trace instrumentation or scratch assets.

## Safe resume point

`main` after the RC6 merge builds with no warnings with the Open6 stove and is
RC6. Start with `git status`, read this file, the RC6 receipt and the story
document, build the package, and run `CanvasSkies MISSION 1 1 RUN FLY` on a lab
with the GFOpenGPU part installed to see a sortie end to end.
