

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/Ext4.h"
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
    rhs.add_edge(sda, gpt, new Using());

    DeviceGraph::vertex_descriptor sda1 = rhs.add_vertex(new Partition("/dev/sda1"));
    rhs.add_edge(gpt, sda1, new Subdevice());

    DeviceGraph::vertex_descriptor sda2 = rhs.add_vertex(new Partition("/dev/sda2"));
    rhs.add_edge(gpt, sda2, new Subdevice());

    DeviceGraph::vertex_descriptor sda1_fs = rhs.add_vertex(new Ext4());
    dynamic_cast<Filesystem*>(rhs.graph[sda1_fs].get())->addMountPoint("/");
    dynamic_cast<Filesystem*>(rhs.graph[sda1_fs].get())->addMountPoint("/var/log");
    rhs.add_edge(sda1, sda1_fs, new Using());

    DeviceGraph::vertex_descriptor sda2_fs = rhs.add_vertex(new Ext4());
    dynamic_cast<Filesystem*>(rhs.graph[sda2_fs].get())->addMountPoint("/var");
    rhs.add_edge(sda2, sda2_fs, new Using());

    rhs.write_graphviz("compare6-device-rhs");

    ActionGraph action_graph(lhs, rhs);

    action_graph.write_graphviz("compare6-action");
}
