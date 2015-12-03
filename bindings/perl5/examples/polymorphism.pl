#!/usr/bin/perl

use strict;
use storage;


my $devicegraph = new storage::Devicegraph();

my $sda = storage::Disk::create($devicegraph, "/dev/sda");

my $gpt = $sda->create_partition_table($storage::PtType_GPT);

my $sda1 = $gpt->create_partition("/dev/sda1", new storage::Region(0, 100, 262144), $storage::PRIMARY);
my $sda2 = $gpt->create_partition("/dev/sda2", new storage::Region(100, 100, 262144), $storage::PRIMARY);

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

    my $partition_table = storage::to_partition_table($device);
    if ($partition_table)
    {
	print "  ", $partition_table->get_displayname(), " is partition table", "\n";
    }

    my $partition = storage::to_partition($device);
    if ($partition)
    {
	print "  ", $partition->get_displayname(), " ", $partition->get_number(), " is partition\n";
    }
}
print "\n";
