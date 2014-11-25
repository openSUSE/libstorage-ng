//
// Python interface definition for libstorage
//

%module storage

// TODO almost identical to Ruby file

// order of includes is crucial

%{
#include <storage/Devices/Device.h>
#include <storage/Devices/Filesystem.h>
#include <storage/Devices/BlkDevice.h>
#include <storage/Devices/Partition.h>
#include <storage/Devices/PartitionTable.h>
#include <storage/Devices/Disk.h>
#include <storage/Devices/Gpt.h>
#include <storage/Holders/Holder.h>
#include <storage/DeviceGraph.h>
%}

%inline %{

    const storage::Partition* CastToPartition(const storage::Device* device) {
	return dynamic_cast<const storage::Partition*>(device);
    }

    const storage::PartitionTable* CastToPartitionTable(const storage::Device* device) {
	return dynamic_cast<const storage::PartitionTable*>(device);
    }

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

using namespace storage;

%template(VectorConstDevicePtr) std::vector<const Device*>;
%template(VectorConstPartitionPtr) std::vector<const Partition*>;

