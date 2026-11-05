#!/usr/bin/env python3
"""make_single_gun_cockpit.py ARMED.bmp UNARMED.bmp OUT.bmp: the pilot's cockpit with one
Vickers.  From 1915 until the twin-gun machines of late 1917 a scout carried a single
synchronised gun on the centreline under the ring sight.  The left gun of the twin-gun painting
is lifted off, moved to the centre, and laid on the gun-less decking; the ring sight goes back on
top of it.  Everything else is the unarmed painting."""
import sys
import numpy as np
from PIL import Image

armed = np.asarray(Image.open(sys.argv[1]).convert("RGB")).astype(int)
out = np.asarray(Image.open(sys.argv[2]).convert("RGB")).copy()
differs = np.abs(armed - out.astype(int)).sum(axis=2) > 30
key = (armed[:, :, 0] > 200) & (armed[:, :, 2] > 200) & (armed[:, :, 1] < 80)
shift = 59                            # the left gun's centre, x 261, onto x 320
gun = np.zeros(differs.shape, bool)
gun[194:306, 212:300] = True
gun[194:234, 290:300] = False         # the ring sight's edge stays put
grown = differs.copy()                # close the small holes in the gun
for dy in (-2, -1, 0, 1, 2):
    for dx in (-2, -1, 0, 1, 2):
        grown |= np.roll(np.roll(differs, dy, 0), dx, 1)
gun &= grown & ~key
sight = np.zeros(differs.shape, bool)
sight[186:276, 300:346] = True
sight &= differs & ~key
ys, xs = np.nonzero(gun)
ok = xs + shift < out.shape[1]
out[ys[ok], xs[ok] + shift] = armed[ys[ok], xs[ok]]
out[sight] = armed[sight]
Image.fromarray(out.astype(np.uint8)).save(sys.argv[3])
