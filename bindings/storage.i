//
// Common SWIG interface definition for libstorage
//

%module(directors="1") storage

%ignore "get_impl";
%ignore "clone";
%ignore "operator <<";

%rename("==") "operator==";
%rename("!=") "operator!=";

// Since dynamic exception specifications are deprecated in C++11 we use the
// SWIG %catches feature instead.

%include "storage-catches.i"

%feature("director") storage::CommitCallbacks;
%feature("director") storage::RemoteCallbacks;
%feature("director") storage::Logger;

// Order of includes is crucial:
// 1. Devices, Filesystems and Holders in order of derivation.

%{

#include "storage/Utils/Swig.h"
#include "storage/Utils/Logger.h"
#include "storage/Utils/Exception.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/Topology.h"
#include "storage/Utils/Remote.h"
#include "storage/FreeInfo.h"

#include "storage/Devices/Device.h"
#include "storage/Filesystems/Filesystem.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Filesystems/Ntfs.h"
#include "storage/Filesystems/Vfat.h"
#include "storage/Filesystems/Btrfs.h"
#include "storage/Filesystems/Xfs.h"
#include "storage/Filesystems/Swap.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partitionable.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Md.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/Luks.h"
#include "storage/Devices/Bcache.h"
#include "storage/Devices/BcacheCset.h"

#include "storage/Holders/Holder.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Holders/User.h"
#include "storage/Holders/MdUser.h"

#include "storage/SystemInfo/Arch.h"

#include "storage/Graphviz.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
%}

%include "std_string.i"
%include "std_vector.i"
%include "std_list.i"
%include "std_map.i"

%include "../../storage/Utils/Swig.h"
%include "../../storage/Utils/Logger.h"
%include "../../storage/Utils/Exception.h"
%include "../../storage/Utils/HumanString.h"
%include "../../storage/Utils/Region.h"
%include "../../storage/Utils/Topology.h"
%include "../../storage/Utils/Remote.h"
%include "../../storage/FreeInfo.h"

%include "../../storage/Devices/Device.h"
%include "../../storage/Filesystems/Filesystem.h"
%include "../../storage/Filesystems/Ext4.h"
%include "../../storage/Filesystems/Ntfs.h"
%include "../../storage/Filesystems/Vfat.h"
%include "../../storage/Filesystems/Btrfs.h"
%include "../../storage/Filesystems/Xfs.h"
%include "../../storage/Filesystems/Swap.h"
%include "../../storage/Devices/BlkDevice.h"
%include "../../storage/Devices/Partition.h"
%include "../../storage/Devices/PartitionTable.h"
%include "../../storage/Devices/Partitionable.h"
%include "../../storage/Devices/Disk.h"
%include "../../storage/Devices/Md.h"
%include "../../storage/Devices/Msdos.h"
%include "../../storage/Devices/Gpt.h"
%include "../../storage/Devices/LvmPv.h"
%include "../../storage/Devices/LvmVg.h"
%include "../../storage/Devices/LvmLv.h"
%include "../../storage/Devices/Encryption.h"
%include "../../storage/Devices/Luks.h"
%include "../../storage/Devices/Bcache.h"
%include "../../storage/Devices/BcacheCset.h"

%include "../../storage/Holders/Holder.h"
%include "../../storage/Holders/Subdevice.h"
%include "../../storage/Holders/User.h"
%include "../../storage/Holders/MdUser.h"

%include "../../storage/SystemInfo/Arch.h"

%include "../../storage/Graphviz.h"
%include "../../storage/Devicegraph.h"
%include "../../storage/Actiongraph.h"
%include "../../storage/Environment.h"
%include "../../storage/Storage.h"

%include "storage-downcast.i"

using namespace storage;

%template(VectorString) std::vector<std::string>;
%template(ListString) std::list<std::string>;
%template(MapStringString) std::map<std::string, std::string>;

%template(VectorDevicePtr) std::vector<Device*>;
%template(VectorConstDevicePtr) std::vector<const Device*>;

%template(VectorBlkDevicePtr) std::vector<BlkDevice*>;
%template(VectorConstBlkDevicePtr) std::vector<const BlkDevice*>;

%template(VectorDiskPtr) std::vector<Disk*>;
%template(VectorConstDiskPtr) std::vector<const Disk*>;

%template(VectorMdPtr) std::vector<Md*>;
%template(VectorConstMdPtr) std::vector<const Md*>;

%template(VectorLvmPvPtr) std::vector<LvmPv*>;
%template(VectorConstLvmPvPtr) std::vector<const LvmPv*>;

%template(VectorLvmVgPtr) std::vector<LvmVg*>;
%template(VectorConstLvmVgPtr) std::vector<const LvmVg*>;

%template(VectorLvmLvPtr) std::vector<LvmLv*>;
%template(VectorConstLvmLvPtr) std::vector<const LvmLv*>;

%template(VectorPartitionablePtr) std::vector<Partitionable*>;
%template(VectorConstPartitionablePtr) std::vector<const Partitionable*>;

%template(VectorPartitionPtr) std::vector<Partition*>;
%template(VectorConstPartitionPtr) std::vector<const Partition*>;

%template(VectorEncryptionPtr) std::vector<Encryption*>;
%template(VectorConstEncryptionPtr) std::vector<const Encryption*>;

%template(VectorLuksPtr) std::vector<Luks*>;
%template(VectorConstLuksPtr) std::vector<const Luks*>;

%template(VectorBcachePtr) std::vector<Bcache*>;
%template(VectorConstBcachePtr) std::vector<const Bcache*>;

%template(VectorBcacheCsetPtr) std::vector<BcacheCset*>;
%template(VectorConstBcacheCsetPtr) std::vector<const BcacheCset*>;

%template(VectorFilesystemPtr) std::vector<Filesystem*>;
%template(VectorConstFilesystemPtr) std::vector<const Filesystem*>;

%template(VectorNtfsPtr) std::vector<Ntfs*>;
%template(VectorConstNtfsPtr) std::vector<const Ntfs*>;

%template(VectorHolderPtr) std::vector<Holder*>;
%template(VectorConstHolderPtr) std::vector<const Holder*>;

%template(VectorPartitionSlot) std::vector<PartitionSlot>;

%template(VectorPtType) std::vector<PtType>;

