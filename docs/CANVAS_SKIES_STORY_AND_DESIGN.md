# Canvas Skies: story and design bible

## The promise

*Canvas Skies* is a cinematic First World War flying game for the Amiga. It
begins in August 1914, when an aeroplane is treated as a fragile curiosity, and
ends at eleven o'clock on 11 November 1918. The player lives that whole change
through one pilot rather than jumping between anonymous machines.

The game grows out of *Sopwith* and *Scramble*, but it is not meant to remain a
re-skinned version of either. Its presentation takes its cue from the human,
period-drama quality of Cinemaware's *Wings*: journal entries, orders pinned to
wooden boards, named friends, changing technology, loss, promotion and a final
return home. The action alternates between side-on attack missions, top-down
operations, cockpit dogfights and rear-gunner defence.

It must remain self-contained and able to run on an Amiga. There is no online
service, generated behaviour or model at runtime. Flight, enemy behaviour,
story, audio and rendering all ship with the game.

## The people

The player is **Neddy Seagoon**, a Royal Flying Corps pilot. He begins as a man
trying to keep an unreliable observation machine above the fields and ends as
the veteran whom new pilots watch before take-off. The journal is written in
his voice. He is capable, observant and often funny, but never glib about the
cost of the war.

His recurring squadron friends are:

- **Eccles**, a loyal pilot who will bring a damaged friend home.
- **Gridpipe Thyne**, an experienced pilot whose absence eventually says more
  than a formal death scene could.
- **Bluebottle**, younger and outwardly cheerful, using humour against fear.
- **Tom**, Neddy's observer and rear gunner. Tom maps the ground, operates the
  camera and wireless, and later stands behind a Lewis gun. He is wounded over
  the Somme and returns with a stiff arm.
- **Alfie**, the mechanic who coaxes another flight from machines assembled out
  of bad parts, mud and optimism.
- **Clara**, connected to the field hospital train and the life continuing
  beneath the aerodromes and battle lines.

The names carry a comic British warmth, but the world around them is played
straight. Their humour should make them human, not turn the war into farce.

## Story structure

There are eight chapters and twelve missions in each chapter: 96 missions in
all. Every chapter mixes the four forms of play. The canonical mission order,
titles and Neddy's current journal lines live in
`src/modern/canvas_campaign.h`; that file is the source of truth used by the
game.

### Chapter 1 — Eyes Above the Marne, August 1914

The aeroplane has no accepted military role. Neddy and Tom fly with a map and
pencil, carry messages, count columns and discover that a view from the sky can
change an army's direction. Encounters with enemy aviators are uncertain and
almost courteous because neither side yet understands what aerial war will
become. The chapter ends with the aeroplane having earned its place.

Missions: A Field in France; The First Message; A Stranger Above; Columns on
the Road; The Marne Bridge; Horses in the Sky; The Empty Flank; Rain Over
Meaux; A Face in the Wind; Turn of the Armies; Orders at Dusk; Eyes Above the
Marne.

### Chapter 2 — The War Learns to Fly, 1915

Cameras, wireless sets and weapons enter the air. The Fokker interrupter gear
changes an awkward encounter into a lethal one, while Tom's Lewis gun gives the
two-seater a defence. Neddy claims his first victory and discovers that a mark
on a board does not feel like a victory.

Missions: Winter Photographs; The Wireless Set; Pistols at Altitude; Gas Cloud;
Alfie's Engine; The Interrupter; Lost Battery; No Man's Land; Tom's Lewis Gun;
The Salient; The Burning Hangar; The War Learns to Fly.

### Chapter 3 — Over the Somme, 1916

The ground becomes an industrial ruin and the air service is asked to observe,
bomb, photograph, resupply and fight over it. Eccles brings Neddy home after a
desperate flight. Tom is wounded and, for a time, his empty seat is filled by a
quiet replacement. The squadron wins very little and loses a great deal.

