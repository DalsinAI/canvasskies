# Canvas Skies art provenance

## Fatal memorial and field hospital

`fatal-memorial-wide.bmp` and `field-hospital.bmp` are original project assets
made for the game on 3 November 2026.
They were directed specifically for Canvas Skies and do not copy an external
game, film still, photograph or named artist's work.

The memorial was iterated into a 16:10 rainy RFC cemetery scene with a blank
stone, three bare-headed fellow pilots holding their caps, and Flanders
poppies. The hospital was directed as a 16:10 British field-hospital scene
with Neddy recovering in a cot and a visibly broken biplane outside. Both
outputs were resized to 640x400, gently desaturated/sepia-toned, and converted
to uncompressed 24-bit BMP for SDL 2 on AmigaOS.

The grave inscription and hospital memorandum are rendered by the game at
runtime so the generated bitmaps contain no baked-in lettering.

## Painted on the host

The 1914 weapons in hand (`cockpit-revolver.bmp`, `rear-carbine.bmp`) and
the top-down mission targets (`topdown-targets.bmp`) are original art for
this game, painted procedurally on the host by
`ports/amigaos3/make_personal_arms.py` and
`ports/amigaos3/make_topdown_targets.py` with the shared painter
`ports/amigaos3/paint.py`. Each part is painted at four times size with
shading lit from the low sun, a soft inner rim, grain and edge highlights,
then filtered down to game size. No photograph, film still or other game's
art was used. The ambulances are marked by a pale tilt and the word
AMBULANCE; the protected red cross emblem is deliberately not drawn.

`pilot-cockpit-single.bmp` is assembled from the existing cockpit
paintings by `ports/amigaos3/make_single_gun_cockpit.py`, and
`journal.bmp` is the existing journal painting with its ruled lines lifted
off by `ports/amigaos3/make_journal_page.py`.

The side-on and top-down sprites for the 1914 two-seater, the 1918 scout,
the enemy's 1915 monoplane and 1918 biplane (`aircraft-player-twoseat.bmp`,
`aircraft-player-late.bmp`, `aircraft-enemy-mono.bmp`,
`aircraft-enemy-late.bmp`, `topdown-player-twoseat.bmp`,
`topdown-player-late.bmp`) are original designs painted the same way by
`ports/amigaos3/make_machine_sprites.py`. They are not copies of any
surviving aircraft's drawings.
