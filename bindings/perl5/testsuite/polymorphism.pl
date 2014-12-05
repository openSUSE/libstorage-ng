#!/usr/bin/perl

use strict;
use Test::More tests => 6;
use Test::Exception;
use storage;


my $device_graph = new storage::DeviceGraph();
my $sda = storage::Disk::create($device_graph, "/dev/sda");
my $gpt = $sda->createPartitionTable($storage::GPT);

is($sda->getSid(), 42);
is($gpt->getSid(), 43);

my $tmp1 = $device_graph->findDevice(42);
isnt(storage::toDisk($tmp1), undef);
is(storage::toPartitionTable($tmp1), undef);

my $tmp2 = $device_graph->findDevice(43);
isnt(storage::toPartitionTable($tmp2), undef);
is(storage::toDisk($tmp2), undef);
