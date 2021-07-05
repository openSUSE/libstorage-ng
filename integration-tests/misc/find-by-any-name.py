#!/usr/bin/python3

# requirements: partition /dev/sdb1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

probed = storage.get_probed()


try:
    BlkDevice.find_by_name(probed, "/dev/block/8:17")
except DeviceNotFoundByName:
    print("device not found")


system_info = SystemInfo()

blk_device = BlkDevice.find_by_any_name(probed, "/dev/block/8:17", system_info)
print(blk_device.get_name())


try:
    BlkDevice.find_by_any_name(probed, "/dev/disk/by-id/non-existing-device", system_info)
except DeviceNotFoundByName:
    print("device not found")