Missions: Before the Barrage; Cut the Balloon Line; Above Albert; Zero Hour;
The First Trench; Eccles' Wing; A Red Rocket; Supplies at Pozieres; The Empty
Seat; The New Trench; Rain and Chalk; Over the Somme.

### Chapter 4 — Bloody April, 1917

The enemy's better machines and trained formations punish inexperienced RFC
crews. Neddy sees missing patrols, writes letters to families and refuses the
safety of a desk when promotion arrives. Tom returns to the rear cockpit, but
the chapter closes on empty chairs and a squadron almost remade from scratch.

Missions: Arras at Dawn; The Forward Strip; The Circus; Missing Patrol; Vimy
Ridge; Six Against Two; The Photograph; Tom Returns; Captain Mercer; Empty
Chairs; The Rail Cutting; Bloody April.

### Chapter 5 — The Iron Road, late 1917

At Cambrai, tanks, aircraft and artillery are expected to work as one system.
The game does not treat tanks as anti-aircraft weapons: the player attacks gun
positions, roads and reserves around them. New Albatros scouts raise the aerial
threat. Gridpipe Thyne turns east and disappears into cloud, leaving no neat
answer. The offensive proves that a new invention cannot end the war by itself.

Missions: Tanks in the Mist; The Gun Line; High Cover; The Canal Map; Road to
Masnieres; The New Albatros; Counterattack; The Stranded Crew; Thyne's Last
Turn; Ground Lost Again; Bring Alfie Home; The Iron Road.

### Chapter 6 — The Spring Storm, March 1918

The line breaks and the squadron retreats west through fog, refugee columns and
abandoned fields. Supplies are destroyed before capture. Neddy protects
Clara's hospital train, helps lost pilots find the line and eventually sees the
red pencil on the map stop moving west. Survival is not presented as triumph.

Missions: Fog on the Somme; Burn the Stores; Westward; The Broken Line; The
Field Hospital; Tom's Flare; Amiens Road; The Last Crate; Hold the Sky; The Line
Holds; Counterstroke; The Spring Storm.

### Chapter 7 — The Hundred Days, August 1918

The Allied advance begins. The front moves quickly enough that maps become old
twice in one day. Neddy is now the veteran, flying with British, French and
American aircraft while the enemy still produces dangerous pilots. The open
country beyond the trenches feels strange, and the nearness of the end becomes
another thing to fear.

Missions: Amiens at Four; Break the Battery; Sky of Many Flags; The Moving
Front; The Railhead; The Veteran; Canal du Nord; Open Country; The Last Ace;
Retreat East; The Bells of Cambrai; The Hundred Days.

### Chapter 8 — The Eleventh Hour, November 1918

Rumours of an armistice arrive before the firing stops. Neddy sees a train
carrying negotiators, meets an old enemy in the air and carries the order that
the guns must stop at eleven. The final missions count down in minutes. They are
not a power fantasy: the objective is to prevent another needless death and be
home when the bells begin.

Missions: November Map; A Letter from Clara; No Easy Truce; The White Train;
Last Battery; The Old Enemy; Orders for Eleven; Ten Thirty; Ten Forty-Five; Ten
Fifty-Five; The Final Message; The Eleventh Hour.

## Mission forms

### Side-on operations

These keep the readable immediacy of a side-scrolling shooter but give the
aircraft room to move in both axes. The player strafes, bombs, observes and
crosses a four-layer landscape: sky, distant hills, buildings and foreground.
The layers move at different speeds and are assembled from reusable elements so
the 96 missions need not repeat one panorama.

Machine guns fire short, height-limited bursts. Artillery fires slower,
less-accurate arcing shells. Bombs that miss a target still hit the ground,
burst upward and leave rising, fading smoke. Enemy aircraft and ground weapons
can damage the player's machine.

### Top-down operations

