
Each subvolume including the top-level subvolume is represented by an
BtrfsSubvolume object in the devicegraph.

The path of the top-level subvolume is "" (empty string). This is the only
consistent value if the paths of subvolumes should start with the path of the
parent.

The id of the top-level subvolume is 5, not the alias 0. As long as btrfs
tools (e.g. btrfs subvolume get-default) and the kernel (e.g. /proc/mounts)
report 5 this is the only possible consistent number.

Each BtrfsSubvolume object has its own mount points. Mount points of
BtrfsSubvolumes include the subvol= option in /etc/fstab. The mount points of
the Btrfs objects do not include the subvol= option, thus apply to the default
subvolume. So the following commands create the following fstab entries:

btrfs->create_mount_point("/test")

  /dev/system/btrfs  /test  btrfs  default  0 0

btrfs->get_top_level_subvolume->create_mount_point("/test")

  /dev/system/btrfs  /test  btrfs  subvol=/  0 0

