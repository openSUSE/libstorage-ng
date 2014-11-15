

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Device.h"
#include "storage/DeviceGraph.h"


using namespace storage;


DeviceGraph device_graph;


void
children1(DeviceGraph::vertex_descriptor v)
{
    cout << "children of " << device_graph.graph[v]->display_name() << ":" << endl;

    for (DeviceGraph::vertex_descriptor tmp : boost::make_iterator_range(adjacent_vertices(v, device_graph.graph)))
    {
	cout << "  " << device_graph.graph[tmp]->display_name() << endl;
    }

    cout << endl;
}


void
children2(DeviceGraph::vertex_descriptor v)
{
    cout << "children of " << device_graph.graph[v]->display_name() << ":" << endl;

    for (DeviceGraph::edge_descriptor tmp : boost::make_iterator_range(out_edges(v, device_graph.graph)))
    {
	cout << "  " << device_graph.graph[source(tmp, device_graph.graph)]->display_name() << " -> "
	     << device_graph.graph[target(tmp, device_graph.graph)]->display_name() << endl;
    }

    cout << endl;
}


void
parents(DeviceGraph::vertex_descriptor v)
{
    cout << "parents of " << device_graph.graph[v]->display_name() << ":" << endl;

    for (DeviceGraph::vertex_descriptor tmp : boost::make_iterator_range(inv_adjacent_vertices(v, device_graph.graph)))
    {
	cout << "  " << device_graph.graph[tmp]->display_name() << endl;
    }

    cout << endl;
}


void
siblings(DeviceGraph::vertex_descriptor v)
{
    cout << "siblings of " << device_graph.graph[v]->display_name() << ":" << endl;

    vector<DeviceGraph::vertex_descriptor> l = device_graph.siblings(v, false);

    for (const DeviceGraph::vertex_descriptor& v : l)
	cout << "  " << device_graph.graph[v]->display_name() << endl;

    cout << endl;
}


void
descendants(DeviceGraph::vertex_descriptor v)
{
    cout << "descendants of " << device_graph.graph[v]->display_name() << ":" << endl;

    vector<DeviceGraph::vertex_descriptor> l = device_graph.descendants(v, false);

    for (const DeviceGraph::vertex_descriptor& v : l)
	cout << "  " << device_graph.graph[v]->display_name() << endl;

    cout << endl;
}


void
ancestors(DeviceGraph::vertex_descriptor v)
{
    cout << "ancestors of " << device_graph.graph[v]->display_name() << ":" << endl;

    vector<DeviceGraph::vertex_descriptor> l = device_graph.ancestors(v, false);

    for (const DeviceGraph::vertex_descriptor& v : l)
	cout << "  " << device_graph.graph[v]->display_name() << endl;

    cout << endl;
}


void
leafs(DeviceGraph::vertex_descriptor v)
{
    cout << "leafs of " << device_graph.graph[v]->display_name() << ":" << endl;

    vector<DeviceGraph::vertex_descriptor> l = device_graph.leafs(v, false);

    for (const DeviceGraph::vertex_descriptor& v : l)
	cout << "  " << device_graph.graph[v]->display_name() << endl;

    cout << endl;
}


void
roots(DeviceGraph::vertex_descriptor v)
{
    cout << "roots of " << device_graph.graph[v]->display_name() << ":" << endl;

    vector<DeviceGraph::vertex_descriptor> l = device_graph.roots(v, false);

    for (const DeviceGraph::vertex_descriptor& v : l)
	cout << "  " << device_graph.graph[v]->display_name() << endl;

    cout << endl;
}


int
main()
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
    DeviceGraph::vertex_descriptor system_home = device_graph.add_vertex(new LvmLv("/dev/system/home"));

    device_graph.add_edge(system, system_root, new Subdevice());
    device_graph.add_edge(system, system_swap, new Subdevice());
    device_graph.add_edge(system, system_home, new Subdevice());

    device_graph.add_edge(sda1, system, new Using());
    device_graph.add_edge(sdb1, system, new Using());

    DeviceGraph::vertex_descriptor sdc = device_graph.add_vertex(new Disk("/dev/sdc"));
    DeviceGraph::vertex_descriptor data = device_graph.add_vertex(new LvmVg("/dev/data"));

    device_graph.add_edge(sdc, data, new Using());

    cout << "num_vertices: " << num_vertices(device_graph.graph) << endl;
    cout << "num_edges: " << num_edges(device_graph.graph) << endl;

    cout << endl;

    children1(sda);

    children2(sdb);
    children2(sdc);

    parents(system);

    siblings(sda1);
    siblings(system_root);

    descendants(sda1);

    ancestors(system_swap);

    leafs(sda1);

    roots(system_swap);

    device_graph.print_graph();

    device_graph.write_graphviz("test1");
}
