#!/usr/bin/python

# requirements: partitionable /dev/sdb with msdos partition table and partition /dev/sdb1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

partitionable = Partitionable.find_by_name(staging, "/dev/sdb")

partition_table = partitionable.get_partition_table()

partition_table.delete_partition("/dev/sdb1")

print(staging)

commit(storage)

