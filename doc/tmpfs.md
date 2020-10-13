
tmpfs
-----

tmpfs is different from other filesystems libstorage-ng handles in
that it is mounted by the system itself at various places, e.g. /run
or /dev/shm, without using /etc/fstab.

For the time being those system tmpfs mount points are ignored by
libstorage-ng: Only tmpfs listed in /etc/fstab are probed.


Resize not supported. The size is only handled by mount options.

