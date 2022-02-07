#!/usr/bin/python3

# requirements: nfs or nfs4 mounted at /test


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

print(FsType_NFS, FsType_NFS4)

print(mount_point.get_mount_type())

if mount_point.get_mount_type() == FsType_NFS:
    mount_point.set_mount_type(FsType_NFS4)
else:
    mount_point.set_mount_type(FsType_NFS)

print(mount_point.get_mount_type())

print(staging)

commit(storage)

