//
// Common SWIG interface definition for libstorage
//

%module storage

%ignore "get_impl";
%ignore "get_classname";
%ignore "clone";

// Since dynamic exception specifications are deprecated in C++11 we use the
// SWIG %catches feature instead.

%catches(storage_bgl::ParseError) storage_bgl::humanstring_to_byte(const string&, bool);

%catches(storage_bgl::DeviceNotFound) storage_bgl::Devicegraph::find_device(sid_t);
%catches(storage_bgl::DeviceNotFound) storage_bgl::Devicegraph::find_device(sid_t) const;
%catches(storage_bgl::HolderNotFound) storage_bgl::Devicegraph::find_holder(sid_t, sid_t);
%catches(storage_bgl::HolderNotFound) storage_bgl::Devicegraph::find_holder(sid_t, sid_t) const;

%catches(storage_bgl::DeviceNotFound) storage_bgl::BlkDevice::find(const Devicegraph*, const string&);

// order of includes is crucial

%{
// workaround for bsc #593954
#ifdef SWIGPERL
#undef seed
#endif

#include "storage/Utils/HumanString.h"
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

using namespace std;

%include "stdint.i"
%include "std_string.i"
%include "std_vector.i"
%include "std_list.i"

%include "../../storage/Utils/HumanString.h"
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
%include "../../storage/StorageInterface.h"

using namespace storage_bgl;

%template(VectorConstDevicePtr) std::vector<const Device*>;
%template(VectorConstPartitionPtr) std::vector<const Partition*>;
%template(VectorConstFilesystemPtr) std::vector<const Filesystem*>;

%inline %{

    const storage_bgl::Disk* to_disk(const storage_bgl::Device* device) {
	return dynamic_cast<const storage_bgl::Disk*>(device);
    }

    const storage_bgl::PartitionTable* to_partition_table(const storage_bgl::Device* device) {
	return dynamic_cast<const storage_bgl::PartitionTable*>(device);
    }

    const storage_bgl::Partition* to_partition(const storage_bgl::Device* device) {
	return dynamic_cast<const storage_bgl::Partition*>(device);
    }

    const storage_bgl::LvmVg* to_lvm_vg(const storage_bgl::Device* device) {
	return dynamic_cast<const storage_bgl::LvmVg*>(device);
    }

    const storage_bgl::LvmLv* to_lvm_lv(const storage_bgl::Device* device) {
	return dynamic_cast<const storage_bgl::LvmLv*>(device);
    }

    const storage_bgl::Filesystem* to_filesystem(const storage_bgl::Device* device) {
	return dynamic_cast<const storage_bgl::Filesystem*>(device);
    }

%}
