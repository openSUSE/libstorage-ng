#!/usr/bin/ruby

require 'storage'


device_graph = Storage::DeviceGraph.new()

sda = Storage::Disk::create(device_graph, "/dev/sda")

gpt = sda.createPartitionTable("gpt")

sda1 = gpt.createPartition("/dev/sda1")
sda2 = gpt.createPartition("/dev/sda2")

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


tmp1 = device_graph.find_blk_device("/dev/sda1")
print "#{tmp1.display_name()}\n"


begin
  tmp3 = device_graph.find_blk_device("/dev/sda3")
  print "#{tmp3.display_name()}\n"
rescue runtime_error => e       # TODO
  print "device not found, #{e.what()}\n"
end
