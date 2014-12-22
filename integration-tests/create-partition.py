#!/usr/bin/python

# requirements: disk /dev/sdb with msdos partition table


from storage import *


environment = Environment(False)

storage = Storage(environment)

current = storage.get_current()

print current

disk = Disk.find(current, "/dev/sdb")

partition_table = disk.get_partition_table()

partition = partition_table.create_partition("/dev/sdb1")

partition.set_type(PRIMARY)
partition.set_id(0x83)
partition.set_region(Region(0, 1000))

print current

storage.commit()

