#!/usr/bin/python3

# requirements: disk /dev/sdc with empty partition sdc1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")

luks = sdc1.create_encryption("cr-test")

luks.set_password("12345678")

test = LvmVg.create(staging, "test")
test.add_lvm_pv(luks)

test.create_lvm_lv("1", LvType_NORMAL, 1 * GiB)

print(staging)

commit(storage)

