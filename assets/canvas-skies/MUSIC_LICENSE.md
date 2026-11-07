# Music credit and licence

## Provenance of every recording and film

Every sound recording and every piece of film the game ships is listed
here with the SHA-256 of the file in this repository and, where it came
from outside, the SHA-256 of the file it was made from. "Ours" means the
sound or picture is synthesised or rendered by this project and nothing
outside was recorded or sampled. The status column says how each is
cleared: proven from a named public archive item with a clear
public-domain or free licence, our own synthesis, or cleared by the
project owner.

| File | SHA-256 of the shipped file | Source | Performer | Recorded | Licence | Status |
|---|---|---|---|---|---|---|
| `colonel-bogey-gramophone.wav` | `06b6b3808c32c5e516f441cc48befdd5f709d7ca805ad1370bcb2f457764d223` | Ours: `ports/amigaos3/make_colonel_bogey.py` | Synthesised band | Rendered by the script | Tune by Kenneth J. Alford, 1914, public domain; render ours (GPL-3.0 with the game) | Proven |
| `cinema-piano.wav` | `64e500e57e1b4a76893de0b9fe94cbab881e9a2d48d38d31b2570e8bd5523994` | Ours: `ports/amigaos3/make_cinema_piano.py` | Synthesised piano | Rendered by the script | Composed for the game | Proven |
| `washington-post-gramophone.wav` | `ef6ec4b0b7571fc6bbea3c7e92a4c929ba3303a3edc1b6851d987d1d7fb354f8` | Ours, rendered from Mutopia piece 1321 (MIDI SHA-256 `d5b3295e89cea010736b57e8b11962d87ed1bfdb553a841acf84e28fc4552dfa`) | FluidR3_GM synthesis | Rendered 2 November 2026 | Composition public domain; MIDI CC BY-SA 3.0, so the render is CC BY-SA 3.0; FluidR3_GM MIT | Proven |
| `airfield-films.bin` (pictures and sound) | `33f74c8a4fd18e83727442b0c3416ec9e04e7ee8e723460a28001db258aebc44` | Ours: `ports/amigaos3/films/` and `make_airfield_films.py` | Rendered and synthesised | Rendered by the scripts | Ours (GPL-3.0 with the game) | Proven |
| `newsreels.bin` | `2bb6e03f9016a4d4980cafef26ba4a066e37dac536cfe32111482e458908bd0f` | Eight films of 1914-1921, listed in `CANVAS_SKIES_SOURCES.md` with their source SHA-256 in `ports/amigaos3/newsreel-sources.json` | Silent film | 1914-1921 | Public domain or CC0; see below | Kept; four films awaiting the project owner's rights confirmation |
| `amazing-grace-bagpipes.wav` | `523d80020a8f1291eafa969fa8283ac13d4dcae49c4d6cb1610db7f653c8bbb7` | Internet Archive item `AmazingGraceBagpipes`, file `amazing_grace.mp3` (SHA-256 `17c7ea3fa595ee8bf7b8eabaa225b32359ff910948af3ef51ce222619d2932e0`) | Band of the U.S. Air Force Reserve | Not stated by the item; a modern recording | Public domain, US federal government work | Kept by the project owner |
| `tipperary-gramophone.wav` | `33829dcb7c7ac7c4451295eb9860eb448e45667116102f2c34b493acac079bd1` | MP3 `2bab57142be64640a20f4c6a302d284a14fd6a9699b35eab25a1aa5234cf00f6`; archive item not recorded | Prince's Band (tag) | 1914 (tag) | Cleared by the project owner | Cleared by the project owner, 7 November 2026 |
| `home-fires-gramophone.wav` | `fa6aee87872acb4766896cf3b6b9e9b530f02e4fb8545866784716313618c163` | MP3 `b9acb6e5db8df31bf1dbf53bfee201658e27a26e7be99e852e7235b92e846a3a`; archive item not recorded | Frederick Wheeler (tag) | 1915 (tag) | Cleared by the project owner | Cleared by the project owner, 7 November 2026 |
| `kit-bag-gramophone.wav` | `ac7845c0be6f6b0a1d9a73e65b3d35782a2855419a3848f2f2e67d66bcfdd35d` | MP3 `6d91a03eea8ea284da7b3bf55581919c043a11a2ea80866d42524bed2ad6a285`; archive item not recorded | Knickerbocker Quartet (tag) | 1916 (tag) | Cleared by the project owner | Cleared by the project owner, 7 November 2026 |
| `holyrood-gramophone.wav` | `33be8c56189f748254a57e0b8fa6c424baf112ff48696c4b3348c14ce4518d92` | MP3 `3baa061647bafb136b1aed5ea9e949b261bf9879d9138a5278276ebc23f2916f`; archive item not recorded | Band of the Welsh Guards (tag) | Not recorded | Cleared by the project owner | Cleared by the project owner, 7 November 2026 |
| `voice-of-the-guns-gramophone.wav` | `aa48b70b2b8254bf287143217e4e3d329c098464490010895c18a9d3c0650295` | MP3 `45640c723b6338e4ad02862a3376e235b94df7efaf7fc729f5359a8548cbfb95`; archive item not recorded | Band of the Grenadier Guards (tag) | Not recorded | Cleared by the project owner | Cleared by the project owner, 7 November 2026 |
| `holst-second-suite-gramophone.wav` | `41192fe8077b5596e10208d766f9839a7b4c9f29555661983909722a40d91638` | MP3 `7cc49b8549dde5023136d2502ddf2b0a591c90e8e7bfc8e5e04c9644e454a234`; archive item not recorded | Band of the Irish Guards (tag) | Not recorded | Cleared by the project owner | Cleared by the project owner, 7 November 2026 |

