

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

    new Disk(lhs, "/dev/sda");

    DeviceGraph rhs;
    lhs.copy(rhs);

    Disk* rhs_sda = dynamic_cast<Disk*>(rhs.find_blk_device("/dev/sda"));

    Gpt* rhs_gpt = new Gpt(rhs);
    new Using(rhs, rhs_sda, rhs_gpt);

    Partition* rhs_sda1 = new Partition(rhs, "/dev/sda1");
    new Subdevice(rhs, rhs_gpt, rhs_sda1);

    Partition* rhs_sda2 = new Partition(rhs, "/dev/sda2");
    new Subdevice(rhs, rhs_gpt, rhs_sda2);

    Ext4* rhs_sda1_fs = new Ext4(rhs);
    rhs_sda1_fs->addMountPoint("/");
    rhs_sda1_fs->addMountPoint("/var/log");
    new Using(rhs, rhs_sda1, rhs_sda1_fs);

    Ext4* rhs_sda2_fs = new Ext4(rhs);
    rhs_sda2_fs->addMountPoint("/var");
    new Using(rhs, rhs_sda2, rhs_sda2_fs);

    rhs.write_graphviz("compare6-device-rhs");

    ActionGraph action_graph(lhs, rhs);

    action_graph.write_graphviz("compare6-action");
}
