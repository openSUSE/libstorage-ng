#!/usr/bin/ruby

require 'storage'


device_graph = Storage::DeviceGraph.new()

sda = Storage::Disk.create(device_graph, "/dev/sda")

gpt = sda.create_partition_table(Storage::GPT)

sda1 = gpt.create_partition("/dev/sda1")
sda2 = gpt.create_partition("/dev/sda2")

ext4 = sda1.create_filesystem(Storage::EXT4)
swap = sda2.create_filesystem(Storage::SWAP)

device_graph.print_graph()


puts "partitions on gpt:"
gpt.get_partitions().each do |partition|
  puts "  #{partition.get_display_name()}  #{partition.get_number()}"
end
puts


puts "descendants of sda:"
sda.get_descendants(false).each do |device|
  puts "  #{device.get_display_name()}"
end
puts


tmp1 = Storage::BlkDevice::find(device_graph, "/dev/sda1")
puts "#{tmp1.get_display_name()}"

