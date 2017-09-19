
Resize
======

Unfortunately the terminology for resizing is not consistent in the Linux
world. In this document we use the following definitions:

- [Shrink/Grow] Change the size of a non-container block device, e.g. change
  the size of a partition from 1GiB to 2GiB.

- [Extend/Reduce] Add or remove a block device to or from a container,
  e.g. add a partition to a MD RAID.

Whether a block devices can be shrunk or grown depends not only on the block
device itself but also on the holder of the block device. E.g. a partition can
in general be grown but if the partition is held by a MD RAID growing is not
possible since MD RAID does not support to grow the underlying devices.

Also note that most block devices and filesystems must be inactive to allow
shrinking or growing.


Shrink/Grow
-----------

- Primary and logical partitions: yes.

- Extended partition: no.

- LUKS devices: yes.

- LVM logical volumes: yes.

- Block devices held by a LVM volume group: yes.

- Block devices held by a MD RAID: no. A cumbersome and slow procedure exists
  but is not supported, see
  http://superuser.com/questions/469117/resize-underlying-partitions-in-mdadm-raid1
  or https://raid.wiki.kernel.org/index.php/Growing.

- Block devices held by a btrfs: yes.


Extending/Reducing
------------------

- MD RAIDs: yes, depending on RAID level.

- LVM volume groups: yes.

- btrfs: yes.


Filesystems
-----------

- ext2/3/4: shrink and grow.

- btrfs: shrink and grow.

- XFS: grow

- reiserfs: shrink and grow

- NTFS: shrink and grow.

- VFAT: shrink and grow.

- swap: shrink and grow (only via recreate, involves data loss).


Using Resize Functionality
--------------------------

To use the resize functionality always set the size on the underlying block
device. E.g. if you have a partition with LUKS and a filesystem on top, set
size on the partition.

Also use detect_resize_info() on the underlying block device. That function
returns ResizeInfo object with the minimal and maximal size for a resize
operation. Note that the minimal and maximal are not aligned.

Limitations of ResizeInfo:

- Although LVM PVs on-disk can be shrunk with limitations (see pvresize) the
  ResizeInfo reports them as if shrinking is not possible.

