

#include "storage/Holders/Subdevice.h"


namespace storage
{

    Subdevice*
    Subdevice::create(DeviceGraph* device_graph, const Device* source, const Device* target)
    {
	return new Subdevice(device_graph, source, target);
    }


    Subdevice::Subdevice(DeviceGraph* device_graph, const Device* source, const Device* target)
	: Holder(device_graph, source, target)
    {
    }

}
