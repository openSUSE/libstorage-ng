#!/usr/bin/python

from storage import Storage, PROBE_NORMAL


storage = Storage(PROBE_NORMAL, True)

probed = storage.getProbed()

probed.print_graph()

