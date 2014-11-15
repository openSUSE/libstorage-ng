

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/DeviceGraph.h"
#include "storage/ActionGraph.h"


using namespace storage;


DeviceGraph lhs;
DeviceGraph rhs;


void
add_disk(const string& name)
{
    Disk* disk = new Disk(name);

    lhs.add_vertex(disk);
}


void
add_partitions(const string& name)
{
    DeviceGraph::vertex_descriptor d = rhs.find_vertex(name);

    Gpt* gpt = new Gpt();

    Partition* partition1 = new Partition(name + "p1");
    Partition* partition2 = new Partition(name + "p2");
    Partition* partition3 = new Partition(name + "p3");

    DeviceGraph::vertex_descriptor pt = rhs.add_vertex(gpt);

    DeviceGraph::vertex_descriptor p1 = rhs.add_vertex(partition1);
    DeviceGraph::vertex_descriptor p2 = rhs.add_vertex(partition2);
    DeviceGraph::vertex_descriptor p3 = rhs.add_vertex(partition3);

    rhs.add_edge(d, pt, new Subdevice());

    rhs.add_edge(pt, p1, new Subdevice());
    rhs.add_edge(pt, p2, new Subdevice());
    rhs.add_edge(pt, p3, new Subdevice());
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
