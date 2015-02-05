

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"
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

    Encryption* rhs_cr_sda1 = Encryption::create(rhs, "/dev/mapper/cr_sda1");
    Using::create(rhs, rhs_sda1, rhs_cr_sda1);

    Ext4* rhs_ext4 = Ext4::create(rhs);
    Using::create(rhs, rhs_cr_sda1, rhs_ext4);

    Swap* rhs_swap = Swap::create(rhs);
    Using::create(rhs, rhs_sda2, rhs_swap);

    rhs->write_graphviz("compare5-device-rhs.gv");

    Actiongraph actiongraph(storage, lhs, rhs);

    actiongraph.write_graphviz("compare5-action.gv");
}
