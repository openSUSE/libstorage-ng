#!/usr/bin/python

# requirements: lvm volume group test with logical volume normal1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

test = LvmVg.find_by_vg_name(staging, "test")

normal1 = test.get_lvm_lv("normal1")

normal1.remove_descendants()
staging.remove_device(normal1)

print staging

commit(storage)

