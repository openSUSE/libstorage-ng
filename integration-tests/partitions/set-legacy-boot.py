#!/usr/bin/python

# requirements: partition /dev/sdb1 on GPT


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

# print(staging)

partition = Partition.find_by_name(staging, "/dev/sdb1")

partition.set_legacy_boot(not partition.is_legacy_boot())

print(staging)

commit(storage)

