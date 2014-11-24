//
// Python interface definition for libstorage
//

%module libstorage

%{
#include <storage/Devices/Device.h>
#include <storage/Devices/BlkDevice.h>
#include <storage/Devices/Disk.h>
#include <storage/Holders/Holder.h>
#include <storage/DeviceGraph.h>
%}

using namespace std;

%include "std_string.i"
%include "std_vector.i"
%include "std_list.i"

%include "../../storage/Devices/Device.h"
%include "../../storage/Devices/BlkDevice.h"
%include "../../storage/Devices/Disk.h"
%include "../../storage/Holders/Holder.h"
%include "../../storage/DeviceGraph.h"

using namespace storage;

