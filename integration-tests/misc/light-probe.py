#!/usr/bin/python3

# requirements: some disks or nothing


from storage import *


set_logger(get_logfile_logger())

something = light_probe()

print(something)

