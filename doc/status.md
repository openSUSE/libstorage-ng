
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

* ECKD (CDL and LDL)
* FBA

Missing:

* zKVM


Partition Table
---------------

Implemented:

* For MS-DOS, GPT and DASD (creating, deleting and resizing partitions).
* Topology calculations.
* Support for boot, legacy_boot and pmbr_boot flags.
* Renumbering of logical partition.
* Query resize information.


LVM
---

Implemented:

* Probing, creating and deleting volume groups.
* Probing, creating and deleting logical volumes.
* Extend and reduce volume groups.
* Resize logical volumes.
* Thin provisioning.
* Snapshots (only probing).
* RAID and mirror (only probing).
* Cache (only probing).

Not planned:

* Renaming volume groups or logical volumes.
* Cluster support.


Plain DM encryption
-------------------

Implemented:

* Probing, creating, deleting, resizing.
* Handling of /etc/crypttab.

Limitations:

* Only supported for swap.


LUKS
----

Implemented:

* Probing, creating, deleting, resizing.
* Activation and deactivation.
* Handling of /etc/crypttab.
* LUKS1 and LUKS2.

Experimental:

* LUKS2 integrity support.

Missing:

* Handling different mount-by options.
* Detached header.


BitLocker
---------

Experimental:

* Probing, deleting.
* Activation and deactivation.
* Handling of /etc/crypttab.


MD RAID
-------

Implemented:

* Probing, creating and deleting.
* Adding and removing devices.
* Calculation of RAID size.
* Handling of /etc/mdadm.conf.
* IMSM and DDF.

Missing:

* Journal device (untested).

Not planned:

* RAID levels MULTIPATH and FAULTY.


DM RAID
-------

Implemented:

* Probing.


Multipath
---------

Implemented:

* Probing.


BCache
------

Implemented:

* Probing.


Filesystem
----------

Implemented:

* For btrfs, ext2, ext3, ext4, ReiserFS, NTFS, VFat, ExFat, XFS, JFS,
  swap, F2FS (excluding multiple devices), NILFS2 (excluding
  checkpoints), UDF, NFS, bcachefs and tmpfs the most important
  operations (probing, creating, deleting, resizing, mounting and
  unmounting).
* For ISO9660, Squashfs and EROFS less operations (probing, deleting,
  mounting and unmounting).
* Handling of /etc/fstab.
* Handling different mount-by options.
* btrfs subvolumes, btrfs multiple-device, quota and qgroup support.

Missing:

* Bcachefs resizing, snapshots and multiple-devices.
* On demand unmounting.


Misc
----

Implemented:

* Loading and saving devicegraph in XML.
* Remote probing (with limitations).

