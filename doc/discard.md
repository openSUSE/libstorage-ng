
Discard
-------

libstorage-ng does calls to blkdiscard e.g. before creating a
partition table and after creating a partition.

In general that should not have a huge advantage since most several
file systems do discard on their own during mkfs (e.g. ext4 and xfs)
or fstrim will do so. But if areas are left unpartitioned it should
be beneficial.