What is known of each source, and what of it is recorded here, is set out
in its section below.

## The Washington Post

`washington-post-gramophone.wav` is a project-made render of John Philip
Sousa's 1889 composition *The Washington Post*. The composition is in the
public domain.

The source MIDI transcription, `washington-post.mid`, was prepared by Renato
Biolcati Rinaldi and published by the Mutopia Project under Creative Commons
Attribution-ShareAlike 3.0:

- https://www.mutopiaproject.org/cgibin/piece-info.cgi?id=1321
- MIDI file: https://www.mutopiaproject.org/ftp/SousaJP/sousa_washington_post/sousa_washington_post.mid
- https://creativecommons.org/licenses/by-sa/3.0/

The MIDI in this repository has SHA-256
`d5b3295e89cea010736b57e8b11962d87ed1bfdb553a841acf84e28fc4552dfa`, the same
as the file Mutopia serves (checked 7 November 2026). Because the render is
made from it, the render is shared under the same CC BY-SA 3.0 licence.

The MIDI was rendered with FluidR3_GM, copyright 2000-2002 and 2008 Frank Wen,
released under the MIT licence. The render was then transformed for this game
with band-limiting, horn resonance, wow/flutter, reduced resolution and
original shellac-noise treatment. The distributed game cue omits the first two
seconds of the render so the musical attack begins cleanly. No script for the
render is kept, so the shipped file (SHA-256
`ef6ec4b0b7571fc6bbea3c7e92a4c929ba3303a3edc1b6851d987d1d7fb354f8`) cannot be
rebuilt bit for bit; its inputs are those above.

The United States Library of Congress catalogue record for the 1889 work is:
https://www.loc.gov/item/2018563911/

## Colonel Bogey

`colonel-bogey-gramophone.wav` is the officers' mess and mission-complete cue.
It is our own synthesis, made by `ports/amigaos3/make_colonel_bogey.py`: a
military band (cornets, clarinets, piccolo, horns, bombardon, bass drum,
cymbal and side drum) playing Kenneth J. Alford's *Colonel Bogey March*,
then put through an acoustic gramophone's horn, the turntable's wow and
flutter, and shellac hiss and crackle. No recording or sample is used. The
script writes the same file on every run, SHA-256
`06b6b3808c32c5e516f441cc48befdd5f709d7ca805ad1370bcb2f457764d223`
(171.6 s, mono, 16-bit, 11025 Hz).

