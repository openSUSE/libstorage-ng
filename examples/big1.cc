

#include "storage/DeviceGraph.h"
#include "storage/ActionGraph.h"


using namespace storage;


DeviceGraph lhs;
DeviceGraph rhs;


void
add_disk(const string& name)
{
    Disk* disk = new Disk(name);

    Partition* partition1 = new Partition(name + "p1");
    Partition* partition2 = new Partition(name + "p2");
    Partition* partition3 = new Partition(name + "p3");

    DeviceGraph::vertex_descriptor d = rhs.add_vertex(disk);

    DeviceGraph::vertex_descriptor p1 = rhs.add_vertex(partition1);
    DeviceGraph::vertex_descriptor p2 = rhs.add_vertex(partition2);
    DeviceGraph::vertex_descriptor p3 = rhs.add_vertex(partition3);

    rhs.add_edge(d, p1, new Subdevice());
    rhs.add_edge(d, p2, new Subdevice());
    rhs.add_edge(d, p3, new Subdevice());
}


int
main()
{
    for (int i = 0; i < 1000; ++i)
    {
	ostringstream s;
	s << "/dev/disk" << i;
	add_disk(s.str());
    }

    rhs.print_graph();

    ActionGraph action_graph(lhs, rhs);

    action_graph.print_graph();
}
