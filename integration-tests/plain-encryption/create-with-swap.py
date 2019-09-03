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
plain_encryption.set_key_file("/dev/urandom")
plain_encryption.set_crypt_options(VectorString([ "swap" ]))
plain_encryption.set_mount_by(MountByType_ID)

swap = plain_encryption.create_blk_filesystem(FsType_SWAP)

mount_point = swap.create_mount_point("swap")
mount_point.set_mount_by(MountByType_DEVICE)

print(staging)

commit(storage)

