#!/usr/bin/python3

# requirements: partitionable /dev/sdb with msdos partition table


from sys import exit
from storage import *
from storageitu import *


type = PartitionType_PRIMARY

set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

partitionable = Partitionable.find_by_name(staging, "/dev/sdb")

partition_table = partitionable.get_partition_table()

partition_slots = partition_table.get_unused_partition_slots()

good_partition_slot = None

for partition_slot in partition_slots:

    if type == PartitionType_PRIMARY and partition_slot.primary_possible:
        good_partition_slot = partition_slot
        break

    if type == PartitionType_EXTENDED and partition_slot.extended_possible:
        good_partition_slot = partition_slot
        break

    if type == PartitionType_LOGICAL and partition_slot.logical_possible:
        good_partition_slot = partition_slot
        break


if not good_partition_slot:
    print("no good partition slot found")
    exit()

region = good_partition_slot.region

region.set_length(1 * GiB / region.get_block_size())

region = partition_table.align(region)

partition = partition_table.create_partition(good_partition_slot.name, region, type)

partition.set_id(ID_RAID)

print(staging)

commit(storage)

