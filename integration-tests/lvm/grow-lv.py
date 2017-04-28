#!/usr/bin/python

# requirements: lvm vg test with lvm lv 1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print staging

lvm_lv = BlkDevice.find_by_name(staging, "/dev/test/1")

lvm_lv.set_size(lvm_lv.get_size() + 512 * MiB)

print staging

commit(storage)

