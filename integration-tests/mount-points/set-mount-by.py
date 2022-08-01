#!/usr/bin/python3

# requirements: blk filesystem mounted at /test, best with filesystem
# label and partition label


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

possible_mount_bys = []

for possible_mount_by in mount_point.possible_mount_bys():
    possible_mount_bys.append(possible_mount_by)
possible_mount_bys.sort()

print(possible_mount_bys)

mount_by = mount_point.get_mount_by()
print(mount_by, get_mount_by_name(mount_by))

idx = possible_mount_bys.index(mount_by)
idx = (idx + 1) % len(possible_mount_bys)

mount_by = possible_mount_bys[idx]
print(mount_by, get_mount_by_name(mount_by))

mount_point.set_mount_by(mount_by)

print(staging)

commit(storage)

