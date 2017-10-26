#!/usr/bin/ruby

require 'storage'


environment = Storage::Environment.new(true)

storage = Storage::Storage.new(environment)
storage.probe()

probed = storage.probed()

partitionable = Storage::Partitionable::find_by_name(probed, "/dev/sdd")

partition_table = partitionable.partition_table()

partition_slots = partition_table.unused_partition_slots()

partition_slots.each() do |partition_slot|

  puts "#{partition_slot}"

end

