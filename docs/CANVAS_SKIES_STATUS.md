# Canvas Skies: implementation status and handoff

Status date: 2 November 2026  
Branch: `canvas-skies-vertical-slice`  
Code state before this documentation commit: `9a33fd9`  
Repository: `DalsinAI/amigasdlsopwith`

This file is the resumable handoff. Update it when a significant feature is
finished, a test result changes, or a new release blocker is found. Do not rely
on chat history as the only record of a decision.

## Current result

Canvas Skies is a running AmigaOS 3 vertical slice built on SDL2 and the
OpenGPU-backed SDL module. It is no longer the original CGA Sopwith game. It has
an original title flow, campaign presentation, four mission renderers, custom
art and fonts, period music, procedural audio, a compact flight model and a
complete 96-mission narrative scaffold.

The most recently tested direct dogfight build is installed in the visible lab
instance at:

`$AMIGACHROME_DEV/labs/opensurge-quick-20261101`

The browser view is normally:

`http://127.0.0.1:8996/?watch=1`

This is a development build, not an RC. A complete campaign play-through has
not been performed.

## What is in the repository

- Main implementation: `src/modern/canvas_skies.c`
- Canonical 96-mission campaign: `src/modern/canvas_campaign.h`
- Art, font, sound and source assets: `assets/canvas-skies/`
- Amiga build: `ports/amigaos3/Makefile.canvas`
- Amiga drawer and start icons: `ports/amigaos3/package/Drawer.info` and
  `ports/amigaos3/package/Start Canvas Skies.info`
- Music provenance currently recorded: `assets/canvas-skies/MUSIC_LICENSE.md`
- Story and intended behaviour: `docs/CANVAS_SKIES_STORY_AND_DESIGN.md`

Three experimental gunner atlases are intentionally untracked at the time of
this handoff and are not used by the build:

- `assets/canvas-skies/gunner-direction-atlas.bmp`
- `assets/canvas-skies/gunner-redrawn-atlas.bmp`
- `assets/canvas-skies/gunner-ring-base.bmp`

Do not accidentally include them in a release. Decide whether to delete,
replace or deliberately adopt them after comparing them with the active
`gunner-single-atlas.bmp`.

## Build and direct tests

Build from the repository root:

```sh
make -f ports/amigaos3/Makefile.canvas -j2
```

The binary is:

`build-canvas-skies/CanvasSkies`

The build targets AmigaOS 3, 68040 plus 68881, with OpenAmigaGCC's
`os32-gcc16` stove and SDL2. The internal framebuffer is 640 by 400, 32-bit,
and is presented through an SDL window surface. Window and fullscreen preserve
aspect ratio.

Direct QA entry points:

```text
CanvasSkies SIDE
CanvasSkies TOPDOWN
CanvasSkies DOGFIGHT
CanvasSkies GUNNER
CanvasSkies DEBRIEF
```

`DOGFIGHT` and `GUNNER` deliberately use a durable late-war test and keep the
sortie open for visual inspection. The normal executable starts at the title
and campaign menu.

Current controls:

| Context | Controls |
|---|---|
| Menus | Up/Down select, Enter accepts, Escape returns |
| Side/top-down | Arrow keys move, X/Z adjust speed, Space fires, B bombs |
| Pilot dogfight | Left/Right bank, Down pulls up, Up pushes down, X/Z throttle, Space fires |
| Rear gunner | Arrow keys traverse/elevate, Space fires |
| Display | F9 toggles fullscreen |

## Implemented and observed working

### Campaign and presentation

- Centred, animated fabric title banner with stitched lettering.
- Victorian officers' menu with full-campaign and year-entry choices.
- Eight chapters, twelve missions each, following Neddy Seagoon from August
  1914 to the Armistice.
- Journal before every mission and a commander-in-chief briefing board.
- Named allied pilots Eccles, Gridpipe Thyne and Bluebottle.
- Debrief and epilogue scene paths exist.
- Fonts are provided as separately rendered sizes; field-sign text is centred.
- Amiga drawer icon and obvious start icon are present in the package folder.

