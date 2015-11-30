#!/usr/bin/python

import storage


devicegraph = storage.Devicegraph()

sda = storage.Disk.create(devicegraph, "/dev/sda")

gpt = sda.create_partition_table(storage.PtType_GPT)

sda1 = gpt.create_partition("/dev/sda1", storage.PRIMARY)
sda2 = gpt.create_partition("/dev/sda2", storage.PRIMARY)

print devicegraph


print "partitions on gpt:"
for partition in gpt.get_partitions():
  print "  %s %s" % (partition, partition.get_number())
print


print "descendants of sda:"
for device in sda.get_descendants(False):

  try:
    partition_table = storage.to_partition_table(device)
    print "  %s is partition table" % partition_table
  except storage.DeviceHasWrongType:
    pass

  try:
    partition = storage.to_partition(device)
    print "  %s %s is partition" % (partition, partition.get_number())
  except storage.DeviceHasWrongType:
    pass

print

