
#include "storage/Devices/Disk.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Ext4.h"
#include "storage/Holders/Using.h"
#include "storage/DeviceGraph.h"


using namespace storage;


int
main()
{
    DeviceGraph* device_graph = new DeviceGraph();

    Disk* sda = Disk::create(device_graph, "/dev/sda");

    PartitionTable* gpt = sda->createPartitionTable("gpt");

    gpt->createPartition("/dev/sda1");
    Partition* sda2 = gpt->createPartition("/dev/sda2");

    LvmVg* system = LvmVg::create(device_graph, "/dev/system");
    Using::create(device_graph, sda2, system);

    LvmLv* system_root = system->createLvmLv("/dev/system/root");

    Filesystem* filesystem = system_root->createFilesystem(FsType::EXT4);
    filesystem->addMountPoint("/");

    cout << "num_vertices: " << device_graph->numVertices() << endl;
    cout << "num_edges: " << device_graph->numEdges() << endl;
    cout << endl;

    device_graph->check();

    device_graph->print_graph();
    device_graph->write_graphviz("test1");

    for (const Filesystem* filesystem : Filesystem::findByMountPoint(device_graph, "/"))
    {
	for (const Device* device : filesystem->getAncestors(false))
	{
	    if (dynamic_cast<const LvmLv*>(device))
		cout << "mount point \"/\" somehow uses a logical volume" << endl;
	}
    }

    delete device_graph;
}
