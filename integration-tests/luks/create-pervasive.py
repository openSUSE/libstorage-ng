#!/usr/bin/python3

# requirements: DASD /dev/dasdb with empty partition /dev/dasdb1,
# working cryptographic coprocessor

# This example shows one possible way to use libstorge-ng with
# pervasive encryption.


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

dasdb1 = Partition.find_by_name(staging, "/dev/dasdb1")

luks = to_luks(dasdb1.create_encryption("cr-dasdb1"))

luks.set_type(EncryptionType_LUKS2)
luks.set_key_file("/etc/luks_keys/dasdb1.key")
luks.set_format_options("--master-key-file '/etc/zkey/repository/secure_xtskey1.skey' --key-size 1024 --cipher paes-xts-plain64 --sector-size 4096")

ext4 = luks.create_blk_filesystem(FsType_EXT4)

print(staging)


system("/usr/bin/zkey generate --name secure_xtskey1 --keybits 256 --xts --volumes '/dev/dasdb1:cr-dasdb1' --volume-type LUKS2 --sector-size 4096")

system("/usr/bin/mkdir /etc/luks_keys")
system("/usr/bin/dd if=/dev/urandom of=/etc/luks_keys/dasdb1.key bs=1024 count=4")

commit(storage)

system("/usr/bin/zkey-cryptsetup setvp '/dev/dasdb1' --key-file '/etc/luks_keys/dasdb1.key' ")

# if needed copy key files from /etc to /mnt/etc

