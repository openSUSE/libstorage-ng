

#include "storage/Device.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


using namespace storage;


int
main()
{
    DeviceGraph lhs;

    DeviceGraph rhs;

    DeviceGraph::vertex_descriptor sda = rhs.add_vertex(new Disk("/dev/sda"));
    DeviceGraph::vertex_descriptor sda1 = rhs.add_vertex(new Partition("/dev/sda1"));
    DeviceGraph::vertex_descriptor sda2 = rhs.add_vertex(new Partition("/dev/sda2"));

    rhs.add_edge(sda, sda1, new Subdevice());
    rhs.add_edge(sda, sda2, new Subdevice());

    DeviceGraph::vertex_descriptor sda1_fs = rhs.add_vertex(new Ext4());
    dynamic_cast<Filesystem*>(rhs.graph[sda1_fs].get())->mount_points.push_back("/");
    dynamic_cast<Filesystem*>(rhs.graph[sda1_fs].get())->mount_points.push_back("/var/log");
    rhs.add_edge(sda1, sda1_fs, new Using());

    DeviceGraph::vertex_descriptor sda2_fs = rhs.add_vertex(new Ext4());
    dynamic_cast<Filesystem*>(rhs.graph[sda2_fs].get())->mount_points.push_back("/var");
    rhs.add_edge(sda2, sda2_fs, new Using());

    rhs.write_graphviz("compare6-device-rhs");

    ActionGraph action_graph(lhs, rhs);

    action_graph.write_graphviz("compare6-action");
}
