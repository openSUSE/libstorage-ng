

#include "Device.h"
#include "DeviceGraph.h"
#include "Action.h"


using namespace storage;


int
main()
{
    DeviceGraph lhs;

    DeviceGraph::vertex_descriptor sda = lhs.add_vertex(new Disk("/dev/sda"));
    DeviceGraph::vertex_descriptor sda1 = lhs.add_vertex(new Partition("/dev/sda1"));
    DeviceGraph::vertex_descriptor sda2 = lhs.add_vertex(new Partition("/dev/sda2"));

    lhs.add_edge(sda, sda1, new Subdevice());
    lhs.add_edge(sda, sda2, new Subdevice());

    DeviceGraph::vertex_descriptor system = lhs.add_vertex(new LvmVg("/dev/system"));
    DeviceGraph::vertex_descriptor system_oracle = lhs.add_vertex(new LvmLv("/dev/system/oracle"));

    lhs.add_edge(system, system_oracle, new Subdevice());

    lhs.add_edge(sda2, system, new Using());

    DeviceGraph rhs;
    lhs.copy(rhs);

    LvmLv* d = dynamic_cast<LvmLv*>(rhs.graph[system_oracle].get());
    d->name = "/dev/system/postgresql";

    ActionGraph action_graph(lhs, rhs);

    action_graph.write_graphviz("compare1-action");
}
