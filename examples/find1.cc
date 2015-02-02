

#include <iostream>

#include "storage/Devices/Disk.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Ext4.h"
#include "storage/Holders/Using.h"
#include "storage/Devicegraph.h"


using namespace std;
using namespace storage;


int
main()
{
    Devicegraph* devicegraph = new Devicegraph();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    gpt->create_partition("/dev/sda1", PRIMARY);
    Partition* sda2 = gpt->create_partition("/dev/sda2", PRIMARY);

    LvmVg* system = LvmVg::create(devicegraph, "/dev/system");
    Using::create(devicegraph, sda2, system);

    LvmLv* system_root = system->create_lvm_lv("/dev/system/root");

    Filesystem* filesystem = system_root->create_filesystem(EXT4);
    filesystem->add_mountpoint("/");

    cout << "num_devices: " << devicegraph->num_devices() << endl;
    cout << "num_holders: " << devicegraph->num_holders() << endl;
    cout << endl;

    devicegraph->check();

    cout << devicegraph << endl;
    devicegraph->write_graphviz("test1.gv");

    for (const Filesystem* filesystem : Filesystem::find_by_mountpoint(devicegraph, "/"))
    {
	for (const Device* device : filesystem->get_ancestors(false))
	{
	    if (dynamic_cast<const LvmLv*>(device))
		cout << "mount point \"/\" somehow uses a logical volume" << endl;
	}
    }

    delete devicegraph;
}
