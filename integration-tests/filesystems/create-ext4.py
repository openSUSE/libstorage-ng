#!/usr/bin/python3

# requirements: disk /dev/sdc with msdos partition table and partition /dev/sdc1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

partition = Partition.find_by_name(staging, "/dev/sdc1")
partition.set_id(ID_LINUX)

ext4 = partition.create_blk_filesystem(FsType_EXT4)
ext4.set_label("TEST")
ext4.set_tune_options("-c 10")

mount_point = ext4.create_mount_point("/test")
mount_point.set_mount_by(MountByType_LABEL)

print(staging)

commit(storage)

