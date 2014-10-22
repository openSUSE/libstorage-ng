

#include "storage/DeviceGraph.h"


using namespace storage;


int
main()
{
    DeviceGraph device_graph;

    Disk* disk = new Disk("/dev/dasda");
    Partition* partition = new Partition("/dev/dasda2");

    DeviceGraph::vertex_descriptor v1 = device_graph.add_vertex(disk);
    DeviceGraph::vertex_descriptor v2 = device_graph.add_vertex(partition);

    device_graph.add_edge(v1, v2, new Subdevice());

    DeviceGraph device_graph_copy;
    device_graph.copy(device_graph_copy);

    {
	BlkDevice* tmp = dynamic_cast<BlkDevice*>(device_graph_copy.find_device(partition->sid));
	assert(tmp);

	tmp->name = "/dev/dasda1";
    }

    device_graph.print_graph();
    device_graph_copy.print_graph();

    {
	Disk* tmp = dynamic_cast<Disk*>(device_graph.find_device(disk->sid));
	assert(tmp);

	assert(tmp->name == "/dev/dasda");
    }

    {
	Partition* tmp = dynamic_cast<Partition*>(device_graph.find_device(partition->sid));
	assert(tmp);

	assert(tmp->name == "/dev/dasda2");
	assert(tmp->is_primary());
    }

    {
	Disk* tmp = dynamic_cast<Disk*>(device_graph_copy.find_device(disk->sid));
	assert(tmp);

	assert(tmp->name == "/dev/dasda");
    }

    {
	Partition* tmp = dynamic_cast<Partition*>(device_graph_copy.find_device(partition->sid));
	assert(partition);

	assert(tmp->name == "/dev/dasda1");
	assert(tmp->is_primary());
    }
}
