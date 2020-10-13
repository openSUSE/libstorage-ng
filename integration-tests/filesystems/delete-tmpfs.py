#!/usr/bin/python3

# requirements: tmpfs mounted at /test


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

tmpfses = Tmpfs.get_all(staging)

for tmpfs in tmpfses:

    if tmpfs.has_mount_point():

        mount_point = tmpfs.get_mount_point()

        if mount_point.get_path() == "/test":
            tmpfs.remove_descendants()
            staging.remove_device(tmpfs)

print(staging)

commit(storage)

