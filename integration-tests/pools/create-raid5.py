#!/usr/bin/python3

# requirements: at least three HDDs with free space


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

storage.generate_pools(storage.get_probed())

pool = storage.get_pool("HDDs (512 B)")

staging = storage.get_staging()

devices = pool.create_partitions(staging, 3, 2 * GiB)

md = Md.create(staging, Md.find_free_numeric_name(staging))
md.set_md_level(MdLevel_RAID5)

for device in devices:
    md.add_device(device)
    device.set_id(ID_RAID)

print(staging)

commit(storage)
