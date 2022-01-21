#!/usr/bin/python3

# requirements: md raid /dev/md0 with partitions sdc[1-5]


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdc5 = Partition.find_by_name(staging, "/dev/sdc5")

md = Md.find_by_name(staging, "/dev/md0")

md.remove_device(sdc5)

print(staging)

commit(storage)

