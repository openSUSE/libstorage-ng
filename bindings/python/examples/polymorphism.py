#!/usr/bin/python

import storage


devicegraph = storage.Devicegraph()

sda = storage.Disk.create(devicegraph, "/dev/sda")

gpt = sda.create_partition_table(storage.GPT)

sda1 = gpt.create_partition("/dev/sda1")
sda2 = gpt.create_partition("/dev/sda2")

print devicegraph


print "partitions on gpt:"
for partition in gpt.get_partitions():
  print "  %s %s" % (partition, partition.get_number())
print


print "descendants of sda:"
for device in sda.get_descendants(False):

  partition_table = storage.to_partition_table(device)
  if partition_table:
    print "  %s is partition table" % partition_table
    
  partition = storage.to_partition(device)
  if partition:
    print "  %s %s is partition" % (partition, partition.get_number())

print

