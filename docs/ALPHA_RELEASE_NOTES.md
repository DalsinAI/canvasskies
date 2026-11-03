# Canvas Skies 0.1.0-alpha.1

Canvas Skies is an original cinematic First World War aviation game for
AmigaOS 3, built from the modernised Sopwith port as a new game. This alpha is
a playable vertical slice, not an RC and not a complete 96-mission campaign.

## Included in this alpha

- Eight-chapter, 96-mission story scaffold following Neddy Seagoon from 1914
  to the Armistice, with journal and commander briefings.
- Side-on, top-down, rear-gunner and first-person dogfight mission modes.
- First-person dogfight flight model with bank, pitch, throttle, stalls,
  ground impact, enemy pursuit, damage and crash states.
- Player aircraft durability calibrated to at least eight equivalent enemy
  airframes, with persistent visible bullet holes on the cockpit and wings.
- Three-life campaign rule. The first two fatal crashes consume a life and
  open an RFC field-hospital recovery scene. Losing the third life opens the
  final rain-soaked graveside memorial.
- Ground impact uses a broken-airframe crumple, dust, snapped spars and a
  layered wood/metal crash sound rather than a fireball.
- Final memorial begins on black with the solo pipes, fades in after two
  seconds, then fades back to the saved-position board.
- Version 3 transactional campaign save records chapter, mission, wingmen,
  completion state and remaining lives; version 1 and 2 saves still load.
- F9 aspect-correct fullscreen, Amiga drawer/start icons, period fonts and
  presentation music.

## Direct QA entry points

`SIDE`, `TOPDOWN`, `DOGFIGHT`, `GUNNER`, `HOSPITAL`, `DEATH`, `DEBRIEF`,
`DEBRIEF_SUCCESS`, `CHAPTER_SUCCESS` and `FINAL_SUCCESS` may be passed as the
first command-line argument.

## Known alpha limitations

- The 96 missions still share generated layouts and require unique objectives,
  scenery and full difficulty balancing.
- Enemy aircraft art, 3D world detail and rear-gunner angle art need another
  polish pass.
- Joystick/pad input and a complete campaign play-through are not qualified.
- Release packaging and audio checks need validation on more real instances.

Music sources and redistribution notes are in
`assets/canvas-skies/MUSIC_LICENSE.md`; generated-art provenance is in
`assets/canvas-skies/ART_PROVENANCE.md`.
