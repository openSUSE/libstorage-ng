

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"
#include "storage/Devices/Device.h"
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"


using namespace storage_bgl;


Devicegraph devicegraph;


void
children(const Device* device)
{
    cout << "children of " << device->get_displayname() << ":" << endl;

    for (const Device* child : device->get_children())
	cout << "  " << child->get_displayname() << endl;

    cout << endl;
}


void
parents(const Device* device)
{
    cout << "parents of " << device->get_displayname() << ":" << endl;

    for (const Device* parent : device->get_parents())
	cout << "  " << parent->get_displayname() << endl;

    cout << endl;
}


void
siblings(const Device* device)
{
    cout << "siblings of " << device->get_displayname() << ":" << endl;

    for (const Device* sibling : device->get_siblings(false))
	cout << "  " << sibling->get_displayname() << endl;

    cout << endl;
}


void
descendants(const Device* device)
{
    cout << "descendants of " << device->get_displayname() << ":" << endl;

    for (const Device* descendant : device->get_descendants(false))
	cout << "  " << descendant->get_displayname() << endl;

    cout << endl;
}


void
ancestors(const Device* device)
{
    cout << "ancestors of " << device->get_displayname() << ":" << endl;

    for (const Device* ancestor : device->get_ancestors(false))
	cout << "  " << ancestor->get_displayname() << endl;

    cout << endl;
}


void
leafs(const Device* device)
{
    cout << "leafs of " << device->get_displayname() << ":" << endl;

    for (const Device* leaf : device->get_leafs(false))
	cout << "  " << leaf->get_displayname() << endl;

    cout << endl;
}


void
roots(const Device* device)
{
    cout << "roots of " << device->get_displayname() << ":" << endl;

    for (const Device* root : device->get_roots(false))
	cout << "  " << root->get_displayname() << endl;

    cout << endl;
}


int
main()
{
    Disk* sda = Disk::create(&devicegraph, "/dev/sda");

    Partition* sda1 = Partition::create(&devicegraph, "/dev/sda1");
    Subdevice::create(&devicegraph, sda, sda1);

    Partition* sda2 = Partition::create(&devicegraph, "/dev/sda2");
    Subdevice::create(&devicegraph, sda, sda2);

    Disk* sdb = Disk::create(&devicegraph, "/dev/sdb");

    Partition* sdb1 = Partition::create(&devicegraph, "/dev/sdb1");
    Subdevice::create(&devicegraph, sdb, sdb1);

    Partition* sdb2 = Partition::create(&devicegraph, "/dev/sdb2");
    Subdevice::create(&devicegraph, sdb, sdb2);

    LvmVg* system = LvmVg::create(&devicegraph, "/dev/system");
    Using::create(&devicegraph, sda1, system);
    Using::create(&devicegraph, sdb1, system);

    LvmLv* system_root = LvmLv::create(&devicegraph, "/dev/system/root");
    Subdevice::create(&devicegraph, system, system_root);

    LvmLv* system_swap = LvmLv::create(&devicegraph, "/dev/system/swap");
    Subdevice::create(&devicegraph, system, system_swap);

    LvmLv* system_home = LvmLv::create(&devicegraph, "/dev/system/home");
    Subdevice::create(&devicegraph, system, system_home);

    Ext4* system_root_fs = Ext4::create(&devicegraph);
    Subdevice::create(&devicegraph, system_root, system_root_fs);
    system_root_fs->set_label("ROOT");
    system_root_fs->add_mountpoint("/");

    Swap* system_swap_fs = Swap::create(&devicegraph);
    Subdevice::create(&devicegraph, system_swap, system_swap_fs);
    system_swap_fs->set_label("SWAP");
    system_swap_fs->add_mountpoint("swap");

    cout << "num_devices: " << devicegraph.num_devices() << endl;
    cout << "num_holders: " << devicegraph.num_holders() << endl;
    cout << endl;

    devicegraph.check();

    children(sda);

    parents(system);

    siblings(sda1);
    siblings(system_root);

    descendants(sda1);

    ancestors(system_swap);

    leafs(sda1);

    roots(system_swap);

    devicegraph.save("test1.info");

    devicegraph.print_graph();

    devicegraph.write_graphviz("test1");
}
