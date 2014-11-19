

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

    Disk* lhs_sda = new Disk(lhs, "/dev/sda");

    Partition* lhs_sda1 = new Partition(lhs, "/dev/sda1");
    new Subdevice(lhs, lhs_sda, lhs_sda1);

    LvmVg* lhs_system_v1 = new LvmVg(lhs, "/dev/system-v1");
    new Using(lhs, lhs_sda1, lhs_system_v1);

    LvmLv* lhs_system_v1_root = new LvmLv(lhs, "/dev/system-v1/root");
    new Subdevice(lhs, lhs_system_v1, lhs_system_v1_root);

    LvmLv* lhs_system_v1_swap = new LvmLv(lhs, "/dev/system-v1/swap");
    new Subdevice(lhs, lhs_system_v1, lhs_system_v1_swap);

    Ext4* lhs_system_v1_root_fs = new Ext4(lhs);
    new Using(lhs, lhs_system_v1_root, lhs_system_v1_root_fs);

    Swap* lhs_system_v1_swap_fs = new Swap(lhs);
    new Using(lhs, lhs_system_v1_swap, lhs_system_v1_swap_fs);

    DeviceGraph rhs;
    lhs.copy(rhs);

    rhs.remove_vertex(lhs_system_v1_root_fs->getSid());
    rhs.remove_vertex(lhs_system_v1_swap_fs->getSid());
    rhs.remove_vertex(lhs_system_v1_root->getSid());
    rhs.remove_vertex(lhs_system_v1_swap->getSid());
    rhs.remove_vertex(lhs_system_v1->getSid());

    LvmVg* rhs_system_v2 = new LvmVg(rhs, "/dev/system-v2");

    LvmLv* rhs_system_v2_root = new LvmLv(rhs, "/dev/system-v2/root");
    new Subdevice(rhs, rhs_system_v2, rhs_system_v2_root);

    LvmLv* rhs_system_v2_swap = new LvmLv(rhs, "/dev/system-v2/swap");
    new Subdevice(rhs, rhs_system_v2, rhs_system_v2_swap);

    Partition* rhs_sda1 = dynamic_cast<Partition*>(rhs.find_device(lhs_sda1->getSid()));
    new Using(rhs, rhs_sda1, rhs_system_v2);

    Ext4* rhs_system_v2_root_fs = new Ext4(rhs);
    rhs_system_v2_root_fs->setLabel("hello");
    new Using(rhs, rhs_system_v2_root, rhs_system_v2_root_fs);

    Swap* rhs_system_v2_swap_fs = new Swap(rhs);
    new Using(rhs, rhs_system_v2_swap, rhs_system_v2_swap_fs);

    ActionGraph action_graph(lhs, rhs);

    lhs.write_graphviz("compare3-device-lhs");
    rhs.write_graphviz("compare3-device-rhs");
    action_graph.write_graphviz("compare3-action");
}
