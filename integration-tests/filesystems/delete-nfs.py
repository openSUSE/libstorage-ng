#!/usr/bin/python

# requirements: nfs server dist exporting path dist and mounted somewhere


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

nfs = Nfs.find_by_server_and_path(staging, "dist", "/dist")

nfs.remove_descendants()
staging.remove_device(nfs)

print(staging)

commit(storage)

