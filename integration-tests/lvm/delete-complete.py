#!/usr/bin/python

# requirements: disk /dev/sdb with lvm on partition sdb1 and sdb2


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

sdb1 = Partition.find_by_name(staging, "/dev/sdb1")
sdb1.remove_descendants()

sdb2 = Partition.find_by_name(staging, "/dev/sdb2")
sdb2.remove_descendants()

print staging

commit(storage)

