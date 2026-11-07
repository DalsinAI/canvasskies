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

## Period gramophone records

The story, diary and briefing scenes play a period record matched to the
chapter's year; the epilogue plays Holst. Each comes from the collection
*British Military Music 1912-1927* (gramophone transfers, MP3, album tag
"Gramophone Marches 1912-1927"; archive SHA-256
`4d6c90419073d317d9445c2db8a0f04beea7057b77607ad368a095495465f4dc`). The
compositions and these recordings are in the public domain.

For the Amiga build each MP3 was decoded to mono, signed 16-bit PCM at
11.025 kHz. The needle run-in and run-out were cut where the music begins
and ends, with a 0.3 s fade-in and a 1.5 s fade-out at the loop point. The
surface noise is kept.

| Game file | Title | Composer | Performer (tag) | Chapters | Source MP3 SHA-256 |
|---|---|---|---|---|---|
| `tipperary-gramophone.wav` | It's a Long, Long Way to Tipperary | Jack Judge and Harry Williams, 1912 | Prince's Band (1914) | 1914 | `2bab57142be64640a20f4c6a302d284a14fd6a9699b35eab25a1aa5234cf00f6` |
| `home-fires-gramophone.wav` | Keep the Home Fires Burning | Ivor Novello, words Lena Guilbert Ford, 1914 | Frederick Wheeler (1915) | 1915 | `b9acb6e5db8df31bf1dbf53bfee201658e27a26e7be99e852e7235b92e846a3a` |
| `kit-bag-gramophone.wav` | Pack Up Your Troubles in Your Old Kit-Bag | Felix Powell, words George Asaf, 1915 | Knickerbocker Quartet (1916) | 1916 | `6d91a03eea8ea284da7b3bf55581919c043a11a2ea80866d42524bed2ad6a285` |
| `holyrood-gramophone.wav` | Holyrood | Kenneth J. Alford, 1912 | Band of the Welsh Guards | April 1917, March 1918 | `3baa061647bafb136b1aed5ea9e949b261bf9879d9138a5278276ebc23f2916f` |
| `voice-of-the-guns-gramophone.wav` | The Voice of the Guns | Kenneth J. Alford, 1917 | Band of the Grenadier Guards | November 1917, August 1918 | `45640c723b6338e4ad02862a3376e235b94df7efaf7fc729f5359a8548cbfb95` |
| `holst-second-suite-gramophone.wav` | Second Suite in F for Military Band | Gustav Holst, 1911 | Band of the Irish Guards | November 1918, epilogue | `7cc49b8549dde5023136d2502ddf2b0a591c90e8e7bfc8e5e04c9644e454a234` |

## The aerodrome films' sound

The soundtracks of the eight aerodrome films (stored with their pictures in
`airfield-films.bin`) are synthesised for this game by
`ports/amigaos3/films/sound.py`, from the same account of each film that its
pictures are drawn from: engine firings, the triangle-wave drone of the
game's own engine sound, the airscrew, wheels, thumps, the tail skid, wind,
the wires, rain and birdsong, finished as an optical track. No recording,
sample or outside sound of any kind is used.

