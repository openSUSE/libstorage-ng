
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"

using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_check1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
    environment.set_devicegraph_filename("wrong-luks.xml");

    Storage storage(environment);
    storage.probe();

    const Devicegraph* probed = storage.get_probed();

    {
	// An exception must be raised if the luks device is BIGGER than
	// the device being encrypted
	BOOST_CHECK_THROW(probed->check(), Exception);
    }
}

BOOST_AUTO_TEST_CASE(test_check2)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
    environment.set_devicegraph_filename("luks-no-header.xml");

    Storage storage(environment);
    storage.probe();

    const Devicegraph* probed = storage.get_probed();

    {
	// A luks with very small metadata header of with no header at all is
	// acceptable (regression test for bsc#1120070 and bsc#1101870)
	BOOST_CHECK_NO_THROW(probed->check());
    }
}

BOOST_AUTO_TEST_CASE(test_check3)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();
    Disk* sda = Disk::create(staging, "/dev/sda", Region(0, 1048576, 512));

    sda->create_encryption("cr-test");

    {
	// Our API should not create an invalid luks
	BOOST_CHECK_NO_THROW(staging->check());
    }
}
