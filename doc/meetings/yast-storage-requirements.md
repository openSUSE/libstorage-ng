# YaST Storage Features and Requirements

This is about *libstorage* and the *yast-storage* Ruby code.

This document was created as a summary of a meeting held in Nuernberg in February 2016

##AIs summary

* AI Lukas: ML: libstorage-ng@opensuse.org
* AI PM: Clarify requirements on FAT partitions
* AI lmb, Holgi: Claryfy requirements on filesystems (see below)
* AI Architecture maintainers: Clarify which configurations of RAID are bootable, possibly under which conditions
* AI: Verify support of resizing logical volumes for LVM
* AI: Clarify why we do not support encryption of root filesystem, which tools would possibly need to be fixed
* AI: speak with different parties which requirements they have on automated proposal (Cloud, SES, SAP, ...)
* AI: Define when dedicated boot partition is needed before merging into the
* AI: Provide specifics for zFCP devices
* AI: Define requirements on /boot/zipl partition
* AI: Provide details about specific tools for first sector writing, reasoning for two copies of kernel (why two?) for DASD


## Disks and Partitions Simple case:

   * Disk
       * Partition table
           * Partition
               * File system
                      * File
                      * File
                      * File
                      * ...
           * Partition
               * File system
                      * File
                      * ...
           * Partition
               * ...
           * ...

## Advanced Cases

This covers various ways to combine disks and/or use advanced
ways to manage the disk space provided by the disks

###RAID (Redundant Array of Independent Disks)

   * Hardware RAID: mostly automatic and transparent
       * Behaves like any regular disk
       * No need for any special handling (or even detection)
   * Linux Software RAID / BIOS RAID (type dependent on BIOS):
       * mdadm RAID: https://en.wikipedia.org/wiki/Mdadm
       * dmraid (Device Mapper RAID): https://en.wikipedia.org/wiki/Device_mapper ?

BIOS-managed raid is configured in BIOS (and Linux system detects it), Linux softwre RAID must be configured in Linux

RAID levels:

   * **RAID 0**: striping; performance improvement due to parallel disk I/O; less fault tolerance than one disk
   * **RAID 1**: mirroring; improved fault tolerance, possibly better performance for reading
   * **RAID 5**: 3 or more disks with redundancy (for parity blocks): Good fault tolerance, improved performance (parallel I/O)
   * **RAID 6**: 4 or more disks; more redundancy than RAID 5, thus even better fault tolerance
   * **RAID 10**: striping + mirroring; combination of RAID 1 and RAID 0

Supported actions:

   * create RAID 0, 1, 5, 6, 10
   * create partitions on RAID (if there already is a partition table; cannot create a partition table if there is not one already due to design limitation of old libstorage)

Not supported by YaST storage - use dedicated tools instead:

   * add another disk (to already created RAID)
   * remove disk (from RAID)
   * create BIOS RAID (use BIOS or mdadm/dmraid)
   * These cases need to be documented properly

Raid and boot partitions

   * AI: Define which Raid configurations are bootable?

