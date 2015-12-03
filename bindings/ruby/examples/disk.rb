#!/usr/bin/ruby

require 'storage'


devicegraph = Storage::Devicegraph.new()

sda = Storage::Disk.create(devicegraph, "/dev/sda")

gpt = sda.create_partition_table(Storage::PtType_GPT)

sda1 = gpt.create_partition("/dev/sda1", Storage::Region.new(0, 100, 262144), Storage::PRIMARY)
sda2 = gpt.create_partition("/dev/sda2", Storage::Region.new(100, 100, 262144), Storage::PRIMARY)

ext4 = sda1.create_filesystem(Storage::EXT4)
swap = sda2.create_filesystem(Storage::SWAP)

print devicegraph


puts "partitions on gpt:"
gpt.partitions().each do |partition|
  puts "  #{partition}  #{partition.number()}"
end
puts


puts "descendants of sda:"
sda.descendants(false).each do |device|
  puts "  #{device}"
end
puts


tmp1 = Storage::BlkDevice::find(devicegraph, "/dev/sda1")
puts "#{tmp1}"

