

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"
#include "storage/Devices/Device.h"
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/DeviceGraph.h"


using namespace storage;


DeviceGraph device_graph;


void
children(const Device* device)
{
    cout << "children of " << device->getDisplayName() << ":" << endl;

    for (const Device* child : device->getChildren())
	cout << "  " << child->getDisplayName() << endl;

    cout << endl;
}


void
parents(const Device* device)
{
    cout << "parents of " << device->getDisplayName() << ":" << endl;

    for (const Device* parent : device->getParents())
	cout << "  " << parent->getDisplayName() << endl;

    cout << endl;
}


void
siblings(const Device* device)
{
    cout << "siblings of " << device->getDisplayName() << ":" << endl;

    for (const Device* sibling : device->getSiblings(false))
	cout << "  " << sibling->getDisplayName() << endl;

    cout << endl;
}


void
descendants(const Device* device)
{
    cout << "descendants of " << device->getDisplayName() << ":" << endl;

    for (const Device* descendant : device->getDescendants(false))
	cout << "  " << descendant->getDisplayName() << endl;

    cout << endl;
}


void
ancestors(const Device* device)
{
    cout << "ancestors of " << device->getDisplayName() << ":" << endl;

    for (const Device* ancestor : device->getAncestors(false))
	cout << "  " << ancestor->getDisplayName() << endl;

    cout << endl;
}


void
leafs(const Device* device)
{
    cout << "leafs of " << device->getDisplayName() << ":" << endl;

    for (const Device* leaf : device->getLeafs(false))
	cout << "  " << leaf->getDisplayName() << endl;

    cout << endl;
}


void
roots(const Device* device)
{
    cout << "roots of " << device->getDisplayName() << ":" << endl;

    for (const Device* root : device->getRoots(false))
	cout << "  " << root->getDisplayName() << endl;

    cout << endl;
}


int
main()
{
    Disk* sda = Disk::create(&device_graph, "/dev/sda");

    Partition* sda1 = Partition::create(&device_graph, "/dev/sda1");
    Subdevice::create(&device_graph, sda, sda1);

    Partition* sda2 = Partition::create(&device_graph, "/dev/sda2");
    Subdevice::create(&device_graph, sda, sda2);

    Disk* sdb = Disk::create(&device_graph, "/dev/sdb");

    Partition* sdb1 = Partition::create(&device_graph, "/dev/sdb1");
    Subdevice::create(&device_graph, sdb, sdb1);

    Partition* sdb2 = Partition::create(&device_graph, "/dev/sdb2");
    Subdevice::create(&device_graph, sdb, sdb2);

    LvmVg* system = LvmVg::create(&device_graph, "/dev/system");
    Using::create(&device_graph, sda1, system);
    Using::create(&device_graph, sdb1, system);

    LvmLv* system_root = LvmLv::create(&device_graph, "/dev/system/root");
    Subdevice::create(&device_graph, system, system_root);

    LvmLv* system_swap = LvmLv::create(&device_graph, "/dev/system/swap");
    Subdevice::create(&device_graph, system, system_swap);

    LvmLv* system_home = LvmLv::create(&device_graph, "/dev/system/home");
    Subdevice::create(&device_graph, system, system_home);

    Ext4* system_root_fs = Ext4::create(&device_graph);
    Subdevice::create(&device_graph, system_root, system_root_fs);
    system_root_fs->setLabel("ROOT");
    system_root_fs->addMountPoint("/");

    Swap* system_swap_fs = Swap::create(&device_graph);
    Subdevice::create(&device_graph, system_swap, system_swap_fs);
    system_swap_fs->setLabel("SWAP");
    system_swap_fs->addMountPoint("swap");

    cout << "num_devices: " << device_graph.numDevices() << endl;
    cout << "num_holders: " << device_graph.numHolders() << endl;
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

    device_graph.save("test1.info");

    device_graph.print_graph();

    device_graph.write_graphviz("test1");
}
