#!/usr/bin/perl

use storage;


$storage = new storage::Storage($storage::PROBE_NORMAL, 1);

$probed = $storage->getProbed();

$probed->print_graph();

