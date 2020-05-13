#!/usr/bin/python3

# requirements: volume group 'test' with missing physical volume


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

test = LvmVg.find_by_vg_name(staging, "test")

test.remove_descendants(View_REMOVE)

for pv in test.get_lvm_pvs():
    staging.remove_device(pv)

staging.remove_device(test)

print(staging)

commit(storage)

