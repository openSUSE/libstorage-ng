

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/Gpt.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Disk::Disk(DeviceGraph& device_graph, const string& name)
	: BlkDevice(device_graph, new Disk::Impl(device_graph, name))
    {
    }


    Disk::Disk(Impl* impl)
	: BlkDevice(impl)
    {
    }

/*
    Disk::Disk(DeviceGraph& device_graph, Impl* impl)
	: BlkDevice(device_graph, impl)
    {
    }
*/


    Disk*
    Disk::clone(DeviceGraph& device_graph) const
    {
	return new Disk(getImpl().clone(device_graph));
    }


    Disk::Impl&
    Disk::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const Disk::Impl&
    Disk::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }


    PartitionTable*
    Disk::createPartitionTable(const string& type)
    {
	if (numChildren() != 0)
	    throw runtime_error("has children");

	PartitionTable* ret = Gpt::create(getImpl().getDeviceGraph());
	Using::create(getImpl().getDeviceGraph(), this, ret);
	return ret;
    }

}
