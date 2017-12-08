#!/usr/bin/python

# requirements: none


from sys import exit
from storage import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(Md.find_free_numeric_name(staging))

