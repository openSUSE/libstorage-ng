#!/usr/bin/perl

use strict;
use Test::More tests => 6;
use Test::Exception;
use storage;


my $devicegraph = new storage::Devicegraph();
my $sda = storage::Disk::create($devicegraph, "/dev/sda");
my $gpt = $sda->create_partition_table($storage::GPT);

is($sda->get_sid(), 42);
is($gpt->get_sid(), 43);

my $tmp1 = $devicegraph->find_device(42);
isnt(storage::to_disk($tmp1), undef);
is(storage::to_partition_table($tmp1), undef);

my $tmp2 = $devicegraph->find_device(43);
isnt(storage::to_partition_table($tmp2), undef);
is(storage::to_disk($tmp2), undef);
