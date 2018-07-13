#!/usr/bin/python3

# requirements: some bcaches on /dev/sdb1, /dev/sdb2 and /dev/sdc1


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

sdb2 = Partition.find_by_name(staging, "/dev/sdb2")
sdb2.remove_descendants()

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")
sdc1.remove_descendants()

print(staging)

commit(storage)

