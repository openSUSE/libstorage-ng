#!/usr/bin/perl

use strict;
use storage;


my $device_graph = new storage::DeviceGraph();

my $sda = storage::Disk::create($device_graph, "/dev/sda");

my $gpt = $sda->createPartitionTable($storage::GPT);

my $sda1 = $gpt->createPartition("/dev/sda1");
my $sda2 = $gpt->createPartition("/dev/sda2");

my $ext4 = $sda1->createFilesystem($storage::EXT4);
my $swap = $sda2->createFilesystem($storage::SWAP);

$device_graph->print_graph();


print "partitions on gpt:\n";
my $x1 = $gpt->getPartitions();
for (my $i = 0; $i < $x1->size(); $i++)
{
    my $partition = $x1->get($i);

    print "  ", $partition->display_name(), " ", $partition->getNumber(), "\n";
}
print "\n";


print "descendants of sda:\n";
my $x2 = $sda->getDescendants(0);
for (my $i = 0; $i < $x2->size(); $i++)
{
    my $device = $x2->get($i);

    print "  ", $device->display_name(), "\n";
}
print "\n";


my $tmp1 = storage::BlkDevice::find($device_graph, "/dev/sda1");
print $tmp1->display_name(), "\n";

