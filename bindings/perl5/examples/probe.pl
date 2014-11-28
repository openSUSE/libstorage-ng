#!/usr/bin/perl

use storage;


$environment = new storage::Environment(1);

$storage = new storage::Storage($environment);

$probed = $storage->getProbed();

$probed->print_graph();

