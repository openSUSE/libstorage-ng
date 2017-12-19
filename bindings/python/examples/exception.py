#!/usr/bin/python3

from storage import Environment, Storage, BlkDevice, DeviceNotFound


environment = Environment(True)

storage = Storage(environment)
storage.probe()

probed = storage.get_probed()

try:
  tmp = BlkDevice.find_by_name(probed, "/dev/sdz")
  print(tmp)
except DeviceNotFound, e:
  print("device not found")

print("done")

