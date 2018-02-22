#!/usr/bin/python3

# requirements: something to activate, e.g. LUKS and LVM


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

class MyActivateCallbacks(ActivateCallbacks):

    def __init__(self):
        super(MyActivateCallbacks, self).__init__()

    def multipath(self):
        print("multipath callback")
        return True

    def luks(self, uuid, attempt):
        print("luks callback uuid:%s attempt:%d" % (uuid, attempt))
        if attempt == 2:
            return PairBoolString(True, "12345678")
        else:
            return PairBoolString(True, "wrong")

    def message(self, message):
        print("message callback")
        print(message)

    def error(self, message, what):
        print("error callback")
        print(message)
        print(what)
        return True


my_activate_callbacks = MyActivateCallbacks()

environment = Environment(False)

storage = Storage(environment)

storage.activate(my_activate_callbacks)

