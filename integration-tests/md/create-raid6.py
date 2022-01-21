#!/usr/bin/python3

# requirements: disk /dev/sdc with five empty and unused partitions (sdc1-sdc5)


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")
sdc2 = Partition.find_by_name(staging, "/dev/sdc2")
sdc3 = Partition.find_by_name(staging, "/dev/sdc3")
sdc4 = Partition.find_by_name(staging, "/dev/sdc4")
sdc5 = Partition.find_by_name(staging, "/dev/sdc5")

md = Md.create(staging, "/dev/md0")
md.set_md_level(MdLevel_RAID6)

md.add_device(sdc1)
md.add_device(sdc2)
md.add_device(sdc3)
md.add_device(sdc4)
md.add_device(sdc5).set_spare(True)

print(staging)

commit(storage)

