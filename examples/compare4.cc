

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


using namespace storage;


int
main()
{
    DeviceGraph lhs;

    Disk::create(lhs, "/dev/sda");

    DeviceGraph rhs;
    lhs.copy(rhs);

    Disk* rhs_sda = dynamic_cast<Disk*>(BlkDevice::find(rhs, "/dev/sda"));

    Gpt* rhs_gpt = Gpt::create(rhs);
    Subdevice::create(rhs, rhs_sda, rhs_gpt);

    Partition* rhs_sda1 = Partition::create(rhs, "/dev/sda1");
    Subdevice::create(rhs, rhs_gpt, rhs_sda1);

    Partition* rhs_sda2 = Partition::create(rhs, "/dev/sda2");
    Subdevice::create(rhs, rhs_gpt, rhs_sda2);

    Partition* rhs_sda3 = Partition::create(rhs, "/dev/sda3");
    Subdevice::create(rhs, rhs_gpt, rhs_sda3);

    LvmVg* rhs_system = LvmVg::create(rhs, "/dev/system");
    Subdevice::create(rhs, rhs_sda1, rhs_system);
    Subdevice::create(rhs, rhs_sda2, rhs_system);
    Subdevice::create(rhs, rhs_sda3, rhs_system);

    rhs.write_graphviz("compare4-device-rhs");

    ActionGraph action_graph(lhs, rhs);

    action_graph.write_graphviz("compare4-action");
}
