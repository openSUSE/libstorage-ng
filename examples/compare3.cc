

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


using namespace storage;


int
main()
{
    DeviceGraph lhs;

    DeviceGraph::vertex_descriptor sda = lhs.add_vertex(new Disk("/dev/sda"));

    DeviceGraph::vertex_descriptor sda1 = lhs.add_vertex(new Partition("/dev/sda1"));
    lhs.add_edge(sda, sda1, new Subdevice());

    DeviceGraph::vertex_descriptor system_v1 = lhs.add_vertex(new LvmVg("/dev/system-v1"));
    lhs.add_edge(sda1, system_v1, new Using());

    DeviceGraph::vertex_descriptor system_v1_root = lhs.add_vertex(new LvmLv("/dev/system-v1/root"));
    lhs.add_edge(system_v1, system_v1_root, new Subdevice());

    DeviceGraph::vertex_descriptor system_v1_swap = lhs.add_vertex(new LvmLv("/dev/system-v1/swap"));
    lhs.add_edge(system_v1, system_v1_swap, new Subdevice());

    DeviceGraph::vertex_descriptor system_v1_root_fs = lhs.add_vertex(new Ext4());
    lhs.add_edge(system_v1_root, system_v1_root_fs, new Using());

    DeviceGraph::vertex_descriptor system_v1_swap_fs = lhs.add_vertex(new Swap());
    lhs.add_edge(system_v1_swap, system_v1_swap_fs, new Using());

    DeviceGraph rhs;
    lhs.copy(rhs);

    DeviceGraph::vertex_descriptor system_v1_rhs = rhs.find_vertex(lhs.graph[system_v1]->getSid());
    DeviceGraph::vertex_descriptor system_v1_root_rhs = rhs.find_vertex(lhs.graph[system_v1_root]->getSid());
    DeviceGraph::vertex_descriptor system_v1_swap_rhs = rhs.find_vertex(lhs.graph[system_v1_swap]->getSid());
    DeviceGraph::vertex_descriptor system_v1_root_fs_rhs = rhs.find_vertex(lhs.graph[system_v1_root_fs]->getSid());
    DeviceGraph::vertex_descriptor system_v1_swap_fs_rhs = rhs.find_vertex(lhs.graph[system_v1_swap_fs]->getSid());

    rhs.remove_vertex(system_v1_root_fs_rhs);
    rhs.remove_vertex(system_v1_swap_fs_rhs);
    rhs.remove_vertex(system_v1_root_rhs);
    rhs.remove_vertex(system_v1_swap_rhs);
    rhs.remove_vertex(system_v1_rhs);

    DeviceGraph::vertex_descriptor system_v2 = rhs.add_vertex(new LvmVg("/dev/system-v2"));

    DeviceGraph::vertex_descriptor system_v2_root = rhs.add_vertex(new LvmLv("/dev/system-v2/root"));
    rhs.add_edge(system_v2, system_v2_root, new Subdevice());

    DeviceGraph::vertex_descriptor system_v2_swap = rhs.add_vertex(new LvmLv("/dev/system-v2/swap"));
    rhs.add_edge(system_v2, system_v2_swap, new Subdevice());

    DeviceGraph::vertex_descriptor sda1_rhs = rhs.find_vertex(lhs.graph[sda1]->getSid());
    rhs.add_edge(sda1_rhs, system_v2, new Using());

    Ext4* ext4 = new Ext4();
    ext4->setLabel("hello");
    DeviceGraph::vertex_descriptor system_v2_root_fs = rhs.add_vertex(ext4);
    rhs.add_edge(system_v2_root, system_v2_root_fs, new Using());

    DeviceGraph::vertex_descriptor system_v2_swap_fs = rhs.add_vertex(new Swap());
    rhs.add_edge(system_v2_swap, system_v2_swap_fs, new Using());

    ActionGraph action_graph(lhs, rhs);

    lhs.write_graphviz("compare3-device-lhs");
    rhs.write_graphviz("compare3-device-rhs");
    action_graph.write_graphviz("compare3-action");
}
