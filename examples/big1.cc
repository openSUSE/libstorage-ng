

#include <iostream>
#include <sstream>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


Devicegraph* lhs = nullptr;
Devicegraph* rhs = nullptr;


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

    Partition* partition1 = Partition::create(rhs, name + "p1", PRIMARY);
    Subdevice::create(rhs, gpt, partition1);

    Partition* partition2 = Partition::create(rhs, name + "p2", PRIMARY);
    Subdevice::create(rhs, gpt, partition2);

    Partition* partition3 = Partition::create(rhs, name + "p3", PRIMARY);
    Subdevice::create(rhs, gpt, partition3);
}


int
main()
{
    storage::Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    lhs = storage.create_devicegraph("lhs");

    const int n = 1000;

    for (int i = 0; i < n; ++i)
    {
	ostringstream s;
	s << "/dev/disk" << i;
	add_disk(s.str());
    }

    rhs = storage.copy_devicegraph("lhs", "rhs");

    for (int i = 0; i < n; ++i)
    {
	ostringstream s;
	s << "/dev/disk" << i;
	add_partitions(s.str());
    }

    cout << rhs << endl;

    Actiongraph actiongraph(storage, lhs, rhs);

    actiongraph.print_graph();
}
