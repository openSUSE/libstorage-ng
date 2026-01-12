//
// Common SWIG interface definition for libstorage-ng
//

%module(directors="1") storage

%ignore "get_impl";
%ignore "clone";
%ignore "clone_v2";
%ignore "operator <<";
%ignore "get_all_if";

%rename("==") "operator==";
%rename("!=") "operator!=";

%typedef int pid_t;

use_ostream(storage::Devicegraph);
use_ostream(storage::Device);
use_ostream(storage::Holder);
use_ostream(storage::Region);
use_ostream(storage::ResizeInfo);
use_ostream(storage::ContentInfo);
use_ostream(storage::SpaceInfo);
use_ostream(storage::PartitionSlot);

// Since dynamic exception specifications are deprecated in C++11 we use the
// SWIG %catches feature instead.

%include "storage-catches.i"

%feature("director") storage::ActivateCallbacks;
%feature("director") storage::ActivateCallbacksLuks;
%feature("director") storage::ActivateCallbacksV3;
%feature("director") storage::ProbeCallbacks;
%feature("director") storage::ProbeCallbacksV2;
%feature("director") storage::ProbeCallbacksV3;
%feature("director") storage::ProbeCallbacksV4;
%feature("director") storage::CheckCallbacks;
%feature("director") storage::CommitCallbacks;
%feature("director") storage::CommitCallbacksV2;
%feature("director") storage::RemoteCallbacks;
%feature("director") storage::RemoteCallbacksV2;
%feature("director") storage::DevicegraphStyleCallbacks;
%feature("director") storage::Logger;

// Order of includes is crucial:
// 1. Devices, Filesystems and Holders in order of derivation.

%{
#include "storage/CompoundAction.h"

#include "storage/Utils/Swig.h"
#include "storage/Utils/Logger.h"
#include "storage/Utils/Exception.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/Topology.h"
#include "storage/Utils/Alignment.h"
#include "storage/Utils/Remote.h"
#include "storage/Utils/Callbacks.h"
#include "storage/Utils/LightProbe.h"
#include "storage/Utils/Lock.h"
#include "storage/FreeInfo.h"
#include "storage/UsedFeatures.h"
#include "storage/LvmDevicesFile.h"
#include "storage/View.h"
#include "storage/Version.h"

#include "storage/SystemInfo/Arch.h"
#include "storage/SystemInfo/SystemInfo.h"

#include "storage/Devices/Device.h"
#include "storage/Filesystems/Mountable.h"
#include "storage/Filesystems/Filesystem.h"
#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Filesystems/Ext.h"
#include "storage/Filesystems/Ext2.h"
#include "storage/Filesystems/Ext3.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Filesystems/Ntfs.h"
#include "storage/Filesystems/Vfat.h"
#include "storage/Filesystems/Exfat.h"
#include "storage/Filesystems/Bcachefs.h"
#include "storage/Filesystems/BtrfsQgroup.h"
#include "storage/Filesystems/Btrfs.h"
#include "storage/Filesystems/BtrfsSubvolume.h"
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
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partitionable.h"
#include "storage/Devices/StrayBlkDevice.h"
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
#include "storage/Devices/LvmPv.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/PlainEncryption.h"
#include "storage/Devices/Luks.h"
#include "storage/Devices/BitlockerV2.h"
#include "storage/Devices/Bcache.h"
#include "storage/Devices/BcacheCset.h"

#include "storage/Holders/Holder.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Holders/MdSubdevice.h"
#include "storage/Holders/User.h"
#include "storage/Holders/MdUser.h"
#include "storage/Holders/FilesystemUser.h"
#include "storage/Holders/Snapshot.h"
#include "storage/Holders/BtrfsQgroupRelation.h"

#include "storage/Actions/Base.h"
#include "storage/Actions/Create.h"
#include "storage/Actions/Modify.h"
#include "storage/Actions/Delete.h"

#include "storage/Graphviz.h"
#include "storage/SimpleEtcFstab.h"
#include "storage/SimpleEtcCrypttab.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Pool.h"
#include "storage/Environment.h"
#include "storage/CommitOptions.h"
#include "storage/Storage.h"
%}

%include "stdint.i"
%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"

%include "../../storage/CompoundAction.h"

