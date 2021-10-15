#!/usr/bin/python3

# requirements: partitionable /dev/ram0 with gpt


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

partitionable = Partitionable.find_by_name(staging, "/dev/ram0")

partition_table = partitionable.get_partition_table()

region = Region(2048, 2048, 512)
partition = partition_table.create_partition("/dev/ram0p4", region, PartitionType_PRIMARY)

region = Region(4096, 2048, 512)
partition = partition_table.create_partition("/dev/ram0p8", region, PartitionType_PRIMARY)

print(staging)

commit(storage)

