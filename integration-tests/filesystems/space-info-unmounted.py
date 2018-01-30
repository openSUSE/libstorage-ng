#!/usr/bin/python3

from sys import exit
from storage import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

nfs = Nfs.create(staging, "server", "/home/surfer")

space_info = nfs.detect_space_info()

print(space_info)

print(byte_to_humanstring(space_info.size, False, 2, True))
print(byte_to_humanstring(space_info.used, False, 2, True))

