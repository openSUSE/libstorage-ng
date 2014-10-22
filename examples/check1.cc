

#include "storage/Device.h"
#include "storage/DeviceGraph.h"


using namespace storage;


DeviceGraph device_graph;


void
make_device_graph()
{
    DeviceGraph::vertex_descriptor sda = device_graph.add_vertex(new Disk("/dev/sda"));
    DeviceGraph::vertex_descriptor sda1 = device_graph.add_vertex(new Partition("/dev/sda1"));
    DeviceGraph::vertex_descriptor sda2 = device_graph.add_vertex(new Partition("/dev/sda2"));

    device_graph.add_edge(sda, sda1, new Subdevice());
    device_graph.add_edge(sda, sda2, new Subdevice());

    DeviceGraph::vertex_descriptor sdb = device_graph.add_vertex(new Disk("/dev/sdb"));
    DeviceGraph::vertex_descriptor sdb1 = device_graph.add_vertex(new Partition("/dev/sdb1"));
    DeviceGraph::vertex_descriptor sdb2 = device_graph.add_vertex(new Partition("/dev/sdb2"));

    device_graph.add_edge(sdb, sdb1, new Subdevice());
    device_graph.add_edge(sdb, sdb2, new Subdevice());

    DeviceGraph::vertex_descriptor system = device_graph.add_vertex(new LvmVg("/dev/system"));
    DeviceGraph::vertex_descriptor system_root = device_graph.add_vertex(new LvmLv("/dev/system/root"));
    DeviceGraph::vertex_descriptor system_swap = device_graph.add_vertex(new LvmLv("/dev/system/swap"));

    device_graph.add_edge(system, system_root, new Subdevice());
    device_graph.add_edge(system, system_swap, new Subdevice());

    device_graph.add_edge(sda1, system, new Using());
    device_graph.add_edge(sdb1, system, new Using());

    DeviceGraph::vertex_descriptor sdc = device_graph.add_vertex(new Disk("/dev/sdc"));
    DeviceGraph::vertex_descriptor data = device_graph.add_vertex(new LvmVg("/dev/data"));

    device_graph.add_edge(sdc, data, new Using());

    device_graph.add_edge(system_swap, sda, new Using()); // cycle

    // device_graph.add_edge(sdb1, system, new Using()); // parallel edge

    device_graph.add_vertex(new LvmVg(""));

    device_graph.add_vertex(new Disk("/dev/sdc")); // duplicate name
}


int
main()
{
    make_device_graph();

    cout << "num_vertices: " << num_vertices(device_graph.graph) << endl;
    cout << "num_edges: " << num_edges(device_graph.graph) << endl;

    cout << endl;

    device_graph.check();

    device_graph.write_graphviz("check1");
}
