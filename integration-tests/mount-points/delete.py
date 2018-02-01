#!/usr/bin/python3

# requirements: partition sdb1 with mounted blk filesystem


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

sdb1 = BlkDevice.find_by_name(staging, "/dev/sdb1")

blk_filesystem = sdb1.get_blk_filesystem()

for descendant in blk_filesystem.get_descendants(False):
    if is_mount_point(descendant):
        blk_filesystem.remove_descendants()

print(staging)

commit(storage)

