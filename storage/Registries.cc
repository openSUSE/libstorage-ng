/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2026] SUSE LLC
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
#include "storage/Devices/BitlockerV2.h"
#include "storage/Devices/Bcache.h"
#include "storage/Devices/BcacheCset.h"
#include "storage/Filesystems/Bcachefs.h"
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
#include "storage/Filesystems/Nilfs2.h"
#include "storage/Filesystems/Swap.h"
#include "storage/Filesystems/Iso9660.h"
#include "storage/Filesystems/Udf.h"
#include "storage/Filesystems/Squashfs.h"
#include "storage/Filesystems/Erofs.h"
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
	{ "Bcache", &Bcache::load },
	{ "BcacheCset", &BcacheCset::load },
	{ "Bcachefs", &Bcachefs::load },
	{ "Bitlocker", &Bitlocker::load },
	{ "BitlockerV2", &BitlockerV2::load },
	{ "Btrfs", &Btrfs::load },
	{ "BtrfsQgroup", &BtrfsQgroup::load },
	{ "BtrfsSubvolume", &BtrfsSubvolume::load },
	{ "Dasd", &Dasd::load },
	{ "DasdPt", &DasdPt::load },
	{ "Disk", &Disk::load },
	{ "DmRaid", &DmRaid::load },
	{ "Encryption", &Encryption::load },
	{ "Erofs", &Erofs::load },
	{ "Exfat", &Exfat::load },
	{ "Ext2", &Ext2::load },
	{ "Ext3", &Ext3::load },
	{ "Ext4", &Ext4::load },
	{ "F2fs", &F2fs::load },
	{ "Gpt", &Gpt::load },
	{ "ImplicitPt", &ImplicitPt::load },
	{ "Iso9660", &Iso9660::load },
	{ "Jfs", &Jfs::load },
	{ "Luks", &Luks::load },
	{ "LvmLv", &LvmLv::load },
	{ "LvmPv", &LvmPv::load },
	{ "LvmVg", &LvmVg::load },
	{ "Md", &Md::load },
	{ "MdContainer", &MdContainer::load },
	{ "MdMember", &MdMember::load },
	{ "MountPoint", &MountPoint::load },
	{ "Msdos", &Msdos::load },
	{ "Multipath", &Multipath::load },
	{ "Nfs", &Nfs::load },
	{ "Nilfs2", &Nilfs2::load },
	{ "Ntfs", &Ntfs::load },
	{ "Partition", &Partition::load },
	{ "PlainEncryption", &PlainEncryption::load },
	{ "Reiserfs", &Reiserfs::load },
	{ "Squashfs", &Squashfs::load },
	{ "StrayBlkDevice", &StrayBlkDevice::load },
	{ "Swap", &Swap::load },
	{ "Tmpfs", &Tmpfs::load },
	{ "Udf", &Udf::load },
	{ "Vfat", &Vfat::load },
	{ "Xfs", &Xfs::load }
    };


    const map<string, holder_load_fnc> holder_load_registry = {
	{ "BtrfsQgroupRelation", &BtrfsQgroupRelation::load },
	{ "FilesystemUser", &FilesystemUser::load },
	{ "MdSubdevice", &MdSubdevice::load },
	{ "MdUser", &MdUser::load },
	{ "Snapshot", &Snapshot::load },
	{ "Subdevice", &Subdevice::load },
	{ "User", &User::load }
    };


    const map<FsType, blk_filesystem_create_fnc> blk_filesystem_create_registry = {
	{ FsType::BCACHEFS, &Bcachefs::create },
	{ FsType::BITLOCKER, &Bitlocker::create },
	{ FsType::BTRFS, &Btrfs::create },
	{ FsType::EROFS1, &Erofs::create },
	{ FsType::EXFAT, &Exfat::create },
	{ FsType::EXT2, &Ext2::create },
	{ FsType::EXT3, &Ext3::create },
	{ FsType::EXT4, &Ext4::create },
	{ FsType::F2FS, &F2fs::create },
	{ FsType::ISO9660, &Iso9660::create },
	{ FsType::JFS, &Jfs::create },
	{ FsType::NILFS2, &Nilfs2::create },
	{ FsType::NTFS, &Ntfs::create },
	{ FsType::REISERFS, &Reiserfs::create },
	{ FsType::SQUASHFS, &Squashfs::create },
	{ FsType::SWAP, &Swap::create },
	{ FsType::UDF, &Udf::create },
	{ FsType::VFAT, &Vfat::create },
	{ FsType::XFS, &Xfs::create }
    };


    const map<EncryptionType, encryption_create_fnc> encryption_create_registry = {
	{ EncryptionType::LUKS1, &Luks::create },
	{ EncryptionType::LUKS2, &Luks::create },
	{ EncryptionType::PLAIN, &PlainEncryption::create }
    };

}
