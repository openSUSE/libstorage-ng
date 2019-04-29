#!/usr/bin/python3

# requirements: btrfs on /dev/sdc1 and /dev/sdd1

# note: removing devices from btrfs is not supported by libstorage-ng
# so do not use it in production code


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

btrfs = to_btrfs(sdc1.get_blk_filesystem())
btrfs.remove_device(sdd1)

print(staging)

commit(storage)

