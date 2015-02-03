#!/usr/bin/python

# requirements: disk /dev/sdb with msdos partition table and partition /dev/sdb1


from storage import *


environment = Environment(False)

storage = Storage(environment)

current = storage.get_current()

print current

partition = Partition.find(current, "/dev/sdb1")
partition.set_id(ID_LINUX)

ext4 = partition.create_filesystem(EXT4)
ext4.set_label("TEST")
ext4.add_mountpoint("/test")

print current

storage.commit()

