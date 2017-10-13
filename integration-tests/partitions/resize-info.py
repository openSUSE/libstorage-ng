#!/usr/bin/python

# requirements: partition /dev/sdb1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print staging

partition = Partition.find_by_name(staging, "/dev/sdb1")

resize_info = partition.detect_resize_info()

print resize_info

print resize_info.resize_ok
print byte_to_humanstring(resize_info.min_size, False, 2, True)
print byte_to_humanstring(resize_info.max_size, False, 2, True)
print byte_to_humanstring(resize_info.block_size, False, 2, True)

