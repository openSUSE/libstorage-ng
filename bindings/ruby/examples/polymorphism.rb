#!/usr/bin/ruby

require 'storage'


devicegraph = Storage::Devicegraph.new()

sda = Storage::Disk::create(devicegraph, "/dev/sda")

gpt = sda.create_partition_table(Storage::PtType_GPT)

sda1 = gpt.create_partition("/dev/sda1", Storage::Region.new(0, 100, 262144), Storage::PRIMARY)
sda2 = gpt.create_partition("/dev/sda2", Storage::Region.new(100, 100, 262144), Storage::PRIMARY)

print devicegraph


puts "partitions on gpt:"
gpt.partitions().each do |partition|
  puts "  #{partition}  #{partition.number()}"
end
puts


puts "descendants of sda:"
sda.descendants(false).each do |device|

  begin
    partition_table = Storage::to_partition_table(device)
    puts "  #{partition_table} is partition table"
  rescue Storage::DeviceHasWrongType
  end

  begin
    partition = Storage::to_partition(device)
    puts "  #{partition} #{partition.number()} is partition"
  rescue Storage::DeviceHasWrongType
  end

end
puts
