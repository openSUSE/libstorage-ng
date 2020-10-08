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

ext3 = partition.create_blk_filesystem(FsType_EXT3)
ext3.set_label("TEST")

mount_point = ext3.create_mount_point("/test")

print(staging)

commit(storage)

