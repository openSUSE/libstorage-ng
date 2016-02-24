

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"
#include "storage/Holders/User.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace storage;


int
main()
{
    storage::Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* lhs = storage.create_devicegraph("lhs");

    Disk* lhs_sda = Disk::create(lhs, "/dev/sda");

    Partition* lhs_sda1 = Partition::create(lhs, "/dev/sda1", Region(0, 1000, 262144), PartitionType::PRIMARY);
    Subdevice::create(lhs, lhs_sda, lhs_sda1);

    LvmVg* lhs_system_v1 = LvmVg::create(lhs, "/dev/system-v1");
    User::create(lhs, lhs_sda1, lhs_system_v1);

    LvmLv* lhs_system_v1_root = LvmLv::create(lhs, "/dev/system-v1/root");
    Subdevice::create(lhs, lhs_system_v1, lhs_system_v1_root);

    LvmLv* lhs_system_v1_swap = LvmLv::create(lhs, "/dev/system-v1/swap");
    Subdevice::create(lhs, lhs_system_v1, lhs_system_v1_swap);

    Ext4* lhs_system_v1_root_fs = Ext4::create(lhs);
    User::create(lhs, lhs_system_v1_root, lhs_system_v1_root_fs);

    Swap* lhs_system_v1_swap_fs = Swap::create(lhs);
    User::create(lhs, lhs_system_v1_swap, lhs_system_v1_swap_fs);

    Devicegraph* rhs = storage.copy_devicegraph("lhs", "rhs");

    rhs->remove_device(lhs_system_v1_root_fs->get_sid());
    rhs->remove_device(lhs_system_v1_swap_fs->get_sid());
    rhs->remove_device(lhs_system_v1_root->get_sid());
    rhs->remove_device(lhs_system_v1_swap->get_sid());
    rhs->remove_device(lhs_system_v1->get_sid());

    LvmVg* rhs_system_v2 = LvmVg::create(rhs, "/dev/system-v2");

    LvmLv* rhs_system_v2_root = LvmLv::create(rhs, "/dev/system-v2/root");
    Subdevice::create(rhs, rhs_system_v2, rhs_system_v2_root);

    LvmLv* rhs_system_v2_swap = LvmLv::create(rhs, "/dev/system-v2/swap");
    Subdevice::create(rhs, rhs_system_v2, rhs_system_v2_swap);

    Partition* rhs_sda1 = to_partition(rhs->find_device(lhs_sda1->get_sid()));
    User::create(rhs, rhs_sda1, rhs_system_v2);

    Ext4* rhs_system_v2_root_fs = Ext4::create(rhs);
    rhs_system_v2_root_fs->set_label("hello");
    User::create(rhs, rhs_system_v2_root, rhs_system_v2_root_fs);

    Swap* rhs_system_v2_swap_fs = Swap::create(rhs);
    User::create(rhs, rhs_system_v2_swap, rhs_system_v2_swap_fs);

    Actiongraph actiongraph(storage, lhs, rhs);

    lhs->write_graphviz("compare3-device-lhs.gv");
    rhs->write_graphviz("compare3-device-rhs.gv");
    actiongraph.write_graphviz("compare3-action.gv", true);
}
