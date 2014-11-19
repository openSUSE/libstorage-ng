

#include "storage/Devices/PartitionImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Partition::Partition(DeviceGraph& device_graph, const string& name)
	: BlkDevice(device_graph, new Partition::Impl(device_graph, name))
    {
    }


    Partition::Partition(Impl* impl)
	: BlkDevice(impl)
    {
    }

/*
    Partition::Partition(DeviceGraph& device_graph, Impl* impl)
	: BlkDevice(device_graph, impl)
    {
    }
*/

    Partition*
    Partition::clone(DeviceGraph& device_graph) const
    {
	return new Partition(getImpl().clone(device_graph));
    }


    Partition::Impl&
    Partition::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const Partition::Impl&
    Partition::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }


    unsigned int
    Partition::getNumber() const
    {
	string::size_type pos = getName().find_last_not_of("0123456789");
	return atoi(getName().substr(pos + 1).c_str());
    }

}
