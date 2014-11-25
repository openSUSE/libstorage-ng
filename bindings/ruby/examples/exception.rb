#!/usr/bin/ruby

require 'storage'


device_graph = Storage::DeviceGraph.new()


begin
  tmp = Storage::BlkDevice::find(device_graph, "/dev/sda")
  print "#{tmp.display_name()}\n"
rescue runtime_error => e       # TODO
  print "device not found, #{e.what()}\n"
end
