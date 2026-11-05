#!/usr/bin/env python3
"""make_single_gun_cockpit.py ARMED.bmp UNARMED.bmp OUT.bmp: the pilot's cockpit with one
Vickers.  From 1915 until the twin-gun Camels of late 1917 a scout carried a single
synchronised gun, offset to the left of the centreline as on the S.E.5 and Pup.  The left gun,
its mounting and the ring sight are lifted from the twin-gun painting onto the gun-less decking;
everything else, including the decking where the right gun stood, is the unarmed painting."""
import sys
import numpy as np
from PIL import Image

armed = np.asarray(Image.open(sys.argv[1]).convert("RGB"))
out = np.asarray(Image.open(sys.argv[2]).convert("RGB")).copy()
key = (armed[:, :, 0] > 200) & (armed[:, :, 2] > 200) & (armed[:, :, 1] < 80)
take = np.zeros(key.shape, bool)
take[222:306, 216:306] = True        # the left gun, its cocking handle and mounting
take[182:276, 312:334] = True        # the ring sight and its post, down to the decking
take[182:226, 300:346] = True        # the whole of the ring
take &= ~key
out[take] = armed[take]
Image.fromarray(out).save(sys.argv[3])
