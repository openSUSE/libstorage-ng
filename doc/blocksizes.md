# Combining devices with different block sizes

This document applies equally to SLE15-SP1, SLE15-SP2, and Tumbleweed-20200405
unless differences are specifically noted.

## RAID

Combining is always possible.

The RAID block size is the maximum of the block size of the involved disk
devices.

This implies that the RAID block size may change when a disk is replaced or
added to the RAID.

## LVM

Generally, only block sizes of 512 and 4096 are supported. Trying to use other block
sizes fails:

```sh
> cat /sys/block/sdd/queue/logical_block_size
1024
> vgcreate test2 /dev/sdd
  Error writing device /dev/sdd at 4096 length 512.
  Failed to write mda header to /dev/sdd fd -1
  Failed to write physical volume "/dev/sdd".
```

In SLE15-SP2 and TW `/etc/lvm/lvm.conf` has an option
`allow_mixed_block_sizes` which defaults to 0.

Unless set to 1, `vgcreate` (and `vgextend`) will refuse to create a VG
consisting of PVs of different block sizes.

Existing VGs can always be activated with `vgchange`.

Up to SLE15-SP1 the option did not exist (and you could mix different block
sizes).

LVs get the maximum of the block size of the PVs used.

`lvresize` changes the block size as needed. This implies that the block size
may decrease when the LV size is reduced.

If there's a filesystem on an LV and the block size changes as a result of an
`lvresize`, things can go wrong (XFS) or seem to continue to work (BTRFS,
EXT4).

An already mounted XFS file system continues to work when the block size changes.
But you cannot resize it nor remount after you unmounted.

Here's what happens when you try to mount an XFS created with a different
block size (`/dev/sdc` and `/dev/sdf` are both 5 GiB):

```sh
> cat /sys/block/sdc/size
10485760
> cat /sys/block/sdf/size
10485760
> cat /sys/block/sdc/queue/logical_block_size
512
> cat /sys/block/sdf/queue/logical_block_size
4096
> vgcreate test1 /dev/sdc /dev/sdf
  Physical volume "/dev/sdc" successfully created.
  Physical volume "/dev/sdf" successfully created.
  Volume group "test1" successfully created
> lvcreate -L 4G -n lv1 test1
  Logical volume "lv1" created.
> cat /sys/block/dm-0/queue/logical_block_size
512
> mkfs.xfs /dev/test1/lv1
meta-data=/dev/test1/lv1         isize=512    agcount=4, agsize=262144 blks
         =                       sectsz=512   attr=2, projid32bit=1
         =                       crc=1        finobt=1, sparse=1, rmapbt=0
         =                       reflink=1
data     =                       bsize=4096   blocks=1048576, imaxpct=25
         =                       sunit=0      swidth=0 blks
naming   =version 2              bsize=4096   ascii-ci=0, ftype=1
log      =internal log           bsize=4096   blocks=2560, version=2
         =                       sectsz=512   sunit=0 blks, lazy-count=1
realtime =none                   extsz=4096   blocks=0, rtextents=0
Discarding blocks...Done.
> lvresize -L +2G /dev/test1/lv1
  Size of logical volume test1/lv1 changed from 4,00 GiB (1024 extents) to 6,00 GiB (1536 extents).
  Logical volume test1/lv1 successfully resized.
> cat /sys/block/dm-0/queue/logical_block_size
4096
> mount /dev/test1/lv1 /mnt
mount: /mnt: mount(2) system call failed: Function not implemented.
> dmesg | tail -1
[  630.888142] XFS (dm-0): device supports 4096 byte sectors (not 512)
```

This is what happens when you try to resize an XFS while the block size has
changed in between (same setup as above up to `mkfs.xfs`):

```sh
> mount /dev/test1/lv1 /mnt
> lvresize -L +2G /dev/test1/lv1
  Size of logical volume test1/lv1 changed from 4,00 GiB (1024 extents) to 6,00 GiB (1536 extents).
  Logical volume test1/lv1 successfully resized.
> cat /sys/block/dm-0/queue/logical_block_size
4096
> xfs_growfs /mnt
meta-data=/dev/mapper/test1-lv1  isize=512    agcount=4, agsize=262144 blks
         =                       sectsz=512   attr=2, projid32bit=1
         =                       crc=1        finobt=1, sparse=1, rmapbt=0
         =                       reflink=1
data     =                       bsize=4096   blocks=1048576, imaxpct=25
         =                       sunit=0      swidth=0 blks
naming   =version 2              bsize=4096   ascii-ci=0, ftype=1
log      =internal log           bsize=4096   blocks=2560, version=2
         =                       sectsz=512   sunit=0 blks, lazy-count=1
realtime =none                   extsz=4096   blocks=0, rtextents=0
xfs_growfs: XFS_IOC_FSGROWFSDATA xfsctl failed: Input/output error
> dmesg | tail -2
[ 1018.710597] sd 0:0:5:0: [sdf] tag#37 request not aligned to the logical block size
[ 1018.710650] blk_update_request: I/O error, dev sdf, sector 2107391 op 0x0:(READ) flags 0x1000 phys_seg 1 prio class 0
```

### LVM Issues

- Can you reliably know in advance which PVs will be used? Probably not.

- Maybe read block size from system after creating the LVs; but at least
  before using parted.

## BTRFS

BTRFS filesystems on multiple devices with different block sizes work.

