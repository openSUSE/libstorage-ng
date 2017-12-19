#!/usr/bin/python3

from storage import Environment, Storage, Partitionable


environment = Environment(True)

storage = Storage(environment)
storage.probe()

probed = storage.get_probed()

partitionable = Partitionable.find_by_name(probed, "/dev/sdd")

partition_table = partitionable.get_partition_table()

partition_slots = partition_table.get_unused_partition_slots()

for partition_slot in partition_slots:

  print("%s" % partition_slot)

