
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Utils/Topology.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_output1)
{
    Topology topology(-512, 4096);

    ostringstream out;
    out << topology;

    BOOST_CHECK_EQUAL(out.str(), "[-512 B, 4096 B]");
}


BOOST_AUTO_TEST_CASE(test_output2)
{
    Topology topology(-512, 4096);
    topology.set_minimal_grain(128 * KiB);

    ostringstream out;
    out << topology;

    BOOST_CHECK_EQUAL(out.str(), "[-512 B, 4096 B, 131072 B]");
}
