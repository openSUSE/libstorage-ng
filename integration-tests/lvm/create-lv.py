#!/usr/bin/python

# requirements: lvm volume group test without logical volume normal1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

test = LvmVg.find_by_vg_name(staging, "test")

normal1 = test.create_lvm_lv("normal1", LvType_NORMAL, 1 * GiB)
# normal1.set_stripes(2)
# normal1.set_stripe_size(256 * KiB)

print staging

check(storage)
commit(storage)

