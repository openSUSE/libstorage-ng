#!/usr/bin/python3

# requirements: partition /dev/sdc1 and /dev/sdc2 on a GPT, parted 3.6 or higher


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

Partition.find_by_name(staging, "/dev/sdc1").set_id(ID_LINUX_VAR)
Partition.find_by_name(staging, "/dev/sdc2").set_id(ID_LINUX_VAR_TMP)

print(staging)

commit(storage)
