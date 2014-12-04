

#include "storage/DeviceGraph.h"


using namespace storage;


int
main()
{
    DeviceGraph device_graph;

    device_graph.load("test1.info");

    cout << "num_devices: " << device_graph.numDevices() << endl;
    cout << "num_holders: " << device_graph.numHolders() << endl;

    cout << endl;

    device_graph.check();

    device_graph.print_graph();

    device_graph.write_graphviz("load1");
}
