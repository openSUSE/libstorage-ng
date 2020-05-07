#!/usr/bin/python3

# requirements: partition sdc1 with mounted BlkFilesystem


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

system("/usr/bin/umount /dev/sdc1")
# system("/sbin/swapoff /dev/sdc1")

staging = storage.get_staging()

print(staging)

sdc1 = BlkDevice.find_by_name(staging, "/dev/sdc1")
if not sdc1.has_blk_filesystem():
    raise Exception("no blk filesystem")

blk_filesystem = sdc1.get_blk_filesystem()
if not blk_filesystem.has_mount_point():
    raise Exception("no mount point")

mount_point = blk_filesystem.get_mount_point()

mount_point.set_active(False)

print(staging)

commit(storage)

