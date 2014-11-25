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

  partition_table = Storage::castToPartitionTable(device)
  if partition_table
    print "  #{partition_table.display_name()} is partition table\n"
  end

  partition = Storage::castToPartition(device)
  if partition
    print "  #{partition.display_name()} #{partition.getNumber()} is partition\n"
  end

end
print
