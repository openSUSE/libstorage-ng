#!/usr/bin/python3

# requirements: disk /dev/sdc with two empty and unused partitions (sdc1-sdc2)


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")
sdc2 = Partition.find_by_name(staging, "/dev/sdc2")

md = Md.create(staging, "/dev/md0")
md.set_md_level(MdLevel_RAID0)

for partition in [sdc1, sdc2]:
    md.add_device(partition)
    partition.set_id(ID_RAID)

test = LvmVg.create(staging, "test")
test.add_lvm_pv(md)

print(staging)

commit(storage)

