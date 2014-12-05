#!/usr/bin/ruby

require 'storage'


device_graph = Storage::DeviceGraph.new()


begin
  tmp = Storage::BlkDevice.find(device_graph, "/dev/sda")
  puts "#{tmp.display_name()}"
rescue Storage::DeviceNotFound => e
  puts "device not found"
end

puts "done"

