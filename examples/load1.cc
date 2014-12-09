

#include "storage/Devicegraph.h"


using namespace storage_bgl;


int
main()
{
    Devicegraph devicegraph;

    devicegraph.load("test1.info");

    cout << "num_devices: " << devicegraph.num_devices() << endl;
    cout << "num_holders: " << devicegraph.num_holders() << endl;

    cout << endl;

    devicegraph.check();

    devicegraph.print_graph();

    devicegraph.write_graphviz("load1");
}
