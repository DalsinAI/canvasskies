# Canvas Skies: Delivery Vision

## The game we are making

`Canvas Skies` is an original cinematic First World War flying game for the
Amiga. It takes the immediate, readable action of a classic arcade shooter and
places it inside a character-led campaign with the atmosphere, presentation
and emotional continuity of a prestige 1990s Amiga production.

It is not a reskin of Sopwith, Scramble, 1942 or Wings. Those games provide
useful reference points for pace and structure, but the story, code, artwork,
missions, aircraft, interface and audio presentation are original. The target
is a complete game which feels purpose-built for the Amiga rather than a small
port with a new title screen.

The player lives through the air war from August 1914 to the Armistice on 11
November 1918 as Royal Flying Corps pilot Neddy Seagoon. The campaign begins
when aircraft are fragile observation machines whose military value is still
doubted. It ends after aviation has become central to reconnaissance, artillery
observation, ground attack, escort and air superiority. Changes in aircraft,
weapons, tactics, landscapes and enemy ability should be felt through play,
not merely described in text.

## The intended player experience

The game should alternate between human-scale storytelling and brisk,
responsive action:

1. Neddy writes in his journal before every mission. The entry records what he
   has seen, what has changed and how the war is affecting him and his friends.
2. The Commander-in-Chief's briefing identifies the operational purpose,
   hazards, controls and success conditions on a physical wooden planning
   board.
3. The player flies one of four mission formats. Each has distinct controls,
   camera language, tactics and scenery rather than being the same mechanics
   viewed from a different angle.
4. The debrief reports the result and advances the continuing story. Success
   should feel earned; failure should allow an immediate retry without losing
   the campaign context.

The game must be readable within seconds, but retain enough movement, target
variety and enemy behaviour that repeated missions do not feel mechanical.
Controls should have weight and damping rather than twitching instantly across
the screen. Weapons must give clear visual and acoustic feedback. A player
should always understand what hit them, what they hit and why a mission ended.

## Campaign and story

The full campaign contains eight chronological chapters with twelve missions
per chapter: 96 missions in total. Each chapter contains:

- four side-on operations;
- four top-down operations;
- two single-seat perspective dogfights;
- two two-seat rear-gunner perspective missions.

The mission order deliberately rotates the formats so that one chapter never
becomes twelve variations of the same view. Missions have individual titles,
journal entries, orders, objective layouts, weather, time of day and dramatic
purpose. The historical progression provides the broad context, while Neddy
and his squadron remain fictional so the story can be coherent without
appropriating a real pilot's private life.

Neddy's recurring allies are Eccles, Gridpipe Thyne and Bluebottle. They are
not disposable HUD icons: they fly alongside him in formation, suffer damage,
appear in journal entries and contribute to the emotional continuity of the
campaign. The supporting cast may also include Tom, Neddy's observer and rear
pilot, the ground crew and medical personnel. Their survival, absence or
return should matter to later scenes.

The tone is affectionate, humane and occasionally funny, but the war is not
treated as consequence-free spectacle. The early wonder of flight gives way
to fatigue and loss, before ending with the extraordinary silence of the
Armistice. Museum-held diaries and contemporary records inform vocabulary,
weather, flying routines and operational details without being copied as
fictional dialogue.

## Mission format 1: side-on operations

The side-on game is the closest descendant of the original Sopwith/Scramble
idea, but it must look and behave as a modern Amiga production.

The aircraft can move across a wide portion of the screen using all four arrow
keys. X and Z adjust throttle. Space fires the forward machine gun and B drops
bombs. Gun tracers point along their true velocity vector instead of always
being drawn horizontally. Bombs leave the aircraft with its momentum, fall
under gravity, strike either a target or the visible ground and produce a
multi-stage flash, fireball, debris burst and rising smoke column.

