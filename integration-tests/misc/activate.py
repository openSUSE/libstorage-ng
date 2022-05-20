#!/usr/bin/python3

# requirements: something to activate, e.g. LUKS and LVM


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

class MyActivateCallbacks(ActivateCallbacksV3):

    def __init__(self):
        super(MyActivateCallbacks, self).__init__()

    def begin(self):
        print("begin callback")

    def end(self):
        print("end callback")

    def multipath(self, looks_like_real_multipath):
        print("multipath callback")
        return looks_like_real_multipath

    def luks(self, luks_info, attempt):
        print("luks callback name:%s dm-table-name:%s size:%u label:%s uuid:%s attempt:%d" %
              (luks_info.get_device_name(), luks_info.get_dm_table_name(), luks_info.get_size(),
               luks_info.get_label(), luks_info.get_uuid(), attempt))
        if attempt == 2:
            return PairBoolString(True, "12345678")
        else:
            return PairBoolString(True, "wrong")

    def bitlocker(self, bitlocker_info, attempt):
        print("bitlocker callback name:%s dm-table-name:%s size:%u uuid:%s attempt:%d" %
              (bitlocker_info.get_device_name(), bitlocker_info.get_dm_table_name(),
               bitlocker_info.get_size(), bitlocker_info.get_uuid(), attempt))
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
        return False


my_activate_callbacks = MyActivateCallbacks()

environment = Environment(False)

storage = Storage(environment)

try:
    storage.activate(my_activate_callbacks)
except Exception as exception:
    print(exception.what())
    exit(1)
