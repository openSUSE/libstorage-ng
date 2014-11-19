

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/DeviceGraph.h"


using namespace storage;


void
partitions()
{
    DeviceGraph device_graph;

    Disk* sda = new Disk(device_graph, "/dev/sda");

    Gpt* gpt = new Gpt(device_graph);
    new Subdevice(device_graph, sda, gpt);

    Partition* sda1 = new Partition(device_graph, "/dev/sda1");
    new Subdevice(device_graph, gpt, sda1);

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
