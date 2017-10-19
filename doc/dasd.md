# DASD devices

DASD devices are special in various aspects: different partition table, different tools used.
At this moment, the library supports DASD disks with the following requirements and constraints.


## ECKD DASD

### Requirements 

* DASD disks can be identified by the device name `/dev/dasd.*`.
* Only supported (E)CKD DASD disks with CDL format (not LDL).

### Constraints

* With DASD disks, only DASD partition table can be used.
* Max of three partitions can be created.
* DASD disk cannot be directly used for MD.
* DASD disk cannot be directly used for LVM PV (but partitions do).
* Alignment towards tracks needed (see alignment documentation). 
* Some partition ids cannot be used, for example swap (parted WIP).

Unfortunately, even only setting the size of a partition requires alignment afterwards.


## FBA DASD

TODO


## Base system bugs (see https://trello.com/c/dnsteUCS)

* *parted* reports wrong sector size (bsc #866535).
* *parted* reports DASD partition table even if none exists.
* *wipefs* cannot delete DASD partition table (bsc #896485).
* *blkid* does not report DASD partition table.

