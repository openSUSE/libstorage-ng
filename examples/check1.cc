

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/DeviceGraph.h"


using namespace storage;


DeviceGraph device_graph;


void
make_device_graph()
{
    Disk* sda = new Disk(device_graph, "/dev/sda");

    Partition* sda1 = new Partition(device_graph, "/dev/sda1");
    new Subdevice(device_graph, sda, sda1);

    Partition* sda2 = new Partition(device_graph, "/dev/sda2");
    new Subdevice(device_graph, sda, sda2);

    LvmVg* system = new LvmVg(device_graph, "/dev/system");
    new Using(device_graph, sda1, system);

    LvmLv* system_root = new LvmLv(device_graph, "/dev/system/root");
    new Subdevice(device_graph, system, system_root);

    LvmLv* system_swap = new LvmLv(device_graph, "/dev/system/swap");
    new Subdevice(device_graph, system, system_swap);

    new Using(device_graph, system_swap, sda); // cycle

    new Disk(device_graph, "/dev/sda"); // duplicate blk device name
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
