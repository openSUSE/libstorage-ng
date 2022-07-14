#!/usr/bin/python3


from storage import *
from storageitu import *


def create_partition(gpt, size):

    for slot in gpt.get_unused_partition_slots():

        if not slot.primary_possible:
            continue

        region = slot.region

        if region.to_bytes(region.get_length()) < size:
            continue

        region.set_length(int(size / region.get_block_size()))
        region = gpt.align(region)
        partition = gpt.create_partition(slot.name, region, PartitionType_PRIMARY)
        return partition

    raise Exception("no suitable partition slot found")


set_logger(get_logfile_logger())

environment = Environment(False, ProbeMode_NONE, TargetMode_IMAGE)
environment.set_rootprefix("/mnt")

storage = Storage(environment)

staging = storage.get_staging()

# TODO find free loop device

disk = Disk.create(staging, "/dev/loop0", 3 * GiB)
disk.set_image_filename("/tmp/test.image")

gpt = to_gpt(disk.create_partition_table(PtType_GPT))

partition1 = create_partition(gpt, 1 * GiB)
file_system1 = partition1.create_filesystem(FsType_EXT4)
mount_point1 = file_system1.create_mount_point("/")

partition2 = create_partition(gpt, 1.5 * GiB)
file_system2 = partition2.create_filesystem(FsType_XFS)
mount_point2 = file_system2.create_mount_point("/data")

print(staging)

commit(storage)

# TODO allow cleanup: unmount, deactivate loop devices, ...
