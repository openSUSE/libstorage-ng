#!/usr/bin/python

# requirements: empty disk /dev/sdb


from storage import *
from storageitu import *


type = PartitionType_PRIMARY

set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print staging

disk = Disk.find_by_name(staging, "/dev/sdb")

gpt = to_gpt(disk.create_partition_table(PtType_GPT))

print staging

commit(storage)

