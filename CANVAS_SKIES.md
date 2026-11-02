# Canvas Skies

`Canvas Skies` is an original cinematic WWI shooter built from the useful
lessons in SDL Sopwith rather than a cosmetic reskin of the 1984 game. It uses
the existing SDL2/OpenGPU module and a CPU-authoritative 32-bit window surface.

The first vertical slice contains:

- an eight-chapter, 96-mission chronological campaign beginning with unarmed
  observation in August 1914 and ending at 11:00 on Armistice Day;
- a continuous first-person story lived by pilot Neddy Seagoon: every mission
  opens on his physical journal, followed by formal General Headquarters
  orders issued by the Commander-in-Chief;
- campaign data for four side-on, four top-down and four perspective 3D
  dogfight missions in every chapter. Side/top-down sorties contain only
  Neddy's aircraft; 3D sorties add persistent allied pilots Eccles, Gridpipe
  Thyne and Bluebottle against increasingly capable enemy formations. The
  opening side-on mission is playable in this vertical slice; the dedicated
  top-down and 3D gameplay/rendering loops remain to be built;
- an original painted title screen;
- period serif display and field-typewriter faces, each rasterised into a
  separate atlas at every final pixel size and copied 1:1 with no run-time
  font scaling. The display face is Noto Serif Display; the field face is
  Liberation Mono Bold;
- squadron briefing and debriefing screens;
- a continuous side-on, Scramble-style mission with fuel, airframe damage,
  armed fighters, machine-gun nests, anti-aircraft guns and ground objectives;
- proximity-fused flak bursts, visible progressive airframe smoke/flame and
  a wide parallax battlefield panorama with ruined villages, church tower,
  shell holes, trenches, waterlogged earth and layered dawn haze;
- newly illustrated multi-angle aircraft sprites;
- a gramophone-treated 1889 military march, with original real-time synth
  fallback, an added marching drum line, and distinct machine-gun/artillery
  sound envelopes;
- music only outside missions; gameplay switches to a continuous-phase engine
  and slipstream sound driven by airspeed and climb/dive load, with channels
  reserved for relative aircraft and real vehicle movement;
- wooden-and-brass squadron boards and separate field-instrument placards;
- a deterministic modular battlefield composer: distant hills scroll at 3.5%
  of flight speed, ruined farms, churches, poplars, smoke, telegraph poles and
  walls at 16%, and trenches, craters, wire, roads and blasted trees at 52%.
  The same chapter and mission always rebuild the same landscape while later
  missions can select different arrangements; interactive gun emplacements
  remain separately anchored in gameplay space;
- keyboard flight controls and an F10 full-screen test toggle.

## Controls

- Arrow keys: move the aircraft in all four directions
- X: increase throttle
- Z: reduce throttle
- Space: fire
- Enter: advance through title, briefing and debriefing
- F10: toggle full screen
- Escape: return to title; Escape again exits

## AmigaOS 3 build

```sh
make -f ports/amigaos3/Makefile.canvas
```

The executable is written to `build-canvas-skies/CanvasSkies`. It expects the
`assets` drawer beside it, preserving the path
`assets/canvas-skies/title.bmp`.

`ports/amigaos3/package` contains a clearly labelled IconX launcher and an
original GlowIcon for the game drawer. Put `Drawer.info` beside the installed
drawer under the drawer's own name (for example `CanvasSkies.info`).

The music is John Philip Sousa's 1889 composition *The Washington Post*. The
composition is public domain. The MIDI transcription is by Renato Biolcati
Rinaldi via Mutopia (CC BY-SA 3.0); this project renders and processes it into
an intentionally narrow-band, shellac-era recording. Full credits are in
`assets/canvas-skies/MUSIC_LICENSE.md`.

The title painting is original generated artwork created for this project. No
artwork, music, branding or story material from Cinemaware's *Wings* is used.
The battlefield panorama and mission board are also original generated assets
created for this project; their full-resolution sources are retained beside the
game-ready BMP files.

The fictional campaign's journal voice and operational details are grounded in
museum-held diaries and aviation records. The source ledger and rules for using
them without copying real lives are in `CANVAS_SKIES_SOURCES.md`.
