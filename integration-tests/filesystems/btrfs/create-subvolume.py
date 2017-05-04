#!/usr/bin/python

# requirements: btrfs on /dev/sdb1 without test subvolume


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print staging

blk_device = BlkDevice.find_by_name(staging, "/dev/sdb1")

btrfs = to_btrfs(blk_device.get_blk_filesystem())
top_level = btrfs.get_top_level_btrfs_subvolume()

btrfs_subvolume = top_level.create_btrfs_subvolume("test")
btrfs_subvolume.set_nocow(True)
btrfs_subvolume.set_default_btrfs_subvolume()

mount_point = btrfs_subvolume.create_mount_point("/test/test")

print staging

commit(storage)

