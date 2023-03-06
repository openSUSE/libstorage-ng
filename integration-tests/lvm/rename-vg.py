#!/usr/bin/python3

# requirements: unused volume group test1

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
lvm_vg.set_vg_name("test2")

print(staging)

commit(storage)