### Volume Managers

   * LVM
   * Btrfs (volume manager features): not yet supported, needs FATE with usecases and technical information
   * EVMS: legacy; no longer supported, drop, not even for upgrade (see https://bugzilla.suse.com/show_bug.cgi?id=848821#c18)

Supported actions (LVM only):

   * create VG (Volume Group)
   * add VG
   * remove VG
   * add PV (Physical Volume) to VG
   * remove PV from VG
   * create LV (Logical Volume)
   * delete LV
   * resize LV
   * thin provisioning (overcommited disk space)

Not supported (use low-level tools instead):

   * force LV to specific PV
   * merge VGs
   * split VG
   * resize PV
   * move content between PVs
   * ...

### Encryption

   * full disk encryptionn (LVM only: encrypted PVs)
   * per partition (not root file system) with device mapper
   * per partition also for the root filesystem (currently not supported, AI: find out why it is not possible and fix it)
   * using LUKS (Linux Unified Key Setup)
   * limited support for legacy method: loop devices and crypto kernel modules

Actions:

   * create
   * mount
   * unmount
   * /etc/crypttab and /etc/fstab handling

Not supported (use LUKS commands instead):

   * LUKS multiple password slots handling
   * change password
   * set cipher (encryption method)

### Multipath

   * network storage devices (typically fibre channel) where multiple low-level kernel devices for the same storage device exist that have redundant connections to the machine
   * improved performance (parallel I/O over multiple channels)
   * fail safe against some I/O channels being blocked
   * special hardware that needs to be set up with special tools (outside the scope of YaST storage)
   * for most operations, it is important to use only the one device mapper device for that storage device (representing the multipath device), not the multiple low-level devices (representing individual paths to the device)

Supported actions:

   * start
   * stop

### Multiple Dimensions

   * LVM on RAID
   * Btrfs (which has its own volume manager) on LVM
   * Btrfs on LVM on RAID
   * Btrfs on encrypted LVM on RAID
   * ... (use your imagination)

## Remote Storage

This covers storage devices, which are not accessible out-of-the-box and some operation is necessary in order to bring the disk to the system (and create a /dev/.* device)

### SystemZ Configuration
(s/390 specific)

   * DASDs (Direct Access Storage Devices) need to be activated for use with Linux with the *dasd\_configure* command
   * DASDs are created/managed by the s/390 host system
   * Ihno wants to provide a script which lists all disks
   * zFCP
   * AI: Provide specifics for zFCP devices

### iSCSI
(Internet SCSI)

   * networked storage using SCSI commands over TCP/IP
   * using a separate set of commands and tools
   * currently out of the scope of YaST storage, using a separate YaST module
   * might need to be integrated into YaST storage some day

### FCoE
(Fibre Channel Over Ethernet)

   * networked storage using fibre channel over ethernet
   * using a separate set of commands and tools
   * currently out of the scope of YaST storage, using a separate YaST module
   * might need to be integrated into YaST storage some day


## File Systems

File Systems and operations to be supported by new libstorage

| Filesystem | Create | Root | Resize | Comment                                                 |
| ---------- |:------:|:----:|:------:| ------------------------------------------------------- |
| ext2       | X      | X    | X      |                                                         |
| ext3       | X      | X    | X      |                                                         |
| ext4       | X      | X    | X      |                                                         |
| Btrfs      | X      | X    | X      | primary root parition, avoid for data                   |
| XFS        | X      | X    | X      | preferred for data                                      |
| JFS        | X      | X    | X      | Drop                                                    |
| ReiserFS   |        | X    | X      | Support existing partitions, not allowed to create new  |
| FAT/VFAT   | X      | X    | X      | for EFI system partition, else? AI for PM to clarify    |
| NTFS       |        |      | X      | AI for PM                                               |
| tmpFS      |        |      |        | ignore in libstorage (except mounting)                  |
| HFS        |        |      |        | drop                                                    |
| HFS+       |        |      |        | drop                                                    |
| NFS        |        | X    |        | installation on NFS as root is supported                |
| ZFS        |        |      |        | Not supported                                           |
| GFS2       | ?      |      |        | AI storage people                                       |
| ceph       | ?      |      |        | AI storage people                                       |
| ocfs2      | ?      |      |        | HA, AI lmb                                              |
| swap       | X      |      | X      | resize via removing and re-creating                     |

Supported actions (for all filesystems, unless excluded in table above):

   * create
   * resize (for some types, see above)
   * /etc/fstab -handling (managing individual mount points, mount options)
   * mount
   * unmount

Local file system can be located in a disk partition, in whole disk, on an MD array, on a logical volume


## Bootloader Constraints
***Caution: Voodoo inside***

   * MS-DOS - Legacy BIOS
       * BIOS loads the first sector of the disk (the MBR (Master Boot Record)) and executes it
       * only 512 bytes - sizeof( partition\_table ) = 446 bytes for this first stage of the boot loader, i.e., the next stage (Grub2, Grub, LILO) has to be chain-loaded
       * the next stage has to be accessible by the BIOS
       * limitations depending on BIOS:
           * legacy: 1024 cylinders in CHS (Cylinder/Head/Sector) mode (obsolete today)
           * LBA (Logical Block Address):
               * 32 bit LBA (obsolete today): 2 TB limit
               * latest (7/2015): 48 bit LBA; 144 PB (144000 TB) limit
           * there have always been limits, they always seemed way out of reach, and we always reached them before we thought it possible.
           * BIOS can only access first 8 disks
       * Stage1 and Stage2 of grub2 should always be on the same disk (means including the partition where stage2 will be insalled into and the config)
       * BIOS can only support 8 harddisks, so bootloader needs to be on the first 8 ones

   * UEFI (Unified Extensible Firmware Interface):
       * typically used with GPT disk label
       * requires EFI system partition (with FAT file system)

   * PPC (Power PC)
       * can use MS-DOS or GPT disk label
       * requires PReP partition in KVM or LPAR, on LPAR less than 8MB, idealy size of grub2 stage1
       * no special partition required for OPAL/PowerNV/Bare
       * ***TO DO: Get more information from the PPC arch maintainers***

   * s/390
       * uses a z/IPL (Initial Boot Loader for z Series) initial boot loader
       * uses a /boot/zipl partition (typically with ext2 file system)
       * AI: Define requirements on /boot/zipl partition

### Disk Labels
(ways to create partition tables)

   * Allow restricting installation to a single disk, even if this is not the first one (use case with many disks)
   * On new, empty disks, if there is no partition or if the disks will be wiped (use full disk): prefer GPT disk label

   * MS-DOS (PC classic/legacy)
       * can be used on all supported architectures: i586, x86\_64, ppc, s/390
       * 4 primary partition entries in MBR (Master Boot Record)
       * one partition can be an "extended partition": A container for *logical partitions*
       * each logical partition links to the next one
       * the first logical partition is always /dev/sd..5 (no matter if /dev/sd..4 etc. exist)
       * need bootflags to mark active partition for BIOS

   * GPT  (GUID Partition Table; new PCs with UEFI boot support)
       * New tool of choice: gptfdisk / sgdisk
       * Backup copy might be present or not (not mandatory)
       * partitions can have labels
       * can be used on all supported architectures: i586, x86\_64, ppc, s/390
       * large number of partitions (128 or more)
       * no longer limited by available space in MBR
       * partitions keep their numbers when other partitions are removed or added
       * parted will use the smallest available number when creating a new partition
       * bootflags are not required (maybe important for legacy BIOS, ignored UEFI)
       * **UEFI**
           * EFI system partiton, vfat formated, might require specific UUID, limited to 32GB (vfat)
       * **legacy BIOS**
           * protective MBR (should have active flag)
           * hybrid MBR -> we do not want that any longer
           * GPT Bios grub partition (needed for grub2)

   * DASD (s/390 specific)
       * Up to 4 partitions
       * Separate zipl partition needed
       * First sector is special, only writeable with appropriate tools
       * Needs to write the blocklist onto zipl partition, kernel/initrd must reside there (needs to be large enough to hold two copies of kernel and initrd)
       * AI: Provide details about specific tools for first sector writing, reasoning for two copies of kernel (why two?) for DASD

   * Not supported or very limited support (which will be dropped in the future) in YaST storage:
       * BSD
           * Fixed number of 8 or 16 partition slots
           * Fixed role (by convention) for some slots:
               * slot a: root
               * slot b: swap
               * slot c: complete disk
       * Sun
       * AIX
       * Mac
       * Amiga

### Dedicated Boot Partitions
(AI: combine with disk labels and bootloader constraints; need to be clarified before merging)
***very architecture/system/bootloader specific***

   * when is a boot partition needed?
   * on what disk should it be?
   * where on the disk should it be? (Boot sequence!)
   * size constraints? (too small vs. too large; ppc for example loads the *PReP* partition into the RAM completely, so it should be as small as possible)
   * dependent on boot loader type
   * dependent on encryption (BIOS cannot load files from encrypted partitions)
   * dependent on presence of LVM

## Automated Storage Proposal

***Caution: Voodoo inside***

   * a lot of code that tries to make a reasonable suggestion for many different storage setups
   * trying to be everybody's darling (well, at least most people's)
Supported scenarios:

   * simple partition-based proposal
   * with or without LVM
   * with or without Btrfs
   * with or without LVM full disk encryption
   * with or without automated file system snapshots (if using Btrfs)
   * with or without SWAP partition
   * using all available space or leaving up unpartitioned disk space
   * using whole disk or keeping (and resizing) already installed system
   * RAID-based proposal?
AI: speak with different parties which requirements they have (Cloud, SES, SAP, ...)
   * -> bring all the use cases up
   * -> mark those that make most sense
   * -> discuss the implementation of them with UX expert

Not supported by the automated proposal:

   * RAID
     * will work for HW RAID or BIOS RAID, which both can be seen as a single disk by proposal
     * -> Options to be explored, so far we have no experience with proposal spread over multiple disks

##Misc. Topics to be taken care of

   * Support Systems with >> 800 hard disks, provide still reasonable performance
   * btrfs subvolume handling
   * btrfs snapshots
   * btrfs rollback: for this we need to install into a btrfs snapshot already
   * Use udisks for storage detection "to do it the right way"? => Not a good idea, see https://mailman.suse.de/mlarch/SuSE/research/2015/research.2015.10/msg00033.html
   * Samba?
   * SSD?
