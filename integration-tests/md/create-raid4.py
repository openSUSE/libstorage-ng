#!/usr/bin/python

# requirements: disk /dev/sdb with three empty and unused partitions (sdb1-sdb3)


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdb1 = Partition.find_by_name(staging, "/dev/sdb1")
sdb2 = Partition.find_by_name(staging, "/dev/sdb2")
sdb3 = Partition.find_by_name(staging, "/dev/sdb3")

md = Md.create(staging, "/dev/md0")
md.set_md_level(MdLevel_RAID4)

md.add_device(sdb1)
md.add_device(sdb2)
md.add_device(sdb3)

print(staging)

commit(storage)

