/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2023] SUSE LLC
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#ifndef STORAGE_STORAGE_DEFINES_H
#define STORAGE_STORAGE_DEFINES_H


// paths

#define DEV_DIR "/dev"
#define DEV_MD_DIR "/dev/md"
#define DEV_MAPPER_DIR "/dev/mapper"

#define DEV_DISK_BY_ID_DIR DEV_DIR "/disk/by-id"
#define DEV_DISK_BY_PATH_DIR DEV_DIR "/disk/by-path"
#define DEV_DISK_BY_UUID_DIR DEV_DIR "/disk/by-uuid"
#define DEV_DISK_BY_LABEL_DIR DEV_DIR "/disk/by-label"
#define DEV_DISK_BY_PARTUUID_DIR DEV_DIR "/disk/by-partuuid"
#define DEV_DISK_BY_PARTLABEL_DIR DEV_DIR "/disk/by-partlabel"

#define ETC_DIR "/etc"
#define SYSFS_DIR "/sys"
#define PROC_DIR "/proc"

#define ETC_LIBSTORAGE_DIR ETC_DIR "/libstorage"
#define USR_SHARE_LIBSTORAGE_DIR "/usr/share/libstorage"

#define EFIVARS_DIR SYSFS_DIR "/firmware/efi/efivars"

#define LOCKFILE_DIR "/run/libstorage-ng"


// files

#define DEV_NULL_FILE DEV_DIR "/null"
#define DEV_ZERO_FILE DEV_DIR "/zero"
#define DEV_URANDOM_FILE DEV_DIR "/urandom"

#define UDEV_FILTERS_FILE "udev-filters.json"
#define ETC_LIBSTORAGE_UDEV_FILTERS_FILE ETC_LIBSTORAGE_DIR "/" UDEV_FILTERS_FILE
#define USR_SHARE_LIBSTORAGE_UDEV_FILTERS_FILE USR_SHARE_LIBSTORAGE_DIR "/" UDEV_FILTERS_FILE


// commands

#define SH_BIN "/bin/sh"

#define ECHO_BIN "/usr/bin/echo"
#define CAT_BIN "/usr/bin/cat"

#define UNAME_BIN "/usr/bin/uname"
#define GETCONF_BIN "/usr/bin/getconf"

#define PARTED_BIN "/usr/sbin/parted"

#define MDADM_BIN "/sbin/mdadm"

#define PVCREATE_BIN "/sbin/pvcreate"
#define PVREMOVE_BIN "/sbin/pvremove"
#define PVRESIZE_BIN "/sbin/pvresize"
#define PVS_BIN "/sbin/pvs"

#define LVCREATE_BIN "/sbin/lvcreate"
#define LVREMOVE_BIN "/sbin/lvremove"
#define LVRENAME_BIN "/sbin/lvrename"
#define LVRESIZE_BIN "/sbin/lvresize"
#define LVCHANGE_BIN "/sbin/lvchange"
#define LVS_BIN "/sbin/lvs"

#define VGCREATE_BIN "/sbin/vgcreate"
#define VGREMOVE_BIN "/sbin/vgremove"
#define VGRENAME_BIN "/sbin/vgrename"
#define VGEXTEND_BIN "/sbin/vgextend"
#define VGREDUCE_BIN "/sbin/vgreduce"
#define VGS_BIN "/sbin/vgs"
#define VGCHANGE_BIN "/sbin/vgchange"

#define CRYPTSETUP_BIN "/sbin/cryptsetup"
#define MULTIPATH_BIN "/sbin/multipath"
#define MULTIPATHD_BIN "/sbin/multipathd"
#define DMSETUP_BIN "/sbin/dmsetup"
#define DMRAID_BIN "/sbin/dmraid"
#define BTRFS_BIN "/sbin/btrfs"
#define WIPEFS_BIN "/sbin/wipefs"
#define BLKDISCARD_BIN "/usr/sbin/blkdiscard"

#define BCACHE_BIN "/usr/sbin/bcache"

#define MOUNT_BIN "/bin/mount"
#define UMOUNT_BIN "/bin/umount"
#define SWAPON_BIN "/sbin/swapon"
#define SWAPOFF_BIN "/sbin/swapoff"

#define DD_BIN "/bin/dd"

#define BLKID_BIN "/sbin/blkid"
#define LSSCSI_BIN "/usr/bin/lsscsi"

#define LS_BIN "/bin/ls"
#define DF_BIN "/usr/bin/df"
#define TEST_BIN "/usr/bin/test"
#define STAT_BIN "/usr/bin/stat"
#define BLOCKDEV_BIN "/usr/sbin/blockdev"

#define LOSETUP_BIN "/sbin/losetup"

#define NVME_BIN "/usr/sbin/nvme"

#define LSATTR_BIN "/usr/bin/lsattr"
#define CHATTR_BIN "/usr/bin/chattr"

#define DASDVIEW_BIN "/sbin/dasdview"

#define UDEVADM_BIN "/usr/bin/udevadm"
#define UDEVADM_BIN_SETTLE UDEVADM_BIN " settle --timeout=20"

#define RPCBIND_BIN "/sbin/rpcbind"

#define EFIBOOTMGR_BIN "/usr/sbin/efibootmgr"

#define NTFSRESIZE_BIN "/usr/sbin/ntfsresize"
#define XFSGROWFS_BIN "/usr/sbin/xfs_growfs"
#define REISERFSRESIZE_BIN "/sbin/resize_reiserfs"
#define RESIZE2FS_BIN "/usr/sbin/resize2fs"
#define FATRESIZE_BIN "/usr/sbin/fatresize"
#define NILFS_RESIZE_BIN "/usr/sbin/nilfs-resize"

#define TUNE2FS_BIN "/sbin/tune2fs"
#define TUNEREISERFS_BIN "/sbin/reiserfstune"
#define XFSADMIN_BIN "/usr/sbin/xfs_admin"
#define TUNEJFS_BIN "/sbin/jfs_tune"
#define NTFSLABEL_BIN "/usr/sbin/ntfslabel"
#define FATLABEL_BIN "/usr/sbin/fatlabel"
#define SWAPLABEL_BIN "/sbin/swaplabel"
#define EXFATLABEL_BIN "/usr/sbin/exfatlabel"
#define NILFS_TUNE_BIN "/usr/sbin/nilfs-tune"

#define DUMPE2FS_BIN "/sbin/dumpe2fs"

#define MKSWAP_BIN "/sbin/mkswap"
#define MKFS_XFS_BIN "/sbin/mkfs.xfs"
#define MKFS_JFS_BIN "/sbin/mkfs.jfs"
#define MKFS_FAT_BIN "/sbin/mkfs.fat"
#define MKFS_NTFS_BIN "/sbin/mkfs.ntfs"
#define MKFS_REISERFS_BIN "/sbin/mkreiserfs"
#define MKFS_EXT2_BIN "/sbin/mke2fs"
#define MKFS_BTRFS_BIN "/sbin/mkfs.btrfs"
#define MKFS_F2FS_BIN "/usr/sbin/mkfs.f2fs"
#define MKFS_NILFS2_BIN "/usr/sbin/mkfs.nilfs2"
#define MKFS_EXFAT_BIN "/usr/sbin/mkfs.exfat"
#define MKFS_UDF_BIN "/usr/sbin/mkfs.udf"

#define DOT_BIN "/usr/bin/dot"
#define DISPLAY_BIN "/usr/bin/display"


// regexes

#define UUID_REGEX "[a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12}"


#endif
