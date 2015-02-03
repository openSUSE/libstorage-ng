#!/usr/bin/python

# requirements: disk /dev/sdb with msdos partition table and partition /dev/sdb1


from storage import *


environment = Environment(False)

storage = Storage(environment)

current = storage.get_current()

print current

partition = Partition.find(current, "/dev/sdb1")
partition.set_id(ID_SWAP)

swap = partition.create_filesystem(SWAP)
swap.add_mountpoint("swap")

print current

storage.commit()

