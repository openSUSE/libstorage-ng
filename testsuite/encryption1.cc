
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/Luks.h"
#include "storage/Holders/FilesystemUser.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"
#include "storage/FreeInfo.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_create1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sdb = Disk::create(staging, "/dev/sdb", Region(0, 1048576, 512));

    Filesystem* xfs = sdb->create_blk_filesystem(FsType::XFS);

    storage.check();

    {
	BOOST_CHECK_EQUAL(staging->num_devices(), 2);
	BOOST_CHECK_EQUAL(staging->num_holders(), 1);

	Holder* holder = staging->find_holder(sdb->get_sid(), xfs->get_sid());
	BOOST_CHECK(is_filesystem_user(holder));
    }

    Encryption* encryption = sdb->create_encryption("cr-test", EncryptionType::LUKS1);
    encryption->set_password("12345678");

    storage.check();

    {
	BOOST_CHECK_EQUAL(staging->num_devices(), 3);
	BOOST_CHECK_EQUAL(staging->num_holders(), 2);

	Holder* holder1 = staging->find_holder(sdb->get_sid(), encryption->get_sid());
	BOOST_CHECK(is_user(holder1));

	Holder* holder2 = staging->find_holder(encryption->get_sid(), xfs->get_sid());
	BOOST_CHECK(is_filesystem_user(holder2));

	BOOST_CHECK_THROW(staging->find_holder(sdb->get_sid(), xfs->get_sid()), Exception);
    }
}


BOOST_AUTO_TEST_CASE(test_create2)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sdb = Disk::create(staging, "/dev/sdb", Region(0, 1048576, 512));

    Encryption* encryption = sdb->create_encryption("cr-test", EncryptionType::LUKS1);
    encryption->set_password("12345678");

    Filesystem* xfs = encryption->create_blk_filesystem(FsType::XFS);

    storage.check();

    {
	BOOST_CHECK_EQUAL(staging->num_devices(), 3);
	BOOST_CHECK_EQUAL(staging->num_holders(), 2);

	Holder* holder1 = staging->find_holder(sdb->get_sid(), encryption->get_sid());
	BOOST_CHECK(is_user(holder1));

	Holder* holder2 = staging->find_holder(encryption->get_sid(), xfs->get_sid());
	BOOST_CHECK(is_filesystem_user(holder2));

	BOOST_CHECK_THROW(staging->find_holder(sdb->get_sid(), xfs->get_sid()), Exception);
    }

    sdb->remove_encryption();

    storage.check();

    {
	BOOST_CHECK_EQUAL(staging->num_devices(), 2);
	BOOST_CHECK_EQUAL(staging->num_holders(), 1);

	Holder* holder = staging->find_holder(sdb->get_sid(), xfs->get_sid());
	BOOST_CHECK(is_filesystem_user(holder));
    }
}
