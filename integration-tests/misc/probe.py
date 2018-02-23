#!/usr/bin/python3

# requirements: something to probe


from sys import exit
from storage import *
from storageitu import *


class MyProbeCallbacks(ProbeCallbacks):

    def __init__(self):
        super(MyProbeCallbacks, self).__init__()

    def message(self, message):
        print("message callback")
        print(message)

    def error(self, message, what):
        print("error callback")
        print(message)
        print(what)
        return True


set_logger(get_logfile_logger())

my_probe_callbacks = MyProbeCallbacks()

environment = Environment(False)

storage = Storage(environment)

try:
    storage.probe(my_probe_callbacks)
except Exception as exception:
    print(exception.what())
    exit(1)

probed = storage.get_probed()

print(probed)

probed.save("devicegraph.xml")

