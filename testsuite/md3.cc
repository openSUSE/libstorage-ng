
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Md.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(create)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Md* md0 = Md::create(staging, "/dev/md0");

    md0->set_md_level(MdLevel::RAID0);
    BOOST_CHECK_EQUAL(md0->minimal_number_of_devices(), 2);

    md0->set_md_level(MdLevel::RAID1);
    BOOST_CHECK_EQUAL(md0->minimal_number_of_devices(), 2);

    md0->set_md_level(MdLevel::RAID5);
    BOOST_CHECK_EQUAL(md0->minimal_number_of_devices(), 3);

    md0->set_md_level(MdLevel::RAID6);
    BOOST_CHECK_EQUAL(md0->minimal_number_of_devices(), 4);

    md0->set_md_level(MdLevel::RAID10);
    BOOST_CHECK_EQUAL(md0->minimal_number_of_devices(), 2);
}
