#!/usr/bin/python

# requirements: lvm volume group test without logical volume foo


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

test = LvmVg.find_by_vg_name(staging, "test")

test.create_lvm_lv("foo", 50 * MiB)

print staging

commit(storage)

