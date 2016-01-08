#!/usr/bin/python

# requirements: md raid md0


from sys import exit
from storage import *
from storageitu import *


environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

md0 = Md.find(staging, "/dev/md0")

staging.remove_device(md0)

print staging

commit(storage)

