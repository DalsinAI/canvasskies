# Canvas Skies

`Canvas Skies` is an original cinematic WWI shooter built from the useful
lessons in SDL Sopwith rather than a cosmetic reskin of the 1984 game. It uses
the existing SDL2/OpenGPU module and a CPU-authoritative 32-bit window surface.

The first vertical slice contains:

- an eight-chapter, 96-mission chronological campaign beginning with basic
  observation in August 1914 and ending at 11:00 on Armistice Day;
- a continuous first-person story lived by pilot Neddy Seagoon: every mission
  opens on his physical journal, followed by formal General Headquarters
  orders issued by the Commander-in-Chief;
- campaign data for four side-on, four top-down, two single-seat dogfights and
  two rear-gunner missions in every chapter. Side/top-down sorties contain
  only Neddy's aircraft; dogfights add persistent allied pilots Eccles,
  Gridpipe Thyne and Bluebottle against increasingly capable formations.
  Rear-gunner sorties put Tom at the controls while Neddy swivels through a
  90-degree rear observation arc with a single Lewis gun on the Scarff ring;
- an original painted title screen with an animated field-made canvas banner;
  its lettering is visibly embroidered into the cloth rather than overlaid as
  a font, so the seams, thread and fabric ripple together. A brass Victorian
  plaque opens an overlaid commission board for the full campaign or a direct
  1914, 1915, 1916, 1917 or 1918 starting year;
- period serif display and field-typewriter faces, each rasterised into a
  separate atlas at every final pixel size and copied 1:1 with no run-time
  font scaling. The display face is Noto Serif Display; the field face is
  Liberation Mono Bold;
- squadron briefing and debriefing screens;
- a continuous side-on, Scramble-style mission with fuel, airframe damage,
  armed fighters, machine-gun nests, anti-aircraft guns and ground objectives;
- distinct ground-fire models: Vickers emplacements fire short rate-limited
  tracer bursts and cannot track above their mount's elevation ceiling, while
  artillery launches deliberately inaccurate ballistic shells which arc and
  detonate;
- proximity-fused flak bursts, visible progressive airframe smoke/flame and
  layered billowing smoke from damaged player and enemy aircraft. Bombs which
  miss a target collide with the visible terrain and produce a multi-stage
  flash, fireball, debris burst and lingering smoke column instead of falling
  silently out of the scene. A wide parallax battlefield panorama has ruined
  villages, church tower,
  shell holes, trenches, waterlogged earth and layered dawn haze;
- newly illustrated multi-angle aircraft sprites;
- a gramophone-treated 1889 military march, with original real-time synth
  fallback, an added marching drum line, and distinct machine-gun/artillery
  sound envelopes;
- music only outside missions; gameplay switches to a continuous-phase engine
  and slipstream sound driven by airspeed and climb/dive load, with channels
  reserved for relative aircraft and real vehicle movement. Successful
  debriefs use a separate jaunty 1910s mess-hall cue through a continuous
  narrow-band gramophone filter with surface hiss but no artificial clicks;
- wooden-and-brass squadron boards and separate field-instrument placards;
- a deterministic four-plane battlefield composer: the sky drifts at 0.8% of
  flight speed, distant hills at 5.5%, ruined farms, churches, poplars, smoke,
  telegraph poles and walls at 22%, and the playable foreground—trenches,
  craters, wire, roads, blasted trees, guns and bullets—at the full 100% world
  rate. This keeps collisions and visible scenery in the same coordinate space.
  The same chapter and mission always rebuild the same landscape while later
  missions can select different arrangements; interactive gun emplacements
  remain separately anchored in gameplay space;
- a fixed 640x400 logical canvas presented as the complete game scene without
  host-display stretching;
- dedicated playable render/update paths for the side-on battlefield, a
  painted vertically scrolling top-down attack view, a single-seat flight-sim
  dogfight with fixed Vickers sight, bank/pitch/throttle and allied formation,
  and a separate two-seat defensive-gunner view with a traversable Scarff ring;
- original painted top-down Western Front terrain, RFC overhead aircraft,
  dawn battlefield perspective and separate pilot/gunner cockpit overlays;
- a rear-gunner Scarff-ring presenter built from a clean cockpit rail, a
  visible DH.4-style rear fuselage and tailplane, and a separate 25-pose
  single-Lewis-gun carriage. The gun stays anchored to the rail while rising
  from near-horizontal to roughly 75 degrees; exact chroma-key cleanup prevents
  magenta edge contamination;
- the rear observer's whole viewpoint pans across a 90-degree arc. The wide
  battlefield panorama, incoming aircraft and visible tail geometry move in
  opposition to the view, while Tom's independent bank and pitch roll and
  shear the horizon. Enemy aircraft close toward the camera using head-on art
  and varied approach rates before crossing the fixed central sight;
- terrain impacts are retained in mission-world coordinates. Top-down blasts
  advance with the vertically scrolling map and side-on blasts move with the
  horizontal landscape rather than remaining attached to the player or screen;
- irregular lantern animation on briefing/debrief boards, with the same warm
  flicker reflected in both hanging goggle lenses on the opposite side.

## Controls

- Side/top-down: arrow keys move the aircraft in all four directions
- Single-seat dogfight: arrow keys bank and pitch; X/Z control throttle
- Two-seat gunner: Left/Right swivels the rear view and Scarff-ring gun through
  90 degrees; Up/Down elevates the single Lewis gun from level to about 75
  degrees while Tom flies an independent evasive course
- On the title campaign board, Up/Down selects the full campaign or a 1914,
  1915, 1916, 1917 or 1918 starting commission
- X: increase throttle
- Z: reduce throttle
- Space: fire
- Enter: open/accept the campaign board and advance through story screens
- Escape: return to title; Escape again exits

## AmigaOS 3 build

```sh
make -f ports/amigaos3/Makefile.canvas
```

The executable is written to `build-canvas-skies/CanvasSkies`. It expects the
`assets` drawer beside it, preserving the path
`assets/canvas-skies/title.bmp`.

For hardware QA without changing campaign progression, launch `CanvasSkies
SIDE`, `CanvasSkies TOPDOWN`, `CanvasSkies DOGFIGHT`, `CanvasSkies GUNNER` or
`CanvasSkies DEBRIEF` to enter each renderer directly. A normal launch always
opens the title and commission board.

`ports/amigaos3/package` contains a clearly labelled IconX launcher and an
original GlowIcon for the game drawer. Put `Drawer.info` beside the installed
drawer under the drawer's own name (for example `CanvasSkies.info`).

The music is John Philip Sousa's 1889 composition *The Washington Post*. The
composition is public domain. The MIDI transcription is by Renato Biolcati
Rinaldi via Mutopia (CC BY-SA 3.0); this project renders and processes it into
an intentionally narrow-band, shellac-era recording. Full credits are in
`assets/canvas-skies/MUSIC_LICENSE.md`.

Music is selected by dramatic context rather than by player control: journals,
briefings, debriefs and chapter transitions use march, reconnaissance, tension,
mourning or Armistice cues. Every player-controlled mission is deliberately
unscored; only engine, wind, aircraft, vehicle, gun and artillery sound remains.

The title painting is original generated artwork created for this project. No
artwork, music, branding or story material from Cinemaware's *Wings* is used.
The battlefield panorama and mission board are also original generated assets
created for this project; their full-resolution sources are retained beside the
game-ready BMP files.

The fictional campaign's journal voice and operational details are grounded in
museum-held diaries and aviation records. The source ledger and rules for using
them without copying real lives are in `CANVAS_SKIES_SOURCES.md`.
