#!/usr/bin/python3

# requirements: lvm vg test with physical volume sdc1, unused sdc2


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

lvm_vg = LvmVg.find_by_vg_name(staging, "test")

sdc2 = Partition.find_by_name(staging, "/dev/sdc2")

lvm_vg.add_lvm_pv(sdc2)

print(staging)

commit(storage)

