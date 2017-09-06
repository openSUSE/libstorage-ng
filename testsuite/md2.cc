
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/MdImpl.h"
#include "storage/Devices/MdContainer.h"
#include "storage/Devices/MdMember.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


namespace std
{
    ostream& operator<<(ostream& s, MdLevel md_level)
    {
	return s << toString(md_level);
    }

    ostream& operator<<(ostream& s, MdParity md_parity)
    {
	return s << toString(md_parity);
    }
}


BOOST_AUTO_TEST_CASE(create)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Md* md0 = Md::create(staging, "/dev/md0");

    BOOST_CHECK_EQUAL(md0->get_metadata(), "");
    BOOST_CHECK_EQUAL(md0->get_md_level(), MdLevel::UNKNOWN);
    BOOST_CHECK_EQUAL(md0->get_md_parity(), MdParity::DEFAULT);
    BOOST_CHECK_EQUAL(md0->get_size(), 0);
    BOOST_CHECK_EQUAL(md0->get_range(), 256);

    MdContainer* md127 = MdContainer::create(staging, "/dev/md127");

    BOOST_CHECK_EQUAL(md127->get_metadata(), "");
    BOOST_CHECK_EQUAL(md127->get_md_level(), MdLevel::CONTAINER);
    BOOST_CHECK_EQUAL(md127->get_md_parity(), MdParity::DEFAULT);
    BOOST_CHECK_EQUAL(md127->get_size(), 0);
    BOOST_CHECK_EQUAL(md127->get_range(), 256);

    MdMember* md126 = MdMember::create(staging, "/dev/md126");

    BOOST_CHECK_EQUAL(md126->get_metadata(), "");
    BOOST_CHECK_EQUAL(md126->get_md_level(), MdLevel::UNKNOWN);
    BOOST_CHECK_EQUAL(md126->get_md_parity(), MdParity::DEFAULT);
    BOOST_CHECK_EQUAL(md126->get_size(), 0);
    BOOST_CHECK_EQUAL(md126->get_range(), 256);
}
