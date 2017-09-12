#!/usr/bin/python

# requirements: lvm volume group test with logical volume foo


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

test = LvmVg.find_by_vg_name(staging, "test")

lv = test.get_lvm_lv("foo")

lv.remove_descendants()
staging.remove_device(lv)

print staging

commit(storage)

