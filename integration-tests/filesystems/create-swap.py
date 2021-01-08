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
partition.set_id(ID_SWAP)

swap = partition.create_blk_filesystem(FsType_SWAP)
# swap.set_uuid("deafbeef-cd50-464e-b0b2-66c1fdc75bbb")

mount_point = swap.create_mount_point("swap")

print(staging)

commit(storage)

