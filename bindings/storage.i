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

%catches(storage::ParseError) storage::humanstring_to_byte(const std::string&, bool);

%catches(storage::DeviceHasWrongType) storage::to_blkdevice(Device*);
%catches(storage::DeviceHasWrongType) storage::to_blkdevice(const Device*);
%catches(storage::DeviceHasWrongType) storage::to_disk(Device*);
%catches(storage::DeviceHasWrongType) storage::to_disk(const Device*);
%catches(storage::DeviceHasWrongType) storage::to_partition_table(Device*);
%catches(storage::DeviceHasWrongType) storage::to_partition_table(const Device*);

%catches(storage::DeviceNotFound) storage::Devicegraph::find_device(sid_t);
%catches(storage::DeviceNotFound) storage::Devicegraph::find_device(sid_t) const;
%catches(storage::HolderNotFound) storage::Devicegraph::find_holder(sid_t, sid_t);
%catches(storage::HolderNotFound) storage::Devicegraph::find_holder(sid_t, sid_t) const;

%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::BlkDevice::find(const Devicegraph*, const std::string&);
%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::Disk::find(const Devicegraph*, const std::string&);
%catches(storage::DeviceNotFound, storage::DeviceHasWrongType) storage::Partition::find(const Devicegraph*, const std::string&);

%catches(storage::WrongNumberOfChildren, storage::DeviceHasWrongType) storage::Disk::get_partition_table();
%catches(storage::WrongNumberOfChildren, storage::DeviceHasWrongType) storage::Disk::get_partition_table() const;

%catches(storage::WrongNumberOfChildren, storage::DeviceHasWrongType) storage::BlkDevice::get_filesystem();
%catches(storage::WrongNumberOfChildren, storage::DeviceHasWrongType) storage::BlkDevice::get_filesystem() const;

%feature("director") storage::CommitCallbacks;
%feature("director") storage::RemoteCallbacks;
%feature("director") storage::Logger;

// order of includes is crucial

%{
// workaround for bsc #593954
#ifdef SWIGPERL
#undef seed
#endif

#include "storage/Utils/HumanString.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/Remote.h"
#include "storage/Devices/Device.h"
#include "storage/Devices/Filesystem.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Btrfs.h"
#include "storage/Devices/Xfs.h"
#include "storage/Devices/Swap.h"
#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Holders/Holder.h"
#include "storage/Devicegraph.h"
#include "storage/Environment.h"
#include "storage/Logger.h"
#include "storage/Storage.h"
%}

%include "stdint.i"
%include "std_string.i"
%include "std_vector.i"
%include "std_list.i"
%include "std_map.i"

%include "../../storage/StorageInterface.h"
%include "../../storage/Utils/HumanString.h"
%include "../../storage/Utils/Region.h"
%include "../../storage/Utils/Remote.h"
%include "../../storage/Devices/Device.h"
%include "../../storage/Devices/Filesystem.h"
%include "../../storage/Devices/Ext4.h"
%include "../../storage/Devices/Btrfs.h"
%include "../../storage/Devices/Xfs.h"
%include "../../storage/Devices/Swap.h"
%include "../../storage/Devices/BlkDevice.h"
%include "../../storage/Devices/Partition.h"
%include "../../storage/Devices/PartitionTable.h"
%include "../../storage/Devices/Disk.h"
%include "../../storage/Devices/Msdos.h"
%include "../../storage/Devices/Gpt.h"
%include "../../storage/Devices/LvmLv.h"
%include "../../storage/Devices/LvmVg.h"
%include "../../storage/Holders/Holder.h"
%include "../../storage/Devicegraph.h"
%include "../../storage/Environment.h"
%include "../../storage/Logger.h"
%include "../../storage/Storage.h"

using namespace storage;

%template(VectorString) std::vector<std::string>;
%template(ListString) std::list<std::string>;
%template(MapStringString) std::map<std::string, std::string>;

%template(VectorConstDevicePtr) std::vector<const Device*>;
%template(VectorDiskPtr) std::vector<Disk*>;
%template(VectorConstDiskPtr) std::vector<const Disk*>;
%template(VectorConstPartitionPtr) std::vector<const Partition*>;
%template(VectorConstFilesystemPtr) std::vector<const Filesystem*>;
