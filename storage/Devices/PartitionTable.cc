

#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    PartitionTable::PartitionTable(Impl* impl)
	: Device(impl)
    {
    }


    PartitionTable::Impl&
    PartitionTable::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const PartitionTable::Impl&
    PartitionTable::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }


    Partition*
    PartitionTable::createPartition(const string& name)
    {
	DeviceGraph* device_graph = getImpl().getDeviceGraph();

	Partition* partition = Partition::create(device_graph, name);
	Subdevice::create(device_graph, this, partition);

	return partition;
    }


    vector<const Partition*>
    PartitionTable::getPartitions() const
    {
	const DeviceGraph* device_graph = getImpl().getDeviceGraph();
	DeviceGraph::Impl::vertex_descriptor vertex = getImpl().getVertex();

	return device_graph->getImpl().getDevices<Partition>(device_graph->getImpl().children(vertex));
    }

}
