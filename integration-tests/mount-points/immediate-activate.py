#!/usr/bin/python3

# requirements: partition sdb1 with blk filesystem listed /etc/fstab


from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

system = storage.get_system()

print(system)

sdb1 = BlkDevice.find_by_name(system, "/dev/sdb1")

blk_filesystem = sdb1.get_blk_filesystem()

mount_point = blk_filesystem.get_mount_point()

if mount_point.is_active():
    mount_point.immediate_deactivate()
else:
    mount_point.immediate_activate()

