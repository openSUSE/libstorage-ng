

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/Gpt.h"
#include "storage/Holders/Using.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Disk*
    Disk::create(DeviceGraph* device_graph, const string& name)
    {
	Disk* ret = new Disk(new Disk::Impl(name));
	ret->addToDeviceGraph(device_graph);
	return ret;
    }


    Disk*
    Disk::load(DeviceGraph* device_graph, const xmlNode* node)
    {
	Disk* ret = new Disk(new Disk::Impl(node));
	ret->addToDeviceGraph(device_graph);
	return ret;
    }


    Disk::Disk(Impl* impl)
	: BlkDevice(impl)
    {
    }


    Disk*
    Disk::clone() const
    {
	return new Disk(getImpl().clone());
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