The march was published in 1914 by Hawkes & Son, London. Alford (F. J.
Ricketts) died in 1945, so the music is in the public domain in the United
Kingdom; it was published before 1931 and is in the public domain in the
United States. Only the tune was taken, read from a piano typesetting of
the march on Wikimedia Commons
(https://commons.wikimedia.org/wiki/File:Colonel_Bogey_March_piano_solo.pdf).
The harmony, the band scoring and the sound are our own.

The file opens with a quarter second of needle noise and ends with the last
chord dying into the run-out groove, so the game's crossfade from the end
into the start joins surface noise to surface noise.

The earlier cue (SHA-256
`fc035935c619239994ae0751d7d4eca79f911b63ff1364813a977c1e508e632e`, shipped
in 0.2.0 RC5) was replaced because its recording could not be traced. Its
tags name the Black Diamonds Band, 1925, and the MP3 set "Gramophone Marches
1912-1927". No archive item, source URL, source file or licence for it was
ever recorded in this repository. The earlier note that it was a
"royalty-free release" checked before release had nothing behind it.

The performance itself can be identified. Its loudness contour matches the
Internet Archive transfer
https://archive.org/details/78_15443-Colonel-Bogey-march (Black Diamonds
Band, Zonophone, matrix 2-40261; MP3 SHA-256
`2fdeb8c034163d84539e85c0addfe2332d5484cfc3a92cffb57e8aefb623a369`) at a
constant offset of 2.50 s over the whole side (correlation 0.95), so it is
the same transfer, running at the same speed. The samples do not match,
though: the waveform correlation is below 0.4. The cue was therefore made
from a processed copy of that transfer, not from the archive's file, and
that copy was never kept. The same matrix is dated March 1917 by the
George Blood transfer
(https://archive.org/details/78_colonel-bogey_the-black-diamonds-band-alforo_gbia3035871a),
not 1925 as the tag says. The archive gives no licence for either item.

## Amazing Grace: the fatal-sortie piper

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
`17c7ea3fa595ee8bf7b8eabaa225b32359ff910948af3ef51ce222619d2932e0`, and the
file the Internet Archive serves today has the same SHA-256 (its SHA-1,
`31026b7b6518a60045382d071cdef6ffa16f49a8`, matches the item's file list;
checked 7 November 2026). For the Amiga build it was converted, without
creative rearrangement, from 44.1 kHz stereo MP3 to 11.025 kHz, signed 16-bit
stereo PCM WAV; the shipped file has SHA-256
`523d80020a8f1291eafa969fa8283ac13d4dcae49c4d6cb1610db7f653c8bbb7`. The hymn
text by John Newton and the traditional New Britain tune are also public
domain.

**Release status: kept.** The source and its public-domain basis are exact.
The item gives no recording date and the album's tracks reach the 1990s, so
this is a modern recording; as a US federal government work it is in the
public domain, and the project owner decided on 7 November 2026 to keep it.

## Period gramophone records

The story, diary and briefing scenes play a period record matched to the
chapter's year; the epilogue plays Holst. Each comes from the collection
*British Military Music 1912-1927* (gramophone transfers, MP3, album tag
"Gramophone Marches 1912-1927"; archive SHA-256
`4d6c90419073d317d9445c2db8a0f04beea7057b77607ad368a095495465f4dc`).

For the Amiga build each MP3 was decoded to mono, signed 16-bit PCM at
11.025 kHz. The needle run-in and run-out were cut where the music begins
and ends, with a 0.3 s fade-in and a 1.5 s fade-out at the loop point. The
surface noise is kept.

| Game file | Title | Composer | Performer (tag) | Chapters | Source MP3 SHA-256 | Shipped WAV SHA-256 |
|---|---|---|---|---|---|---|
| `tipperary-gramophone.wav` | It's a Long, Long Way to Tipperary | Jack Judge and Harry Williams, 1912 | Prince's Band (1914) | 1914 | `2bab57142be64640a20f4c6a302d284a14fd6a9699b35eab25a1aa5234cf00f6` | `33829dcb7c7ac7c4451295eb9860eb448e45667116102f2c34b493acac079bd1` |
| `home-fires-gramophone.wav` | Keep the Home Fires Burning | Ivor Novello, words Lena Guilbert Ford, 1914 | Frederick Wheeler (1915) | 1915 | `b9acb6e5db8df31bf1dbf53bfee201658e27a26e7be99e852e7235b92e846a3a` | `fa6aee87872acb4766896cf3b6b9e9b530f02e4fb8545866784716313618c163` |
| `kit-bag-gramophone.wav` | Pack Up Your Troubles in Your Old Kit-Bag | Felix Powell, words George Asaf, 1915 | Knickerbocker Quartet (1916) | 1916 | `6d91a03eea8ea284da7b3bf55581919c043a11a2ea80866d42524bed2ad6a285` | `ac7845c0be6f6b0a1d9a73e65b3d35782a2855419a3848f2f2e67d66bcfdd35d` |
| `holyrood-gramophone.wav` | Holyrood | Kenneth J. Alford, 1912 | Band of the Welsh Guards | April 1917, March 1918 | `3baa061647bafb136b1aed5ea9e949b261bf9879d9138a5278276ebc23f2916f` | `33be8c56189f748254a57e0b8fa6c424baf112ff48696c4b3348c14ce4518d92` |
| `voice-of-the-guns-gramophone.wav` | The Voice of the Guns | Kenneth J. Alford, 1917 | Band of the Grenadier Guards | November 1917, August 1918 | `45640c723b6338e4ad02862a3376e235b94df7efaf7fc729f5359a8548cbfb95` | `aa48b70b2b8254bf287143217e4e3d329c098464490010895c18a9d3c0650295` |
| `holst-second-suite-gramophone.wav` | Second Suite in F for Military Band | Gustav Holst, 1911 | Band of the Irish Guards | November 1918, epilogue | `7cc49b8549dde5023136d2502ddf2b0a591c90e8e7bfc8e5e04c9644e454a234` | `41192fe8077b5596e10208d766f9839a7b4c9f29555661983909722a40d91638` |

**Release status: cleared by the project owner.** The compositions are in
the public domain. The archive item, source URL and licence for these
recordings are not recorded in this repository: the performer and year above
come from the MP3 tags, and three have no recording date. The recordings
were checked separately by the project owner, who cleared them for release
on 7 November 2026; the source and shipped SHA-256 above identify exactly
what was cleared.

Leads, not matches (no audio was compared): the Internet Archive's 78 rpm
collection holds the three Guards bands' records of these pieces, all on
the Boosey & Hawkes label, which was formed in 1930. *Holyrood*, Band of
the Welsh Guards, Capt. T. S. Chandler, matrix 7396; *The Voice of the
Guns*, Band of the Grenadier Guards, Capt. F. J. Harris, matrix 7391;
*Second Suite in F*, Band of the Irish Guards, Capt. G. H. Willcocks,
matrix 7255. If these are the sources, those three recordings date from
after 1930, despite the collection's title. For the other three, the
Library of Congress National Jukebox and the Internet Archive hold
Frederick Wheeler's *Keep the Home Fires Burning* (1915) and the
Knickerbocker Quartet's *Pack Up Your Troubles* (1916); no Prince's Band
*Tipperary* was found.

## The picture-house pianist

`cinema-piano.wav`, the accompaniment to the newsreels, is composed for the
game and synthesised by `ports/amigaos3/make_cinema_piano.py`. The script
rebuilds the shipped file exactly (SHA-256
`64e500e57e1b4a76893de0b9fe94cbab881e9a2d48d38d31b2570e8bd5523994`). No
recording or sample is used.

## The aerodrome films' sound

The soundtracks of the eight aerodrome films (stored with their pictures in
`airfield-films.bin`) are synthesised for this game by
`ports/amigaos3/films/sound.py`, from the same account of each film that its
pictures are drawn from: engine firings, the triangle-wave drone of the
game's own engine sound, the airscrew, wheels, thumps, the tail skid, wind,
the wires, rain and birdsong, finished as an optical track. No recording,
sample or outside sound of any kind is used. The films' pictures and how to
rebuild the file are described in `ART_PROVENANCE.md`.

## The newsreels

The film in `newsreels.bin` (SHA-256
`2bb6e03f9016a4d4980cafef26ba4a066e37dac536cfe32111482e458908bd0f`) is
silent; its sources, licences, catalogue records and source SHA-256 are in
`CANVAS_SKIES_SOURCES.md` and `ports/amigaos3/newsreel-sources.json`.
**Release status: kept; rights confirmation being sought by the project
owner.** All eight films date from 1914-1921 and are tagged public domain or
CC0 where they are hosted. The US National Archives' own catalogue marks four
of them "Copyright: Restricted - Possibly" or "Undetermined" (see that file);
the project owner is seeking confirmation from the Archives for those four.
