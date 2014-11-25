

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/DeviceGraph.h"


using namespace storage;


int
main()
{
    DeviceGraph device_graph;

    Disk* disk = Disk::create(device_graph, "/dev/dasda");

    Partition* partition = Partition::create(device_graph, "/dev/dasda2");
    Subdevice::create(device_graph, disk, partition);

    DeviceGraph device_graph_copy;
    device_graph.copy(device_graph_copy);

    device_graph.check();
    device_graph_copy.check();

    {
	BlkDevice* tmp = dynamic_cast<BlkDevice*>(device_graph_copy.find_device(partition->getSid()));
	assert(tmp);

	tmp->setName("/dev/dasda1");
    }

    device_graph.print_graph();
    device_graph_copy.print_graph();

    {
	Disk* tmp = dynamic_cast<Disk*>(device_graph.find_device(disk->getSid()));
	assert(tmp);

	assert(tmp->getName() == "/dev/dasda");
    }

    {
	Partition* tmp = dynamic_cast<Partition*>(device_graph.find_device(partition->getSid()));
	assert(tmp);

	assert(tmp->getName() == "/dev/dasda2");
    }

    {
	Disk* tmp = dynamic_cast<Disk*>(device_graph_copy.find_device(disk->getSid()));
	assert(tmp);

	assert(tmp->getName() == "/dev/dasda");
    }

    {
	Partition* tmp = dynamic_cast<Partition*>(device_graph_copy.find_device(partition->getSid()));
	assert(partition);

	assert(tmp->getName() == "/dev/dasda1");
    }
}
