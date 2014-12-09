#!/usr/bin/ruby

require 'storage'


devicegraph = Storage::Devicegraph.new()


begin
  tmp = Storage::BlkDevice.find(devicegraph, "/dev/sda")
  puts "#{tmp}"
rescue Storage::DeviceNotFound => e
  puts "device not found"
end

puts "done"

