#!/usr/bin/python

# requirements: disk /dev/sdb with two empty and unused partitions (sdb1-sdb2)


from sys import exit
from storage import *
from storageitu import *


environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

sdb1 = Partition.find(staging, "/dev/sdb1")
sdb2 = Partition.find(staging, "/dev/sdb2")

md = Md.create(staging, "/dev/md0")
md.set_md_level(RAID0)

md.add_device(sdb1)
md.add_device(sdb2)

print staging

commit(storage)

