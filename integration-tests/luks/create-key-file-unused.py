#!/usr/bin/python3

# requirements: empty partition /dev/sdc1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")

luks = to_luks(sdc1.create_encryption("cr-test"))
luks.set_type(EncryptionType_LUKS2)

# password is needed since the key-file is not used in commit

luks.set_key_file("/root/does-not-exist.key")
luks.set_use_key_file_in_commit(False)
luks.set_password("12345678")

ext4 = luks.create_blk_filesystem(FsType_EXT4)

print(staging)

commit(storage)

