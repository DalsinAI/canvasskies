# Canvas Skies

Canvas Skies is an original First World War flying game for AmigaOS 3. You
follow pilot Neddy Seagoon of the Royal Flying Corps through an eight-chapter,
96-mission campaign, from observation flights in August 1914 to 11:00 on
Armistice Day.

It grew out of the Sopwith porting work in
[amigasdlsopwith](https://github.com/DalsinAI/amigasdlsopwith), but it is a new
game: no SDL Sopwith code is used.

Current release: **0.2.0 RC3**, a release candidate. Download it from
[the v0.2.0-rc3 release](https://github.com/DalsinAI/canvasskies/releases/tag/v0.2.0-rc3).
New in RC3: the cockpit seen from the pilot's seat, machines that change with
the war, top-down raids on convoys, railways, aerodromes and trench lines,
civilians and ambulances to spare on pain of court martial, the morning paper
set like a real one, and a picture house showing real film of the war. The
earlier alpha is described in
[docs/ALPHA_RELEASE_NOTES.md](docs/ALPHA_RELEASE_NOTES.md).

## Features

- Side-on, top-down, cockpit dogfight and rear-gunner sorties
- The cockpit from the pilot's seat: the top wing overhead, the struts beside
  the gun, the world banking around the machine
- Machines that change with the war, ours and theirs: a 1914 two-seater
  armed with a rifle and revolver, a single-Vickers scout, a twin-gun scout
  and a late-war scout, against the Fokker monoplane, the Albatros and the
  late Fokker
- Top-down raids built from the war's targets: lorry convoys, trains and
  stations, aerodromes, depots and trench runs, mixed to suit each year
- Ambulances, villages and our own line to spare: a reprimand, a lost rank,
  then a court martial that ends the career
- Bombs that burst too close hurt your own machine
- Pilot's diary and General Headquarters orders before every sortie, and a
  career of victories, promotion and medals
- The morning paper on its tied bundle, quoting real cable news of
  1914-1918, and the Book of Press Cuttings
- The picture house behind the lines, showing real film of each year
- Three-life campaign with field hospital recovery and a graveside memorial
- Resumable campaign saves
- Period typefaces, painted art and period gramophone records
- F9 toggles aspect-correct fullscreen

## Screenshots

Captured from the 68040 Amiga build running on AmigaOS with GFOpenUp, at the
game's own 640x400.

| | |
|---|---|
| ![Title screen](docs/screenshots/title.png) | ![Pilot's diary](docs/screenshots/diary.png) |
| The title screen | Neddy Seagoon's flight log, ruled and written in his hand |
| ![The morning paper](docs/screenshots/newspaper.png) | ![The picture house](docs/screenshots/picture-house.png) |
| The morning paper on its bundle | The picture house behind the lines, showing real film of 1916 |
| ![Cockpit dogfight](docs/screenshots/cockpit-dogfight.png) | ![New machines](docs/screenshots/new-machines.png) |
| A 1916 patrol from the pilot's seat, one Vickers under the sight | New machines from the aircraft park |
| ![Top-down sortie](docs/screenshots/top-down-sortie.png) | ![Side-on sortie](docs/screenshots/side-on-sortie.png) |
| A top-down raid on a convoy and its depot | A side-on sortie at dusk |
| ![Court martial](docs/screenshots/court-martial.png) | |
| Fire on civilians three times and it ends here | |

## Requirements

- AmigaOS 3.x on a 68040 with FPU (or an AmiCradle instance)
- RTG graphics: the game opens a 640x400 window on a 16-bit (or deeper)
  Workbench screen
- GFOpenUp with the OpenGPU SDL2 runtime (`LIBS:OpenGPU/SDL2.module` and
  `LIBS:opengpu.library`)
- AHI for sound

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
| Side/top-down | Arrow keys move, X/Z adjust speed, Space fires (a rifle in 1914), B bombs |
| Pilot dogfight | Left/Right bank, Down pulls up, Up pushes down, X/Z throttle, Space fires (a revolver in 1914), R clears a jam, H turns for home |
| Rear gunner | Arrow keys traverse/elevate, Space fires |
| Display | F9 toggles fullscreen |

## Documentation

- [Overview](CANVAS_SKIES.md)
- [Story and design](docs/CANVAS_SKIES_STORY_AND_DESIGN.md)
- [Implementation status](docs/CANVAS_SKIES_STATUS.md)
- [Delivery vision](CANVAS_SKIES_DELIVERY_VISION.md)
- [Historical sources](CANVAS_SKIES_SOURCES.md)

## We Remember

At the very end of the game, after the Armistice, the camera drifts across
the old front in the summer of 1919, the trenches grassed over and the
poppies grown up through the wire, with nothing but the wind. Over it, under
the words **WE REMEMBER**, a roll of names is shown.

Those names are given by people who have given to the
[Royal British Legion](https://www.britishlegion.org.uk/get-involved/donate),
in memory of someone of theirs who served.

**To give:** please give directly on the Legion's own site,
<https://www.britishlegion.org.uk/get-involved/donate>. The game is free;
we never see or handle any money, and we do not ask how much you gave.

**To add a name:** after giving, either
[open a We Remember request](https://github.com/DalsinAI/canvasskies/issues/new?template=we-remember.yml)
or email us at WE-REMEMBER-ADDRESS-TO-COME, with the name as it should be
shown, for example `Rank. Name, Unit, Year` (rank, unit and year are
optional). Names are added in the next release, in
[`ports/amigaos3/package/WeRemember.txt`](ports/amigaos3/package/WeRemember.txt),
which the game reads from the `CanvasSkies` drawer. Only the name to be
remembered is shown, never yours, and no email addresses or amounts are kept
in the game or this repository.

**To remove a name:** open an issue or email asking for it to be taken off,
and it will be removed in the next release.

## Licence

Canvas Skies is free software under the
[GNU General Public License, version 3](LICENSE). Music, font, film and art
licences are listed below.

## Credits

Designed, tested and delivered by The AmiCradle Team.

Music, font, film and art credits are in
[assets/canvas-skies/MUSIC_LICENSE.md](assets/canvas-skies/MUSIC_LICENSE.md),
[assets/canvas-skies/ART_PROVENANCE.md](assets/canvas-skies/ART_PROVENANCE.md),
[CANVAS_SKIES_SOURCES.md](CANVAS_SKIES_SOURCES.md) (the newsreels and
typefaces) and the font licences in `assets/fonts/`.

Neddy Seagoon, Eccles, Bluebottle and Gridpipe Thyne are an affectionate nod
to *The Goon Show*.
