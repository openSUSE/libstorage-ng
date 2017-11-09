#!/usr/bin/python

# requirements: some partitionables


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

partitionables = Partitionable.get_all(staging)

for partitionable in sorted(partitionables, lambda lhs, rhs:
                            -1 if Partitionable.compare_by_name(lhs, rhs) else +1):

    print "%s" % partitionable.get_name()

    if partitionable.has_partition_table():

        partition_table = partitionable.get_partition_table()

        for partition in sorted(partition_table.get_partitions(), lambda lhs, rhs:
                                -1 if Partition.compare_by_number(lhs, rhs) else +1):

            print "  %s" % partition.get_name()

