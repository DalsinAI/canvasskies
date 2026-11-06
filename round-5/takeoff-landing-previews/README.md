# Take-off and landing previews

Previews only, not in the game. Each is rendered at 640x400 by `src/airfield.py`
(our own aerodrome, machines and men, ray-cast and lit), then finished as old
newsreel: silver-sepia, grain, gate weave, flicker, scratches and dust, a
vignette, 17 frames a second, with a leader countdown or a splice at the start.

Take-offs:

- `takeoff-1-dawn-prop-swing.webp`: dawn; the airscrew swung, chocks pulled, away into the low sun
- `takeoff-2-flight-of-three.webp`: a flight of three in a vee
- `takeoff-3-muddy-field.webp`: wallowing through mud, a bounce off a rut, mud thrown up
- `takeoff-4-1914-observer-climbs-in.webp`: 1914 two-seater, the observer climbs in

Landings:

- `landing-1-dusk-three-point.webp`: a clean three-point landing at dusk
- `landing-2-shot-up.webp`: tattered fabric, a wisp of smoke, a hard bounce, a lurch onto one wheel, the ground crew running out
- `landing-3-dead-stick.webp`: a glide in with the airscrew stopped
- `landing-4-rain.webp`: landing in rain, spray off the wheels

`*-keys.png` are five key frames of each; `contact-sheet.png` has them all.
In the game they could reuse the newsreel codec (8x8 block deltas, 4-bit grey).
