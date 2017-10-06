#!/usr/bin/python

# requirements: partition /dev/sdb1 with ext4


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print staging

sdb1 = BlkDevice.find_by_name(staging, "/dev/sdb1")

ext4 = sdb1.get_blk_filesystem()

ext4.set_tune_options("-c 20")

print staging

commit(storage)

