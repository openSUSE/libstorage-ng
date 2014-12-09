

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"


using namespace storage;


int
main()
{
    Devicegraph devicegraph;

    Disk* sda = Disk::create(&devicegraph, "/dev/sda");

    Partition* sda1 = Partition::create(&devicegraph, "/dev/sda1");
    Subdevice::create(&devicegraph, sda, sda1);

    Partition* sda2 = Partition::create(&devicegraph, "/dev/sda2");
    Subdevice::create(&devicegraph, sda, sda2);

    Devicegraph devicegraph_copy;
    devicegraph.copy(devicegraph_copy);

    devicegraph_copy.remove_vertex(sda2);

    devicegraph.print_graph();
    devicegraph_copy.print_graph();
}
