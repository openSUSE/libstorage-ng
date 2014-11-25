

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
    Disk* sda = Disk::create(device_graph, "/dev/sda");

    Partition* sda1 = Partition::create(device_graph, "/dev/sda1");
    Subdevice::create(device_graph, sda, sda1);

    Partition* sda2 = Partition::create(device_graph, "/dev/sda2");
    Subdevice::create(device_graph, sda, sda2);

    LvmVg* system = LvmVg::create(device_graph, "/dev/system");
    Using::create(device_graph, sda1, system);

    LvmLv* system_root = LvmLv::create(device_graph, "/dev/system/root");
    Subdevice::create(device_graph, system, system_root);

    LvmLv* system_swap = LvmLv::create(device_graph, "/dev/system/swap");
    Subdevice::create(device_graph, system, system_swap);

    Using::create(device_graph, system_swap, sda); // cycle

    Disk::create(device_graph, "/dev/sda"); // duplicate blk device name
}


int
main()
{
    make_device_graph();

    cout << "num_vertices: " << device_graph.num_vertices() << endl;
    cout << "num_edges: " << device_graph.num_edges() << endl;

    cout << endl;

    device_graph.check();

    device_graph.write_graphviz("check1");
}
