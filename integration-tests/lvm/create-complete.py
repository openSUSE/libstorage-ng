#!/usr/bin/python3

# requirements: disk /dev/sdc with empty partitions sdc1 and sdd1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")
sdd1 = Partition.find_by_name(staging, "/dev/sdd1")

test = LvmVg.create(staging, "test")
test.add_lvm_pv(sdc1)
test.add_lvm_pv(sdd1)

test.create_lvm_lv("1", LvType_NORMAL, 1 * GiB)
test.create_lvm_lv("2", LvType_NORMAL, 1 * GiB)
test.create_lvm_lv("3", LvType_NORMAL, 1 * GiB)

print(staging)

commit(storage)

