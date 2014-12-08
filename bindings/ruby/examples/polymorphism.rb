#!/usr/bin/ruby

require 'storage'


device_graph = Storage::DeviceGraph.new()

sda = Storage::Disk::create(device_graph, "/dev/sda")

gpt = sda.create_partition_table(Storage::GPT)

sda1 = gpt.create_partition("/dev/sda1")
sda2 = gpt.create_partition("/dev/sda2")

device_graph.print_graph()


puts "partitions on gpt:"
gpt.get_partitions().each do |partition|
  puts "  #{partition.get_display_name()}  #{partition.get_number()}"
end
puts


puts "descendants of sda:"
sda.get_descendants(false).each do |device|

  partition_table = Storage::to_partition_table(device)
  if partition_table
    puts "  #{partition_table.get_display_name()} is partition table"
  end

  partition = Storage::to_partition(device)
  if partition
    puts "  #{partition.get_display_name()} #{partition.get_number()} is partition"
  end

end
puts
