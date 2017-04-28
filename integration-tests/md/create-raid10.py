#!/usr/bin/python

# requirements: disks /dev/sdb and /dev/sdc with six empty and unused
# partitions (sdb1-sdb3, sdc1-sdc3)


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

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")
sdc2 = Partition.find_by_name(staging, "/dev/sdc2")
sdc3 = Partition.find_by_name(staging, "/dev/sdc3")

md = Md.create(staging, "/dev/md0")
md.set_md_level(RAID10)

md.add_device(sdb1).set_sort_key(1)
md.add_device(sdb2).set_sort_key(3)
md.add_device(sdb3).set_sort_key(5)

md.add_device(sdc1).set_sort_key(2)
md.add_device(sdc2).set_sort_key(4)
md.add_device(sdc3).set_sort_key(6)

print staging

commit(storage)

