# Canvas Skies

Canvas Skies is an original First World War flying game for AmigaOS 3. You
follow pilot Neddy Seagoon of the Royal Flying Corps through an eight-chapter,
96-mission campaign, from observation flights in August 1914 to 11:00 on
Armistice Day.

It grew out of the Sopwith porting work in
[amigasdlsopwith](https://github.com/DalsinAI/amigasdlsopwith), but it is a new
game: no SDL Sopwith code is used.

Current release: **0.1.0-alpha.1**, a playable vertical slice. See
[docs/ALPHA_RELEASE_NOTES.md](docs/ALPHA_RELEASE_NOTES.md).

## Features

- Side-on, top-down, first-person dogfight and rear-gunner mission modes
- Pilot's journal and General Headquarters briefing before every sortie
- Six-axis dogfight flight model with stalls, damage and crashes
- Three-life campaign with field hospital recovery and a graveside memorial
- Resumable campaign saves
- Period fonts, painted art and gramophone-style music
- F9 toggles aspect-correct fullscreen

## Requirements

- AmigaOS 3.x on a 68040 with FPU (or an AmiCradle instance)
- SDL2 for AmigaOS 3

## Building

Cross-compile with the GFOpenAmigaGCC `os32-gcc16` stove:

```sh
make -f ports/amigaos3/Makefile.canvas -j2
```

Set `AMIGA_STOVE` if your stove is not at `~/AmiCradle/stoves/os32-gcc16`.
The binary is `build-canvas-skies/CanvasSkies`. It loads its art and sound
from `PROGDIR:assets/canvas-skies/`, so keep the `assets` drawer beside it.

`make -f ports/amigaos3/Makefile.canvas package` builds a release drawer in
`build-canvas-skies/package/` holding the executable, icons, licences and only
the assets the game loads, without the source PNGs.

## Controls

| Context | Controls |
|---|---|
| Menus | Up/Down select, Enter accepts, Escape returns |
| Side/top-down | Arrow keys move, X/Z adjust speed, Space fires, B bombs |
| Pilot dogfight | Left/Right bank, Down pulls up, Up pushes down, X/Z throttle, Space fires |
| Rear gunner | Arrow keys traverse/elevate, Space fires |
| Display | F9 toggles fullscreen |

## Documentation

- [Overview](CANVAS_SKIES.md)
- [Story and design](docs/CANVAS_SKIES_STORY_AND_DESIGN.md)
- [Implementation status](docs/CANVAS_SKIES_STATUS.md)
- [Delivery vision](CANVAS_SKIES_DELIVERY_VISION.md)
- [Historical sources](CANVAS_SKIES_SOURCES.md)

## Licence

Canvas Skies is free software under the
[GNU General Public License, version 3](LICENSE). Music, font and art
licences are listed below.

## Credits

Designed, tested and delivered by The AmiCradle Team.

Music, font and art credits are in
[assets/canvas-skies/MUSIC_LICENSE.md](assets/canvas-skies/MUSIC_LICENSE.md),
[assets/canvas-skies/ART_PROVENANCE.md](assets/canvas-skies/ART_PROVENANCE.md)
and the font copyright files in `assets/canvas-skies/`.

Neddy Seagoon, Eccles, Bluebottle and Gridpipe Thyne are an affectionate nod
to *The Goon Show*.
