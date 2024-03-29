#!/usr/bin/python3

# requirements: partition sdc1 with mounted blk filesystem


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdc1 = BlkDevice.find_by_name(staging, "/dev/sdc1")

blk_filesystem = sdc1.get_blk_filesystem()

mount_point = blk_filesystem.get_mount_point()

mount_point.set_in_etc_fstab(not mount_point.is_in_etc_fstab())
mount_point.set_default_mount_by()

print(staging)

commit(storage)

