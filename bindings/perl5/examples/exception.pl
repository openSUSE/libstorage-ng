#!/usr/bin/perl

use storage;


$device_graph = new storage::DeviceGraph();


# try:
$tmp = storage::BlkDevice::find($device_graph, "/dev/sda");
print $tmp->display_name(), "\n";
# except runtime_error:    # TODO
#   print "device not found, %s" % e.what()

