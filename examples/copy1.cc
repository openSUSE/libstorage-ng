

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/DeviceGraph.h"


using namespace storage;


int
main()
{
    DeviceGraph device_graph;

    Disk* sda = new Disk(device_graph, "/dev/sda");

    Partition* sda1 = new Partition(device_graph, "/dev/sda1");
    new Subdevice(device_graph, sda, sda1);

    Partition* sda2 = new Partition(device_graph, "/dev/sda2");
    new Subdevice(device_graph, sda, sda2);

    DeviceGraph device_graph_copy;
    device_graph.copy(device_graph_copy);

    device_graph_copy.remove_vertex(sda2);

    device_graph.print_graph();
    device_graph_copy.print_graph();
}
