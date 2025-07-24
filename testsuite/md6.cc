
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/MdImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(md_values)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    // will create /dev/md0
    Md* md0 = Md::create(staging, "/dev/md0");

    BOOST_CHECK_EQUAL(md0->get_name(), "/dev/md0");
    BOOST_CHECK(md0->is_numeric());
    BOOST_CHECK_EQUAL(md0->get_number(), 0);
    BOOST_CHECK_THROW(md0->get_impl().get_short_name(), Exception);

    BOOST_CHECK_EQUAL(md0->get_sysfs_name(), "md0");
    BOOST_CHECK_EQUAL(md0->get_sysfs_path(), "/devices/virtual/block/md0");

    // will create /dev/md2
    Md* md2 = Md::create(staging, "/dev/md/2");

    BOOST_CHECK_EQUAL(md2->get_name(), "/dev/md/2");
    BOOST_CHECK(md2->is_numeric());
    BOOST_CHECK_EQUAL(md2->get_number(), 2);
    BOOST_CHECK_EQUAL(md2->get_impl().get_short_name(), "2");

    BOOST_CHECK_EQUAL(md2->get_sysfs_name(), "md2");
    BOOST_CHECK_EQUAL(md2->get_sysfs_path(), "/devices/virtual/block/md2");

    // will create /dev/md7
    Md* md7 = Md::create(staging, "/dev/md/007");

    BOOST_CHECK_EQUAL(md7->get_name(), "/dev/md/007");
    BOOST_CHECK(md7->is_numeric());
    BOOST_CHECK_EQUAL(md7->get_number(), 7);
    BOOST_CHECK_EQUAL(md7->get_impl().get_short_name(), "007");

    BOOST_CHECK_EQUAL(md7->get_sysfs_name(), "md7");
    BOOST_CHECK_EQUAL(md7->get_sysfs_path(), "/devices/virtual/block/md7");

    // will likely "rename" /dev/md7 to /dev/md127
    md7->set_name("/dev/md/bond");

    BOOST_CHECK_EQUAL(md7->get_name(), "/dev/md/bond");
    BOOST_CHECK(!md7->is_numeric());
    BOOST_CHECK_THROW(md7->get_number(), Exception);
    BOOST_CHECK_EQUAL(md7->get_impl().get_short_name(), "bond");

    BOOST_CHECK_EQUAL(md7->get_sysfs_name(), "");
    BOOST_CHECK_EQUAL(md7->get_sysfs_path(), "");

    // will likely create /dev/md126
    Md* md_test = Md::create(staging, "/dev/md/test");

    BOOST_CHECK_EQUAL(md_test->get_name(), "/dev/md/test");
    BOOST_CHECK(!md_test->is_numeric());
    BOOST_CHECK_THROW(md_test->get_number(), Exception);
    BOOST_CHECK_EQUAL(md_test->get_impl().get_short_name(), "test");

    BOOST_CHECK_EQUAL(md_test->get_sysfs_name(), "");
    BOOST_CHECK_EQUAL(md_test->get_sysfs_path(), "");

    // invalid names cause an exception
    BOOST_CHECK_THROW(Md::create(staging, "/dev/foo"), Exception);
    BOOST_CHECK_THROW(Md::create(staging, "/dev/md_foo"), Exception);
    BOOST_CHECK_THROW(md_test->set_name("/dev/foo"), Exception);
    BOOST_CHECK_THROW(md_test->set_name("/dev/md_foo"), Exception);
}
