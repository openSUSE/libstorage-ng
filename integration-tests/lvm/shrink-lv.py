#!/usr/bin/python

# requirements: lvm vg test with lvm lv normal1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

normal1 = BlkDevice.find_by_name(staging, "/dev/test/normal1")

normal1.set_size(normal1.get_size() - 512 * MiB)

print(staging)

commit(storage)

