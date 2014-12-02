

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/DeviceGraph.h"


using namespace storage;


void
partitions()
{
    DeviceGraph device_graph;

    Disk* sda = Disk::create(&device_graph, "/dev/sda");

    Gpt* gpt = Gpt::create(&device_graph);
    Using::create(&device_graph, sda, gpt);

    Partition* sda1 = Partition::create(&device_graph, "/dev/sda1");
    Subdevice::create(&device_graph, gpt, sda1);

    gpt->createPartition("/dev/sda2");

    device_graph.check();
    device_graph.print_graph();

    for (const Partition* partition : gpt->getPartitions())
    {
	cout << partition->getName() << endl;
    }
}


int
main()
{
    partitions();
}
