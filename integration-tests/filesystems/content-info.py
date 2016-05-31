#!/usr/bin/python

# requirements: partition /dev/sdb1 with a filesystem


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)

probed = storage.get_probed()

print probed

partition = Partition.find_by_name(probed, "/dev/sdb1")
filesystem = partition.get_filesystem()

content_info = filesystem.detect_content_info()

print content_info

print content_info.is_windows
print content_info.is_efi
print content_info.num_homes

