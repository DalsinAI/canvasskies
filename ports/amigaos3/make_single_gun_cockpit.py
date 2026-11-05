#!/usr/bin/env python3
"""make_single_gun_cockpit.py ARMED.bmp UNARMED.bmp OUT.bmp: the pilot's cockpit with one
Vickers.  From 1915 until the twin-gun machines of late 1917 a scout carried a single
synchronised gun on the centreline under the ring sight.  The whole of the left gun of the
twin-gun painting, with the decking it sits on, is lifted as one block and laid on the gun-less
decking so that the gun's centre is on the screen's centre line; the ring sight and its post go
back on top.  Everything else is the unarmed painting."""
import sys
import numpy as np
from PIL import Image

armed = np.asarray(Image.open(sys.argv[1]).convert("RGB"))
out = np.asarray(Image.open(sys.argv[2]).convert("RGB")).copy()
key = (armed[:, :, 0] > 200) & (armed[:, :, 2] > 200) & (armed[:, :, 1] < 80)
X0, X1, Y0, Y1 = 212, 310, 190, 304       # the left gun and its mounting
CENTRE = 253                               # the breech's centre in that block
shift = 320 - CENTRE
unarmed = np.asarray(Image.open(sys.argv[2]).convert("RGB")).astype(int)
differs = (np.abs(armed.astype(int) - unarmed).sum(axis=2) > 24)[Y0:Y1, X0:X1]
filled = Image.fromarray((differs * 255).astype(np.uint8))
from PIL import ImageFilter
filled = filled.filter(ImageFilter.MaxFilter(7)).filter(ImageFilter.MinFilter(5))
block = (np.asarray(filled) > 0) & ~key[Y0:Y1, X0:X1]
block[:232 - Y0, 296 - X0:] = False      # not the ring sight
ys, xs = np.nonzero(block)
# The left gun was painted turned toward the centre, its barrel jacket
# off to the right of the breech.  Laid on the centreline, the jacket is
# swung back over the breech: rows above the breech move left, more the
# higher they are, so the jacket lies along the line of sight.
lean = np.clip((285 - (ys + Y0)) / 45.0, 0.0, 1.0) * 40.0
nx = (xs + X0 + shift - lean).round().astype(int)
out[ys + Y0, nx] = armed[ys + Y0, xs + X0]
# The ring sight and its post, from the twin-gun painting, over the gun.
sight = np.zeros(key.shape, bool)
sight[186:232, 298:346] = True             # the ring
sight[186:262, 316:326] = True             # the post down to the gun
sight &= ~key
dark = armed.astype(int).sum(axis=2) < 200 # the sight is dark iron
sight[232:262] &= dark[232:262]
out[sight] = armed[sight]
Image.fromarray(out).save(sys.argv[3])
