#!/usr/bin/python3

# requirements: a HDD and a SSD with free space


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

storage.generate_pools(storage.get_probed())

hdds_pool = storage.get_pool("HDDs (512 B)")
ssds_pool = storage.get_pool("SSDs (512 B)")

staging = storage.get_staging()

backing_device = hdds_pool.create_partitions(staging, 1, 4 * GiB)[0]
caching_device = ssds_pool.create_partitions(staging, 1, 1 * GiB)[0]

bcache = backing_device.create_bcache(Bcache.find_free_name(staging))
bcache_cset = caching_device.create_bcache_cset()
bcache.add_bcache_cset(bcache_cset)

print(staging)

commit(storage)
