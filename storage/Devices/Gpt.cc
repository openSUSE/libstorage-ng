

#include "storage/Devices/GptImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Gpt::Gpt(DeviceGraph& device_graph)
	: PartitionTable(device_graph, new Gpt::Impl(device_graph))
    {
    }


    Gpt::Gpt(Impl* impl)
	: PartitionTable(impl)
    {
    }


    Gpt::Gpt(DeviceGraph& device_graph, Impl* impl)
	: PartitionTable(device_graph, impl)
    {
    }


    Gpt*
    Gpt::clone(DeviceGraph& device_graph) const
    {
	return new Gpt(getImpl().clone(device_graph));
    }


    Gpt::Impl&
    Gpt::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const Gpt::Impl&
    Gpt::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }

}
