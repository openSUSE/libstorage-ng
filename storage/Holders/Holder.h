#ifndef HOLDER_H
#define HOLDER_H


#include "storage/Holders/Holder.h"


namespace storage
{

    class DeviceGraph;
    class Device;


    // abstract class

    class Holder
    {
    public:

	virtual ~Holder() {}

    protected:

	Holder(DeviceGraph& device_graph, const Device* source, const Device* target);

    };

}

#endif
