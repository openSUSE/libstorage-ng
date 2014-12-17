
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    Devicegraph* devicegraph = new Devicegraph();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");

    Gpt* gpt = Gpt::create(devicegraph);
    Using::create(devicegraph, sda, gpt);

    Partition* sda1 = Partition::create(devicegraph, "/dev/sda1");
    Subdevice::create(devicegraph, gpt, sda1);

    Encryption::create(devicegraph, "/dev/mapper/cr_sda1");

    LvmVg::create(devicegraph, "/dev/system");
    LvmLv::create(devicegraph, "/dev/system/root");

    Ext4::create(devicegraph);
    Swap::create(devicegraph);

    BOOST_CHECK_EQUAL(devicegraph->num_devices(), 8);
    BOOST_CHECK_EQUAL(devicegraph->num_holders(), 2);

    devicegraph->check();

    Devicegraph* devicegraph_copy = new Devicegraph();
    devicegraph->copy(*devicegraph_copy);

    BOOST_CHECK_EQUAL(devicegraph_copy->num_devices(), 8);
    BOOST_CHECK_EQUAL(devicegraph_copy->num_holders(), 2);

    devicegraph_copy->check();

    delete devicegraph;
    delete devicegraph_copy;
}
