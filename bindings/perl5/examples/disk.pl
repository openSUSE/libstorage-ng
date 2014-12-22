#!/usr/bin/perl

use strict;
use storage;


my $devicegraph = new storage::Devicegraph();

my $sda = storage::Disk::create($devicegraph, "/dev/sda");

my $gpt = $sda->create_partition_table($storage::GPT);

my $sda1 = $gpt->create_partition("/dev/sda1");
my $sda2 = $gpt->create_partition("/dev/sda2");

my $ext4 = $sda1->create_filesystem($storage::EXT4);
my $swap = $sda2->create_filesystem($storage::SWAP);

print $devicegraph->as_string(), "\n";


print "partitions on gpt:\n";
my $x1 = $gpt->get_partitions();
for (my $i = 0; $i < $x1->size(); $i++)
{
    my $partition = $x1->get($i);

    print "  ", $partition->get_displayname(), " ", $partition->get_number(), "\n";
}
print "\n";


print "descendants of sda:\n";
my $x2 = $sda->get_descendants(0);
for (my $i = 0; $i < $x2->size(); $i++)
{
    my $device = $x2->get($i);

    print "  ", $device->get_displayname(), "\n";
}
print "\n";


my $tmp1 = storage::BlkDevice::find($devicegraph, "/dev/sda1");
print $tmp1->get_displayname(), "\n";

