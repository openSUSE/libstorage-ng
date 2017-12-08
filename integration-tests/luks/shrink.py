#!/usr/bin/python

# requirements: partition /dev/sdb1 with size at least 1GiB and luks on it,
# possible also something on luks, e.g. filesystem or lvm pv


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

sdb1 = BlkDevice.find_by_name(staging, "/dev/sdb1")

sdb1.set_size(sdb1.get_size() - 512 * MiB)

print(staging)

commit(storage)

