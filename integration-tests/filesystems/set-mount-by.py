#!/usr/bin/python

# requirements: blk filesystem mounted at /test


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

mount_points = MountPoint.find_by_path(staging, "/test")

mount_point = mount_points[0]

print(MountByType_DEVICE, MountByType_UUID, MountByType_LABEL)

print(mount_point.get_mount_by())

if mount_point.get_mount_by() == MountByType_UUID:
    mount_point.set_mount_by(MountByType_DEVICE)
else:
    mount_point.set_mount_by(MountByType_UUID)

print(mount_point.get_mount_by())

print(staging)

commit(storage)

