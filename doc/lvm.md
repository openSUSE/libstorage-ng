
Metadata used by LVM:

When trying to create a thin-pool using all free space the metadata has to be
accounted for. In contrast to linear LVs the metadata for thin-pool uses space
of the VG. E.g. if there are 2048 GiB free in the VG the metadata for a
maximal size thin-pool is about 128 MiB and the pool thus only about 2047.9
GiB big.

Additionally LVM creates a spare metadata with the same size. This spare
metadata is shared between all pools and thus has the size of the biggest pool
metadata. The spare metadata can be deleted manually. All pool metadata can
also be resized.

When starting with an empty VG it is relative easy to account for the
metadata. But how to handle this with an already existing volume group? Also
take into account a volume group containing e.g. RAID LVs or cache pools
(which also have metadata).

Solutions:

1. Insert all (even the private) LVs in the devicegraph and emulate the exact
   space calculations LVM does. Given the number of different LV types this
   looks complicated and very difficult to test. Might also often require
   additions.

2. During probing check how much free space the VG has. Then calculate a
   "reserved" value for the volume group.

     reserved = total size - used by LVs the library handles - free

   The "reserved" will contain e.g. all metadata.

   When calculating available space for a normal or thin pool take the
   "reserved" into account:

     max size = total size - reserved - used by LVs the library handles

   The drawback is that the max size for the pool can be smaller than actually
   possible since e.g. the spare metadata might be shared with an already
   existing thin pool.

   Also deleting a thin pool does not make the space of the metadata
   available (in memory).

Solution 2 is implemented.

