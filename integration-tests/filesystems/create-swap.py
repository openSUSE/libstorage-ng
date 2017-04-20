#!/usr/bin/python

# requirements: disk /dev/sdb with msdos partition table and partition /dev/sdb1


from storage import *
from storageitu import *


environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

print staging

partition = Partition.find_by_name(staging, "/dev/sdb1")
partition.set_id(ID_SWAP)

swap = partition.create_blk_filesystem(FsType_SWAP)

mount_point = swap.create_mount_point("swap")

print staging

commit(storage)

