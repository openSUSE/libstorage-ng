//
// Common SWIG interface definition for libstorage
//

%module storage

%ignore getImpl;
%ignore clone;

// Since dynamic exception specifications are deprecated in C++11 we use the
// SWIG %catches feature instead.

%catches(storage::DeviceNotFound) storage::DeviceGraph::findDevice(sid_t);
%catches(storage::DeviceNotFound) storage::DeviceGraph::findDevice(sid_t) const;
%catches(storage::HolderNotFound) storage::DeviceGraph::findHolder(sid_t, sid_t);
%catches(storage::HolderNotFound) storage::DeviceGraph::findHolder(sid_t, sid_t) const;

%catches(storage::DeviceNotFound) storage::BlkDevice::find(const DeviceGraph*, const string&);

// order of includes is crucial

%{
// workaround for bsc #593954
#ifdef SWIGPERL
#undef seed
#endif

#include <storage/Devices/Device.h>
#include <storage/Devices/Filesystem.h>
#include <storage/Devices/BlkDevice.h>
#include <storage/Devices/Partition.h>
#include <storage/Devices/PartitionTable.h>
#include <storage/Devices/Disk.h>
#include <storage/Devices/Gpt.h>
#include <storage/Devices/LvmLv.h>
#include <storage/Devices/LvmVg.h>
#include <storage/Holders/Holder.h>
#include <storage/DeviceGraph.h>
#include <storage/Environment.h>
#include <storage/Storage.h>
%}

using namespace std;

%include "stdint.i"
%include "std_string.i"
%include "std_vector.i"
%include "std_list.i"

%include "../../storage/Devices/Device.h"
%include "../../storage/Devices/Filesystem.h"
%include "../../storage/Devices/BlkDevice.h"
%include "../../storage/Devices/Partition.h"
%include "../../storage/Devices/PartitionTable.h"
%include "../../storage/Devices/Disk.h"
%include "../../storage/Devices/Gpt.h"
%include "../../storage/Devices/LvmLv.h"
%include "../../storage/Devices/LvmVg.h"
%include "../../storage/Holders/Holder.h"
%include "../../storage/DeviceGraph.h"
%include "../../storage/Environment.h"
%include "../../storage/Storage.h"

using namespace storage;

%template(VectorConstDevicePtr) std::vector<const Device*>;
%template(VectorConstPartitionPtr) std::vector<const Partition*>;
%template(VectorConstFilesystemPtr) std::vector<const Filesystem*>;

%inline %{

    const storage::Disk* toDisk(const storage::Device* device) {
	return dynamic_cast<const storage::Disk*>(device);
    }

    const storage::PartitionTable* toPartitionTable(const storage::Device* device) {
	return dynamic_cast<const storage::PartitionTable*>(device);
    }

    const storage::Partition* toPartition(const storage::Device* device) {
	return dynamic_cast<const storage::Partition*>(device);
    }

    const storage::LvmVg* toLvmVg(const storage::Device* device) {
	return dynamic_cast<const storage::LvmVg*>(device);
    }

    const storage::LvmLv* toLvmLv(const storage::Device* device) {
	return dynamic_cast<const storage::LvmLv*>(device);
    }

    const storage::Filesystem* toFilesystem(const storage::Device* device) {
	return dynamic_cast<const storage::Filesystem*>(device);
    }

%}
