#!/usr/bin/python

# requirements: disk /dev/sdb with msdos partition table and partition /dev/sdb1


from storage import *


environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

print staging

partition = Partition.find_by_name(staging, "/dev/sdb1")
partition.set_id(ID_LINUX)

xfs = partition.create_filesystem(FsType_XFS)
xfs.set_label("TEST")
xfs.add_mountpoint("/test")

print staging

storage.calculate_actiongraph()
storage.commit()

