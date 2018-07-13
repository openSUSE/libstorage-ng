#!/usr/bin/python3

# requirements: unused partitions /dev/sdb1, /dev/sdb2 and /dev/sdc1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

sdb1 = Partition.find_by_name(staging, "/dev/sdb1")
sdb2 = Partition.find_by_name(staging, "/dev/sdb2")

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")

bcache_cset = sdc1.create_bcache_cset()

bcache0 = sdb1.create_bcache("/dev/bcache0")
bcache0.attach_bcache_cset(bcache_cset)
bcache0.create_blk_filesystem(FsType_EXT4)

bcache1 = sdb2.create_bcache("/dev/bcache1")
bcache1.attach_bcache_cset(bcache_cset)
bcache1.create_blk_filesystem(FsType_EXT4)

print(staging)

commit(storage)

