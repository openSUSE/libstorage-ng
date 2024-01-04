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

- If you need to know the block size for LVs you'd probably have to read it
  from the system after creating the LVs. From libstorage-ng's perspective
  the block size chiefly matters for parted but LVs are not partitioned.

## BTRFS

BTRFS filesystems on multiple devices with different block sizes work.

## BCACHE

The backing device must have a block size larger or equal to that of the caching device.

```sh
> cat /sys/block/sdc/queue/logical_block_size
512
> cat /sys/block/sdf/queue/logical_block_size
4096
> bcache make -B /dev/sdc
Name                    /dev/sdc
Label
Type                    data
UUID:                   f3852085-2fdb-4518-8af0-8c5e7cb947ae
Set UUID:               371b292f-0cdd-46c8-b95d-29cb7431e42f
version:                1
block_size_in_sectors:  1
data_offset_in_sectors: 16
> bcache make -C /dev/sdf
Name                    /dev/sdf
Label
Type                    cache
UUID:                   327916fb-83d7-4296-adc1-b632d37a8c6a
Set UUID:               423801be-5553-4205-9650-c8d12eb7897d
version:                0
nbuckets:               10240
block_size_in_sectors:  8
bucket_size_in_sectors: 1024
nr_in_set:              1
nr_this_dev:            0
first_bucket:           1
/dev/sdf blkdiscard beginning...done
> bcache attach /dev/sdf /dev/sdc
Failed to attach to cset 423801be-5553-4205-9650-c8d12eb7897d
> dmesg | tail -1
[ 5084.307221] bcache: bch_cached_dev_attach() Couldn't attach sdc: block size less than set's block size
```

The resulting bcache device has the block size of the backing device.

```sh
> cat /sys/block/sdd/queue/logical_block_size
1024
> cat /sys/block/sde/queue/logical_block_size
2048
> bcache make -C /dev/sdd
Name                    /dev/sdd
Label
Type                    cache
UUID:                   b194c119-e520-48ed-8f9c-5f8ff621958a
Set UUID:               866e5f65-bf4c-4789-b5dd-8f6046a21f09
version:                0
nbuckets:               10240
block_size_in_sectors:  2
bucket_size_in_sectors: 1024
nr_in_set:              1
nr_this_dev:            0
first_bucket:           1
/dev/sdd blkdiscard beginning...done
> bcache make -B /dev/sde
Name                    /dev/sde
Label
Type                    data
UUID:                   3dc4a478-c3ff-4ac8-b22d-070a2ce88657
Set UUID:               6a252cb1-3b93-4c91-9844-e01525bb11c9
version:                1
block_size_in_sectors:  4
data_offset_in_sectors: 16
> bcache attach /dev/sdd /dev/sde
root ~> dmesg | tail -1
[ 5235.728582] bcache: bch_cached_dev_attach() Caching sde as bcache1 on set 866e5f65-bf4c-4789-b5dd-8f6046a21f09
> cat /sys/block/bcache1/queue/logical_block_size
2048
```

When you combine disks with different block sizes into a single cache set
the cache set gets the maximum block size of its disks. You can then attach
it to backing devices with larger or equal block sizes.

```sh
> cat /sys/block/sdc/queue/logical_block_size
512
> cat /sys/block/sdd/queue/logical_block_size
1024
> cat /sys/block/sde/queue/logical_block_size
2048
> cat /sys/block/sdf/queue/logical_block_size
4096
> bcache make -C /dev/sdc /dev/sde
Name                    /dev/sdc
Label
Type                    cache
UUID:                   43db4a47-7958-40b1-8582-b355eddb2796
Set UUID:               921ea429-2483-4382-ab8f-290511ed0d55
version:                0
nbuckets:               10240
block_size_in_sectors:  4
bucket_size_in_sectors: 1024
nr_in_set:              1
nr_this_dev:            0
first_bucket:           1
/dev/sdc blkdiscard beginning...done
Name                    /dev/sde
Label
Type                    cache
UUID:                   a279ea07-11bc-4b52-82f2-8ff4f689b72f
Set UUID:               921ea429-2483-4382-ab8f-290511ed0d55
version:                0
nbuckets:               10240
block_size_in_sectors:  4
bucket_size_in_sectors: 1024
nr_in_set:              1
nr_this_dev:            0
first_bucket:           1
/dev/sde blkdiscard beginning...done
> bcache make -B /dev/sdd
Name                    /dev/sdd
Label
Type                    data
UUID:                   44461ca1-1f38-4a96-89c4-efb0afbdc2d7
Set UUID:               55c02b33-75ec-4c04-be24-2f5ca0319ff9
version:                1
block_size_in_sectors:  2
data_offset_in_sectors: 16
> bcache make -B /dev/sdf
Name                    /dev/sdf
Label
Type                    data
UUID:                   535917a0-58c0-4320-8d91-6918ba48e636
Set UUID:               e33c4cbd-a230-43df-9eea-6c5b55f98c3c
version:                1
block_size_in_sectors:  8
data_offset_in_sectors: 16
> bcache show
Name            Type            State                   Bname           AttachToDev
/dev/sdf        1 (data)        no cache(running)       bcache1         Alone
/dev/sdd        1 (data)        no cache(running)       bcache0         Alone
/dev/sde        0 (cache)       active                  N/A             N/A
/dev/sdc        3 (cache)       active                  N/A             N/A
> bcache attach 921ea429-2483-4382-ab8f-290511ed0d55 /dev/sdd
Failed to attach to cset 921ea429-2483-4382-ab8f-290511ed0d55
> bcache attach 921ea429-2483-4382-ab8f-290511ed0d55 /dev/sdf
```

On SLE15-SP1 (bcache-tools-1.0.9) and SLE15-SP2 (bcache-tools-1.1) `bcache`
does not report an error if attaching fails (same setup as above):


```sh
> bcache attach 921ea429-2483-4382-ab8f-290511ed0d55 /dev/sdd
> echo $?
0
> dmesg | tail -1
[  383.520928] bcache: bch_cached_dev_attach() Couldn't attach sdd: block size less than set's block size
> bcache show
Name            Type            State                   Bname           AttachToDev
/dev/sdf        1 (data)        no cache(running)       bcache1         Alone
/dev/sdd        1 (data)        no cache(running)       bcache0         Alone
/dev/sde        0 (cache)       active                  N/A             N/A
/dev/sdc        3 (cache)       active                  N/A             N/A
```

Tumbleweed also reports bcache-tools version 1.1 but does report the error properly.
