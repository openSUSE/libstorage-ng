#!/usr/bin/python3

# requirements: btrfs on /dev/sdc1


from storage import *
from storageitu import *


def subvolume_exists(btrfs, path):
    for subvolume in btrfs.get_btrfs_subvolumes():
        if subvolume.get_path() == path:
            return True
    return False


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

blk_device = BlkDevice.find_by_name(staging, "/dev/sdc1")

btrfs = to_btrfs(blk_device.get_blk_filesystem())
top_level = btrfs.get_top_level_btrfs_subvolume()

btrfs.set_quota(not btrfs.has_quota())

if subvolume_exists(btrfs, "c"):
    subvolume1 = btrfs.find_btrfs_subvolume_by_path("c")
    subvolume1.remove_descendants(View_REMOVE)
    staging.remove_device(subvolume1)
else:
    subvolume1 = top_level.create_btrfs_subvolume("c")

btrfs.set_quota(not btrfs.has_quota())

if subvolume_exists(btrfs, "d"):
    subvolume2 = btrfs.find_btrfs_subvolume_by_path("d")
    subvolume2.remove_descendants(View_REMOVE)
    staging.remove_device(subvolume2)
else:
    subvolume2 = top_level.create_btrfs_subvolume("d")

print(staging)

commit(storage)
