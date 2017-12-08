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

print(staging)

partition = Partition.find_by_name(staging, "/dev/sdb5")

surrounding = partition.get_unused_surrounding_region()

print(surrounding)

print(byte_to_humanstring(surrounding.to_bytes(surrounding.get_start()), False, 2, True))
print(byte_to_humanstring(surrounding.to_bytes(surrounding.get_length()), False, 2, True))
print(byte_to_humanstring(surrounding.to_bytes(surrounding.get_end()), False, 2, True))

