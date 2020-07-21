#!/usr/bin/python3

from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(True)

storage = Storage(environment)
storage.probe()

probed = storage.get_probed()

storage.generate_pools(probed)

pool = storage.get_pool("HDDs (512 B)")

for n in range(1, pool.size(probed) + 1):

    try:
        max_size = pool.max_partition_size(probed, n)
    except PoolOutOfSpace as exception:
        print(exception.what())
        exit(1)

    print(n, max_size, byte_to_humanstring(max_size, False, 2, True))
