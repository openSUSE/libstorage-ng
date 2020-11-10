#!/usr/bin/python3

# requirements: unused partitions /dev/sdc1 and /dev/sdd1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")
sdc1.set_id(ID_LINUX)

sdd1 = Partition.find_by_name(staging, "/dev/sdd1")
sdd1.set_id(ID_LINUX)

btrfs = to_btrfs(sdc1.create_blk_filesystem(FsType_BTRFS))
btrfs.add_device(sdd1)
btrfs.set_quota(True)

btrfs.set_label("TEST")
btrfs.set_metadata_raid_level(BtrfsRaidLevel_RAID1)
btrfs.set_data_raid_level(BtrfsRaidLevel_RAID1)

mount_point = btrfs.create_mount_point("/test")

print(staging)

commit(storage)

