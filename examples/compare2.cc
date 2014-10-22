

#include "storage/Device.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


using namespace storage;


int
main()
{
    DeviceGraph lhs;

    DeviceGraph rhs;

    DeviceGraph::vertex_descriptor sda1 = lhs.add_vertex(new Partition("/dev/sda1"));
    DeviceGraph::vertex_descriptor sda = lhs.add_vertex(new Disk("/dev/sda"));
    DeviceGraph::vertex_descriptor sda2 = lhs.add_vertex(new Partition("/dev/sda2"));

    lhs.add_edge(sda, sda2, new Subdevice());
    lhs.add_edge(sda, sda1, new Subdevice());

    DeviceGraph::vertex_descriptor system_swap = lhs.add_vertex(new LvmLv("/dev/system/swap"));
    DeviceGraph::vertex_descriptor system = lhs.add_vertex(new LvmVg("/dev/system"));

    lhs.add_edge(system, system_swap, new Subdevice());

    lhs.add_edge(sda1, system, new Using());
    lhs.add_edge(sda2, system, new Using());

    ActionGraph action_graph(lhs, rhs);

    action_graph.write_graphviz("compare2-action");
}
