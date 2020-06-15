#!/usr/bin/python3

from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(True)

storage = Storage(environment)
storage.probe()

storage.generate_pools(storage.get_probed())

for name in storage.get_pool_names():
    print(name)

    pool = storage.get_pool(name)
    for device in pool.get_devices(storage.get_probed()):
        print("    " + device.get_displayname())
