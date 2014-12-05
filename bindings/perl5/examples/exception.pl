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
    if ($@->isa("storage::DeviceNotFound"))
    {
	print "device not found\n";
    }
}

print "done\n";

