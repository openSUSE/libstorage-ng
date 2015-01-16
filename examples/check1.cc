

#include <iostream>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"


using namespace std;
using namespace storage;


Devicegraph devicegraph;


void
make_devicegraph()
{
    Disk* sda = Disk::create(&devicegraph, "/dev/sda");

    Partition* sda1 = Partition::create(&devicegraph, "/dev/sda1");
    Subdevice::create(&devicegraph, sda, sda1);

    Partition* sda2 = Partition::create(&devicegraph, "/dev/sda2");
    Subdevice::create(&devicegraph, sda, sda2);

    LvmVg* system = LvmVg::create(&devicegraph, "/dev/system");
    Using::create(&devicegraph, sda1, system);

    LvmLv* system_root = LvmLv::create(&devicegraph, "/dev/system/root");
    Subdevice::create(&devicegraph, system, system_root);

    LvmLv* system_swap = LvmLv::create(&devicegraph, "/dev/system/swap");
    Subdevice::create(&devicegraph, system, system_swap);

    Using::create(&devicegraph, system_swap, sda); // cycle

    Disk::create(&devicegraph, "/dev/sda"); // duplicate blk device name
}


int
main()
{
    make_devicegraph();

    cout << "num_devices: " << devicegraph.num_devices() << endl;
    cout << "num_holders: " << devicegraph.num_holders() << endl;
    cout << endl;

    devicegraph.check();

    devicegraph.write_graphviz("check1.gv");
}
