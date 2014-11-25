#!/usr/bin/perl

use storage;


$device_graph = new storage::DeviceGraph();

$sda = storage::Disk::create($device_graph, "/dev/sda");

$gpt = $sda->createPartitionTable("gpt");

$sda1 = $gpt->createPartition("/dev/sda1");
$sda2 = $gpt->createPartition("/dev/sda2");

$device_graph->print_graph();


print "partitions on gpt:\n";
$x1 = $gpt->getPartitions();
for (my $i = 0; $i < $x1->size(); $i++)
{
    $partition = $x1->get($i);

    print "  ", $partition->display_name(), " ", $partition->getNumber(), "\n";
}
print "\n";


print "descendants of sda:\n";
$x2 = $sda->getDescendants(0);
for (my $i = 0; $i < $x2->size(); $i++)
{
    $device = $x2->get($i);

    $partition_table = storage::castToPartitionTable($device);
    if ($partition_table)
    {
	print "  ", $partition_table->display_name(), " is partition table", "\n";
    }

    $partition = storage::castToPartition($device);
    if ($partition)
    {
	print "  ", $partition->display_name(), " ", $partition->getNumber(), " is partition\n";
    }
}
print "\n";
