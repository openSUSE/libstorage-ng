

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Holders/Subdevice.h"
#include "storage/DeviceGraph.h"
#include "storage/ActionGraph.h"


using namespace storage;


DeviceGraph lhs;
DeviceGraph rhs;


void
add_disk(const string& name)
{
    Disk::create(lhs, name);
}


void
add_partitions(const string& name)
{
    Disk* disk = dynamic_cast<Disk*>(BlkDevice::find(rhs, name));

    Gpt* gpt = Gpt::create(rhs);
    Subdevice::create(rhs, disk, gpt);

    Partition* partition1 = Partition::create(rhs, name + "p1");
    Subdevice::create(rhs, gpt, partition1);

    Partition* partition2 = Partition::create(rhs, name + "p2");
    Subdevice::create(rhs, gpt, partition2);

    Partition* partition3 = Partition::create(rhs, name + "p3");
    Subdevice::create(rhs, gpt, partition3);
}


int
main()
{
    const int n = 1000;

    for (int i = 0; i < n; ++i)
    {
	ostringstream s;
	s << "/dev/disk" << i;
	add_disk(s.str());
    }

    lhs.copy(rhs);

    for (int i = 0; i < n; ++i)
    {
	ostringstream s;
	s << "/dev/disk" << i;
	add_partitions(s.str());
    }

    rhs.print_graph();

    ActionGraph action_graph(lhs, rhs);

    action_graph.print_graph();
}
