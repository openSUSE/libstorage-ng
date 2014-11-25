//
// Common SWIG interface definition for libstorage
//

%module storage

%ignore getImpl;
%ignore clone;

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

