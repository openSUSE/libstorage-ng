

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Holders/Using.h"
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

    Disk::create(lhs, "/dev/sda");

    Devicegraph* rhs = storage.copy_devicegraph("lhs", "rhs");

    Disk* rhs_sda = dynamic_cast<Disk*>(BlkDevice::find(rhs, "/dev/sda"));

    Gpt* rhs_gpt = Gpt::create(rhs);
    Subdevice::create(rhs, rhs_sda, rhs_gpt);

    Partition* rhs_sda1 = Partition::create(rhs, "/dev/sda1", PRIMARY);
    Subdevice::create(rhs, rhs_gpt, rhs_sda1);

    Partition* rhs_sda2 = Partition::create(rhs, "/dev/sda2", PRIMARY);
    Subdevice::create(rhs, rhs_gpt, rhs_sda2);

    Partition* rhs_sda3 = Partition::create(rhs, "/dev/sda3", PRIMARY);
    Subdevice::create(rhs, rhs_gpt, rhs_sda3);

    LvmVg* rhs_system = LvmVg::create(rhs, "/dev/system");
    Using::create(rhs, rhs_sda1, rhs_system);
    Using::create(rhs, rhs_sda2, rhs_system);
    Using::create(rhs, rhs_sda3, rhs_system);

    rhs->write_graphviz("compare4-device-rhs.gv");

    Actiongraph actiongraph(storage, lhs, rhs);

    actiongraph.write_graphviz("compare4-action.gv");
}
