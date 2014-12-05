#!/usr/bin/perl

use strict;
use Test::More tests => 7;
use Test::Exception;
use storage;


my $device_graph = new storage::DeviceGraph();
my $sda = storage::Disk::create($device_graph, "/dev/sda");
my $gpt = $sda->createPartitionTable($storage::GPT);

is($sda->getSid(), 42);
is($gpt->getSid(), 43);

is($device_graph->findDevice(42)->getSid(), 42);

throws_ok { $device_graph->findDevice(99) } 'storage::DeviceNotFound';

is($device_graph->findHolder(42, 43)->getSourceSid(), 42);
is($device_graph->findHolder(42, 43)->getTargetSid(), 43);

throws_ok { $device_graph->findHolder(99, 99) } 'storage::HolderNotFound';
