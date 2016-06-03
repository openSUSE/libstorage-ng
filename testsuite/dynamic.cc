
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Holders/User.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dynamic)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");

    Gpt* gpt = to_gpt(sda->create_partition_table(PtType::GPT));

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(2048, 1024000, 512), PartitionType::PRIMARY);
    Partition* sda2 = gpt->create_partition("/dev/sda2", Region(1026048, 1024000, 512), PartitionType::PRIMARY);

    LvmVg* system = LvmVg::create(devicegraph, "system");
    User::create(devicegraph, sda1, system);

    LvmLv* root = system->create_lvm_lv("root");

    Ext4* ext4 = to_ext4(sda2->create_filesystem(FsType::EXT4));

    BOOST_CHECK_EQUAL(devicegraph->num_devices(), 7);
    BOOST_CHECK_EQUAL(devicegraph->num_holders(), 6);

    BOOST_CHECK(dynamic_cast<const Disk*>(devicegraph->find_device(sda->get_sid())));

    BOOST_CHECK(dynamic_cast<const Gpt*>(devicegraph->find_device(gpt->get_sid())));
    BOOST_CHECK(dynamic_cast<const User*>(devicegraph->find_holder(sda->get_sid(), gpt->get_sid())));

    BOOST_CHECK(dynamic_cast<const Partition*>(devicegraph->find_device(sda1->get_sid())));
    BOOST_CHECK(dynamic_cast<const Subdevice*>(devicegraph->find_holder(gpt->get_sid(), sda1->get_sid())));

    BOOST_CHECK(dynamic_cast<const LvmVg*>(devicegraph->find_device(system->get_sid())));
    BOOST_CHECK(dynamic_cast<const User*>(devicegraph->find_holder(sda1->get_sid(), system->get_sid())));

    BOOST_CHECK(dynamic_cast<const LvmLv*>(devicegraph->find_device(root->get_sid())));
    BOOST_CHECK(dynamic_cast<const Subdevice*>(devicegraph->find_holder(system->get_sid(), root->get_sid())));

    BOOST_CHECK(dynamic_cast<const Ext4*>(devicegraph->find_device(ext4->get_sid())));
    BOOST_CHECK(dynamic_cast<const User*>(devicegraph->find_holder(sda2->get_sid(), ext4->get_sid())));

    BOOST_CHECK(sda->has_partition_table());

    BOOST_CHECK(!sda->has_filesystem());
    BOOST_CHECK(!sda1->has_filesystem());
    BOOST_CHECK(sda2->has_filesystem());
    BOOST_CHECK(!root->has_filesystem());

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
