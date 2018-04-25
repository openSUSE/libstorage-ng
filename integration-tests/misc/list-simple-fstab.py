#!/usr/bin/python3

# requirements: /etc/fstab


from storage import *


entries = read_simple_etc_fstab("/etc/fstab")

for entry in entries:

    print(entry.device, entry.mount_point, get_fs_type_name(entry.fs_type),
          ",".join(entry.mount_options), entry.fs_freq, entry.fs_passno)

