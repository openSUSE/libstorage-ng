#!/usr/bin/python

# requirements: disk /dev/sdb with msdos partition table and partition /dev/sdb1


from storage import *
from storageitu import *


environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

print staging

partition = Partition.find_by_name(staging, "/dev/sdb1")
partition.set_id(ID_LINUX)

ext4 = partition.create_blk_filesystem(FsType_EXT4)
ext4.set_label("TEST")
ext4.add_mountpoint("/test")
ext4.set_mount_by(MountByType_UUID)

print staging

commit(storage)

