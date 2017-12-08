#!/usr/bin/python

# requirements: btrfs on /dev/system/btrfs with test subvolume


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

blk_device = BlkDevice.find_by_name(staging, "/dev/system/btrfs")
btrfs = to_btrfs(blk_device.get_blk_filesystem())

btrfs_subvolume = btrfs.find_btrfs_subvolume_by_path("test")

btrfs_subvolume.set_nocow(not btrfs_subvolume.is_nocow())

print(staging)

commit(storage)