%include "../../storage/Utils/Swig.h"
%include "../../storage/Utils/Logger.h"
%include "../../storage/Utils/Exception.h"
%include "../../storage/Utils/HumanString.h"
%include "../../storage/Utils/Region.h"
%include "../../storage/Utils/Topology.h"
%include "../../storage/Utils/Alignment.h"
%include "../../storage/Utils/Remote.h"
%include "../../storage/Utils/Callbacks.h"
%include "../../storage/Utils/LightProbe.h"
%include "../../storage/Utils/Lock.h"
%include "../../storage/FreeInfo.h"
%include "../../storage/UsedFeatures.h"
%include "../../storage/LvmDevicesFile.h"
%include "../../storage/View.h"
%include "../../storage/Version.h"

%include "../../storage/SystemInfo/Arch.h"
%include "../../storage/SystemInfo/SystemInfo.h"

%include "../../storage/Devices/Device.h"
%include "../../storage/Filesystems/Mountable.h"
%include "../../storage/Filesystems/Filesystem.h"
%include "../../storage/Filesystems/BlkFilesystem.h"
%include "../../storage/Filesystems/Ext.h"
%include "../../storage/Filesystems/Ext2.h"
%include "../../storage/Filesystems/Ext3.h"
%include "../../storage/Filesystems/Ext4.h"
%include "../../storage/Filesystems/Ntfs.h"
%include "../../storage/Filesystems/Vfat.h"
%include "../../storage/Filesystems/Exfat.h"
%include "../../storage/Filesystems/Bcachefs.h"
%include "../../storage/Filesystems/BtrfsQgroup.h"
%include "../../storage/Filesystems/Btrfs.h"
%include "../../storage/Filesystems/BtrfsSubvolume.h"
%include "../../storage/Filesystems/Reiserfs.h"
%include "../../storage/Filesystems/Xfs.h"
%include "../../storage/Filesystems/Jfs.h"
%include "../../storage/Filesystems/F2fs.h"
%include "../../storage/Filesystems/Nilfs2.h"
%include "../../storage/Filesystems/Swap.h"
%include "../../storage/Filesystems/Iso9660.h"
%include "../../storage/Filesystems/Udf.h"
%include "../../storage/Filesystems/Squashfs.h"
%include "../../storage/Filesystems/Erofs.h"
%include "../../storage/Filesystems/Bitlocker.h"
%include "../../storage/Filesystems/Nfs.h"
%include "../../storage/Filesystems/Tmpfs.h"
%include "../../storage/Filesystems/MountPoint.h"
%include "../../storage/Devices/BlkDevice.h"
%include "../../storage/Devices/Partition.h"
%include "../../storage/Devices/PartitionTable.h"
%include "../../storage/Devices/Partitionable.h"
%include "../../storage/Devices/StrayBlkDevice.h"
%include "../../storage/Devices/Disk.h"
%include "../../storage/Devices/Dasd.h"
%include "../../storage/Devices/Multipath.h"
%include "../../storage/Devices/DmRaid.h"
%include "../../storage/Devices/Md.h"
%include "../../storage/Devices/MdContainer.h"
%include "../../storage/Devices/MdMember.h"
%include "../../storage/Devices/Msdos.h"
%include "../../storage/Devices/Gpt.h"
%include "../../storage/Devices/DasdPt.h"
%include "../../storage/Devices/ImplicitPt.h"
%include "../../storage/Devices/LvmPv.h"
%include "../../storage/Devices/LvmLv.h"
%include "../../storage/Devices/LvmVg.h"
%include "../../storage/Devices/Encryption.h"
%include "../../storage/Devices/PlainEncryption.h"
%include "../../storage/Devices/Luks.h"
%include "../../storage/Devices/BitlockerV2.h"
%include "../../storage/Devices/Bcache.h"
%include "../../storage/Devices/BcacheCset.h"

%include "../../storage/Holders/Holder.h"
%include "../../storage/Holders/Subdevice.h"
%include "../../storage/Holders/MdSubdevice.h"
%include "../../storage/Holders/User.h"
%include "../../storage/Holders/MdUser.h"
%include "../../storage/Holders/FilesystemUser.h"
%include "../../storage/Holders/Snapshot.h"
%include "../../storage/Holders/BtrfsQgroupRelation.h"

%include "../../storage/Actions/Base.h"
%include "../../storage/Actions/Create.h"
%include "../../storage/Actions/Modify.h"
%include "../../storage/Actions/Delete.h"

%include "../../storage/Graphviz.h"
%include "../../storage/SimpleEtcFstab.h"
%include "../../storage/SimpleEtcCrypttab.h"
%include "../../storage/Devicegraph.h"
%include "../../storage/Actiongraph.h"
%include "../../storage/Pool.h"
%include "../../storage/Environment.h"
%include "../../storage/CommitOptions.h"
%include "../../storage/Storage.h"

%include "storage-downcast.i"

using namespace storage;

%include "storage-template.i"
