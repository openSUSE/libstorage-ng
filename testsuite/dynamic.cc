
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Holders/User.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dynamic)
{
    Devicegraph devicegraph;

    Disk* sda = Disk::create(&devicegraph, "/dev/sda");

    Partition* sda1 = Partition::create(&devicegraph, "/dev/sda1", Region(0, 10, 262144), PartitionType::PRIMARY);
    Subdevice::create(&devicegraph, sda, sda1);

    LvmVg* system = LvmVg::create(&devicegraph, "/dev/system");
    User::create(&devicegraph, sda1, system);

    LvmLv* root = LvmLv::create(&devicegraph, "/dev/system/root");
    Subdevice::create(&devicegraph, system, root);

    BOOST_CHECK_EQUAL(devicegraph.num_devices(), 4);
    BOOST_CHECK_EQUAL(devicegraph.num_holders(), 3);

    BOOST_CHECK(dynamic_cast<const Disk*>(devicegraph.find_device(sda->get_sid())));

    BOOST_CHECK(dynamic_cast<const Partition*>(devicegraph.find_device(sda1->get_sid())));
    BOOST_CHECK(dynamic_cast<const Subdevice*>(devicegraph.find_holder(sda->get_sid(), sda1->get_sid())));

    BOOST_CHECK(dynamic_cast<const LvmVg*>(devicegraph.find_device(system->get_sid())));
    BOOST_CHECK(dynamic_cast<const User*>(devicegraph.find_holder(sda1->get_sid(), system->get_sid())));

    BOOST_CHECK(dynamic_cast<const LvmLv*>(devicegraph.find_device(root->get_sid())));
    BOOST_CHECK(dynamic_cast<const Subdevice*>(devicegraph.find_holder(system->get_sid(), root->get_sid())));

    {
	Device* tmp = sda;

	BOOST_CHECK(is_disk(tmp));
	BOOST_CHECK_NO_THROW(to_disk(tmp));

	BOOST_CHECK(is_blk_device(tmp));
	BOOST_CHECK_NO_THROW(to_blk_device(tmp));

	BOOST_CHECK(!is_partition(tmp));
	BOOST_CHECK_EXCEPTION(to_partition(tmp), DeviceHasWrongType, [](const DeviceHasWrongType& e) {
	    return strcmp(e.what(), "device has wrong type, seen 'Disk', expected 'Partition'") == 0;
	});
    }

    {
	Device* tmp = sda1;

	BOOST_CHECK(is_partition(tmp));
	BOOST_CHECK_NO_THROW(to_partition(tmp));

	BOOST_CHECK(is_blk_device(tmp));
	BOOST_CHECK_NO_THROW(to_blk_device(tmp));

	BOOST_CHECK(!is_disk(tmp));
	BOOST_CHECK_EXCEPTION(to_disk(tmp), DeviceHasWrongType, [](const DeviceHasWrongType& e) {
	    return strcmp(e.what(), "device has wrong type, seen 'Partition', expected 'Disk'") == 0;
	});
    }
}
