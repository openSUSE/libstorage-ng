
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Md.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(calculate_size)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", Region(0, 10000000, 512));
    Disk* sdb = Disk::create(staging, "/dev/sdb", Region(0, 10000000, 512));
    Disk* sdc = Disk::create(staging, "/dev/sdc", Region(0, 10000000, 512));

    Md* md0 = Md::create(staging, "/dev/md0");
    md0->set_md_level(MdLevel::RAID0);

    md0->add_device(sda);

    BOOST_CHECK_EQUAL(md0->get_size(), 0);

    md0->add_device(sdb);

    BOOST_CHECK_EQUAL(md0->get_size(), 10066974720);

    md0->add_device(sdc);

    BOOST_CHECK_EQUAL(md0->get_size(), 15100462080);

    md0->remove_device(sdc);

    BOOST_CHECK_EQUAL(md0->get_size(), 10066974720);
}
