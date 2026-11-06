# Memory per scene, Lab-51

Fast memory the game holds 25 s into each scene (10 s for the side-on sortie,
which otherwise crashes before the reading), measured with `Avail FLUSH` before
and after launching it. rc4 held about 63 MB in every scene.

| Scene | QA entry | Cache off | Cache on |
|---|---|---|---|
| Title | (none) | 15.7 MB | |
| Settings page | SETTINGS | 8.1 MB | |
| Side-on sortie | SIDE 1 | 14.7 MB | 23.2 MB |
| Top-down sortie | TOPMIX 1 | 12.5 MB | |
| Cockpit dogfight | DOGFIGHT 5 | 15.6 MB | |
| Rear seat, Lewis gun | GUNNER | 23.1 MB | |
| Rear seat, 1914 carbine | UNARMED_GUNNER | 13.5 MB | |
| Picture house | CINEMA 3 | 9.0 MB | |
| Mess | MESS | 11.6 MB | |
| Newspaper | NEWS | 17.3 MB | |
| Diary | DIARY | 17.2 MB | |
| Debrief | DEBRIEF | 9.4 MB | |
| Award | AWARD 1 | 11.3 MB | |
| Dawn | DAWN | 13.2 MB | |
| Reprimand | REPRIMAND | 8.2 MB | |
| Every scene's art loaded | SETTINGS ALL | | 47.5 MB |

With cache on, art stays once used, so the figure grows towards the last row
as a campaign goes on.
