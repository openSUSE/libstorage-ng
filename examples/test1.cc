

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Device.h"
#include "storage/DeviceGraph.h"


using namespace storage;


DeviceGraph device_graph;


void
children(const Device* device)
{
    cout << "children of " << device->display_name() << ":" << endl;

    for (const Device* child : device->getChildren())
	cout << "  " << child->display_name() << endl;

    cout << endl;
}


void
parents(const Device* device)
{
    cout << "parents of " << device->display_name() << ":" << endl;

    for (const Device* parent : device->getParents())
	cout << "  " << parent->display_name() << endl;

    cout << endl;
}


void
siblings(const Device* device)
{
    cout << "siblings of " << device->display_name() << ":" << endl;

    for (const Device* parent : device->getSiblings(false))
	cout << "  " << parent->display_name() << endl;

    cout << endl;
}


void
descendants(const Device* device)
{
    cout << "descendants of " << device->display_name() << ":" << endl;

    for (const Device* parent : device->getDescendants(false))
	cout << "  " << parent->display_name() << endl;

    cout << endl;
}


void
ancestors(const Device* device)
{
    cout << "ancestors of " << device->display_name() << ":" << endl;

    for (const Device* parent : device->getAncestors(false))
	cout << "  " << parent->display_name() << endl;

    cout << endl;
}


void
leafs(const Device* device)
{
    cout << "leafs of " << device->display_name() << ":" << endl;

    for (const Device* parent : device->getLeafs(false))
	cout << "  " << parent->display_name() << endl;

    cout << endl;
}


void
roots(const Device* device)
{
    cout << "roots of " << device->display_name() << ":" << endl;

    for (const Device* parent : device->getRoots(false))
	cout << "  " << parent->display_name() << endl;

    cout << endl;
}


int
main()
{
    Disk* sda = Disk::create(device_graph, "/dev/sda");

    Partition* sda1 = Partition::create(device_graph, "/dev/sda1");
    Subdevice::create(device_graph, sda, sda1);

    Partition* sda2 = Partition::create(device_graph, "/dev/sda2");
    Subdevice::create(device_graph, sda, sda2);

    Disk* sdb = Disk::create(device_graph, "/dev/sdb");

    Partition* sdb1 = Partition::create(device_graph, "/dev/sdb1");
    Subdevice::create(device_graph, sdb, sdb1);

    Partition* sdb2 = Partition::create(device_graph, "/dev/sdb2");
    Subdevice::create(device_graph, sdb, sdb2);

    LvmVg* system = LvmVg::create(device_graph, "/dev/system");
    Using::create(device_graph, sda1, system);
    Using::create(device_graph, sdb1, system);

    LvmLv* system_root = LvmLv::create(device_graph, "/dev/system/root");
    Subdevice::create(device_graph, system, system_root);

    LvmLv* system_swap = LvmLv::create(device_graph, "/dev/system/swap");
    Subdevice::create(device_graph, system, system_swap);

    LvmLv* system_home = LvmLv::create(device_graph, "/dev/system/home");
    Subdevice::create(device_graph, system, system_home);

    cout << "num_vertices: " << device_graph.numVertices() << endl;
    cout << "num_edges: " << device_graph.numEdges() << endl;

    cout << endl;

    device_graph.check();

    children(sda);

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
