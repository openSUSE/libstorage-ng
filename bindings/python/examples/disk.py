#!/usr/bin/python

import libstorage


device_graph = libstorage.DeviceGraph()

disk = libstorage.Disk.create(device_graph, "/dev/sda")

