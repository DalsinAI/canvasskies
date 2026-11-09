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

The observer's Lewis gun on its Scarff ring (`rear-lewis.bmp`) is original
art made the same way by `ports/amigaos3/make_rear_lewis.py`, in the colours
of the earlier painted gun: the ring and its bow once, and the gun for a grid
of aims from 30 degrees below level to 75 above. It replaces the painted
atlas, which had no frames below level.

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
`ports/amigaos3/make_medals.py`.

The squadron mess is rendered by `ports/amigaos3/make_mess.py` on the
same renderer as the picture house, in three evenings: `mess.bmp` (an
ordinary good evening), `mess-rowdy.bmp` (the night of a victory) and
`mess-wake.bmp` (the night of a loss).  The officers are made with our own
rigged figure kit, `ports/amigaos3/figurekit.py`: men built from shaped
solids on a small skeleton, posed by joint angles, with no outside models,
textures or reference pictures.  The kit draws them into a buffer that
keeps each pixel's colour, surface, depth and material, and then paints
them in code: the faces put on in a few strokes (eyes, brows, the shadow
under the nose, the mouth, colour in the cheeks), the lamplight wrapped
round them with its falloff, rim light, shadow and a painterly mottle.
Their chairs, the cards, letters, books and newspaper in their hands are
the kit's own flat and solid pieces.  Knowledge of 1916-18 R.F.C. dress
(service dress and the double-breasted jacket, the Sam Browne belt,
collar and tie, the wings over the left breast, breeches and riding
boots) was used only for accuracy; nothing was copied or traced from any
photograph, film or other artwork.  The dog, the flying helmet, the map
and everything else in the room are built from the renderer's own lit
solids and surfaces (`figures3d.py`); no outside textures, photographs or
figures are used.  Each picture is 640x400, 24-bit BMP holding RGB565
values (768,054 bytes); SHA-256:

    mess.bmp        52f97093d203ffaa02873f13a7d2406a754e9e39f050a6acffe48517ea34ec20
    mess-rowdy.bmp  58f7eb8affa145f2e282cf7aad755e7f395ff714c8acfe9243908a4803fad501
    mess-wake.bmp   8393113f77116c5b96fdf931c481b9f902c43eb46c7db0346b7d0c5c77881961

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

## The aerodrome films

The eight take-off and landing films in `airfield-films.bin` are original
work for this game, rendered on the host by our own small ray-caster in
`ports/amigaos3/films/` (`render.py` for the solids, the sun's shadows,
the sky and the haze; `models.py` for the machines, the men and the
aerodrome; `airfield.py` for the films themselves and their old-newsreel
finish), then packed for the game by `ports/amigaos3/make_airfield_films.py`.
Everything in them is built from simple solids and procedural noise: the
grass and its wear, the Bessonneau hangars with their ropes and pegs, the
tents, windsock, drums, hedges, poplars and farms, the three machines (after
the game's own 1914 two-seater, Sopwith-type scout and late-war flat-nosed
scout sprites), the ground crew, the weather and the light. No photograph,
film still, outside texture or reference image was used or is kept in the
repository.

Their soundtracks are synthesised by `ports/amigaos3/films/sound.py` from
the same account of each film (see `MUSIC_LICENSE.md`); no recording or
sample is used. The shipped file has SHA-256
`a894e1c36e4e00a96c85ae8345e2000e8fbe3ac2a50d402e80961323c14179a2`. It is
rebuilt, from `ports/amigaos3`, with:

    python3 films/airfield.py out/master
    python3 films/sound.py out/sound
    python3 make_airfield_films.py out/master/frames out/sound


## We Remember

The roll call's picture of the old front in the summer of 1919 is original
art for this game, painted on the host by `ports/amigaos3/make_remember.py`.
It comes in four layers that the game slides at different speeds
(`remember-sky.bmp`, `remember-far.bmp`, `remember-mid.bmp`,
`remember-near.bmp`),
with swaying plants (`remember-poppies.bmp`) and butterflies, a lark and
thistledown (`remember-life.bmp`). The land is a height field lit by a low
evening sun with its shadows traced across it; the clouds are built from
lit puffs; the flowers, grasses, wire, pickets, trees, ruins and the things
left in the grass are painted as shaded shapes from the distance forward.
All of it is made from procedural noise and simple shapes. The poppies
are drawn as wild flowers growing, not as any emblem. No photograph,
outside texture or reference image was used.
