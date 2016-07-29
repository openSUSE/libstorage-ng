#!/usr/bin/python

# requirements: lvm vg test with lvm pv on /dev/sdb2


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

print staging

sdb2 = BlkDevice.find_by_name(staging, "/dev/sdb2")

sdb2.set_size(sdb2.get_size() + 512 * MiB)

print staging

commit(storage)

