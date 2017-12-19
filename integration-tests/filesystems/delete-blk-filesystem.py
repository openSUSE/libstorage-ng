#!/usr/bin/python3

# requirements: disk /dev/sdb with msdos partition table and partition
# /dev/sdb1 and some filesystem on /dev/sdb1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

sdb1 = Partition.find_by_name(staging, "/dev/sdb1")

sdb1.remove_descendants()

print(staging)

commit(storage)

