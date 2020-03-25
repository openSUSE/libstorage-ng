RAID
====

This is a collection of *various* notes.

See also:
- <https://www.suse.com/documentation/sles-15/book_storage/data/part_software_raid.html>
- <https://www.suse.com/documentation/sles-15/book_storage/data/sec_raid_yast.html>

States of Operation
-------------------

A RAID array, once it has been set up, has 3 possible states:

1. working normally: all devices are present
2. working but degraded: some devices are missing but the data is still
   accessible
3. not working: too many devices are missing

Yes, it's a simplification. YaST simplifies this even more: it treats the two
"working" states as the same.

In SLE12, YaST just ignores non-working RAIDs. In SLE15, it pops up some
warning dialogs (with a bug if I select Abort).

YaST does not provide any assistance in recovering from a worse state to a
better state. You have to be very careful with `mdadm` not to break things
even further.

Existing RAID Arrays
--------------------

For MD RAIDs already on disk only a few operations are supported:

- removing faulty devices
- adding and removing spare devices

Unsupported are e.g. changing the RAID level, the chunk size or any operation
changing the RAID size.

Nested RAIDs
------------

Nested RAIDs are NOT supported. The reason is RAID auto assemble can ruin the
game. E.g. after creating md0 and md1 there maybe old signatures on them and
thus md2 is auto assembled before the library can wipe the signatures or
create md2. Thus creating md2 fails due to md0 and md1 being busy.

Naming
------

There are three name formats for MD RAIDs:

- numeric, /dev/md$NUMBER

- format1 name, /dev/md/$NAME

  The device node itself is still numeric. A link from /dev/md/$NAME to
  /dev/md$NUMBER exists.

- format2 name, /dev/md_$NAME

  There is no numeric device node. A link from /dev/md/$NAME to
  /dev/md_$NAME exists.

  Needs `CREATE names=yes` in /etc/mdadm.conf.

The library uses the numeric or the format1 name as the name of the Md object.
So either use `Md::create(devicegraph, "/dev/md0")` or
`Md::create(devicegraph, "/dev/md/test")` when creating MD RAIDs.

Renaming existing MD RAIDs is not supported.

Support for `CREATE names=yes` in /etc/mdadm.conf should works but is
not supported due to missing testing from QA.

BIOS RAIDs
----------

Some BIOS RAIDs (IMSM and DDF) can be handled by `mdadm` as MD RAIDs. For each
of these RAIDs a container device exists in the system. The container device
has type MdContainer and the RAIDs inside the container have type
MdMember. Modifying (even deleting) BIOS RAIDs is not supported.

MdContainers must not be used as a block devices.

MdMembers can be used as generic Mds.

MdContainers and MdMembers can be added to and removed from /etc/mdadm.conf.

Combining disks with different block sizes
------------------------------------------

If you combine disks with different block sizes into a RAID, the RAID device
will have the maximum block size of its disks.

This seems to work (at least most of the time) even if some parts of the
RAID are not aligned to this block size.

Md::add_device() takes care to update Md::Region to use the correct block
size. It is, however not possible to combine regions with different block
sizes (in libstorage-ng) if the start or length of a region are misaligned.

This is, however, not really a problem here as the region start is always 0
and the RAID size is an approximation anyway - so we might simply round down.
