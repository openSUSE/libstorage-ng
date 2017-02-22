#!/usr/bin/python

# requirements: nfs server dist exporting path dist


from storage import *
from storageitu import *


environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

print staging

nfs = Nfs.create(staging, "dist", "/dist")
nfs.add_mountpoint("/test")

print staging

commit(storage)

