#!/usr/bin/python3

# requirements: unused logical volume a in volume group test1

# The operation is not supported. On it own it works but not in
# combination with other operations.


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

lvm_vg = LvmVg.find_by_vg_name(staging, "test1")

lvm_lv = lvm_vg.get_lvm_lv("a")
lvm_lv.set_lv_name("b")

print(staging)

commit(storage)
