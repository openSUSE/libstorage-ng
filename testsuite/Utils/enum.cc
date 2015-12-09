
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Enum.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_to_string)
{
    BOOST_CHECK_EQUAL(toString(PRIMARY), "primary");

    BOOST_CHECK_THROW(toString(PartitionType(-1)), IndexOutOfRangeException);
    BOOST_CHECK_THROW(toString(PartitionType(+4)), IndexOutOfRangeException);
}


BOOST_AUTO_TEST_CASE(test_to_value)
{
    PartitionType type;

    BOOST_CHECK(toValue("primary", type));
    BOOST_CHECK_EQUAL(type, PRIMARY);

    BOOST_CHECK(!toValue("hyperbolic", type));
}


BOOST_AUTO_TEST_CASE(test_to_value_with_fallback)
{
    BOOST_CHECK_EQUAL(toValueWithFallback("primary", LOGICAL), PRIMARY);

    BOOST_CHECK_EQUAL(toValueWithFallback("hyperbolic", LOGICAL), LOGICAL);
}
