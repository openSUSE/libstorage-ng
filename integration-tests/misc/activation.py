#!/usr/bin/python

# requirements: something to activate, e.g. LUKS or LVM


from storage import *
from storageitu import *

set_logger(get_logfile_logger())


class MyActivationCallbacks(ActivationCallbacks):

    def __init__(self):
        super(MyActivationCallbacks, self).__init__()

    def multipath(self):
        print "multipath callback"
        return True

    def luks(self, uuid, attempt):
        print "luks callback uuid:%s attempt:%d" % (uuid, attempt)
        if attempt == 2:
            return PairBoolString(True, "12345678")
        else:
            return PairBoolString(True, "wrong")

my_activation_callbacks = MyActivationCallbacks()

environment = Environment(False)

storage = Storage(environment, my_activation_callbacks)

