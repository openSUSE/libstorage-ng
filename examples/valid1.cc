

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
}


void
test1()
{
    DeviceGraph::vertex_descriptor v1 = device_graph.find_vertex("/dev/sda");
    DeviceGraph::vertex_descriptor v2 = device_graph.find_vertex("/dev/sda1");
    DeviceGraph::vertex_descriptor v3 = device_graph.find_vertex("/dev/sda2");

    clear_vertex(v2, device_graph.graph);
    remove_vertex(v2, device_graph.graph);

    // v1 and v3 still valid and correct due to using boost::listS for VertexList,
    // see http://www.boost.org/doc/libs/1_56_0/libs/graph/doc/adjacency_list.html

    cout << device_graph.graph[v1]->display_name() << endl;
    cout << device_graph.graph[v3]->display_name() << endl;

    cout << endl;
}


int
main()
{
    make_device_graph();

    cout << "num_vertices: " << num_vertices(device_graph.graph) << endl;
    cout << "num_edges: " << num_edges(device_graph.graph) << endl;

    cout << endl;

    test1();
}
