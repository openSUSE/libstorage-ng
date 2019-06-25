#!/usr/bin/python3

from storage import *


environment = Environment(True)

storage = Storage(environment)

staging = storage.get_staging()

sda = Disk.create(staging, "/dev/sda")

gpt = sda.create_partition_table(PtType_GPT)

sda1 = gpt.create_partition("/dev/sda1", Region(0, 1000, 512), PartitionType_PRIMARY)
sda2 = gpt.create_partition("/dev/sda2", Region(1000, 1000, 512), PartitionType_PRIMARY)

ext4 = sda1.create_filesystem(FsType_EXT4)
swap = sda2.create_filesystem(FsType_SWAP)

print(staging)


print("partitions on gpt:")
for partition in gpt.get_partitions():
    print("  %s %s" % (partition, partition.get_number()))
print()


print("descendants of sda:")
for device in sda.get_descendants(False):
    print("  %s" % device)
print()


tmp1 = BlkDevice.find_by_name(staging, "/dev/sda1")
print(tmp1)

