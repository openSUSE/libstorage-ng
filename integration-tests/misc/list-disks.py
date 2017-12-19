#!/usr/bin/python3

# requirements: some disks


from sys import exit
from storage import *
from storageitu import *
from functools import cmp_to_key


# place USB disks at the end

def compare(lhs, rhs):

    lhs_usb = lhs.get_transport() == Transport_USB
    rhs_usb = rhs.get_transport() == Transport_USB

    if lhs_usb != rhs_usb:
        return -1 if lhs_usb < rhs_usb else +1

    return -1 if Disk.compare_by_name(lhs, rhs) else +1


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

disks = Disk.get_all(staging)


for disk in sorted(disks, key = cmp_to_key(compare)):

    print("%s" % disk.get_name())

