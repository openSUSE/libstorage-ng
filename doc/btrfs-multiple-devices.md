
Multiple Devices Btrfs
======================


Create Btrfs
------------

Before creating a multiple devices btrfs the metadata and data RAID
level can be set. The value DEFAULT will not pass a RAID level to
mkfs.btrfs thus mkfs.btrfs will decide the RAID level.

When using the "--mixed" option (via set_mkfs_options()) the metadata
and data RAID level must be identical.


Mounting Degraded Btrfs
-----------------------

Mounting a degraded btrfs requires the 'degraded' mount option.

Using the 'degraded' mount option in /etc/fstab by default was not recommended
by btrfs experts (on IRC).

To be decided: What should be done during probing with degraded btrfses?

The 'degraded' option is a nop if no devices are missing.


Querying RAID Levels
--------------------

The RAID levels can be queried using get_{metadata,data}_raid_level().

During a balance job to change the RAID level the RAID level as
reported by btrfs can be a combination of RAID levels. In that case
get_{metadata,data}_raid_level() returns only one of the reported RAID
levels.


Changing RAID Levels
--------------------

Changing the RAID levels required running btrfs balance job.

  btrfs balance start -dconvert=raid1,soft -mconvert=raid1,soft /test

Changing the RAID levels can require copying huge amounts of data so is
potentially very time consuming. Usually such operations are not supported by
libstorage-ng.

As a benefit the balance can be started in the background. But this fails if
there is already a balance job running (without an error message from the
btrfs command). The system does not provide a way to queue balance jobs and it
should not be the task of libstorage-ng to implement such a way.

The balance job can also fail due to missing space. When run in the background
reporting that to the user is not possible (the balance job starts and simply
fails later).

If the filesystem needs to be mounted temporarily for the balance job,
starting the balance job in the background would prevent unmounting. Normally
this should be a minor problem since the philosophy of btrfs is to be always
mounted. But during installation this could be problematic.

Thus operations requiring a balance job are not supported.


Adding a Device
---------------

Adding a device to an existing btrfs is easy.

  btrfs device add /dev/sdc1 /test

Sometimes after that the RAID levels should be changed, e.g. from DUP
to RAID1, but that is not supported (see above).


Removing a Device
-----------------

Removing a device from an existing btrfs is at first sight also easy.

  btrfs device delete /dev/sdc1 /test

"It redistributes any extents in use on the device being removed to the other
devices in the filesystem" [1]. So this also may be very slow.

Even more problematic is to know in advance whether the operation will
succeed. It may fail due to two reasons:

1. Minimal number of devices for RAID level.

   Should be easy to check in advance.

2. Not enough free space.

   Using 'btrfs filesystem usage /test' it can be checked how much of a device
   is used. But how likely is the case that only a few GiB are used? And what
   happens when removing several devices?

For the time being removing devices is not supported.


Resizing a Device
-----------------

The btrfs command to resize a block device used by btrfs defaults to use the
devid 1. This must be extended to query the devid and pass it the btrfs (since
btrfs does not accept the device name here).

Shrinking can again be very slow. Additionally 'btrfs inspect-internal
min-dev-size [--id 1] /test' still reports wrong values.

For the time being shrinking is not supported.


References
----------

[1] https://btrfs.wiki.kernel.org/index.php/Using_Btrfs_with_Multiple_Devices

