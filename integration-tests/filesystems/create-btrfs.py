#!/usr/bin/python

# requirements: disk /dev/sdb with msdos partition table and partition /dev/sdb1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

partition = Partition.find_by_name(staging, "/dev/sdb1")
partition.set_id(ID_LINUX)

btrfs = partition.create_blk_filesystem(FsType_BTRFS)
btrfs.set_label("TEST")

mount_point = btrfs.create_mount_point("/test")

print(staging)

commit(storage)

