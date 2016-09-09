#!/usr/bin/python

# requirements: disk /dev/sdb with empty partition sdb1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

sdb1 = Partition.find_by_name(staging, "/dev/sdb1")

luks = sdb1.create_encryption("cr_test")

luks.set_password("12345678")

test = LvmVg.create(staging, "test")
test.add_lvm_pv(luks)

test.create_lvm_lv("1", 1 * GiB)

print staging

commit(storage)

