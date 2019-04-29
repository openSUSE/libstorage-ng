#!/usr/bin/python3

# requirements: btrfs on /dev/sdc1 and unused /dev/sdd1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")

sdd1 = Partition.find_by_name(staging, "/dev/sdd1")
sdd1.set_id(ID_LINUX)

btrfs = to_btrfs(sdc1.get_blk_filesystem())
btrfs.add_device(sdd1)

print(staging)

commit(storage)

