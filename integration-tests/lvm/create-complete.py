#!/usr/bin/python

# requirements: disk /dev/sdb with empty partitions sdb1 and sdb2


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

sdb1 = Partition.find_by_name(staging, "/dev/sdb1")
sdb2 = Partition.find_by_name(staging, "/dev/sdb2")

test = LvmVg.create(staging, "test")
test.add_lvm_pv(sdb1)
test.add_lvm_pv(sdb2)

test.create_lvm_lv("1", 1 * GiB)
test.create_lvm_lv("2", 1 * GiB)
test.create_lvm_lv("3", 1 * GiB)

print staging

commit(storage)

