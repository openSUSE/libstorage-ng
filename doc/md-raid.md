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

(yes, it's a simplification)

YaST simplifies this even more: it treats the two "working" states as the
same. (That's, uh, surprising.)

In SLE12, YaST just ignores non-working RAIDs. In SLE15, it pops up some
warning dialogs (with a bug if I select Abort).

YaST does not provide any assistance in recovering from a worse state to a
better state. You have to be very careful with `mdadm` not to break things
even further.

Other Notes, FIXME: categorize
------------------------------

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


BIOS RAIDs

Some BIOS RAIDs (IMSM and DDF) can be handled by mdadm as MD RAIDs. For each
of these RAIDs a container device exists in the system. The container device
has type MdContainer and the RAIDs inside the container have type
MdMember. Modifying (even deleting) BIOS RAIDs is not supported.

MdContainers must not be used as a block devices.

MdMembers can be used as generic Mds.

MdContainers and MdMembers can be added to and removed from /etc/mdadm.conf.