### Side-on mode

- Four parallax concepts are represented: sky, hills, buildings and foreground.
- Landscape is assembled from reusable midground and foreground elements.
- Player movement uses all four cursor keys and has greater screen range than
  the original prototype.
- Machine-gun and artillery ground positions fire with distinct trajectories.
- Bombs and shells produce multi-frame explosions and smoke rather than
  silently leaving the screen.
- Player health, ammunition, bombs, enemy fire and damage exist.

### Top-down mode

- Dedicated top-down battlefield, player art and explosion atlas.
- Ground targets and moving vehicles are represented.
- Bombs and ground effects use world-axis tracking so they are intended to
  scroll with the landscape rather than the player.
- Gameplay resources and increasing chapter/mission difficulty exist.

### Rear-gunner mode

- Rear cockpit, tail/airframe view, separate crosshair and movable gun atlas.
- Journal and briefing now consistently describe the intended single Lewis gun;
  the abandoned twin-gun presentation is no longer part of the active UI.
- Aim range, tail obstruction and muzzle-origin calculations exist.
- Attackers approach in depth rather than being a static row of sprites.
- Bullet holes, player damage, ammunition and enemy damage exist.

### Pilot dogfight mode

- Compact fixed-step six-degree-of-freedom flight model with quaternion
  attitude, body velocity and angular rates.
- Direct cable-style controls, throttle, energy loss, stall, ground collision
  and crash state.
- Horizon, ground and sky follow pitch and bank. The old steep-bank stipple was
  removed by replacing one-pixel atmosphere bands with a solid signed-distance
  fill.
- Perspective terrain, clouds and detailed cockpit foreground.
- Enemy flight states, entry paths, pursuit, firing, damage, collision
  avoidance and ground-impact crashes.
- Mid-air collision dooms both aircraft and carries both to the ground.
- Short machine-gun tracers, muzzle flashes, ejected cases and separate hit
  feedback replaced the earlier laser-like lines.
- Enemy aircraft take multiple hits, smoke and spiral down rather than simply
  exploding in the air.
- The mission completes after all enemies are down in normal campaign play.
- The bottom instrument strip now shows `ENEMIES remaining/total`.
- Ammunition was increased by 50%; the early-war base is 390 rather than 260,
  while the later-chapter reduction remains proportional.

### Audio

- Music is limited to presentation scenes; interactive missions use engine and
  action audio.
- Player engine, nearby aircraft, vehicle, gun, artillery, hit, explosion and
  crash states are represented in the SDL audio callback.
- The Washington Post cue has a recorded licence/provenance chain and its first
  two seconds were removed for a cleaner start.

## Known problems and incomplete work

These are not optional polish if the game is to become an RC.

### Dogfight visual world

- The current solid sky/ground fix removes raster holes, but the landscape at
  steep bank still needs visual qualification in every quadrant and through a
  full 360-degree turn.
- Distant hills, horizon transition, clouds and ground geometry require more
  detail and better continuity. Clouds must be less repetitive.
- Enemy aircraft are still visually basic. They need convincing biplane and
  monoplane silhouettes, bank/pitch poses, damage states and distance-based
  levels of detail.
- Enemy manoeuvres need longer testing for physically implausible sideways
  motion, bad spawn visibility and collision-avoidance failures.
- Player/enemy gunfire, hit registration, crash timing and mission completion
  need a complete play test rather than isolated visual checks.

### Rear gunner

- The active atlas does not yet convincingly redraw the gun for every traverse
  and elevation angle. It can look bent or detached from its mounting.
- Verify that the muzzle is correct in every frame, tracers begin at it, the
  sight reaches the useful screen limits, the tail blocks fire correctly, and
  attackers face and move toward the rear quarter.
- Pilot evasive movement, gunner movement and the visible horizon/airframe must
  be clearly separated.

### Side-on and top-down

- Both modes need a systematic play test of target counts, mission completion,
  ammunition use, damage and difficulty across 1914-1918.
