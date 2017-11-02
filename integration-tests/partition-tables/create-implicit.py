#!/usr/bin/python

# requirements: fba dasd /dev/dasdb


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print staging

partitionable = Partitionable.find_by_name(staging, "/dev/dasdb")

partitionable.remove_descendants()

partitionable.create_partition_table(PtType_IMPLICIT)

print staging

commit(storage)

