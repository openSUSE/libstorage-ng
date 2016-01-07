#!/usr/bin/python

# requirements: disk /dev/sdb with msdos partition table


from sys import exit
from storage import *
from storageitu import *


environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

print staging

disk = Disk.find(staging, "/dev/sdb")

partition_table = disk.get_partition_table()

partition_slots = partition_table.get_unused_partition_slots()

good_partition_slot = None

for partition_slot in partition_slots:
    if partition_slot.primary_slot and partition_slot.primary_possible:
        good_partition_slot = partition_slot
        break

if not good_partition_slot:
    print "no good partition slot found"
    exit()

partition = partition_table.create_partition(good_partition_slot.name, good_partition_slot.region, PRIMARY)
partition.set_id(ID_LINUX)

print staging

commit(storage)

