#!/usr/bin/python

from storage import DeviceGraph, BlkDevice, device_not_found


device_graph = DeviceGraph()


try:
  tmp = BlkDevice.find(device_graph, "/dev/sda")
  print tmp.display_name()
except device_not_found, e:
  print "device_not_found"

print "done"

