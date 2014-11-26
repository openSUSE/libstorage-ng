#!/usr/bin/python

from storage import DeviceGraph, Disk, BlkDevice, EXT4, SWAP


device_graph = DeviceGraph()

sda = Disk.create(device_graph, "/dev/sda")

gpt = sda.createPartitionTable("gpt")

sda1 = gpt.createPartition("/dev/sda1")
sda2 = gpt.createPartition("/dev/sda2")

ext4 = sda1.createFilesystem(EXT4)
swap = sda2.createFilesystem(SWAP)

device_graph.print_graph()


print "partitions on gpt:"
for partition in gpt.getPartitions():
  print "  %s %s" % (partition.display_name(), partition.getNumber())
print


print "descendants of sda:"
for device in sda.getDescendants(False):
  print "  %s" % device.display_name()
print


tmp1 = BlkDevice.find(device_graph, "/dev/sda1")
print tmp1.display_name()

