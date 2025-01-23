#!/usr/bin/python3

# requirements: lvm vg test with physical volume /dev/sdc1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

probed = storage.get_probed()

print(probed)

sdc1 = BlkDevice.find_by_name(probed, "/dev/sdc1")

resize_info = sdc1.detect_resize_info()

print(resize_info)

print(resize_info.resize_ok)
print(byte_to_humanstring(resize_info.min_size, False, 2, True))
print(byte_to_humanstring(resize_info.max_size, False, 2, True))

