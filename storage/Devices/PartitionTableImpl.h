#ifndef PARTITION_TABLE_IMPL_H
#define PARTITION_TABLE_IMPL_H


#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    // abstract class

    class PartitionTable::Impl : public Device::Impl
    {
    public:

	virtual void save(xmlNode* node) const override;

	// read-only

    protected:

	Impl(DeviceGraph& device_graph)
	    : Device::Impl(device_graph) {}

	Impl(DeviceGraph& device_graph, const Impl& impl)
	    : Device::Impl(device_graph, impl) {}

    };

}

#endif
