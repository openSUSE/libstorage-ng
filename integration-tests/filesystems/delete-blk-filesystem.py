#!/usr/bin/python3

# requirements: disk /dev/sdd with msdos partition table and partition
# /dev/sdd1 and some filesystem on /dev/sdd1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

sdd1 = Partition.find_by_name(staging, "/dev/sdd1")

sdd1.remove_descendants()

print(staging)

commit(storage)

