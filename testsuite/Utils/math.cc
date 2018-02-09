
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


BOOST_AUTO_TEST_CASE(test_next_power_of_two)
{
    BOOST_CHECK_EQUAL(next_power_of_two(0), 1);

    BOOST_CHECK_EQUAL(next_power_of_two(1), 1);

    BOOST_CHECK_EQUAL(next_power_of_two(2), 2);

    BOOST_CHECK_EQUAL(next_power_of_two(3), 4);
    BOOST_CHECK_EQUAL(next_power_of_two(4), 4);

    BOOST_CHECK_EQUAL(next_power_of_two(1000), 1024);

    BOOST_CHECK_EQUAL(next_power_of_two((1ULL << 50) - 1), (1ULL << 50));
    BOOST_CHECK_EQUAL(next_power_of_two((1ULL << 50)), (1ULL << 50));

    BOOST_CHECK_EQUAL(next_power_of_two((1ULL << 50) + 1), (1ULL << 51));
}


BOOST_AUTO_TEST_CASE(test_round_down)
{
    BOOST_CHECK_EQUAL(round_down(9, 10), 0);
    BOOST_CHECK_EQUAL(round_down(10, 10), 10);
    BOOST_CHECK_EQUAL(round_down(11, 10), 10);
}


BOOST_AUTO_TEST_CASE(test_round_up)
{
    BOOST_CHECK_EQUAL(round_up(9, 10), 10);
    BOOST_CHECK_EQUAL(round_up(10, 10), 10);
    BOOST_CHECK_EQUAL(round_up(11, 10), 20);
}


BOOST_AUTO_TEST_CASE(test_clz)
{
    BOOST_CHECK_EQUAL(clz(1023), 54);
    BOOST_CHECK_EQUAL(clz(1024), 53);
}
