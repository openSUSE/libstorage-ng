#!/usr/bin/python3

# requirements: disk /dev/sdc and /dev/sdd with three empty and unused partitions (sdc1-sdc3
# and sdd1-sdd3)


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

md0 = Md.create(staging, "/dev/md0")
md0.set_md_level(MdLevel_RAID1)

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")
sdc2 = Partition.find_by_name(staging, "/dev/sdc2")
sdc3 = Partition.find_by_name(staging, "/dev/sdc3")

for partition in [sdc1, sdc2, sdc3]:
    md0.add_device(partition)
    partition.set_id(ID_RAID)

md1 = Md.create(staging, "/dev/md1")
md1.set_md_level(MdLevel_RAID1)

sdd1 = Partition.find_by_name(staging, "/dev/sdd1")
sdd2 = Partition.find_by_name(staging, "/dev/sdd2")
sdd3 = Partition.find_by_name(staging, "/dev/sdd3")

for partition in [sdd1, sdd2, sdd3]:
    md1.add_device(partition)
    partition.set_id(ID_RAID)

md2 = Md.create(staging, "/dev/md2")
md2.set_md_level(MdLevel_RAID0)

for md in [md0, md1]:
    md2.add_device(md)

print(staging)

commit(storage)

