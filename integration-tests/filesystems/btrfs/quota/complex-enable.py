#!/usr/bin/python3

# requirements: btrfs on /dev/sdc1 and quota disabled


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

partition = Partition.find_by_name(staging, "/dev/sdc1")

btrfs = to_btrfs(partition.get_blk_filesystem())
btrfs.set_quota(True)

top_level = btrfs.get_top_level_btrfs_subvolume()

qgroup1 = btrfs.find_btrfs_qgroup_by_id(BtrfsQgroupId(0, 5))
qgroup1.set_referenced_limit(1 * GiB)

qgroup2 = btrfs.create_btrfs_qgroup(BtrfsQgroupId(1, 0))
qgroup2.set_referenced_limit(2 * GiB)
qgroup2.assign(qgroup1)

qgroup3 = btrfs.create_btrfs_qgroup(BtrfsQgroupId(2, 0))
qgroup3.set_referenced_limit(3 * GiB)
qgroup3.assign(qgroup2)

subvolume1 = top_level.create_btrfs_subvolume("a")
qgroup4 = subvolume1.get_btrfs_qgroup()
qgroup4.set_referenced_limit(4 * GiB)

subvolume2 = top_level.create_btrfs_subvolume("b")
qgroup5 = subvolume2.get_btrfs_qgroup()
qgroup5.set_referenced_limit(5 * GiB)

qgroup2.assign(qgroup4)
qgroup2.assign(qgroup5)

print(staging)

commit(storage)

