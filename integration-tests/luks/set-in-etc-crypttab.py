#!/usr/bin/python3

# requirements: partition sdc1 with LUKS


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

sdc1 = BlkDevice.find_by_name(staging, "/dev/sdc1")

encryption = sdc1.get_encryption()

encryption.set_in_etc_crypttab(not encryption.is_in_etc_crypttab())
encryption.set_default_mount_by()

print(staging)

commit(storage)

