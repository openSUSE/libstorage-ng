/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include "storage/Registries.h"

#include "storage/Devices/Disk.h"
#include "storage/Devices/Dasd.h"
#include "storage/Devices/Multipath.h"
#include "storage/Devices/DmRaid.h"
#include "storage/Devices/Md.h"
#include "storage/Devices/MdContainer.h"
#include "storage/Devices/MdMember.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/DasdPt.h"
#include "storage/Devices/ImplicitPt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/StrayBlkDevice.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/PlainEncryption.h"
#include "storage/Devices/Luks.h"
#include "storage/Devices/Bcache.h"
#include "storage/Devices/BcacheCset.h"
#include "storage/Filesystems/Ext2.h"
#include "storage/Filesystems/Ext3.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Filesystems/Ntfs.h"
#include "storage/Filesystems/Vfat.h"
#include "storage/Filesystems/Exfat.h"
#include "storage/Filesystems/Btrfs.h"
#include "storage/Filesystems/BtrfsSubvolume.h"
#include "storage/Filesystems/BtrfsQgroup.h"
#include "storage/Filesystems/Reiserfs.h"
#include "storage/Filesystems/Xfs.h"
#include "storage/Filesystems/Jfs.h"
#include "storage/Filesystems/F2fs.h"
#include "storage/Filesystems/Swap.h"
#include "storage/Filesystems/Iso9660.h"
#include "storage/Filesystems/Udf.h"
#include "storage/Filesystems/Bitlocker.h"
#include "storage/Filesystems/Nfs.h"
#include "storage/Filesystems/Tmpfs.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Holders/User.h"
#include "storage/Holders/MdUser.h"
#include "storage/Holders/FilesystemUser.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Holders/MdSubdevice.h"
#include "storage/Holders/Snapshot.h"
#include "storage/Holders/BtrfsQgroupRelation.h"


namespace storage
{

    const map<string, device_load_fnc> device_load_registry = {
	{ "Disk", &Disk::load },
	{ "Dasd", &Dasd::load },
	{ "Multipath", &Multipath::load },
	{ "DmRaid", &DmRaid::load },
	{ "Md", &Md::load },
	{ "MdContainer", &MdContainer::load },
	{ "MdMember", &MdMember::load },
	{ "Msdos", &Msdos::load },
	{ "Gpt", &Gpt::load },
	{ "DasdPt", &DasdPt::load },
	{ "ImplicitPt", &ImplicitPt::load },
	{ "Partition", &Partition::load },
	{ "StrayBlkDevice", &StrayBlkDevice::load },
	{ "LvmPv", &LvmPv::load },
	{ "LvmVg", &LvmVg::load },
	{ "LvmLv", &LvmLv::load },
	{ "Encryption", &Encryption::load },
	{ "PlainEncryption", &PlainEncryption::load },
	{ "Luks", &Luks::load },
	{ "Bcache", &Bcache::load },
	{ "BcacheCset", &BcacheCset::load },
	{ "Ext2", &Ext2::load },
	{ "Ext3", &Ext3::load },
	{ "Ext4", &Ext4::load },
	{ "Ntfs", &Ntfs::load },
	{ "Vfat", &Vfat::load },
	{ "Exfat", &Exfat::load },
	{ "Btrfs", &Btrfs::load },
	{ "BtrfsSubvolume", &BtrfsSubvolume::load },
	{ "BtrfsQgroup", &BtrfsQgroup::load },
	{ "Reiserfs", &Reiserfs::load },
	{ "Xfs", &Xfs::load },
	{ "Jfs", &Jfs::load },
	{ "F2fs", &F2fs::load },
	{ "Swap", &Swap::load },
	{ "Iso9660", &Iso9660::load },
	{ "Udf", &Udf::load },
	{ "Bitlocker", &Bitlocker::load },
	{ "Nfs", &Nfs::load },
	{ "Tmpfs", &Tmpfs::load },
	{ "MountPoint", &MountPoint::load }
    };


    const map<string, holder_load_fnc> holder_load_registry = {
	{ "User", &User::load },
	{ "MdUser", &MdUser::load },
	{ "FilesystemUser", &FilesystemUser::load },
	{ "Subdevice", &Subdevice::load },
	{ "MdSubdevice", &MdSubdevice::load },
	{ "Snapshot", &Snapshot::load },
	{ "BtrfsQgroupRelation", &BtrfsQgroupRelation::load }
    };


    const map<FsType, blk_filesystem_create_fnc> blk_filesystem_create_registry = {
	{ FsType::BTRFS, &Btrfs::create },
	{ FsType::EXT2, &Ext2::create },
	{ FsType::EXT3, &Ext3::create },
	{ FsType::EXT4, &Ext4::create },
	{ FsType::ISO9660, &Iso9660::create },
	{ FsType::NTFS, &Ntfs::create },
	{ FsType::REISERFS, &Reiserfs::create },
	{ FsType::SWAP, &Swap::create },
	{ FsType::UDF, &Udf::create },
	{ FsType::VFAT, &Vfat::create },
	{ FsType::EXFAT, &Exfat::create },
	{ FsType::XFS, &Xfs::create },
	{ FsType::JFS, &Jfs::create },
	{ FsType::F2FS, &F2fs::create },
	{ FsType::BITLOCKER, &Bitlocker::create }
    };


    const map<EncryptionType, encryption_create_fnc> encryption_create_registry = {
	{ EncryptionType::PLAIN, &PlainEncryption::create },
	{ EncryptionType::LUKS1, &Luks::create },
	{ EncryptionType::LUKS2, &Luks::create }
    };

}
