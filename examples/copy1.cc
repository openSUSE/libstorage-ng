

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/DeviceGraph.h"


using namespace storage;


int
main()
{
    DeviceGraph device_graph;

    Disk* sda = Disk::create(device_graph, "/dev/sda");

    Partition* sda1 = Partition::create(device_graph, "/dev/sda1");
    Subdevice::create(device_graph, sda, sda1);

    Partition* sda2 = Partition::create(device_graph, "/dev/sda2");
    Subdevice::create(device_graph, sda, sda2);

    DeviceGraph device_graph_copy;
    device_graph.copy(device_graph_copy);

    device_graph_copy.remove_vertex(sda2);

    device_graph.print_graph();
    device_graph_copy.print_graph();
}
