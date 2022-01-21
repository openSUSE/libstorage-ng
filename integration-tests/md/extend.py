#!/usr/bin/python3

# requirements: md raid /dev/md0 with partitions sdc[1-4] and unused partition sdc5


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdc5 = Partition.find_by_name(staging, "/dev/sdc5")

md = Md.find_by_name(staging, "/dev/md0")

md_user = md.add_device(sdc5)
md_user.set_spare(True)

print(staging)

commit(storage)

