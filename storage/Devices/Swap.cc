

#include "storage/Devices/SwapImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Swap*
    Swap::create(DeviceGraph* device_graph)
    {
	Swap* ret = new Swap(new Swap::Impl());
	ret->addToDeviceGraph(device_graph);
	return ret;
    }


    Swap*
    Swap::load(DeviceGraph* device_graph, const xmlNode* node)
    {
	Swap* ret = new Swap(new Swap::Impl(node));
	ret->addToDeviceGraph(device_graph);
	return ret;
    }


    Swap::Swap(Impl* impl)
	: Filesystem(impl)
    {
    }


    Swap*
    Swap::clone() const
    {
	return new Swap(getImpl().clone());
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
