
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/DasdImpl.h"
#include "storage/Devices/MultipathImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_disk1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 1000000, 512));

    BOOST_CHECK_EQUAL(sda->get_impl().partition_name(1), "/dev/sda1");
}


BOOST_AUTO_TEST_CASE(test_disk2)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* pmem0 = Disk::create(devicegraph, "/dev/pmem0", Region(0, 1000000, 512));

    BOOST_CHECK_EQUAL(pmem0->get_impl().partition_name(1), "/dev/pmem0p1");
}


BOOST_AUTO_TEST_CASE(test_dasd1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Dasd* dasda = Dasd::create(devicegraph, "/dev/dasda", Region(0, 601020, 4096));

    BOOST_CHECK_EQUAL(dasda->get_impl().partition_name(1), "/dev/dasda1");
}


BOOST_AUTO_TEST_CASE(test_multipath1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Multipath* multipath = Multipath::create(devicegraph, "/dev/mapper/36005076305ffc73a00000000000013b5", Region(0, 1000000, 512));

    BOOST_CHECK_EQUAL(multipath->get_impl().partition_name(1), "/dev/mapper/36005076305ffc73a00000000000013b5-part1");
}


BOOST_AUTO_TEST_CASE(test_multipath2)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Multipath* multipath = Multipath::create(devicegraph, "/dev/mapper/some-alias", Region(0, 1000000, 512));

    BOOST_CHECK_EQUAL(multipath->get_impl().partition_name(1), "/dev/mapper/some-alias-part1");
}
