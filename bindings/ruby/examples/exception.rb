#!/usr/bin/ruby

require 'storage'


device_graph = Storage::DeviceGraph.new()


begin
  tmp = Storage::BlkDevice::find(device_graph, "/dev/sda")
  print "#{tmp.display_name()}\n"
rescue Storage::Device_not_found => e
  print "device_not_found\n"
end

print "done\n"

