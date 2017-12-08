#!/usr/bin/python

# requirements: disk /dev/sdb with six empty and unused partitions (sdb1-sdb6)


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

md0 = Md.create(staging, "/dev/md0")
md0.set_md_level(MdLevel_RAID1)

sdb1 = Partition.find_by_name(staging, "/dev/sdb1")
sdb2 = Partition.find_by_name(staging, "/dev/sdb2")
sdb3 = Partition.find_by_name(staging, "/dev/sdb3")

md0.add_device(sdb1)
md0.add_device(sdb2)
md0.add_device(sdb3)

md1 = Md.create(staging, "/dev/md1")
md1.set_md_level(MdLevel_RAID1)

sdb4 = Partition.find_by_name(staging, "/dev/sdb4")
sdb5 = Partition.find_by_name(staging, "/dev/sdb5")
sdb6 = Partition.find_by_name(staging, "/dev/sdb6")

md1.add_device(sdb4)
md1.add_device(sdb5)
md1.add_device(sdb6)

md2 = Md.create(staging, "/dev/md2")
md2.set_md_level(MdLevel_RAID0)

md2.add_device(md0)
md2.add_device(md1)

print(staging)

commit(storage)