The battlefield is assembled from reusable authored elements so the campaign
can produce many coherent landscapes: distant sky, slow hills, villages and
buildings, then the full-speed foreground of trenches, crater fields, roads,
wire, trees, gun pits and troops. Four independent parallax rates establish
depth. High clouds and smoke banks move independently above the terrain.

Ground Vickers positions fire short, rate-limited bursts and have a realistic
elevation limit. Anti-aircraft artillery fires slower, inaccurate ballistic
shells which burst near the aircraft rather than travelling as sideways blobs.
Enemy aircraft attack from readable approaches, fire, manoeuvre and depart.
Damaged aircraft trail increasingly dense, billowing smoke before catching
fire. Bombs and shells never fall silently out of the world.

## Mission format 2: top-down operations

Top-down missions evoke the clarity of a vertical arcade shooter while keeping
the scale, aircraft and targets grounded in 1914-1918.

The detailed overhead aircraft moves in all four directions above a vertically
scrolling Western Front. The terrain contains fields, roads, trenches, ruined
settlements, shell holes, water and gun positions rather than a repeated flat
tile. A separate cloud layer drifts above the battlefield, creating moving
areas of haze and shadow.

Forward guns fire into the direction of travel. Bombing is explicitly a
height-based action: a released bomb begins small near the aircraft, separates
from its moving ground shadow, grows as it descends and can only damage a ground
target near the end of its fall. It then creates a proper ground impact with
expanding fire, thrown debris and smoke that rises and disperses. Targets fire
back, so the player must balance lining up an attack with avoiding predictable
and unpredictable fire.

The scrolling, scenery and projectile motion must agree about world direction.
Nothing should appear to slide independently of the ground or reverse direction
when the player moves.

## Mission format 3: single-seat dogfights

Single-seat perspective missions are a compact flight simulation rather than
a movable-cursor shooting gallery.

The player sees a detailed pilot cockpit, fixed Vickers guns and a fixed
gunsight. Arrow keys bank and pitch the aircraft; X and Z control throttle.
Bank, pitch, horizon movement, engine load and enemy projection respond as one
system. The player brings an opponent through the fixed sight by flying the
aircraft, not by moving the sight independently.

Enemy formations begin as small silhouettes at distance, grow continuously on
approach, cross laterally, climb, dive, break across the player's quarter and
continue through a close pass. Aircraft can fly overhead and leave through the
edge of the camera rather than disappearing at their largest size. Different
pilots use different approach paths, and enemy aggression, accuracy and
durability increase across the campaign.

Eccles, Gridpipe Thyne and Bluebottle can fly in loose formation, engage
opponents and be threatened themselves. The landscape below supplies a stable
sense of speed and altitude: damaged front line, roads, villages, rivers,
smoke columns and changing weather extend to the horizon.

## Mission format 4: two-seat rear-gunner defence

Rear-gunner missions are mechanically separate from dogfights. Tom flies a
scripted but visibly evasive course while Neddy swivels on the observer's stool
and operates one Lewis gun on a Scarff ring.

The arrow keys move a clear open crosshair through a 90-degree rear arc and up
to roughly 75 degrees of elevation. That crosshair is the authoritative hit
point. The Lewis gun sits visibly lower in the cockpit and points toward it;
the wide landscape, enemy aircraft, visible rear fuselage and tailplane follow
with damped movement rather than snapping to the gun. Tom's evasive banking
remains a separate motion. The gun, drum magazine and upper mount visibly
follow the aim while the yoke stays attached to the Scarff rail. The crosshair
turns red when the tail blocks the line of fire. Firing anyway damages the tail
controls, reduces Tom's steering authority and can end the sortie. Every burst
produces visible tracers from the lower gun through the crosshair. A defeated
aircraft does not become a mid-air fireball: it smokes, rolls out of control and
spirals down until it leaves the combat view.

Enemy aircraft use true depth choreography: distant approach, increasing
apparent size, lateral attack, close break, overhead pass and departure behind
the camera. Some make a direct firing run, some slash across the rear quarter,
and others climb over the tail. The player must traverse between threats rather
than wait for targets to enter one fixed point. Relative engine sound should
rise during a close approach and fall after the pass.

