#!/usr/bin/python

from storage import Environment, Storage


environment = Environment(True)

storage = Storage(environment)

probed = storage.get_probed()

probed.print_graph()

