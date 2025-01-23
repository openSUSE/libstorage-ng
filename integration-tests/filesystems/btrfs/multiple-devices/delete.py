#!/usr/bin/python3

# requirements: btrfs on /dev/sdc1 and /dev/sdd1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")
sdc1.remove_descendants(View_REMOVE)

print(staging)

commit(storage)

