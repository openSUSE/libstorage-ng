#!/usr/bin/python

from storage import DeviceGraph, BlkDevice, DeviceNotFound


device_graph = DeviceGraph()


try:
  tmp = BlkDevice.find(device_graph, "/dev/sda")
  print tmp.getDisplayName()
except DeviceNotFound, e:
  print "device not found"

print "done"

