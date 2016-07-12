

#include <iostream>

#include "storage/Utils/HumanString.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"


using namespace std;
using namespace storage;


int
main()
{
    Devicegraph* devicegraph = new Devicegraph();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    gpt->create_partition("/dev/sda1", Region(0, 1000, 262144), PartitionType::PRIMARY);
    Partition* sda2 = gpt->create_partition("/dev/sda2", Region(1000, 1000, 262144), PartitionType::PRIMARY);

    LvmVg* system = LvmVg::create(devicegraph, "system");
    User::create(devicegraph, sda2, system);

    LvmLv* system_root = system->create_lvm_lv("root", 4 * GiB);

    Filesystem* filesystem = system_root->create_filesystem(FsType::EXT4);
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
	    if (is_lvm_lv(device))
		cout << "mount point \"/\" somehow uses a logical volume" << endl;
	}
    }

    delete devicegraph;
}
