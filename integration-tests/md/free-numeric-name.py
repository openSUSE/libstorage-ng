#!/usr/bin/python3

# requirements: none


from storage import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(Md.find_free_numeric_name(staging))

