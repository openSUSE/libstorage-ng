//
// Common SWIG interface definition for libstorage
//

%module storage

%ignore "get_impl";
%ignore "get_classname";
%ignore "clone";
%ignore "operator <<";
%ignore "print";

%rename("==") "operator==";
%rename("!=") "operator!=";

// Since dynamic exception specifications are deprecated in C++11 we use the
// SWIG %catches feature instead.

%catches(storage::ParseError) storage::humanstring_to_byte(const std::string&, bool);

%catches(storage::DeviceNotFound) storage::Devicegraph::find_device(sid_t);
%catches(storage::DeviceNotFound) storage::Devicegraph::find_device(sid_t) const;
%catches(storage::HolderNotFound) storage::Devicegraph::find_holder(sid_t, sid_t);
%catches(storage::HolderNotFound) storage::Devicegraph::find_holder(sid_t, sid_t) const;

%catches(storage::DeviceNotFound) storage::BlkDevice::find(const Devicegraph*, const std::string&);
%catches(storage::DeviceNotFound) storage::Disk::find(const Devicegraph*, const std::string&);
%catches(storage::DeviceNotFound) storage::Partition::find(const Devicegraph*, const std::string&);

// order of includes is crucial

%{
// workaround for bsc #593954
#ifdef SWIGPERL
#undef seed
#endif

#include "storage/Utils/HumanString.h"
#include "storage/Utils/Region.h"
#include "storage/Devices/Device.h"
#include "storage/Devices/Filesystem.h"
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
%include "../../storage/Devices/Device.h"
%include "../../storage/Devices/Filesystem.h"
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
%include "../../storage/Storage.h"

using namespace storage;

%template(VectorString) std::vector<std::string>;
%template(ListString) std::list<std::string>;
%template(MapStringString) std::map<std::string, std::string>;

%template(VectorConstDevicePtr) std::vector<const Device*>;
%template(VectorConstPartitionPtr) std::vector<const Partition*>;
%template(VectorConstFilesystemPtr) std::vector<const Filesystem*>;

