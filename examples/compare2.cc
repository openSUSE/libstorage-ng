

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


using namespace storage;


int
main()
{
    DeviceGraph lhs;

    Disk* lhs_sda = new Disk(lhs, "/dev/sda");

    DeviceGraph rhs;
    lhs.copy(rhs);

    Disk* rhs_sda = dynamic_cast<Disk*>(rhs.find_device(lhs_sda->getSid()));

    Gpt* rhs_gpt = new Gpt(rhs);
    new Using(rhs, rhs_sda, rhs_gpt);

    Partition* rhs_sda1 = new Partition(rhs, "/dev/sda1");
    new Subdevice(rhs, rhs_gpt, rhs_sda1);

    Partition* rhs_sda2 = new Partition(rhs, "/dev/sda2");
    new Subdevice(rhs, rhs_gpt, rhs_sda2);

    LvmVg* rhs_system = new LvmVg(rhs, "/dev/system");
    new Using(rhs, rhs_sda1, rhs_system);
    new Using(rhs, rhs_sda2, rhs_system);

    LvmLv* rhs_system_swap = new LvmLv(rhs, "/dev/system/swap");
    new Subdevice(rhs, rhs_system, rhs_system_swap);

    ActionGraph action_graph(lhs, rhs);

    action_graph.write_graphviz("compare2-action");
}