- Confirm that every ground explosion remains attached to world coordinates
  while scrolling. This was repeatedly wrong during development and must have
  a regression test.
- Top-down art, vehicles and explosions still need more variety and detail.
- Side-on foreground/background separation needs final colour, silhouette and
  parallax tuning.
- Check every sprite edge for magenta-key leakage.

### Campaign and balance

- All 96 mission titles and journal lines exist, but many missions currently
  share mechanics and generated layouts. Each needs objectives and scenery
  that fulfil its written story.
- Difficulty should begin forgiving in 1914 and rise through 1918 by enemy
  number, durability, aim, manoeuvre quality and resource pressure—not merely
  by faster bullets.
- Limited ammunition and bombs are intentional. The recent 50% ammunition
  increase needs balancing through a campaign rather than treating 390 as a
  final number.
- Wingman persistence, survival consequences and named-friend story outcomes
  are scaffolded but not yet a proven campaign system.
- Save/load and campaign continuation need explicit implementation or
  qualification before RC.

### Audio and licensing

- Re-test for the recurring tick previously heard in music/game audio. It has
  not received a final signed-off listening test on the real instance.
- Mission-complete music still needs a better period-appropriate cue and a
  selection matched to drama.
- `colonel-bogey-gramophone.wav` is used by the build, but its exact recording
  provenance and redistribution licence are not described in
  `MUSIC_LICENSE.md`. Do not release it until that is resolved or replace it
  with a project-made render from a documented public-domain/compatible source.
- Joystick/pad behaviour and actual speaker output need qualification on the
  intended Amiga instance.

### Packaging and release

- Build, assets, launch script, drawer icon and start icon must be assembled
  into one reproducible release drawer.
- Perform a clean install test without relying on files left in the lab.
- Verify F9 fullscreen and return to windowed mode with the current SDL2 module.
- Remove diagnostic traces and unused experimental assets from release output.
- Record checksums, version, source commit, toolchain, ROM/SDL2 module versions
  and licence inventory.
- This cannot be called an RC until the acceptance pass below is complete.

## Recommended next work

1. Freeze dogfight feature additions long enough to run a controlled visual and
   physics pass: level flight, left/right 360-degree turns, climb to stall,
   dive to ground impact, firing, enemy damage, collision and mission end.
2. Correct the rear-gunner mode to one Lewis gun everywhere and produce a
   deliberate angle atlas tied to measured muzzle positions.
3. Add a repeatable world-effect test proving side and top-down explosions stay
   attached to ground coordinates.
4. Give one complete chapter all twelve distinct objectives and play it from
   journal to debrief. Use that as the campaign template.
5. Resolve the audio tick and music licensing before adding more recordings.
6. Build a clean release drawer and only then start RC qualification.

## Acceptance before RC

- Normal campaign starts, advances, saves/continues and reaches a chapter end.
- All four mission modes complete and fail correctly.
- Fullscreen/windowed switching preserves aspect ratio and colours.
- Keyboard controls match the briefing; joystick/pad is tested if advertised.
- No magenta edges, horizon holes, detached world effects or corrupt frames.
- No recurring audio tick, delayed hit feedback or buffer stutter.
- Dogfight can bank, turn, climb, stall, dive, crash and complete naturally.
- Rear gun muzzle, crosshair, rail, tail obstruction and enemy approaches agree.
- 1914 is approachable and 1918 is materially harder without cheating.
- Drawer/start icons are obvious and the game runs from a clean installed drawer.
- Every distributed asset and recording has explicit source and licence data.
- Release contains no trace instrumentation or scratch assets.

## Safe resume point

At this handoff, the branch is pushed through `9a33fd9`, the build succeeds with
`ports/amigaos3/Makefile.canvas`, and the visible lab has the enemy counter and
50% ammunition increase installed. The next developer should first pull the
branch, read these two documents, run the direct `DOGFIGHT` test and inspect
`git status` before touching the three untracked gunner experiments.

