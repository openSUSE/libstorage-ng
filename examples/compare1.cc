

#include "storage/Devices/Disk.h"
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

    Disk* sda = new Disk(lhs, "/dev/sda");

    Partition* sda1 = new Partition(lhs, "/dev/sda1");
    new Subdevice(lhs, sda, sda1);

    Partition* sda2 = new Partition(lhs, "/dev/sda2");
    new Subdevice(lhs, sda, sda2);

    LvmVg* system = new LvmVg(lhs, "/dev/system");
    new Using(lhs, sda2, system);

    LvmLv* system_oracle = new LvmLv(lhs, "/dev/system/oracle");
    new Subdevice(lhs, system, system_oracle);

    DeviceGraph rhs;
    lhs.copy(rhs);

    LvmLv* d = dynamic_cast<LvmLv*>(rhs.find_device(system_oracle->getSid()));
    assert(d);
    d->setName("/dev/system/postgresql");

    ActionGraph action_graph(lhs, rhs);

    action_graph.write_graphviz("compare1-action");
}
