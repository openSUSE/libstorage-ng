

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"


using namespace storage_bgl;


int
main()
{
    Devicegraph devicegraph;

    Disk* disk = Disk::create(&devicegraph, "/dev/dasda");

    Partition* partition = Partition::create(&devicegraph, "/dev/dasda2");
    Subdevice::create(&devicegraph, disk, partition);

    Devicegraph devicegraph_copy;
    devicegraph.copy(devicegraph_copy);

    devicegraph.check();
    devicegraph_copy.check();

    {
	BlkDevice* tmp = dynamic_cast<BlkDevice*>(devicegraph_copy.find_device(partition->get_sid()));
	assert(tmp);

	tmp->set_name("/dev/dasda1");
    }

    devicegraph.print_graph();
    devicegraph_copy.print_graph();

    {
	Disk* tmp = dynamic_cast<Disk*>(devicegraph.find_device(disk->get_sid()));
	assert(tmp);

	assert(tmp->get_name() == "/dev/dasda");
    }

    {
	Partition* tmp = dynamic_cast<Partition*>(devicegraph.find_device(partition->get_sid()));
	assert(tmp);

	assert(tmp->get_name() == "/dev/dasda2");
    }

    {
	Disk* tmp = dynamic_cast<Disk*>(devicegraph_copy.find_device(disk->get_sid()));
	assert(tmp);

	assert(tmp->get_name() == "/dev/dasda");
    }

    {
	Partition* tmp = dynamic_cast<Partition*>(devicegraph_copy.find_device(partition->get_sid()));
	assert(partition);

	assert(tmp->get_name() == "/dev/dasda1");
    }
}
