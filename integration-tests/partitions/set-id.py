#!/usr/bin/python

# requirements: partition /dev/sdb1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

print staging

partition = Partition.find_by_name(staging, "/dev/sdb1")

partition.set_id(ID_ESP)

print staging

commit(storage)

