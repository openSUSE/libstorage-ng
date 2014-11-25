

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

    Disk* sda = Disk::create(lhs, "/dev/sda");

    Partition* sda1 = Partition::create(lhs, "/dev/sda1");
    Subdevice::create(lhs, sda, sda1);

    Partition* sda2 = Partition::create(lhs, "/dev/sda2");
    Subdevice::create(lhs, sda, sda2);

    LvmVg* system = LvmVg::create(lhs, "/dev/system");
    Using::create(lhs, sda2, system);

    LvmLv* system_oracle = LvmLv::create(lhs, "/dev/system/oracle");
    Subdevice::create(lhs, system, system_oracle);

    DeviceGraph rhs;
    lhs.copy(rhs);

    LvmLv* d = dynamic_cast<LvmLv*>(rhs.find_device(system_oracle->getSid()));
    assert(d);
    d->setName("/dev/system/postgresql");

    ActionGraph action_graph(lhs, rhs);

    action_graph.write_graphviz("compare1-action");
}
