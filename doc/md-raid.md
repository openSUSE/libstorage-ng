
For MD RAIDs already on disk only a few operations are supported:

- removing faulty devices
- adding and removing spare devices

Unsupported are e.g. changing the RAID level, the chunk size or any operation
changing the RAID size.

Nested RAIDs are not supported. The reason is RAID auto assemble can ruin the
game. E.g. after creating md0 and md1 there maybe old signatures on them and
thus md2 is auto assembled before the library can wipe the signatures or
create md2. Thus creating md2 fails due to md0 and md1 being busy.


There are three name formats for MD RAIDs:

- numeric, /dev/md<number>

- format1 name, /dev/md/<name>

  The device node itself is still numeric. A link from /dev/md/<name> to
  /dev/md<number> exists.

- format2 name, /dev/md_<name>

  There is no numeric device node. A link from /dev/md/<name> to
  /dev/md_<name> exists.

  Needs "CREATE names=yes" in /etc/mdadm.conf.

The library uses the numeric or the format1 name as the name of the Md object.
So either use Md::create(devicegraph, "/dev/md0") or Md::create(devicegraph,
"/dev/md/test") when creating MD RAIDs.


Renaming existing MD RAIDs is not supported.

Support for "CREATE names=yes" in /etc/mdadm.conf should works but is
not supported due to missing testing from QA.

