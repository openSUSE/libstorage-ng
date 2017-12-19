#!/usr/bin/python3

# requirements: something mounted at /test


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

mount_points = MountPoint.find_by_path(staging, "/test")

if not mount_points.empty():

    filesystem = mount_points[0].get_mountable().get_filesystem()

    space_info = filesystem.detect_space_info()

    print(space_info)

    print(byte_to_humanstring(space_info.size, False, 2, True))
    print(byte_to_humanstring(space_info.used, False, 2, True))

