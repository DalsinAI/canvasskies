
SDL Sopwith is a port of the game "Sopwith", which was originally
by BMB Compuscience Canada. The original author David L. Clark
has kindly released the source code under the GNU GPL.

Features:

* Uses LibSDL, so will run on most modern operating systems, and also
  the web (via emscripten)
* Support for loading custom mission files (new levels)
* TCP/IP multiplayer
* PC speaker emulation
* Multiple palettes that emulate a selection of old monitors
* Medals and high score table

## More Info

More information about SDL Sopwith is available in some of the files
included here:

* AUTHORS - People responsible for this
* COPYING - License information (GNU GPL)
* FAQ - Frequently asked questions about SDL Sopwith
* NEWS - What's new, ChangeLog

## Canvas Skies

This repository also contains the in-development Amiga game *Canvas Skies*, an
original First World War campaign that grew from the Sopwith porting work.

* [Story and design bible](docs/CANVAS_SKIES_STORY_AND_DESIGN.md)
* [Implementation status and resumable handoff](docs/CANVAS_SKIES_STATUS.md)
* Campaign data: `src/modern/canvas_campaign.h`
* Amiga build: `make -f ports/amigaos3/Makefile.canvas -j2`

You can find lots more information about the original Sopwith here:

* http://www.sopwith.org/
* https://fragglet.github.io/sdl-sopwith
* https://en.wikipedia.org/wiki/Sopwith_(video_game)

## Feedback

If you like SDL Sopwith, [join the Discord community](https://discord.gg/S2P5wUUzNU)
where you can chat with other fans of the game!
