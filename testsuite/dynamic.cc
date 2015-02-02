
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/Region.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dynamic)
{
    Devicegraph devicegraph;

    Disk* sda = Disk::create(&devicegraph, "/dev/sda");

    Partition* sda1 = Partition::create(&devicegraph, "/dev/sda1", PRIMARY, Region(0, 100));
    Subdevice::create(&devicegraph, sda, sda1);

    LvmVg* system = LvmVg::create(&devicegraph, "/dev/system");
    Using::create(&devicegraph, sda1, system);

    LvmLv* root = LvmLv::create(&devicegraph, "/dev/system/root");
    Subdevice::create(&devicegraph, system, root);

    BOOST_CHECK_EQUAL(devicegraph.num_devices(), 4);
    BOOST_CHECK_EQUAL(devicegraph.num_holders(), 3);

    BOOST_CHECK(dynamic_cast<const Disk*>(devicegraph.find_device(sda->get_sid())));

    BOOST_CHECK(dynamic_cast<const Partition*>(devicegraph.find_device(sda1->get_sid())));
    BOOST_CHECK(dynamic_cast<const Subdevice*>(devicegraph.find_holder(sda->get_sid(), sda1->get_sid())));

    BOOST_CHECK(dynamic_cast<const LvmVg*>(devicegraph.find_device(system->get_sid())));
    BOOST_CHECK(dynamic_cast<const Using*>(devicegraph.find_holder(sda1->get_sid(), system->get_sid())));

    BOOST_CHECK(dynamic_cast<const LvmLv*>(devicegraph.find_device(root->get_sid())));
    BOOST_CHECK(dynamic_cast<const Subdevice*>(devicegraph.find_holder(system->get_sid(), root->get_sid())));
}
