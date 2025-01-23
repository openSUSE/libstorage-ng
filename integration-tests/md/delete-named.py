#!/usr/bin/python3

# requirements: md raid /dev/md/test


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

md_test = Md.find_by_name(staging, "/dev/md/test")

md_test.remove_descendants(View_REMOVE)
staging.remove_device(md_test)

print(staging)

commit(storage)

