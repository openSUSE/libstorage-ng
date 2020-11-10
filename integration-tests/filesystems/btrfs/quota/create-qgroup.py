#!/usr/bin/python3

# requirements: btrfs on /dev/sdc1 with quota enabled


from storage import *
from storageitu import *


def qgroup_exists(btrfs, id):
    return any(qgroup.get_id() == id for qgroup in btrfs.get_btrfs_qgroups())


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

blk_device = BlkDevice.find_by_name(staging, "/dev/sdc1")

btrfs = to_btrfs(blk_device.get_blk_filesystem())

if qgroup_exists(btrfs, BtrfsQgroupId(1, 0)):
    staging.remove_device(btrfs.find_btrfs_qgroup_by_id(BtrfsQgroupId(1, 0)))
else:
    btrfs.create_btrfs_qgroup(BtrfsQgroupId(1, 0))

if qgroup_exists(btrfs, BtrfsQgroupId(2, 0)):
    staging.remove_device(btrfs.find_btrfs_qgroup_by_id(BtrfsQgroupId(2, 0)))
else:
    btrfs.create_btrfs_qgroup(BtrfsQgroupId(2, 0))

print(staging)

commit(storage)

