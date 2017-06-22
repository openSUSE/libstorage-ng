
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Md.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(free_numeric_name)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    // will create /dev/md0
    Md::create(staging, "/dev/md0");

    BOOST_CHECK_EQUAL(Md::find_free_numeric_name(staging), "/dev/md1");

    // will likely create /dev/md127
    Md::create(staging, "/dev/md/test");

    BOOST_CHECK_EQUAL(Md::find_free_numeric_name(staging), "/dev/md1");

    // will create /dev/md1
    Md::create(staging, "/dev/md/1");

    BOOST_CHECK_EQUAL(Md::find_free_numeric_name(staging), "/dev/md2");
}
