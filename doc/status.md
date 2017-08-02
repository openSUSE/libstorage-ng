
Status
======


Disk
----

Implemented:

* Probing.

Not planned:

* BIOS-ID handling.


DASD
----

Implemented:

* ECKD

Missing:

* FBA
* zKVM


Partition Table
---------------

Implemented:

* For MS-DOS, GPT and DASD (creating, deleting and resizing partitions).
* Topology calculations.
* Support for boot, legacy_boot and pmbr_boot flags.
* Renumbering of logical partition.

Missing:

* Query resize information.


LVM
---

Implemented:

* Probing, creating and deleting volume groups.
* Probing, creating and deleting logical volumes.
* Extend and reduce volume groups.
* Resize logical volumes.

Missing:

* Snapshots.
* Thin provisioning.


LUKS
----

Implemented:

* Probing, creating, deleting, resizing.
* Activation during installation and upgrade.
* Handling of /etc/crypttab.

Missing:

* Handling different mount-by options.


Plain DM encryption
-------------------

Missing.


MD RAID
-------

Implemented:

* Probing, creating and deleting.
* Adding and removing devices.
* Calculation of RAID size.
* Handling of /etc/mdadm.conf.
* IMSM and DDF.


DM RAID
-------

Implemented.

* Probing.


Multipath
---------

Implemented.

* Probing.


BCache
------

Implemented:

* Probing.


Filesystem
----------

Implemented:

* For btrfs, ext2, ext3, ext4, reiserfs, NTFS, VFat, XFS, swap, ISO9660, UDF
  and NFS the most important operations (probing, creating, deleting,
  resizing, mounting and unmounting).
* Handling of /etc/fstab.
* Handling different mount-by options.
* btrfs subvolumes.

Missing:

* tmpfs
* btrfs multi-device support.
* On demand unmounting.


Misc
----

Implemented:

* Loading and saving devicegraph in XML.
* Remote probing.

Missing:

* Complete udev link handling.

