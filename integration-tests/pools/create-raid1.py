#!/usr/bin/python3

# requirements: at least two HDDs with free space


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

storage.generate_pools(storage.get_probed())

pool = storage.get_pool("HDDs (512 B)")

staging = storage.get_staging()

try:
    devices = pool.create_partitions(staging, 2, 2 * GiB)
except PoolOutOfSpace as exception:
    print(exception.what())
    exit(1)

md = Md.create(staging, Md.find_free_numeric_name(staging))
md.set_md_level(MdLevel_RAID1)

for device in devices:
    md.add_device(device)
    device.set_id(ID_RAID)

print(staging)

commit(storage)
