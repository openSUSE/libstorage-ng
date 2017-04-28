#!/usr/bin/python

# requirements: lvm vg test with physical volumes sdb1 and sdb2, physical volume sdb2 unused


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print staging

lvm_vg = LvmVg.find_by_vg_name(staging, "test")

sdb2 = Partition.find_by_name(staging, "/dev/sdb2")

lvm_vg.remove_lvm_pv(sdb2)

print staging

commit(storage)

