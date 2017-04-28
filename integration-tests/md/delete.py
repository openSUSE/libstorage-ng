#!/usr/bin/python

# requirements: md raid md0


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

md0 = Md.find_by_name(staging, "/dev/md0")

md0.remove_descendants()
staging.remove_device(md0)

print staging

commit(storage)

