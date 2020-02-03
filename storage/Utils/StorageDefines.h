/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2020] SUSE LLC
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

#define SYSFS_DIR "/sys"


// commands

#define SHBIN "/bin/sh"

#define ECHO_BIN "/usr/bin/echo"
#define CAT_BIN "/usr/bin/cat"

#define UNAME_BIN "/usr/bin/uname"
#define GETCONF_BIN "/usr/bin/getconf"

#define PARTED_BIN "/usr/sbin/parted"

#define MDADM_BIN "/sbin/mdadm"

#define PVCREATEBIN "/sbin/pvcreate"
#define PVREMOVEBIN "/sbin/pvremove"
#define PVRESIZEBIN "/sbin/pvresize"
#define PVS_BIN "/sbin/pvs"

#define LVCREATEBIN "/sbin/lvcreate"
#define LVREMOVEBIN "/sbin/lvremove"
#define LVRESIZEBIN "/sbin/lvresize"
#define LVCHANGEBIN "/sbin/lvchange"
#define LVS_BIN "/sbin/lvs"

#define VGCREATEBIN "/sbin/vgcreate"
#define VGREMOVEBIN "/sbin/vgremove"
#define VGEXTENDBIN "/sbin/vgextend"
#define VGREDUCEBIN "/sbin/vgreduce"
#define VGS_BIN "/sbin/vgs"
#define VGCHANGEBIN "/sbin/vgchange"

#define CRYPTSETUP_BIN "/sbin/cryptsetup"
#define MULTIPATHBIN "/sbin/multipath"
#define MULTIPATHDBIN "/sbin/multipathd"
#define DMSETUP_BIN "/sbin/dmsetup"
#define DMRAIDBIN "/sbin/dmraid"
#define BTRFSBIN "/sbin/btrfs"
#define WIPEFSBIN "/sbin/wipefs"

#define BCACHE_BIN "/usr/sbin/bcache"

#define MOUNTBIN "/bin/mount"
#define UMOUNTBIN "/bin/umount"
#define SWAPONBIN "/sbin/swapon"
#define SWAPOFFBIN "/sbin/swapoff"

#define DD_BIN "/bin/dd"

#define BLKID_BIN "/sbin/blkid"
#define LSSCSI_BIN "/usr/bin/lsscsi"

#define LSBIN "/bin/ls"
#define DFBIN "/usr/bin/df"
#define TEST_BIN "/usr/bin/test"
#define STATBIN "/usr/bin/stat"

#define LSATTR_BIN "/usr/bin/lsattr"
#define CHATTRBIN "/usr/bin/chattr"

#define DASDVIEW_BIN "/sbin/dasdview"

#define UDEVADM_BIN "/usr/bin/udevadm"
#define UDEVADM_BIN_SETTLE UDEVADM_BIN " settle --timeout=20"

#define RPCBINDBIN     "/sbin/rpcbind"

#define EFIBOOTMGRBIN "/usr/sbin/efibootmgr"

#define NTFSRESIZE_BIN "/usr/sbin/ntfsresize"
#define XFSGROWFSBIN  "/usr/sbin/xfs_growfs"
#define REISERFSRESIZEBIN "/sbin/resize_reiserfs"
#define RESIZE2FS_BIN "/usr/sbin/resize2fs"
#define FATRESIZEBIN "/usr/sbin/fatresize"

#define TUNE2FSBIN     "/sbin/tune2fs"
#define TUNEREISERFSBIN "/sbin/reiserfstune"
#define XFSADMINBIN    "/usr/sbin/xfs_admin"
#define TUNEJFSBIN "/sbin/jfs_tune"
#define NTFSLABELBIN   "/usr/sbin/ntfslabel"
#define FATLABELBIN	"/usr/sbin/fatlabel"
#define SWAPLABELBIN   "/sbin/swaplabel"
#define EXFAT_LABEL_BIN "/sbin/exfatlabel"

#define DUMPE2FS_BIN "/sbin/dumpe2fs"

#define MKSWAPBIN      "/sbin/mkswap"
#define MKFS_XFS_BIN "/sbin/mkfs.xfs"
#define MKFSJFSBIN "/sbin/mkfs.jfs"
#define MKFSFATBIN	"/sbin/mkfs.fat"
#define MKFSNTFSBIN	"/sbin/mkfs.ntfs"
#define MKFSREISERFSBIN "/sbin/mkreiserfs"
#define MKFSEXT2BIN    "/sbin/mke2fs"
#define MKFSBTRFSBIN   "/sbin/mkfs.btrfs"
#define MKFSF2FSBIN "/usr/sbin/mkfs.f2fs"
#define MKFS_EXFAT_BIN "/sbin/mkfs.exfat"
#define MKFS_UDF_BIN "/usr/sbin/mkfs.udf"

#define DOT_BIN "/usr/bin/dot"
#define DISPLAY_BIN "/usr/bin/display"


//regexes

#define UUID_REGEX "[a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12}"


#endif
