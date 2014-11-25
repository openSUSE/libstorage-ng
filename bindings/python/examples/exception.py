#!/usr/bin/python

from storage import DeviceGraph, BlkDevice


device_graph = DeviceGraph()


try:
  tmp = BlkDevice.find(device_graph, "/dev/sda")
  print tmp.display_name()
except runtime_error:    # TODO
  print "device not found, %s" % e.what()

