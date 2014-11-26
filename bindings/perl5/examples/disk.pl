#!/usr/bin/perl

use storage;


$device_graph = new storage::DeviceGraph();

$sda = storage::Disk::create($device_graph, "/dev/sda");

$gpt = $sda->createPartitionTable("gpt");

$sda1 = $gpt->createPartition("/dev/sda1");
$sda2 = $gpt->createPartition("/dev/sda2");

$ext4 = $sda1->createFilesystem($storage::EXT4);
$swap = $sda2->createFilesystem($storage::SWAP);

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

    print "  ", $device->display_name(), "\n";
}
print "\n";


$tmp1 = storage::BlkDevice::find($device_graph, "/dev/sda1");
print $tmp1->display_name(), "\n";

