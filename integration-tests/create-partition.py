#!/usr/bin/python

# requirements: disk /dev/sdb with msdos partition table


from storage import *


class MyCommitCallbacks(CommitCallbacks):

    def __init__(self):
        super(MyCommitCallbacks, self).__init__()

    def message(self, s):
        print "message '%s'" % s

    def error(self, s):
        print "error '%s'" % s
        return False


environment = Environment(False)

storage = Storage(environment)

staging = storage.get_staging()

print staging

disk = Disk.find(staging, "/dev/sdb")

partition_table = disk.get_partition_table()

partition = partition_table.create_partition("/dev/sdb1")

partition.set_type(PRIMARY)
partition.set_id(0x83)
partition.set_region(Region(0, 1000))

print staging

my_commit_callbacks = MyCommitCallbacks()

storage.commit(my_commit_callbacks)

