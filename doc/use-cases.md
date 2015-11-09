Use Cases
=========

This is an incomplete list of some typical scenarios.


Modern Bare Metal PC
--------------------

Starting scenario:

- One new, empty hard disk with MS-DOS disk label
- UEFI boot

Expected outcome:

- GPT disk label on the disk
- EFI system partition
- Swap partition
- Linux root partition
- Linux /home partition


Legacy Bare Metal PC
--------------------

Starting scenario:

- One hard disk
- MS-DOS disk label
- Legacy PC boot (MBR)

Expected outcome:

- MS-DOS disk label remains on the disk
- Swap partition
- Linux root partition
- Linux /home partition



Multi-Boot with Windows
-----------------------

Starting scenario:

- One hard disk with one large Windows partition
- MS-DOS disk label
- Need working Windows parallel to Linux

Expected outcome:

- MS-DOS disk label remains on the disk
- Windows partition is resized smaller
- Swap partition (primary partition)
- Linux root partition (primary partition)
- Linux /home partition (logical partition on extended partition)

Swap and root might be primary partitions, or they might be on the extended
partition as well. In any case, there should be an extended partition so there
is at least the possibility to add another partition: There are only 4
partition table slots in an MS-DOS partition table; if they are all used, the
last one should be an extended partition which can hold any number of
partitions. So if /home is a file system type that can be resized, another
partition could be added in the extended partition without losing all the data
on /home.



Simple LVM
----------

Starting scenario:

- One hard disk with any number of partitions that can be deleted
- UEFI boot

Expected outcome:

- GPT disk label
- EFI boot partition
- One large partition as an LVM physical volume (PV)
- One LVM volume group with that LVM PV
- LVM logical volume (LV) for swap
- LVM LV for root filesystem
- LVM LV for /home filesystem



LVM on Multiple Disks
---------------------

Starting scenario:

- hard disk /dev/sda with any number of partitions that can be deleted
- hard disk /dev/sdb with any number of partitions that can be deleted
- UEFI boot

Expected outcome:

- GPT disk label on /dev/sda
- EFI boot partition on /dev/sda
- One large partition as LVM PV on /dev/sda
- GPT disk label on /dev/sdb
- One large partition as LVM PV on /dev/sdb
- One LVM volume group with both PVs
- LVM logical volume (LV) for swap
- LVM LV for root filesystem
- LVM LV for /home filesystem



Full Disk Encryption
--------------------

Starting scenario:

- One hard disk with any number of partitions that can be deleted
- UEFI boot

Expected outcome:

- GPT disk label
- EFI boot partition
- /boot partition
- One large partition as an LVM physical volume (PV)
- Encryption on that PV
- One LVM volume group with that PV
- LVM logical volume (LV) for swap
- LVM LV for root filesystem
- LVM LV for /home filesystem



Simple RAID 1 (Mirroring)
-------------------------

(to do)


RAID 5 with Btrfs on LVM
------------------------

(to do)
