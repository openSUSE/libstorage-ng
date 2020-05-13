#!/usr/bin/python3

# requirements: lvm volume group test with thin-pool thin-pool and thin thin1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

test = LvmVg.find_by_vg_name(staging, "test")

thin_pool = test.get_lvm_lv("thin-pool")

thin1 = thin_pool.get_lvm_lv("thin1")

thin1.remove_descendants(View_REMOVE)
staging.remove_device(thin1)

print(staging)

commit(storage)

