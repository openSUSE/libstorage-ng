#!/usr/bin/python3

# requirements: lvm volume group test without logical volume thin-pool, thin1 and thin2


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

test = LvmVg.find_by_vg_name(staging, "test")

thin_pool = test.create_lvm_lv("thin-pool", LvType_THIN_POOL, 1 * GiB)
# thin_pool.set_stripes(2)
# thin_pool.set_stripe_size(128 * KiB)
# thin_pool.set_chunk_size(256 * KiB)

thin_pool.create_lvm_lv("thin1", LvType_THIN, 2 * GiB)
thin_pool.create_lvm_lv("thin2", LvType_THIN, 3 * GiB)

print(staging)

commit(storage)

