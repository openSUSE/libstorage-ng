

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"


using namespace storage;


int
main()
{
    Devicegraph lhs;

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

    Devicegraph rhs;
    lhs.copy(rhs);

    rhs.remove_vertex(lhs_system_v1_root_fs->get_sid());
    rhs.remove_vertex(lhs_system_v1_swap_fs->get_sid());
    rhs.remove_vertex(lhs_system_v1_root->get_sid());
    rhs.remove_vertex(lhs_system_v1_swap->get_sid());
    rhs.remove_vertex(lhs_system_v1->get_sid());

    LvmVg* rhs_system_v2 = LvmVg::create(&rhs, "/dev/system-v2");

    LvmLv* rhs_system_v2_root = LvmLv::create(&rhs, "/dev/system-v2/root");
    Subdevice::create(&rhs, rhs_system_v2, rhs_system_v2_root);

    LvmLv* rhs_system_v2_swap = LvmLv::create(&rhs, "/dev/system-v2/swap");
    Subdevice::create(&rhs, rhs_system_v2, rhs_system_v2_swap);

    Partition* rhs_sda1 = dynamic_cast<Partition*>(rhs.find_device(lhs_sda1->get_sid()));
    Using::create(&rhs, rhs_sda1, rhs_system_v2);

    Ext4* rhs_system_v2_root_fs = Ext4::create(&rhs);
    rhs_system_v2_root_fs->set_label("hello");
    Using::create(&rhs, rhs_system_v2_root, rhs_system_v2_root_fs);

    Swap* rhs_system_v2_swap_fs = Swap::create(&rhs);
    Using::create(&rhs, rhs_system_v2_swap, rhs_system_v2_swap_fs);

    Actiongraph actiongraph(&lhs, &rhs);

    lhs.write_graphviz("compare3-device-lhs");
    rhs.write_graphviz("compare3-device-rhs");
    actiongraph.write_graphviz("compare3-action");
}
