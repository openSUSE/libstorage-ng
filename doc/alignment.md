
Partition Alignment
===================

Unfortunately these days for optimal performance care must be taken that
partitions are properly aligned to avoid excessive read-modify-write cycles.
For details please refer to https://en.wikipedia.org/wiki/Partition_alignment,
esp. "4 KB sector alignment" and "SSD page partition alignment".


Goals:
------

- Only create aligned partitions.

- Avoid gaps between partitions.


General Workflow:
-----------------

- Call get_unused_partition_slots().

  Start of regions is aligned, end is not aligned.

- Choose a region.

- Optionally set the start and/or size of the region.

- Optionally call align(), always call align() if you changed the start of the
  region.

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

  Do not call align or call it with KEEP_SIZE.

- Create partition using rest of slot:

  Do not call align or call it with KEEP_END.


Following these workflows ensures that new partitions are aligned and that
there are no gaps unless technical inevitable. Usually gaps will appear before
the first partition and before logical partitions.

On the other hand by not using the align() function still unaligned partitions
can be created. Gaps can also be created when exact sizes are preferred over
no gaps.

It should be obviously that "exact sizes" are only possible within the
possibility of the hardware, usually multiples of 512 B or 4 KiB.

