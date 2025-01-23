#!/usr/bin/python3

# requirements: /dev/sdc with a partition table


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

# print(staging)

partitionable = Partitionable.find_by_name(staging, "/dev/sdc")

partitionable.remove_descendants(View_REMOVE)

# print(staging)

commit(storage)

