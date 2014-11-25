

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"
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

    Disk* rhs_sda = dynamic_cast<Disk*>(BlkDevice::find(rhs, "/dev/sda"));

    Gpt* rhs_gpt = new Gpt(rhs);
    new Subdevice(rhs, rhs_sda, rhs_gpt);

    Partition* rhs_sda1 = new Partition(rhs, "/dev/sda1");
    new Subdevice(rhs, rhs_gpt, rhs_sda1);

    Partition* rhs_sda2 = new Partition(rhs, "/dev/sda2");
    new Subdevice(rhs, rhs_gpt, rhs_sda2);

    Encryption* rhs_cr_sda1 = new Encryption(rhs, "/dev/mapper/cr_sda1");
    new Using(rhs, rhs_sda1, rhs_cr_sda1);

    Ext4* rhs_ext4 = new Ext4(rhs);
    new Using(rhs, rhs_cr_sda1, rhs_ext4);

    Swap* rhs_swap = new Swap(rhs);
    new Using(rhs, rhs_sda2, rhs_swap);

    rhs.write_graphviz("compare5-device-rhs");

    ActionGraph action_graph(lhs, rhs);

    action_graph.write_graphviz("compare5-action");
}
