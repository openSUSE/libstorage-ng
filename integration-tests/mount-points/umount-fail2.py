#!/usr/bin/python3

# requirements: something mounted at /test (e.g. BlkFilesystem, BtrfsSubvolume or Nfs)


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

system("/usr/bin/umount /test")

staging = storage.get_staging()

print(staging)

mount_points = MountPoint.find_by_path(staging, "/test")
if mount_points.empty():
    raise Exception("no mount point")

mount_point = mount_points[0]

mount_point.set_active(False)

print(staging)

commit(storage)

