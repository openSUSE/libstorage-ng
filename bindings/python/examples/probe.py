#!/usr/bin/python

from storage import Environment, Storage


environment = Environment(True)

storage = Storage(environment)
storage.probe()

probed = storage.get_probed()

print(probed)

