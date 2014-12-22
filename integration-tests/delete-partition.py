#!/usr/bin/python

# requirements: disk /dev/sdb with msdos partition table and partition /dev/sdb1


from storage import *


environment = Environment(False)

storage = Storage(environment)

current = storage.get_current()

print current

disk = Disk.find(current, "/dev/sdb")

partition_table = disk.get_partition_table()

partition_table.delete_partition("/dev/sdb1")

print current

storage.commit()

