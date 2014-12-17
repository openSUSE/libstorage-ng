/*
 * Copyright (c) [2004-2014] Novell, Inc.
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


#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    // strings must match /etc/fstab
    const vector<string> EnumInfo<FsType>::names({
	"unknown", "reiserfs", "ext2", "ext3", "ext4", "btrfs", "vfat", "xfs", "jfs", "hfs",
	"ntfs-3g", "swap", "hfsplus", "nfs", "nfs4", "tmpfs", "iso9660", "udf", "none"
    });


    const vector<string> EnumInfo<PartitionType>::names({
	"primary", "extended", "logical", "any"
    });


    const vector<string> EnumInfo<MountByType>::names({
	"device", "uuid", "label", "id", "path"
    });


    const vector<string> EnumInfo<EncryptType>::names({
	"none", "twofish256", "twofish", "twofishSL92", "luks", "unknown"
    });


    // strings must match /proc/mdstat
    const vector<string> EnumInfo<MdType>::names({
	"unknown", "raid0", "raid1", "raid5", "raid6", "raid10", "multipath"
    });


    // strings must match "mdadm --parity" option
    const vector<string> EnumInfo<MdParity>::names({
	"default", "left-asymmetric", "left-symmetric", "right-asymmetric", "right-symmetric",
	"parity-first", "parity-last",
	"left-asymmetric-6", "left-symmetric-6", "right-asymmetric-6", "right-symmetric-6",
	"parity-first-6",
	"n2", "o2", "f2", "n3", "o3", "f3"
    });


    // strings must match /sys/block/md*/md/array_state
    const vector<string> EnumInfo<MdArrayState>::names({
	"unknown", "clear", "inactive", "suspended", "readonly", "read-auto", "clean", "active",
	"write-pending", "active-idle"
    });


    const vector<string> EnumInfo<UsedByType>::names({
	"NONE", "LVM", "MD", "MDPART", "DM", "DMRAID", "DMMULTIPATH", "BTRFS"
    });


    const vector<string> EnumInfo<CType>::names({
	"UNKNOWN", "DISK", "MD", "LOOP", "LVM", "DM", "DMRAID", "NFS", "DMMULTIPATH", "MDPART",
	"BTRFS", "TMPFS"
    });


    const vector<string> EnumInfo<Transport>::names({
	"UNKNOWN", "SBP", "ATA", "FC", "iSCSI", "SAS", "SATA", "SPI", "USB", "FCoE"
    });


    const vector<string> EnumInfo<MultipathAutostart>::names({
	"UNDECIDED", "ON", "OFF"
    });


    // strings must match "parted --align" option
    const vector<string> EnumInfo<PartAlign>::names({
	"optimal", "cylinder"
    });

}
