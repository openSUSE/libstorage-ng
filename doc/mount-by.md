
The library allows to set a default mount-by method. The initial value is
```UUID```. The library does not store the value across runs.

The default mount-by method is used when creating a mount point using
Mountable::create_mount_point(). For filesystems on LVM the mount-by method is
set to ```DEVICE```.

MountPoint::set_default_mount_by() can be used to reset the value to the
default value.

The default mount-by method is used during probing for filesystems if
otherwise no mount-by method can be detected. E.g. this happens if a
filesystem is mounted but not included in /etc/fstab.

Also see https://fate.suse.com/316204/.

