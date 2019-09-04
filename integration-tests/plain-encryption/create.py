#!/usr/bin/python3

# requirements: disk /dev/sdc with empty partition sdc1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")

plain_encryption = to_plain_encryption(sdc1.create_encryption("cr-test", EncryptionType_PLAIN))
plain_encryption.set_password("secret")
plain_encryption.set_crypt_options(VectorString([ "plain" ]))

print(staging)

commit(storage)

