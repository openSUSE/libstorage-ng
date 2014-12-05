#!/usr/bin/perl

use strict;
use storage;


my $device_graph = new storage::DeviceGraph();

my $sda = storage::Disk::create($device_graph, "/dev/sda");

my $gpt = $sda->createPartitionTable($storage::GPT);

my $sda1 = $gpt->createPartition("/dev/sda1");
my $sda2 = $gpt->createPartition("/dev/sda2");

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

    my $partition_table = storage::toPartitionTable($device);
    if ($partition_table)
    {
	print "  ", $partition_table->display_name(), " is partition table", "\n";
    }

    my $partition = storage::toPartition($device);
    if ($partition)
    {
	print "  ", $partition->display_name(), " ", $partition->getNumber(), " is partition\n";
    }
}
print "\n";
