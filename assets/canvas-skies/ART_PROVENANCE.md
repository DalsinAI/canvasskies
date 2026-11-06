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

The pilot's revolver (`cockpit-revolver.bmp`) and
the top-down mission targets (`topdown-targets.bmp`) are original art for
this game, painted procedurally on the host by
`ports/amigaos3/make_personal_arms.py` and
`ports/amigaos3/make_topdown_targets.py` with the shared painter
`ports/amigaos3/paint.py`. Each part is painted at four times size with
shading lit from the low sun, a soft inner rim, grain and edge highlights,
then filtered down to game size. No photograph, film still or other game's
art was used. The ambulances are marked by a pale tilt and the word
AMBULANCE; the protected red cross emblem is deliberately not drawn.

The observer's carbine in hand (`rear-carbine.bmp`) is original art for
this game, rendered on the host by `ports/amigaos3/make_rear_carbine.py`:
the rifle alone is built of simple solids ray-cast in perspective from the
observer's eye and lit by the low sun, the sky and a rim of sunlight, one
frame for each aim of a grid over the rear arc. It is stored as a 256-colour
run-length encoded BMP.

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

`side-civilians.bmp` (the side-on farmhouse, church and refugee cart, whole
and wrecked) is painted the same way by
`ports/amigaos3/make_side_civilians.py`.

`riflemen.bmp` (the enemy's riflemen in field grey, in the covered spiked
helmet of 1914 and the steel helmet of 1916, side on and from above) is
original art for this game, rendered on the host by
`ports/amigaos3/make_riflemen.py` with our own small renderer
(`scene3d.py`): each man is built of simple lit solids, lit from the upper
left and filtered down with hard edges. No photograph or other game's art
was used.

`newspaper-table.bmp` (the mess table, the newsvendor's bundle and its
string, and the blank top copy with its halftone, screened from our own
`title.bmp`) is painted by `ports/amigaos3/make_newspaper_table.py`.

`cinema-hut.bmp` (the Y.M.C.A. picture house) is rendered by
`ports/amigaos3/make_cinema_hut.py` with our own small renderer
(`scene3d.py`, figures from `figures3d.py`); no outside textures. The film in `newsreels.bin` is real
period footage; its sources and licences are listed in
`CANVAS_SKIES_SOURCES.md`.

`cockpit-cellule.bmp` (the wings, struts and wires seen from the pilot's
seat) is painted by `ports/amigaos3/make_cockpit_cellule.py`.

`medals.bmp` (the five decorations and the office crest) is painted by
`ports/amigaos3/make_medals.py`, and `mess.bmp` (the squadron mess) is
rendered by `ports/amigaos3/make_mess.py` on the same renderer.

`explosion-atlas.bmp` and `topdown-explosion-atlas.bmp` are re-cut from
their painted `-source.png` files by `ports/amigaos3/make_explosion_atlases.py`.

The top-down trench tiles in `topdown-targets.bmp` are lit height fields
built by `ports/amigaos3/trenches.py`.  `foreground-panorama.bmp` is the
painted ground (`foreground-panorama-base.bmp`) with fire trenches and
wire set into it by `ports/amigaos3/make_side_trenches.py`.

`loss-card.bmp` (the empty chair) is rendered by
`ports/amigaos3/make_loss_card.py`.

`machine-cards.bmp` (the papers of the new-machines cards) is painted by
`ports/amigaos3/make_cards.py`; `office-paper.bmp` and `office-stamp.bmp`
(the adjutant's desk, the foolscap and the rubber stamp) by
`ports/amigaos3/make_office_paper.py`.
