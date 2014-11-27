#!/usr/bin/ruby

require 'storage'


storage = Storage::Storage.new(Storage::PROBE_NORMAL, true)

probed = storage.getProbed()

probed.print_graph()

