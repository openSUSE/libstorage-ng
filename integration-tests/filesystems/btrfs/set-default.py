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

top_level = btrfs.get_top_level_btrfs_subvolume()

btrfs_subvolume = btrfs.find_btrfs_subvolume_by_path("test")

if not btrfs_subvolume.is_default_btrfs_subvolume():
    btrfs_subvolume.set_default_btrfs_subvolume()
else:
    top_level.set_default_btrfs_subvolume()

print(staging)

commit(storage)

