#!/usr/bin/python

from storage import Devicegraph, Disk, BlkDevice, PtType_GPT, EXT4, SWAP, PRIMARY


devicegraph = Devicegraph()

sda = Disk.create(devicegraph, "/dev/sda")

gpt = sda.create_partition_table(PtType_GPT)

sda1 = gpt.create_partition("/dev/sda1", PRIMARY)
sda2 = gpt.create_partition("/dev/sda2", PRIMARY)

ext4 = sda1.create_filesystem(EXT4)
swap = sda2.create_filesystem(SWAP)

print devicegraph


print "partitions on gpt:"
for partition in gpt.get_partitions():
  print "  %s %s" % (partition, partition.get_number())
print


print "descendants of sda:"
for device in sda.get_descendants(False):
  print "  %s" % device
print


tmp1 = BlkDevice.find(devicegraph, "/dev/sda1")
print tmp1

