

#include "DeviceGraph.h"


using namespace storage;


int
main()
{
    DeviceGraph device_graph;

    DeviceGraph::vertex_descriptor sda = device_graph.add_vertex(new Disk("/dev/sda"));
    DeviceGraph::vertex_descriptor sda1 = device_graph.add_vertex(new Partition("/dev/sda1"));
    DeviceGraph::vertex_descriptor sda2 = device_graph.add_vertex(new Partition("/dev/sda2"));

    device_graph.add_edge(sda, sda1, new Subdevice());
    device_graph.add_edge(sda, sda2, new Subdevice());

    DeviceGraph device_graph_copy;
    device_graph.copy(device_graph_copy);

    device_graph_copy.remove_vertex(sda2);

    device_graph.print_graph();
    device_graph_copy.print_graph();
}
