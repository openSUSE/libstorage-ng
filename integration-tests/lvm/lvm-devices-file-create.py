#!/usr/bin/python3

# requirements: lvm tools installed


from storage import *


set_logger(get_logfile_logger())


environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

LvmDevicesFile.create(staging)
