
Partition Alignment
===================

Unfortunately these days for optimal performance care must be taken that
partitions are properly aligned, see e.g.
https://en.wikipedia.org/wiki/Partition_alignment.


Goals:
------

- Only create aligned partitions.

- Avoid gaps between partitions.


General Workflow:
-----------------

- Call get_unused_partition_slots().

  Start of regions is aligned, end is not aligned.

- Choose a region.

- Set the size of the region, optionally the start of the region.

- Call align().

  Per default this will align the start and end of the region. The start is
  rounded up and the end is rounded down. This way it is assured that the
  aligned region is still within the partition slot.

  As a consequence the size of the region can be reduced by upto approximately
  two times the grain. As a further consequence the resulting region may not
  exist/be empty.

- Pass aligned region to create_partition().


Use Cases:
----------

- Create partition:

  Just use general workflow.

- Create small (just a few grains) partition:

  Call align with KEEP_SIZE.


Following these workflows ensures that new partitions are aligned and that
there are only few gaps. E.g. there can be a gap when creating small
partitions or before unaligned existing partitions and at the end of the
partitionable.


TODO logical partitions

