#!/usr/bin/python3

from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(True)

storage = Storage(environment)
storage.probe()

storage.generate_pools(storage.get_probed())

pool = storage.get_pool("HDDs (512 B)")

try:
    max_size = pool.max_partition_size(storage.get_probed(), 2)
except PoolOutOfSpace as exception:
    print(exception.what())
    exit(1)

print(max_size)
print(byte_to_humanstring(max_size, False, 2, True))
