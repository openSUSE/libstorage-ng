#!/usr/bin/python3

# requirements: partition /dev/sdc1 with swap


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

sdc1 = BlkDevice.find_by_name(staging, "/dev/sdc1")

blk_filesystem = sdc1.get_blk_filesystem()

blk_filesystem.set_uuid("7420b069-cd50-464e-b0b2-66c1fdc75bcd")

print(staging)

commit(storage)

