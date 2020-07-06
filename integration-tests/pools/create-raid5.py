#!/usr/bin/python3

# requirements: at least three HDDs and one SSD with free space


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

try:
    devices = hdds_pool.create_partitions(staging, 3, 2 * GiB)
    journal = ssds_pool.create_partitions(staging, 1, 128 * MiB)[0]
except PoolOutOfSpace as exception:
    print(exception.what())
    exit(1)

md = Md.create(staging, Md.find_free_numeric_name(staging))
md.set_md_level(MdLevel_RAID5)
md.set_metadata("1.2")

for device in devices:
    md.add_device(device)
    device.set_id(ID_RAID)

md.add_device(journal).set_journal(True)
journal.set_id(ID_RAID)

print(staging)

commit(storage)
