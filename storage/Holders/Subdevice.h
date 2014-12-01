#ifndef SUBDEVICE_H
#define SUBDEVICE_H


#include "storage/Holders/Holder.h"


namespace storage
{

    class Subdevice : public Holder
    {
    public:

	static Subdevice* create(DeviceGraph& device_graph, const Device* source, const Device* target);

    private:

	Subdevice(DeviceGraph& device_graph, const Device* source, const Device* target);

    };

}

#endif
