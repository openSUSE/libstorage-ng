

#include "storage/Storage.h"
#include "storage/DeviceGraph.h"


using namespace storage;


int
main()
{
    Environment environment(true);

    Storage storage(environment);

    for (const string& name : storage.getDeviceGraphNames())
	cout << name << endl;
    cout << endl;

    const DeviceGraph* probed = storage.getProbed();

    probed->check();
    probed->print_graph();
    probed->write_graphviz("probe1");

    const DeviceGraph* current = storage.getCurrent();

    current->check();
    current->print_graph();
}
