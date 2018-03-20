#!/usr/bin/python3

# requirements: blk device /dev/sdb1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

blk_device = BlkDevice.find_by_name(staging, "/dev/sdb1")

region = blk_device.get_region()

resize_info = blk_device.detect_resize_info()

if not resize_info.resize_ok:
    print("resize not possible")
    exit(1)

new_length = int(resize_info.max_size / region.get_block_size())

if new_length == region.get_length():
    print("already maximum size")
    exit(0)

region.set_length(new_length)

blk_device.set_region(region)

print(staging)

commit(storage)
