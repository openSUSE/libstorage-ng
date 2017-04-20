#!/usr/bin/python

# requirements: nfs server dist exporting path dist and mounted somewhere


from storage import *
from storageitu import *


environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

print staging

nfs = Nfs.find_by_server_and_path(staging, "dist", "/dist")

nfs.remove_descendants()
staging.remove_device(nfs)

print staging

commit(storage)

