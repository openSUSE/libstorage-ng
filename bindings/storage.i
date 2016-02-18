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

// order of includes is crucial

%{
// workaround for bsc #593954
#ifdef SWIGPERL
#undef seed
#endif

#include "storage/Utils/Logger.h"
#include "storage/Utils/Exception.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/Remote.h"
#include "storage/Geometry.h"

#include "storage/Devices/Device.h"
#include "storage/Devices/Filesystem.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Ntfs.h"
#include "storage/Devices/Vfat.h"
#include "storage/Devices/Btrfs.h"
#include "storage/Devices/Xfs.h"
#include "storage/Devices/Swap.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partitionable.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Md.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/LvmVg.h"

#include "storage/Holders/Holder.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Holders/User.h"
#include "storage/Holders/MdUser.h"

#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
%}

%include "stdint.i"
%include "std_string.i"
%include "std_vector.i"
%include "std_list.i"
%include "std_map.i"

%include "../../storage/StorageInterface.h"

%include "../../storage/Utils/Logger.h"
%include "../../storage/Utils/Exception.h"
%include "../../storage/Utils/HumanString.h"
%include "../../storage/Utils/Region.h"
%include "../../storage/Utils/Remote.h"
%include "../../storage/Geometry.h"

%include "../../storage/Devices/Device.h"
%include "../../storage/Devices/Filesystem.h"
%include "../../storage/Devices/Ext4.h"
%include "../../storage/Devices/Ntfs.h"
%include "../../storage/Devices/Vfat.h"
%include "../../storage/Devices/Btrfs.h"
%include "../../storage/Devices/Xfs.h"
%include "../../storage/Devices/Swap.h"
%include "../../storage/Devices/BlkDevice.h"
%include "../../storage/Devices/Partition.h"
%include "../../storage/Devices/PartitionTable.h"
%include "../../storage/Devices/Partitionable.h"
%include "../../storage/Devices/Disk.h"
%include "../../storage/Devices/Md.h"
%include "../../storage/Devices/Msdos.h"
%include "../../storage/Devices/Gpt.h"
%include "../../storage/Devices/LvmLv.h"
%include "../../storage/Devices/LvmVg.h"

%include "../../storage/Holders/Holder.h"
%include "../../storage/Holders/Subdevice.h"
%include "../../storage/Holders/User.h"
%include "../../storage/Holders/MdUser.h"

%include "../../storage/Devicegraph.h"
%include "../../storage/Actiongraph.h"
%include "../../storage/Environment.h"
%include "../../storage/Storage.h"

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

%template(VectorPartitionPtr) std::vector<Partition*>;
%template(VectorConstPartitionPtr) std::vector<const Partition*>;

%template(VectorFilesystemPtr) std::vector<Filesystem*>;
%template(VectorConstFilesystemPtr) std::vector<const Filesystem*>;

%template(VectorHolderPtr) std::vector<Holder*>;
%template(VectorConstHolderPtr) std::vector<const Holder*>;

%template(VectorPartitionSlot) std::vector<PartitionSlot>;

%template(VectorPtType) std::vector<PtType>;

