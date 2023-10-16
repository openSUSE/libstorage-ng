#!/usr/bin/python3

# requirements: partitionable /dev/sdc with partition table


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

partitionable = Partitionable.find_by_name(staging, "/dev/sdc")

partition_table = partitionable.get_partition_table()

partition_slots = partition_table.get_unused_partition_slots()

for partition_slot in partition_slots:
    print(partition_slot)

