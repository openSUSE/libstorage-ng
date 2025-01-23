#!/usr/bin/python3

# requirements: dasd /dev/dasdb

# Since parted reports a DASD partition table even if there is none this test
# first removes the partition table (which does not work on disk since wipefs
# does not know about DASD partition tables) and then creates a new. As a
# result the "volume serial" as displayed by fdasd is reset.


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

partitionable = Partitionable.find_by_name(staging, "/dev/dasdb")

partitionable.remove_descendants(View_REMOVE)

partitionable.create_partition_table(PtType_DASD)

print(staging)

commit(storage)

