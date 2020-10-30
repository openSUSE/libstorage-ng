#!/usr/bin/python3

# requirements: btrfs on /dev/sdc1 with qgroups 0/257, 1/0 and 2/0


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

qgroup1 = btrfs.find_btrfs_qgroup_by_id(BtrfsQgroupId(0, 257))
qgroup2 = btrfs.find_btrfs_qgroup_by_id(BtrfsQgroupId(1, 0))
qgroup3 = btrfs.find_btrfs_qgroup_by_id(BtrfsQgroupId(2, 0))

if not qgroup2.is_assigned(qgroup1):
    qgroup2.assign(qgroup1)
else:
    qgroup2.unassign(qgroup1)

if not qgroup3.is_assigned(qgroup2):
    qgroup3.assign(qgroup2)
else:
    qgroup3.unassign(qgroup2)

print(staging)

commit(storage)

