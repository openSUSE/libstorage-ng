//
// Common SWIG interface definition for libstorage
//

%module storage

%ignore getImpl;
%ignore clone;

// Since dynamic exception specifications are deprecated in C++11 we use the
// SWIG %catches feature instead.

%catches(storage::device_not_found) storage::BlkDevice::find(const DeviceGraph* device_graph, const string& name);

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
#include <storage/Holders/Holder.h>
#include <storage/DeviceGraph.h>
#include <storage/Environment.h>
#include <storage/Storage.h>
%}

using namespace std;

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
%include "../../storage/Holders/Holder.h"
%include "../../storage/DeviceGraph.h"
%include "../../storage/Environment.h"
%include "../../storage/Storage.h"

using namespace storage;

%template(VectorConstDevicePtr) std::vector<const Device*>;
%template(VectorConstPartitionPtr) std::vector<const Partition*>;

%inline %{

    const storage::Partition* castToPartition(const storage::Device* device) {
	return dynamic_cast<const storage::Partition*>(device);
    }

    const storage::PartitionTable* castToPartitionTable(const storage::Device* device) {
	return dynamic_cast<const storage::PartitionTable*>(device);
    }

%}
