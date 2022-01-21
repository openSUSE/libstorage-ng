#!/usr/bin/python3

# requirements: disk /dev/sdc with three empty and unused partitions (sdc1-sdc3)


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

md = Md.create(staging, "/dev/md0")
md.set_md_level(MdLevel_RAID4)

for partition in [sdc1, sdc2, sdc3]:
    md.add_device(partition)
    partition.set_id(ID_RAID)

print(staging)

commit(storage)

