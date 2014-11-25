#!/usr/bin/ruby

require 'storage'


device_graph = Storage::DeviceGraph.new()

sda = Storage::Disk::create(device_graph, "/dev/sda")

gpt = sda.createPartitionTable("gpt")

sda1 = gpt.createPartition("/dev/sda1")
sda2 = gpt.createPartition("/dev/sda2")

ext4 = sda1.createFilesystem("ext4")

device_graph.print_graph()


print "partitions on gpt:\n"
gpt.getPartitions().each do |partition|
  print "  #{partition.display_name()}  #{partition.getNumber()}\n"
end
print "\n"


print "descendants of sda:\n"
sda.getDescendants(false).each do |device|
  print "  #{device.display_name()}\n"
end
print "\n"


tmp1 = Storage::BlkDevice::find(device_graph, "/dev/sda1")
print "#{tmp1.display_name()}\n"

