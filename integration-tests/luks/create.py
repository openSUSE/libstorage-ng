#!/usr/bin/python3

# requirements: disk /dev/sdc with empty partition sdc1


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdc1 = Partition.find_by_name(staging, "/dev/sdc1")

luks = to_luks(sdc1.create_encryption("cr-test", EncryptionType_LUKS2))

luks.set_pbkdf("argon2i")
luks.set_format_options("--pbkdf-memory=1024")
luks.set_password("12345678")
luks.set_label("TOP-SECRET")
luks.set_mount_by(MountByType_LABEL)

print(staging)

commit(storage)

