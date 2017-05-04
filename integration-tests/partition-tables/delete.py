#!/usr/bin/python

# requirements: /dev/sdb with a partition table


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

# print staging

disk = Disk.find_by_name(staging, "/dev/sdb")

disk.remove_descendants()

# print staging

commit(storage, skip_save_graphs = False)

