

#include "storage/Device.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


using namespace storage;


int
main()
{
    DeviceGraph lhs;
    lhs.add_vertex(new Disk("/dev/sda"));

    DeviceGraph rhs;
    lhs.copy(rhs);

    DeviceGraph::vertex_descriptor sda = rhs.find_vertex("/dev/sda");

    DeviceGraph::vertex_descriptor gpt = rhs.add_vertex(new Gpt());
    rhs.add_edge(sda, gpt, new Subdevice());

    DeviceGraph::vertex_descriptor sda1 = rhs.add_vertex(new Partition("/dev/sda1"));
    rhs.add_edge(gpt, sda1, new Subdevice());

    DeviceGraph::vertex_descriptor sda2 = rhs.add_vertex(new Partition("/dev/sda2"));
    rhs.add_edge(gpt, sda2, new Subdevice());

    DeviceGraph::vertex_descriptor cr_sda1 = rhs.add_vertex(new Encryption("/dev/mapper/cr_sda1"));
    rhs.add_edge(sda1, cr_sda1, new Using());

    DeviceGraph::vertex_descriptor ext4 = rhs.add_vertex(new Ext4());
    rhs.add_edge(cr_sda1, ext4, new Using());

    DeviceGraph::vertex_descriptor swap = rhs.add_vertex(new Swap());
    rhs.add_edge(sda2, swap, new Using());

    rhs.write_graphviz("compare5-device-rhs");

    ActionGraph action_graph(lhs, rhs);

    action_graph.write_graphviz("compare5-action");
}
