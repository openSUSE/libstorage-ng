#!/usr/bin/python

# requirements: disk /dev/sdb with four empty and unused partitions (sdb1-sdb4)


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
sdb4 = Partition.find_by_name(staging, "/dev/sdb4")

md = Md.create(staging, "/dev/md0")
md.set_md_level(MdLevel_RAID5)

md.add_device(sdb1)
md.add_device(sdb2)
md.add_device(sdb3)
md.add_device(sdb4)

print(staging)

commit(storage)

