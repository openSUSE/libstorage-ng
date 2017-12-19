#!/usr/bin/python3

# requirements: gpt on /dev/sdb


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

disk = Disk.find_by_name(staging, "/dev/sdb")

gpt = to_gpt(disk.get_partition_table())

gpt.set_pmbr_boot(not gpt.is_pmbr_boot())

print(staging)

commit(storage)

