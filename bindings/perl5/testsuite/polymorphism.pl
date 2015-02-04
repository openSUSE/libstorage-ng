#!/usr/bin/perl

use strict;
use Test::More tests => 10;
use Test::Exception;
use storage;

my $environment = new storage::Environment(1, $storage::PROBE_NONE, $storage::TARGET_NORMAL);

my $storage = new storage::Storage($environment);

my $devicegraph = new storage::Devicegraph($storage);
my $sda = storage::Disk::create($devicegraph, "/dev/sda");
my $gpt = $sda->create_partition_table($storage::GPT);

is($sda->get_sid(), 42);
is($gpt->get_sid(), 43);

my $tmp1 = $devicegraph->find_device(42);
is(storage::is_disk($tmp1), 1);
isnt(storage::is_partition_table($tmp1), 1);
isnt(storage::to_disk($tmp1), undef);
is(storage::to_partition_table($tmp1), undef);

my $tmp2 = $devicegraph->find_device(43);
is(storage::is_partition_table($tmp2), 1);
isnt(storage::is_disk($tmp2), 1);
isnt(storage::to_partition_table($tmp2), undef);
is(storage::to_disk($tmp2), undef);
