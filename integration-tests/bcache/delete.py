#!/usr/bin/python3

# requirements: some bcaches on /dev/sdc1, /dev/sdc2 and /dev/sdd1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")
sdc1.remove_descendants()

sdc2 = Partition.find_by_name(staging, "/dev/sdc2")
sdc2.remove_descendants()

sdd1 = Partition.find_by_name(staging, "/dev/sdd1")
sdd1.remove_descendants()

print(staging)

commit(storage)

