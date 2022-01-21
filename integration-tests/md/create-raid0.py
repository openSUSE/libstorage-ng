#!/usr/bin/python3

# requirements: disks /dev/sdc and /dev/sdd with one empty and unused partitions each (sdc1 and sdd1)


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")
sdd1 = Partition.find_by_name(staging, "/dev/sdd1")

md = Md.create(staging, "/dev/md0")
md.set_metadata("default")
md.set_md_level(MdLevel_RAID0)

for partition in [sdc1, sdd1]:
    md.add_device(partition)
    partition.set_id(ID_RAID)

print(staging)

commit(storage)

