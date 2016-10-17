#!/usr/bin/python

# requirements: disk /dev/sdb with msdos partition table


from sys import exit
from storage import *
from storageitu import *


type = PartitionType_PRIMARY

set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

print staging

disk = Disk.find_by_name(staging, "/dev/sdb")

partition_table = disk.get_partition_table()

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
    print "no good partition slot found"
    exit()

# good_partition_slot.region.set_length(10 * GiB / 512)

partition = partition_table.create_partition(good_partition_slot.name, good_partition_slot.region,
                                             type)

print staging

commit(storage)

