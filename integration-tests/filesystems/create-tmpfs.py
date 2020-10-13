#!/usr/bin/python3

# requirements: almost none


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

tmpfs = Tmpfs.create(staging)

mount_point = tmpfs.create_mount_point("/test")

print(staging)

commit(storage)

