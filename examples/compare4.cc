

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
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

    DeviceGraph::vertex_descriptor sda3 = rhs.add_vertex(new Partition("/dev/sda3"));
    rhs.add_edge(gpt, sda3, new Subdevice());

    DeviceGraph::vertex_descriptor system = rhs.add_vertex(new LvmVg("/dev/system"));
    rhs.add_edge(sda1, system, new Subdevice());
    rhs.add_edge(sda2, system, new Subdevice());
    rhs.add_edge(sda3, system, new Subdevice());

    rhs.write_graphviz("compare4-device-rhs");

    ActionGraph action_graph(lhs, rhs);

    action_graph.write_graphviz("compare4-action");
}
