#!/usr/bin/python

# requirements: disk /dev/sdb with two empty and unused partitions (sdb1-sdb2)


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdb1 = Partition.find_by_name(staging, "/dev/sdb13")
sdb2 = Partition.find_by_name(staging, "/dev/sdb14")

md_test = Md.create(staging, "/dev/md/test")
md_test.set_md_level(RAID0)

md_test.add_device(sdb1)
sdb1.set_id(ID_RAID)

md_test.add_device(sdb2)
sdb2.set_id(ID_RAID)

print staging

commit(storage)

