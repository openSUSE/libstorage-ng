

#include "storage/Devices/GptImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Gpt*
    Gpt::create(DeviceGraph* device_graph)
    {
	Gpt* ret = new Gpt(new Gpt::Impl());
	ret->addToDeviceGraph(device_graph);
	return ret;
    }


    Gpt*
    Gpt::load(DeviceGraph* device_graph, const xmlNode* node)
    {
	Gpt* ret = new Gpt(new Gpt::Impl(node));
	ret->addToDeviceGraph(device_graph);
	return ret;
    }


    Gpt::Gpt(Impl* impl)
	: PartitionTable(impl)
    {
    }


    Gpt*
    Gpt::clone() const
    {
	return new Gpt(getImpl().clone());
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
