
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Math.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_is_power_of_two)
{
    BOOST_CHECK(!is_power_of_two(0));
    BOOST_CHECK(is_power_of_two(1));
    BOOST_CHECK(is_power_of_two(2));
    BOOST_CHECK(!is_power_of_two(3));

    BOOST_CHECK(is_power_of_two(2048));
    BOOST_CHECK(!is_power_of_two(2049));
}
