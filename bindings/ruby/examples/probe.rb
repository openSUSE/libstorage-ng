#!/usr/bin/ruby

require 'storage'


environment = Storage::Environment.new(true)

storage = Storage::Storage.new(environment)

probed = storage.get_probed()

probed.print_graph()

