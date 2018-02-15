#!/usr/bin/python3

# requirements: partition sdb1 with mounted blk filesystem


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdb1 = BlkDevice.find_by_name(staging, "/dev/sdb1")

blk_filesystem = sdb1.get_blk_filesystem()

mount_point = blk_filesystem.get_mount_point()

mount_point.set_in_etc_fstab(not mount_point.is_in_etc_fstab())

print(staging)

commit(storage)

