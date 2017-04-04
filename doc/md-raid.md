
MD RAIDs with non-numeric names are not supported. E.g. /dev/md_test (see
CRETAE names=yes in mdadm.conf).


For MD RAIDs already on disk only a few operations are supported:

- removing faulty devices
- adding and removing spare devices

Unsupported are e.g. changing the RAID level, the chunk size or any operation
changing the RAID size.

