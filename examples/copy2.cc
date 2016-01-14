

#include <assert.h>
#include <iostream>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/Partition.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"


using namespace std;
using namespace storage;


int
main()
{
    Devicegraph devicegraph;

    Disk* disk = Disk::create(&devicegraph, "/dev/dasda");

    Partition* partition = Partition::create(&devicegraph, "/dev/dasda2", Region(0, 1000, 262144), PRIMARY);
    Subdevice::create(&devicegraph, disk, partition);

    Devicegraph devicegraph_copy;
    devicegraph.copy(devicegraph_copy);

    devicegraph.check();
    devicegraph_copy.check();

    {
	BlkDevice* tmp = to_blk_device(devicegraph_copy.find_device(partition->get_sid()));

	tmp->get_impl().set_name("/dev/dasda1");
    }

    cout << devicegraph << endl;
    cout << devicegraph_copy << endl;

    {
	Disk* tmp = to_disk(devicegraph.find_device(disk->get_sid()));

	assert(tmp->get_name() == "/dev/dasda");
    }

    {
	Partition* tmp = to_partition(devicegraph.find_device(partition->get_sid()));

	assert(tmp->get_name() == "/dev/dasda2");
    }

    {
	Disk* tmp = to_disk(devicegraph_copy.find_device(disk->get_sid()));

	assert(tmp->get_name() == "/dev/dasda");
    }

    {
	Partition* tmp = to_partition(devicegraph_copy.find_device(partition->get_sid()));

	assert(tmp->get_name() == "/dev/dasda1");
    }
}
