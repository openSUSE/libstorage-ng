/*
 * Copyright (c) [2004-2015] Novell, Inc.
 * Copyright (c) [2016-2018] SUSE LLC
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


#define DEVDIR "/dev"
#define DEVMDDIR "/dev/md"
#define DEVMAPPERDIR "/dev/mapper"

#define SYSFSDIR "/sys"

#define SYSCONFIGFILE "/etc/sysconfig/storage"

#define SHBIN "/bin/sh"

#define UNAMEBIN "/usr/bin/uname"
#define GETCONFBIN "/usr/bin/getconf"

#define PARTEDBIN "/usr/sbin/parted"
#define ADDPARTBIN "/usr/sbin/addpart"
#define DELPARTBIN "/usr/sbin/delpart"

#define MDADMBIN "/sbin/mdadm"

#define PVCREATEBIN "/sbin/pvcreate"
#define PVREMOVEBIN "/sbin/pvremove"
#define PVRESIZEBIN "/sbin/pvresize"
#define PVSBIN "/sbin/pvs"

#define LVCREATEBIN "/sbin/lvcreate"
#define LVREMOVEBIN "/sbin/lvremove"
#define LVRESIZEBIN "/sbin/lvresize"
#define LVCHANGEBIN "/sbin/lvchange"
#define LVSBIN "/sbin/lvs"

#define VGCREATEBIN "/sbin/vgcreate"
#define VGREMOVEBIN "/sbin/vgremove"
#define VGEXTENDBIN "/sbin/vgextend"
#define VGREDUCEBIN "/sbin/vgreduce"
#define VGSBIN "/sbin/vgs"
#define VGSCANBIN "/sbin/vgscan"
#define VGCHANGEBIN "/sbin/vgchange"

#define CRYPTSETUPBIN "/sbin/cryptsetup"
#define LOSETUPBIN "/sbin/losetup"
#define MULTIPATHBIN "/sbin/multipath"
#define MULTIPATHDBIN "/sbin/multipathd"
#define DMSETUPBIN "/sbin/dmsetup"
#define DMRAIDBIN "/sbin/dmraid"
#define BTRFSBIN "/sbin/btrfs"
#define WIPEFSBIN "/sbin/wipefs"

#define MOUNTBIN "/bin/mount"
#define UMOUNTBIN "/bin/umount"
#define SWAPONBIN "/sbin/swapon"
#define SWAPOFFBIN "/sbin/swapoff"

#define DDBIN "/bin/dd"

#define BLKIDBIN "/sbin/blkid"
#define BLOCKDEVBIN "/sbin/blockdev"
#define LSSCSIBIN "/usr/bin/lsscsi"

#define LSBIN "/bin/ls"
#define DFBIN "/usr/bin/df"
#define TESTBIN "/usr/bin/test"
#define STATBIN "/usr/bin/stat"

#define LSATTRBIN "/usr/bin/lsattr"
#define CHATTRBIN "/usr/bin/chattr"

#define DASDFMTBIN "/sbin/dasdfmt"
#define DASDVIEWBIN "/sbin/dasdview"

#define UDEVADMBIN "/sbin/udevadm"
#define UDEVADMBIN_SETTLE UDEVADMBIN " settle --timeout=20"

#define MODPROBEBIN "/sbin/modprobe"
#define RPCBINDBIN     "/sbin/rpcbind"

#define EFIBOOTMGRBIN "/usr/sbin/efibootmgr"

#define NTFSRESIZEBIN "/usr/sbin/ntfsresize"
#define XFSGROWFSBIN  "/usr/sbin/xfs_growfs"
#define REISERFSRESIZEBIN "/sbin/resize_reiserfs"
#define EXT2RESIZEBIN "/sbin/resize2fs"
#define FATRESIZE     "/usr/sbin/fatresize"

#define TUNE2FSBIN     "/sbin/tune2fs"
#define TUNEREISERFSBIN "/sbin/reiserfstune"
#define XFSADMINBIN    "/usr/sbin/xfs_admin"
#define TUNEJFSBIN "/sbin/jfs_tune"
#define NTFSLABELBIN   "/usr/sbin/ntfslabel"
#define FATLABELBIN	"/usr/sbin/fatlabel"
#define SWAPLABELBIN   "/sbin/swaplabel"

#define FSCKBIN        "/sbin/fsck"
#define FSCKEXT2BIN    "/sbin/fsck.ext2"
#define FSCKEXT3BIN    "/sbin/fsck.ext3"
#define FSCKEXT4BIN    "/sbin/fsck.ext4"
#define XFSREPAIRBIN   "/usr/sbin/xfs_repair"
#define FSCKREISERBIN  "/sbin/reiserfsck"

#define MKSWAPBIN      "/sbin/mkswap"
#define MKFSXFSBIN     "/sbin/mkfs.xfs"
#define MKFSJFSBIN "/sbin/mkfs.jfs"
#define MKFSFATBIN	"/sbin/mkfs.fat"
#define MKFSNTFSBIN	"/sbin/mkfs.ntfs"
#define MKFSREISERFSBIN "/sbin/mkreiserfs"
#define MKFSEXT2BIN    "/sbin/mke2fs"
#define MKFSBTRFSBIN   "/sbin/mkfs.btrfs"


#endif
