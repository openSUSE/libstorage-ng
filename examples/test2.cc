

#include "storage/Device.h"
#include "storage/DeviceGraph.h"


using namespace storage;


void
partitions()
{
    DeviceGraph device_graph;

    DeviceGraph::vertex_descriptor v1 = device_graph.add_vertex(new Disk("/dev/sda"));
    DeviceGraph::vertex_descriptor v2 = device_graph.add_vertex(new Partition("/dev/sda1"));
    DeviceGraph::vertex_descriptor v3 = device_graph.add_vertex(new Partition("/dev/sda2"));

    device_graph.add_edge(v1, v2, new Subdevice());
    device_graph.add_edge(v1, v3, new Subdevice());

    /*
    Disk* disk = dynamic_cast<Disk*>(device_graph.graph[v1].get());
    for (Partition* partition : disk->getPartitions(device_graph))
    {
	cout << partition->name << endl;
    }
    */
}


int
main()
{
    partitions();
}
