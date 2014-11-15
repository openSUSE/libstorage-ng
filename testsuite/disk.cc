
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/DeviceGraph.h"
#include "storage/ActionGraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(disk)
{
    DeviceGraph lhs;

    DeviceGraph rhs;

    rhs.add_vertex(new Disk("/dev/sda"));

    BOOST_CHECK_EXCEPTION(ActionGraph action_graph(lhs, rhs), runtime_error,
	[](const runtime_error& e) { return e.what() == string("cannot create disk"); }
    );
}
