#!/usr/bin/python3

# requirements: partition /dev/sdc1 on MS-DOS


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

partition = Partition.find_by_name(staging, "/dev/sdc1")

partition.set_boot(not partition.is_boot())

print(staging)

commit(storage)

