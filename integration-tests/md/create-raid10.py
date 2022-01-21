#!/usr/bin/python3

# requirements: disks /dev/sdc and /dev/sdd with six empty and unused
# partitions (sdc1-sdc3, sdd1-sdd3)


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

sdd1 = Partition.find_by_name(staging, "/dev/sdd1")
sdd2 = Partition.find_by_name(staging, "/dev/sdd2")
sdd3 = Partition.find_by_name(staging, "/dev/sdd3")

md = Md.create(staging, "/dev/md0")
md.set_metadata("default")
md.set_md_level(MdLevel_RAID10)

for count, partition in enumerate([sdc1, sdd1, sdc2, sdd2, sdc3, sdd3], start=1):
    md.add_device(partition).set_sort_key(count)
    partition.set_id(ID_RAID)

print(staging)

commit(storage)

