#!/usr/bin/python3

# requirements: btrfs on /dev/sdc1 and /dev/sdd1 and space after both partitions


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

def grow(partition):
    region = partition.get_region()
    region.set_length(int(region.get_length() + 1 * GiB / region.get_block_size()))
    region = partition.get_partition_table().align(region, AlignPolicy_KEEP_START_ALIGN_END)
    partition.set_region(region)

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")
grow(sdc1)

sdd1 = Partition.find_by_name(staging, "/dev/sdd1")
grow(sdd1)

print(staging)

commit(storage, skip_save_graphs = False)
