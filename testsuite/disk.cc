
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE disk

#include <boost/test/unit_test.hpp>

#include "storage/Device.h"
#include "storage/DeviceGraph.h"
#include "storage/ActionGraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(disk)
{
    DeviceGraph lhs;

    DeviceGraph rhs;

    rhs.add_vertex(new Disk("/dev/sda"));

    BOOST_CHECK_THROW(ActionGraph action_graph(lhs, rhs), runtime_error);
}
