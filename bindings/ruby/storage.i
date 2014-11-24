//
// Ruby interface definition for libstorage
//

%module storage

%{
#include <storage/Devices/Device.h>
#include <storage/Devices/BlkDevice.h>
#include <storage/Devices/Disk.h>
#include <storage/Devices/Partition.h>
#include <storage/Devices/PartitionTable.h>
#include <storage/Devices/Gpt.h>
#include <storage/Holders/Holder.h>
#include <storage/DeviceGraph.h>
%}

using namespace std;

%include "std_string.i"
%include "std_list.i"
%include "std_vector.i"

%include "../../storage/Devices/Device.h"
%include "../../storage/Devices/BlkDevice.h"
%include "../../storage/Devices/Disk.h"
%include "../../storage/Devices/Partition.h"
%include "../../storage/Devices/PartitionTable.h"
%include "../../storage/Devices/Gpt.h"
%include "../../storage/Holders/Holder.h"
%include "../../storage/DeviceGraph.h"

using namespace storage;

%template(VectorConstDevicePtr) std::vector<const Device*>;

%template(VectorConstPartitionPtr) std::vector<const Partition*>;

