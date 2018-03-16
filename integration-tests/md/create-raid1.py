#!/usr/bin/python3

# requirements: disk /dev/sdb and /dev/sdc each with one empty and unused partitions (sdb1 and sdc1)


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdb1 = Partition.find_by_name(staging, "/dev/sdb1")
sdc1 = Partition.find_by_name(staging, "/dev/sdc1")

md = Md.create(staging, "/dev/md0")
md.set_md_level(MdLevel_RAID1)

md.add_device(sdb1)
sdb1.set_id(ID_RAID)

md.add_device(sdc1)
sdc1.set_id(ID_RAID)

print(staging)

commit(storage)

