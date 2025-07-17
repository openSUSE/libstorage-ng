
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Md.h"
#include "storage/Devicegraph.h"
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

    // next free is /dev/md1
    BOOST_CHECK_EQUAL(Md::find_free_numeric_name(staging), "/dev/md1");

    // will likely create /dev/md127
    Md::create(staging, "/dev/md/test");

    // next free is still /dev/md1
    BOOST_CHECK_EQUAL(Md::find_free_numeric_name(staging), "/dev/md1");

    // will create /dev/md2 and link /dev/md/2
    Md::create(staging, "/dev/md/2");

    // will create /dev/md3 and link /dev/md/03
    Md::create(staging, "/dev/md/03");

    // next free is still /dev/md1
    BOOST_CHECK_EQUAL(Md::find_free_numeric_name(staging), "/dev/md1");

    // will create /dev/md1
    Md::create(staging, "/dev/md1");

    // next free is now /dev/md4
    BOOST_CHECK_EQUAL(Md::find_free_numeric_name(staging), "/dev/md4");
}
