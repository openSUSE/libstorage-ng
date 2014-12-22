#!/usr/bin/perl

use strict;
use storage;


my $environment = new storage::Environment(1);

my $storage = new storage::Storage($environment);

my $probed = $storage->get_probed();

print $probed->as_string(), "\n";

