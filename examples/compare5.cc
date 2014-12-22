

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


using namespace storage;


int
main()
{
    Devicegraph* lhs = new Devicegraph();

    Disk::create(lhs, "/dev/sda");

    Devicegraph* rhs = new Devicegraph();
    lhs->copy(*rhs);

    Disk* rhs_sda = dynamic_cast<Disk*>(BlkDevice::find(rhs, "/dev/sda"));

    Gpt* rhs_gpt = Gpt::create(rhs);
    Subdevice::create(rhs, rhs_sda, rhs_gpt);

    Partition* rhs_sda1 = Partition::create(rhs, "/dev/sda1");
    Subdevice::create(rhs, rhs_gpt, rhs_sda1);

    Partition* rhs_sda2 = Partition::create(rhs, "/dev/sda2");
    Subdevice::create(rhs, rhs_gpt, rhs_sda2);

    Encryption* rhs_cr_sda1 = Encryption::create(rhs, "/dev/mapper/cr_sda1");
    Using::create(rhs, rhs_sda1, rhs_cr_sda1);

    Ext4* rhs_ext4 = Ext4::create(rhs);
    Using::create(rhs, rhs_cr_sda1, rhs_ext4);

    Swap* rhs_swap = Swap::create(rhs);
    Using::create(rhs, rhs_sda2, rhs_swap);

    rhs->write_graphviz("compare5-device-rhs");

    Actiongraph actiongraph(lhs, rhs);

    actiongraph.write_graphviz("compare5-action");

    delete lhs;
    delete rhs;
}
