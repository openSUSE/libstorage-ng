

#include <assert.h>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"


using namespace storage;


int
main()
{
    storage::Environment environment(true, ProbeMode::PROBE_NONE, TargetMode::TARGET_NORMAL);

    Storage storage(environment);

    Devicegraph* lhs = storage.create_devicegraph("lhs");

    Disk* sda = Disk::create(lhs, "/dev/sda");

    Partition* sda1 = Partition::create(lhs, "/dev/sda1", PRIMARY, Region(0, 100));
    Subdevice::create(lhs, sda, sda1);

    Partition* sda2 = Partition::create(lhs, "/dev/sda2", PRIMARY, Region(100, 100));
    Subdevice::create(lhs, sda, sda2);

    LvmVg* system = LvmVg::create(lhs, "/dev/system");
    Using::create(lhs, sda2, system);

    LvmLv* system_oracle = LvmLv::create(lhs, "/dev/system/oracle");
    Subdevice::create(lhs, system, system_oracle);

    Devicegraph* rhs = storage.copy_devicegraph("lhs", "rhs");

    LvmLv* d = dynamic_cast<LvmLv*>(rhs->find_device(system_oracle->get_sid()));
    assert(d);
    d->set_name("/dev/system/postgresql");

    Actiongraph actiongraph(storage, lhs, rhs);

    actiongraph.write_graphviz("compare1-action.gv");
}