## Visual direction

The target presentation is a rich, authored 24-bit painted look presented on a
fixed 640x400 logical canvas. It should retain the strong silhouettes and
instant legibility associated with Amiga games while using the available
OpenGPU path for colour depth, detailed scenery and smooth presentation.

The visual language combines:

- painted landscapes and skies;
- detailed aircraft sprites at the correct viewpoint;
- layered parallax and atmospheric separation;
- wooden briefing boards, paper maps, journals, brass plaques and stitched
  canvas rather than generic computer panels;
- pristine native-size raster fonts, created separately for every displayed
  size rather than enlarged from one small bitmap;
- persistent debris, fire, smoke and cloud effects which explain gameplay
  events and make the battlefield feel active.

Windowed and full-screen presentation must show the same complete composition
with the correct aspect ratio. No mode may reveal missing layers, stretch the
art, change colours or depend on undefined swap-buffer contents.

## Audio direction

Music belongs to the narrative and dramatic screens. Period military-band,
music-hall and reflective material should be arranged for the game and passed
through a restrained gramophone treatment: narrow-band response, gentle
surface hiss and mechanical character without a repetitive artificial tick.
Different cues should support mobilisation, reconnaissance, loss, the mess,
victory and the Armistice.

Music stops whenever the player is in control. Gameplay is carried by engine
load, slipstream, other aircraft, vehicles, machine guns, Lewis guns,
artillery, impacts and damage. Engine pitch must respond continuously to
throttle and manoeuvre. Approaching and departing aircraft should be audible
in relative motion. Guns must sound like rapid mechanical guns; artillery must
have launch, travel/air and heavy impact character. Audio and visible action
must remain synchronised through AHI/ACAHI without periodic buffer stutter.

## Amiga technical target

The primary build targets AmigaOS 3 on the project's 68040/68881 toolchain.
The game uses the existing SDL2.module and OpenGPU/gfopenrtg stack; it does not
create a private replacement SDL or graphics system.

Software surfaces remain CPU-authoritative because game code, sprite masks and
legacy-compatible drawing can read their pixels directly. Presentation may use
the retained OpenGPU surface so unchanged tiles are not uploaded repeatedly.
Memory-to-memory copies stay on the CPU. The game must continue to run through
the software fallback with identical pixels.

The performance target is a stable, smooth 50/60 Hz presentation on the test
instance without one-second stalls, audio underruns or hidden periodic work.
Large effects and close aircraft must be bounded so they do not destroy frame
pacing. Every release drawer includes a clear game icon, drawer icon and an
obvious start icon.

## What constitutes a release candidate

A release candidate is not merely a build that reaches the title screen. It
must satisfy all of the following:

- title, menu, journal, briefing, all four mission modes, debrief and campaign
  progression work from the packaged drawer;
- every view has its final control scheme, visible weapons, enemy response,
  objectives, damage, success and failure paths;
- aircraft do not pop, scenery does not corrupt and full-screen/windowed modes
  preserve colour and aspect ratio;
- bombs visibly fall and strike targets or terrain; explosions have fire,
  debris and persistent rising smoke;
- perspective enemies approach, manoeuvre, pass the camera and depart with
  continuous depth and matching relative sound;
- rear guns traverse with the sight and recoil when fired;
- music stops during control, gameplay audio is synchronised and no recurring
  tick, buffer stutter or one-second hitch remains;
- save/progression data, icons and launcher survive a clean installation;
- the Amiga cross-build is warning-free, direct QA entry points for side,
  top-down, dogfight and gunner modes survive sustained play, and the normal
  launcher is restored before packaging;
- all source code, build rules, runtime assets and full-resolution original
  asset sources are committed to the repository.

The finished result should make the player want to see the next journal page,
but also want to replay the current mission because flying and fighting are
enjoyable in their own right.
