
Status
======


Disk
----

Implemented:

* Probing.

Missing:

* BIOS-ID handling.


DASD
----

Missing.


Partition Table
---------------

Implemented:

* For MS-DOS and GPT (creating, deleting and resizing partitions).
* Topology calculations.
* Support for boot, legacy_boot and pmbr_boot flags.

Missing:

* Renumbering of logical partition.
* Query resize information.
* DASD partition table.


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

Missing:

* Probing of inactive devices and activation during installation.
* Handling different mount-by options.


Plain DM encryption
-------------------

Missing.


MD RAID
-------

Implemented:

* Probing, creating and deleting.
* Adding and removing devices.

Missing:

* Correct calculation of RAID size.
* Handling of /etc/mdadm.conf.
* IMSM and DDF.


DM RAID
-------

Missing.


Multipath
---------

Missing.


BCache
------

Implemented:

* Probing.


Filesystem
----------

Implemented:

* For btrfs, ext4, NTFS, VFat, XFS and swap the most important operations
  (probing, creating, deleting, resizing, mounting and unmounting).

Missing:

* Handling different mount-by options.
* NFS, tmpfs, ext2, ext3, reiserfs, ISO9660, UDF.
* Subvolumes for btrfs, multi-device support for btrfs.
* On demand unmounting.


Misc
----

Implemented:

* Loading and saving devicegraph in XML.
* Remote probing.

Missing:

* Complete udev link handling.

