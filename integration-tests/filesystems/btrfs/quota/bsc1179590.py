#!/usr/bin/python3

# requirements: btrfs on /dev/sdc1 with quota enabled


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

blk_device = BlkDevice.find_by_name(staging, "/dev/sdb1")

btrfs = to_btrfs(blk_device.get_blk_filesystem())

top_level = btrfs.get_top_level_btrfs_subvolume()

subvolume = top_level.create_btrfs_subvolume("z")

qgroup = subvolume.get_btrfs_qgroup()
qgroup.set_referenced_limit(1 * GiB)

subvolume.remove_descendants(View_REMOVE)
staging.remove_device(subvolume)

print(staging)

commit(storage)

