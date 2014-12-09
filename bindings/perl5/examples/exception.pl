#!/usr/bin/perl

use strict;
use storage;


my $devicegraph = new storage::Devicegraph();


eval
{
    my $tmp = storage::BlkDevice::find($devicegraph, "/dev/sda");
    print $tmp->get_displayname(), "\n";
};
if ($@)
{
    if ($@->isa("storage::DeviceNotFound"))
    {
	print "device not found\n";
    }
}

print "done\n";

