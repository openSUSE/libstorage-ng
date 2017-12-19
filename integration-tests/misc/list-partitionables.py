#!/usr/bin/python3

# requirements: some partitionables


from sys import exit
from storage import *
from storageitu import *
from functools import cmp_to_key


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()


def cmp1(lhs, rhs):
    return -1 if Partitionable.compare_by_name(lhs, rhs) else +1


def cmp2(lhs, rhs):
    return -1 if Partition.compare_by_number(lhs, rhs) else +1


partitionables = Partitionable.get_all(staging)

for partitionable in sorted(partitionables, key = cmp_to_key(cmp1)):

    print("%s" % partitionable.get_name())

    if partitionable.has_partition_table():

        partition_table = partitionable.get_partition_table()

        for partition in sorted(partition_table.get_partitions(), key = cmp_to_key(cmp2)):

            print("  %s" % partition.get_name())

