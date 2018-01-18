#!/usr/bin/python3

# requirements: some mount points


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

mount_points = MountPoint.get_all(staging)


for mount_point in mount_points:

    print(mount_point.get_path(), "  ", end='')

    print(get_fs_type_name(mount_point.get_filesystem().get_type()), "  ", end='')

    if mount_point.is_active():
        print("active", "  ", end='')

    if mount_point.is_in_etc_fstab():
        print("in-etc-fstab", "  ", end='')

    print(",".join(mount_point.get_mount_options()), "  ", end='')

    if is_nfs(mount_point.get_filesystem()):
        nfs = to_nfs(mount_point.get_filesystem())
        print(nfs.get_server(), "  ", nfs.get_path(), end='')

    print()

