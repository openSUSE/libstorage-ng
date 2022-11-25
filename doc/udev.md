
The library only keeps udev-paths and udev-ids known to represent the
disk/device, not its content. E.g. it ignores lvm-pv-<pv-uuid> since it
vanishes when the lvm physical volume is removed.

It does not keep virtio paths and ids either. They are not considered
reliable enough because they have changed multiple times, as mentioned at
https://lists.freedesktop.org/archives/systemd-devel/2017-March/038397.html

Since udev comes up with new udev links once in a while a whitelist looks
more future-proof than a blacklist.

A whitelist will also reduce the risk of using erroneous udev links, e.g. see
bsc #1063249 and bsc #1140630.

The configuration is loaded from the file udev-filters.json. This is
so far not documented since in detail since it may have to change
depending on future development of udev.

Unresolved bugs concerning udev links:
- pmem (bsc #1140630)
- nvme (bsc #1205352)

