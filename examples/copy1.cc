

#include <iostream>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"


using namespace std;
using namespace storage;


int
main()
{
    Devicegraph devicegraph;

    Disk* sda = Disk::create(&devicegraph, "/dev/sda");

    Partition* sda1 = Partition::create(&devicegraph, "/dev/sda1", Region(0, 1000, 262144), PartitionType::PRIMARY);
    Subdevice::create(&devicegraph, sda, sda1);

    Partition* sda2 = Partition::create(&devicegraph, "/dev/sda2", Region(1000, 1000, 262144), PartitionType::PRIMARY);
    Subdevice::create(&devicegraph, sda, sda2);

    Devicegraph devicegraph_copy;
    devicegraph.copy(devicegraph_copy);

    devicegraph_copy.remove_device(sda2);

    cout << devicegraph << endl;
    cout << devicegraph_copy << endl;
}
