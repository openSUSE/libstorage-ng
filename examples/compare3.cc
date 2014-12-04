

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/DeviceGraph.h"
#include "storage/ActionGraph.h"


using namespace storage;


int
main()
{
    DeviceGraph lhs;

    Disk* lhs_sda = Disk::create(&lhs, "/dev/sda");

    Partition* lhs_sda1 = Partition::create(&lhs, "/dev/sda1");
    Subdevice::create(&lhs, lhs_sda, lhs_sda1);

    LvmVg* lhs_system_v1 = LvmVg::create(&lhs, "/dev/system-v1");
    Using::create(&lhs, lhs_sda1, lhs_system_v1);

    LvmLv* lhs_system_v1_root = LvmLv::create(&lhs, "/dev/system-v1/root");
    Subdevice::create(&lhs, lhs_system_v1, lhs_system_v1_root);

    LvmLv* lhs_system_v1_swap = LvmLv::create(&lhs, "/dev/system-v1/swap");
    Subdevice::create(&lhs, lhs_system_v1, lhs_system_v1_swap);

    Ext4* lhs_system_v1_root_fs = Ext4::create(&lhs);
    Using::create(&lhs, lhs_system_v1_root, lhs_system_v1_root_fs);

    Swap* lhs_system_v1_swap_fs = Swap::create(&lhs);
    Using::create(&lhs, lhs_system_v1_swap, lhs_system_v1_swap_fs);

    DeviceGraph rhs;
    lhs.copy(rhs);

    rhs.remove_vertex(lhs_system_v1_root_fs->getSid());
    rhs.remove_vertex(lhs_system_v1_swap_fs->getSid());
    rhs.remove_vertex(lhs_system_v1_root->getSid());
    rhs.remove_vertex(lhs_system_v1_swap->getSid());
    rhs.remove_vertex(lhs_system_v1->getSid());

    LvmVg* rhs_system_v2 = LvmVg::create(&rhs, "/dev/system-v2");

    LvmLv* rhs_system_v2_root = LvmLv::create(&rhs, "/dev/system-v2/root");
    Subdevice::create(&rhs, rhs_system_v2, rhs_system_v2_root);

    LvmLv* rhs_system_v2_swap = LvmLv::create(&rhs, "/dev/system-v2/swap");
    Subdevice::create(&rhs, rhs_system_v2, rhs_system_v2_swap);

    Partition* rhs_sda1 = dynamic_cast<Partition*>(rhs.findDevice(lhs_sda1->getSid()));
    Using::create(&rhs, rhs_sda1, rhs_system_v2);

    Ext4* rhs_system_v2_root_fs = Ext4::create(&rhs);
    rhs_system_v2_root_fs->setLabel("hello");
    Using::create(&rhs, rhs_system_v2_root, rhs_system_v2_root_fs);

    Swap* rhs_system_v2_swap_fs = Swap::create(&rhs);
    Using::create(&rhs, rhs_system_v2_swap, rhs_system_v2_swap_fs);

    ActionGraph action_graph(lhs, rhs);

    lhs.write_graphviz("compare3-device-lhs");
    rhs.write_graphviz("compare3-device-rhs");
    action_graph.write_graphviz("compare3-action");
}
