#!/usr/bin/python3

# requirements: lvm tools installed


from storage import *


set_logger(get_logfile_logger())


status = LvmDevicesFile.status()
print(status)
