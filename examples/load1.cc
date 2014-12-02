

#include "storage/DeviceGraph.h"


using namespace storage;


int
main()
{
    DeviceGraph device_graph;

    device_graph.load("test1.info");

    cout << "num_vertices: " << device_graph.numVertices() << endl;
    cout << "num_edges: " << device_graph.numEdges() << endl;

    cout << endl;

    device_graph.check();

    device_graph.print_graph();

    device_graph.write_graphviz("load1");
}
