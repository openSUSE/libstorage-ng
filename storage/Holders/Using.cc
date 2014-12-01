

#include "storage/Holders/Using.h"


namespace storage
{

    Using*
    Using::create(DeviceGraph& device_graph, const Device* source, const Device* target)
    {
	return new Using(device_graph, source, target);
    }


    Using::Using(DeviceGraph& device_graph, const Device* source, const Device* target)
	    : Holder(device_graph, source, target)
    {
    }

}
