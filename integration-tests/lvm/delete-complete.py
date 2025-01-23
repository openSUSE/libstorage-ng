#!/usr/bin/python3

# requirements: disk /dev/sdc with lvm on partition sdc1 and sdd1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")
sdc1.remove_descendants(View_REMOVE)

sdd1 = Partition.find_by_name(staging, "/dev/sdd1")
sdd1.remove_descendants(View_REMOVE)

print(staging)

commit(storage)

