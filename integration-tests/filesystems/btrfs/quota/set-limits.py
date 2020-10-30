#!/usr/bin/python3

# requirements: btrfs on /dev/sdc1 with qgroup 1/0


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

blk_device = BlkDevice.find_by_name(staging, "/dev/sdc1")

btrfs = to_btrfs(blk_device.get_blk_filesystem())

qgroup = btrfs.find_btrfs_qgroup_by_id(BtrfsQgroupId(1, 0))

if qgroup.has_referenced_limit():
    qgroup.clear_referenced_limit()
else:
    qgroup.set_referenced_limit(2 * GiB)

if qgroup.has_exclusive_limit():
    qgroup.clear_exclusive_limit()
else:
    qgroup.set_exclusive_limit(1 * GiB)

print(staging)

commit(storage)

