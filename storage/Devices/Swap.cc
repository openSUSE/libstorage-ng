

#include "storage/Devices/SwapImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Swap::Swap(DeviceGraph& device_graph)
	: Filesystem(device_graph, new Swap::Impl(device_graph))
    {
    }


    Swap::Swap(Impl* impl)
	: Filesystem(impl)
    {
    }


/*
    Swap::Swap(DeviceGraph& device_graph, Impl* impl)
	: Filesystem(device_graph, impl)
    {
    }
*/


    Swap*
    Swap::clone(DeviceGraph& device_graph) const
    {
	return new Swap(getImpl().clone(device_graph));
    }


    Swap::Impl&
    Swap::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const Swap::Impl&
    Swap::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }

}
