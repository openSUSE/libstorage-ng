
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"


using namespace storage_bgl;


BOOST_AUTO_TEST_CASE(disk)
{
    Devicegraph lhs;

    Devicegraph rhs;

    Disk::create(&rhs, "/dev/sda");

    BOOST_CHECK_EXCEPTION(Actiongraph actiongraph(lhs, rhs), runtime_error,
	[](const runtime_error& e) { return e.what() == string("cannot create disk"); }
    );
}
