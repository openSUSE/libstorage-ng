#!/usr/bin/perl

use storage;


$device_graph = new storage::DeviceGraph();

$sda = storage::Disk::create($device_graph, "/dev/sda");

$gpt = $sda->createPartitionTable("gpt");

print $gpt->display_name(), "\n";

$sda1 = $gpt->createPartition("/dev/sda1");
$sda2 = $gpt->createPartition("/dev/sda2");

$device_graph->print_graph();


print "partitions on gpt:\n";
foreach ($gpt->getPartitions())
{
    # print "  ", $_->display_name(), $_->getNumber(), "\n";
}
print "\n";


print "descendants of sda:\n";
foreach ($sda->getDescendants(false))
{
    # print "  ", $_->display_name(), "\n";
}
print "\n";


$tmp1 = $device_graph->find_blk_device("/dev/sda1");
print $tmp1->display_name(), "\n";


# try:
$tmp3 = $device_graph->find_blk_device("/dev/sda3");
print $tmp3->display_name(), "\n";
# except runtime_error:    # TODO
#   print "device not found, %s" % e.what()

