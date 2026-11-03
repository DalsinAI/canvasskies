# Music credit and licence

`washington-post-gramophone.wav` is a project-made render of John Philip
Sousa's 1889 composition *The Washington Post*. The composition is in the
public domain.

The source MIDI transcription, `washington-post.mid`, was prepared by Renato
Biolcati Rinaldi and published by the Mutopia Project under Creative Commons
Attribution-ShareAlike 3.0:

- https://www.mutopiaproject.org/cgibin/piece-info.cgi?id=1321
- https://creativecommons.org/licenses/by-sa/3.0/

The MIDI was rendered with FluidR3_GM, copyright 2000-2002 and 2008 Frank Wen,
released under the MIT licence. The render was then transformed for this game
with band-limiting, horn resonance, wow/flutter, reduced resolution and
original shellac-noise treatment. The distributed game cue omits the first two
seconds of the render so the musical attack begins cleanly.

The United States Library of Congress catalogue record for the 1889 work is:
https://www.loc.gov/item/2018563911/

## Colonel Bogey

`colonel-bogey-gramophone.wav` is the officers' mess and mission-complete cue.
Its first two seconds have been removed and an offline autoregressive de-click
pass has reduced the embedded impulse noise. The game also crossfades the end
of the decoded cue into its beginning.

The composition *Colonel Bogey March* was written by Kenneth J. Alford in 1914.
The recording used is a royalty-free release; its provenance was checked by
the project before public release.

## Amazing Grace — fatal-sortie piper

`amazing-grace-bagpipes.wav` is the solo-pipes cue used after a fatal failed
sortie. It is a conversion of `amazing_grace.mp3` from the Internet Archive
item *Amazing Grace Bagpipes*:

- https://archive.org/details/AmazingGraceBagpipes
- identifier: `AmazingGraceBagpipes`
- recording artist: Band of the U.S. Air Force Reserve
- creator credited by the item: Air Force Bands Program
- album metadata: *Our Citizen Airmen*, track 17

The Internet Archive item marks the recording Public Domain and says that the
Air Force public information may be distributed or copied, requesting an
appropriate byline/credit. The game and this repository therefore credit the
Band of the U.S. Air Force Reserve and Air Force Bands Program even though the
item does not require a copyright licence.

The supplied MP3 had SHA-256
`17c7ea3fa595ee8bf7b8eabaa225b32359ff910948af3ef51ce222619d2932e0`.
For the Amiga build it was converted, without creative rearrangement, from
44.1 kHz stereo MP3 to 11.025 kHz, signed 16-bit stereo PCM WAV. The hymn text
by John Newton and the traditional New Britain tune are also public domain.
