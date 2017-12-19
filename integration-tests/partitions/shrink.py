#!/usr/bin/python3

# requirements: partition /dev/sdb1 with size at least 1 GiB


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

partition = Partition.find_by_name(staging, "/dev/sdb1")

region = partition.get_region()

region.set_length(region.get_length() - 512 * MiB / region.get_block_size())

region = partition.get_partition_table().align(region)

partition.set_region(region)

print(staging)

commit(storage)