The player's aircraft is the only friendly aircraft on screen. Roads, trenches,
guns, buildings and moving vehicles scroll with the ground. Bombs fall from
height rather than acting like side-on projectiles. Ground explosions belong to
world coordinates and must continue scrolling with the landscape, never follow
the aircraft. Their art and animation are viewed from above.

### Pilot dogfights

This is a flight simulator, not a collection of sprites sliding over a picture.
The aircraft has a compact six-degree-of-freedom model, forward-only flight,
banking turns, climb, dive, energy loss, stall, terrain impact and collision.
The horizon, sky, clouds, hills and ground respond coherently to pitch, heading
and bank. Left banks left, Right banks right, Down pulls the nose up and Up
pushes it down. X and Z change throttle. Space fires the forward guns.

Enemy aircraft must obey the same visual logic. They enter by diving, crossing
or catching the player; they do not pop into existence or fly sideways. They
aim in the direction their noses point, break away before collision and become
harder as the war advances. A damaged aircraft takes several hits, smokes and
spirals down. It does not simply explode in mid-air. A ground impact has its own
crash sound and effect. If two aircraft collide, both are lost and both fall.

### Rear-gunner defence

The pilot flies the two-seater while the player moves around the rear cockpit.
The intended British installation is a single Lewis gun on its mounting rail.
The gun sits low in the frame, traverses around an approximately 90-degree rear
arc and elevates up to roughly 75 degrees. The crosshair is separate from the
weapon; the muzzle points toward it and bullets emerge from the muzzle. The
tail and tailplane remain visible and block unsafe fire.

One or two enemy machines approach, dodge, weave, close and pull away. They do
not sit in a fixed cluster or face away from the gunner. Pilot evasive movement
changes the horizon, visible airframe and attackers independently of the
gunner's aim.

## Presentation

- The title is a centred fabric banner, taller than a normal text plate, moving
  as cloth in wind. Its lettering is stitched into the asset.
- The first prompt is a Victorian plaque. It opens an officers' menu where the
  player can begin in 1914 or join a later year.
- Before every mission, Neddy's journal appears with lettering aligned to the
  ruled paper. A commander-in-chief briefing follows on a wooden operations
  board.
- Fonts are drawn separately for each required size. They are not scaled from
  one bitmap. Field-sign text is centred.
- Gameplay information sits on military-style signs at the bottom, keeping the
  combat sky clear. Dogfights show enemies remaining, ammunition, airspeed,
  altitude, fuel and airframe condition.
- Windowed and fullscreen presentation preserve the same 640:400 (8:5) scene
  without stretching. F9 toggles fullscreen; F10 is deliberately not used.

## Audio

Menus, journals, briefings and debriefs use period military-band music with a
gramophone character. The selection should match the emotional weight of the
scene rather than reuse one success cue everywhere. Interactive missions stop
the music and use engine, wind, other aircraft, vehicles, guns, hits, crashes
and explosions.

Machine guns must sound like machine guns, not clicks or lasers. Artillery must
have a distinct report and distant detonation. A hit on either the player or an
enemy needs audible confirmation. Engine pitch and load should respond to the
aircraft rather than loop as an unrelated sample. No recurring tick is
acceptable in music or engine playback.

## Historical tone and research

The story is fictional, but its texture should come from real journals and
contemporary language. Use source material for routine, weather, fear,
maintenance, observation and the changing role of aircraft; do not lift diary
entries verbatim or turn real people into the fictional cast.

Starting references supplied during development:

- Smithsonian transcription project, Air and Space collection:
  https://transcription.si.edu/projects/air-and-space/17872
- Army Flying Museum, diary of Capt. E. J. Garland, RFC:
  https://armyflying.com/the-collections/online-exhibitions/diary-of-capt-e-j-garland-rfc/

Before release, historical aircraft, weapons, ranks, dates and unit terminology
should receive a dedicated accuracy pass. Music and every sourced visual or
recording must also have explicit provenance in the repository.

