#!/usr/bin/python

# requirements: disk /dev/sdb with empty partition sdb1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

sdb1 = Partition.find_by_name(staging, "/dev/sdb1")

luks = sdb1.create_encryption("cr_test")

luks.set_password("12345678")

ext4 = luks.create_blk_filesystem(FsType_EXT4)

print staging

commit(storage)

