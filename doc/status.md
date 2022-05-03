
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
* Activation during installation and upgrade.
* Handling of /etc/crypttab.
* LUKS1 and LUKS2.

Experimental:

* LUKS2 integrity sypport.

Missing:

* Handling different mount-by options.
* Detached header.


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

* For btrfs, ext2, ext3, ext4, ReiserFS, NTFS, VFat, ExFat, XFS, JFS,
  swap, F2FS, ISO9660, UDF, NFS and tmpfs the most important operations
  (probing, creating, deleting, resizing, mounting and unmounting).
* Handling of /etc/fstab.
* Handling different mount-by options.
* btrfs subvolumes, btrfs multiple-device, quota and qgroup support.

Missing:

* On demand unmounting.


Misc
----

Implemented:

* Loading and saving devicegraph in XML.
* Remote probing.

