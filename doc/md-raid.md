
MD RAIDs with non-numeric names are not supported. E.g. /dev/md_test (see
CREATE names=yes in mdadm.conf).


For MD RAIDs already on disk only a few operations are supported:

- removing faulty devices
- adding and removing spare devices

Unsupported are e.g. changing the RAID level, the chunk size or any operation
changing the RAID size.

Nested RAIDs are not supported. The reason is RAID auto assemble can ruin the
game. E.g. after creating md0 and md1 there maybe old signatures on them and
thus md2 is auto assembled before the library can wipe the signatures or
create md2. Thus creating md2 fails due to md0 and md1 being busy.

