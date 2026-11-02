# Canvas Skies: 1917

`Canvas Skies: 1917` is an original cinematic WWI shooter built from the useful
lessons in SDL Sopwith rather than a cosmetic reskin of the 1984 game. It uses
the existing SDL2/OpenGPU module and a CPU-authoritative 32-bit window surface.

The first vertical slice contains:

- an original painted title screen;
- a period serif display face and field-typewriter briefing face, rasterised
  from the OFL-licensed Liberation fonts;
- squadron briefing and debriefing screens;
- a continuous side-on, Scramble-style mission with fuel, airframe damage,
  armed fighters, machine-gun nests, anti-aircraft guns and ground objectives;
- newly illustrated multi-angle aircraft sprites;
- a gramophone-treated 1889 military march, with original real-time synth
  fallback and sound effects;
- keyboard flight controls and an F10 full-screen test toggle.

## Controls

- Up/Down or comma/slash: climb/dive
- A/D: move fore and aft within the visible formation
- X/Right: increase speed
- Z/Left: reduce speed
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
