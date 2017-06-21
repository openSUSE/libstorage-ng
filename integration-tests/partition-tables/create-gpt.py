#!/usr/bin/python

# requirements: empty disk /dev/sdb


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print staging

partitionable = Partitionable.find_by_name(staging, "/dev/sdb")

partitionable.create_partition_table(PtType_GPT)

print staging

commit(storage)

