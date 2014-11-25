#!/usr/bin/python

import storage


device_graph = storage.DeviceGraph()

sda = storage.Disk.create(device_graph, "/dev/sda")

gpt = sda.createPartitionTable("gpt")

print gpt.display_name()

sda1 = gpt.createPartition("/dev/sda1")
sda2 = gpt.createPartition("/dev/sda2")

device_graph.print_graph()


print "partitions on gpt:"
for partition in gpt.getPartitions():
  print "  %s %s" % (partition.display_name(), partition.getNumber())
print


print "descendants of sda:"
for device in sda.getDescendants(False):
  print "  %s" % device.display_name()
print


tmp1 = device_graph.find_blk_device("/dev/sda1")
print tmp1.display_name()


try:
  tmp3 = device_graph.find_blk_device("/dev/sda3")
  print tmp3.display_name()
except runtime_error:    # TODO
  print "device not found, %s" % e.what()

