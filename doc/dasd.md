
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

- Partition Types: DASD

- Use as Block Device: No


LDL formatted ECKD DASDs
------------------------

Implicit partition table with one big partition.

Can be used directly as a block device. But since the implicit partition stays
this looks like a recipe for disaster.

- Partition Types: Implicit

- Use as Block Device: Unsupported


Unformatted ECKD DASDs
----------------------

- Partition Types: None

- Use as Block Device: No


FBA DASDs
---------

Can be used with a MSDOS or GPT partition table. Otherwise implicit partition
table with one big partition.

Can be used directly as a block device. But since the implicit partition stays
this looks like a recipe for disaster.

- Partition Types: Implicit, MS-DOS, GPT

- Use as Block Device: Unsupported


Glossary
--------

DASD = Direct Access Storage Device

ECKD = Extended Count Key Data

FBA = Fixed Block Architecture

CDL = Compatible Disk Layout

LDL = Linux Disk Layout

