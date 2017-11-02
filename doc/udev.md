
The library only keeps udev-paths and udev-ids known to represent the
disk/device, not its content. E.g. it ignores lvm-pv-<pv-uuid> since it
vanishes when the lvm physical volume is removed. Since udev comes up with new
udev links once in a while a whitelist looks more future-proof than a
blacklist.

A whitelist will also reduce the risk of using erroneous udev links, e.g. see
bsc #1063249. 

