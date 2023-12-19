#!/usr/bin/python3

# requirements: unused partitions /dev/sdc1, /dev/sdc2 and /dev/sdd1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")
sdc2 = Partition.find_by_name(staging, "/dev/sdc2")

sdd1 = Partition.find_by_name(staging, "/dev/sdd1")

bcache_cset = sdd1.create_bcache_cset()

bcache0 = sdc1.create_bcache("/dev/bcache0")
bcache0.attach_bcache_cset(bcache_cset)
bcache0.create_blk_filesystem(FsType_EXT4)

bcache1 = sdc2.create_bcache("/dev/bcache1")
bcache1.attach_bcache_cset(bcache_cset)
bcache1.create_blk_filesystem(FsType_XFS)

print(staging)

commit(storage)

