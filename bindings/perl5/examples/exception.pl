#!/usr/bin/perl

use storage;


$device_graph = new storage::DeviceGraph();


eval
{
    $tmp = storage::BlkDevice::find($device_graph, "/dev/sda");
    print $tmp->display_name(), "\n";
};
if ($@)
{
    if ($@->isa("storage::device_not_found"))
    {
	print "device_not_found\n";
    }
}

print "done\n";

