#!/usr/bin/python3

# requirements: something to probe


from sys import exit
from storage import *
from storageitu import *


class MyProbeCallbacks(ProbeCallbacksV3):

    def __init__(self):
        super(MyProbeCallbacks, self).__init__()

    def begin(self):
        print("begin callback")

    def end(self):
        print("end callback")

    def message(self, message):
        print("message callback")
        print(message)

    def error(self, message, what):
        print("error callback")
        print(message)
        print(what)
        return True

    def missing_command(self, message, what, command, used_features):
        print("missing command callback")
        print(message)
        print(what)
        print(command)
        print(used_features)
        return False


set_logger(get_logfile_logger())

my_probe_callbacks = MyProbeCallbacks()

environment = Environment(True)

try:
    storage = Storage(environment)
except LockException as exception:
    print(exception.what())
    print("locker pid %d" % exception.get_locker_pid())
    exit(1)

try:
    storage.probe(my_probe_callbacks)
except Exception as exception:
    print("caught exception")
    print(exception.what())
    exit(1)

probed = storage.get_probed()

print(probed)

probed.save("devicegraph.xml")

