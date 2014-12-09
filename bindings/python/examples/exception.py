#!/usr/bin/python

from storage import Devicegraph, BlkDevice, DeviceNotFound


devicegraph = Devicegraph()


try:
  tmp = BlkDevice.find(devicegraph, "/dev/sda")
  print tmp
except DeviceNotFound, e:
  print "device not found"

print "done"

