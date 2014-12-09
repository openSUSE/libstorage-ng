#!/usr/bin/ruby

require 'storage'


devicegraph = Storage::Devicegraph.new()

sda = Storage::Disk::create(devicegraph, "/dev/sda")

gpt = sda.create_partition_table(Storage::GPT)

sda1 = gpt.create_partition("/dev/sda1")
sda2 = gpt.create_partition("/dev/sda2")

devicegraph.print_graph()


puts "partitions on gpt:"
gpt.partitions().each do |partition|
  puts "  #{partition}  #{partition.number()}"
end
puts


puts "descendants of sda:"
sda.descendants(false).each do |device|

  partition_table = Storage::to_partition_table(device)
  if partition_table
    puts "  #{partition_table} is partition table"
  end

  partition = Storage::to_partition(device)
  if partition
    puts "  #{partition} #{partition.number()} is partition"
  end

end
puts
