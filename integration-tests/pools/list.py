#!/usr/bin/python3

from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(True)

storage = Storage(environment)
storage.probe()

probed = storage.get_probed()

storage.generate_pools(probed)

for name, pool in storage.get_pools().items():
    print(name)

    for device in pool.get_devices(probed):
        print("    " + device.get_displayname())
