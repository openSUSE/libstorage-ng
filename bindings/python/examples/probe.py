#!/usr/bin/python3

from storage import Environment, Storage


environment = Environment(True)

storage = Storage(environment)
storage.probe()

probed = storage.get_probed()

print(probed)

