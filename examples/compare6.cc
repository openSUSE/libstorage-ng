

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/Ext4.h"
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
    Using::create(rhs, rhs_sda, rhs_gpt);

    Partition* rhs_sda1 = Partition::create(rhs, "/dev/sda1");
    Subdevice::create(rhs, rhs_gpt, rhs_sda1);

    Partition* rhs_sda2 = Partition::create(rhs, "/dev/sda2");
    Subdevice::create(rhs, rhs_gpt, rhs_sda2);

    Ext4* rhs_sda1_fs = Ext4::create(rhs);
    rhs_sda1_fs->add_mountpoint("/");
    rhs_sda1_fs->add_mountpoint("/var/log");
    Using::create(rhs, rhs_sda1, rhs_sda1_fs);

    Ext4* rhs_sda2_fs = Ext4::create(rhs);
    rhs_sda2_fs->add_mountpoint("/var");
    Using::create(rhs, rhs_sda2, rhs_sda2_fs);

    rhs->write_graphviz("compare6-device-rhs");

    Actiongraph actiongraph(lhs, rhs);

    actiongraph.write_graphviz("compare6-action");

    delete lhs;
    delete rhs;
}
