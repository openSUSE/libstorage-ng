
DASDs
-----

There are ECKD and FBA DASDs.


ECKD DASDs
----------

Can be CDL or LDL formatted.


CDL formatted ECKD DASDs
------------------------

Most common type.

Can be used with a DASD partition table.

Cannot be used as a block device since the two first tracks are special.

- Partition Table Types: DASD

- Use as Block Device: No


LDL formatted ECKD DASDs
------------------------

Implicit partition table with one big partition.

Can be used directly as a block device. But since the implicit partition stays
this looks like a recipe for disaster.

- Partition Table Types: Implicit

- Use as Block Device: Unsupported


Unformatted ECKD DASDs
----------------------

- Partition Table Types: None

- Use as Block Device: No


FBA DASDs
---------

Can be used with a MS-DOS partition table or GPT. Otherwise implicit partition
table with one big partition.

Can be used directly as a block device. But since the implicit partition stays
this looks like a recipe for disaster.

- Partition Table Types: Implicit, MS-DOS, GPT

- Use as Block Device: Unsupported


Implicit Partition Table
------------------------

What is an implicit partition table? As mentioned above the kernel creates a
partition on some DASDs even though no partition table exists on-disk. This
situation is represented in libstorage-ng by an implicit partition table
having one partition. Since the implicit partition table does not exist
on-disk no modifications whatsoever are possible.

On FBA DASDs the implicit partition table can be replaced by a MS-DOS partition
table or GPT.


Glossary
--------

DASD = Direct Access Storage Device (https://en.wikipedia.org/wiki/Direct-access_storage_device)

ECKD = Extended Count Key Data

FBA = Fixed Block Architecture

CDL = Compatible Disk Layout

LDL = Linux Disk Layout

