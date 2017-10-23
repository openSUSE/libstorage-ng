#!/usr/bin/python

# requirements: partitionable /dev/sdb with partition table


from sys import exit
from storage import *
from storageitu import *


type = PartitionType_PRIMARY

set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print staging

partitionable = Partitionable.find_by_name(staging, "/dev/sdb")

partition_table = partitionable.get_partition_table()

partition_slots = partition_table.get_unused_partition_slots()

for partition_slot in partition_slots:
    print partition_slot

