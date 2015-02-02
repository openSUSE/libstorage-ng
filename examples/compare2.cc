

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
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

    Disk* lhs_sda = Disk::create(lhs, "/dev/sda");

    Devicegraph* rhs = storage.copy_devicegraph("lhs", "rhs");

    Disk* rhs_sda = dynamic_cast<Disk*>(rhs->find_device(lhs_sda->get_sid()));

    Gpt* rhs_gpt = Gpt::create(rhs);
    Using::create(rhs, rhs_sda, rhs_gpt);

    Partition* rhs_sda1 = Partition::create(rhs, "/dev/sda1", PRIMARY, Region(0, 100));
    Subdevice::create(rhs, rhs_gpt, rhs_sda1);

    Partition* rhs_sda2 = Partition::create(rhs, "/dev/sda2", PRIMARY, Region(100, 100));
    Subdevice::create(rhs, rhs_gpt, rhs_sda2);

    LvmVg* rhs_system = LvmVg::create(rhs, "/dev/system");
    Using::create(rhs, rhs_sda1, rhs_system);
    Using::create(rhs, rhs_sda2, rhs_system);

    LvmLv* rhs_system_swap = LvmLv::create(rhs, "/dev/system/swap");
    Subdevice::create(rhs, rhs_system, rhs_system_swap);

    Actiongraph actiongraph(storage, lhs, rhs);

    actiongraph.write_graphviz("compare2-action.gv");
}
