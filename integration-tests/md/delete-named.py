#!/usr/bin/python

# requirements: md raid /dev/md/test


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

md_test = Md.find_by_name(staging, "/dev/md/test")

md_test.remove_descendants()
staging.remove_device(md_test)

print staging

commit(storage)

