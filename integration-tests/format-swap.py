#!/usr/bin/python

# requirements: disk /dev/sdb with msdos partition table and partition /dev/sdb1


from storage import *


environment = Environment(False)

storage = Storage(environment)

current = storage.get_current()

print current

blkdevice = BlkDevice.find(current, "/dev/sdb1")

blkdevice.create_filesystem(SWAP)

print current

storage.commit()

