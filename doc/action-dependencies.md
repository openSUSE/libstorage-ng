
Action Dependencies
===================

For most actions the order is important, e.g. first a partition has to be
created, then the filesystem on it and finally it has to be mounted.

The cardinal rules are that 1. children are deleted before their parents and
2. children are created after their parents.

For some devices the issue is much more complex, e.g. MSDOS partition tables.


MSDOS Partition Table
---------------------

Obviously deleting and shrinking partitions should be done before creating and
growing partitions. Additionally creating must be done sorted according to the
partition number since parted always uses the next free partition number.

Deleting of logical partitions makes the situation even more complex since
other logical partitions can get renumbered and that must be considered for
actions involving the renumbered partitions.

For example consider two logical partitions:

  sda5

  sda6 -> ext4

When sda5 is deleted sda6 is renumbered to sda5. When sda6 is supposed to be
shrunk the shrink action for the filesystem and the partition must either be
done with the name "sda6" before sda5 is deleted of afterwards with the name
"sda5".

In libstorage-ng the shrinking is done before the deletion. As a consequence
the do_resize() function must be called on the partition object on the LHS
devicegraph.

Growing is done after deletion so in that case do_resize() must be called on
the partition object on the RHS devicegraph.

