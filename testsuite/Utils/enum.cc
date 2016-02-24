
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/PartitionImpl.h"


using namespace std;
using namespace storage;


namespace std
{
    ostream& operator<<(ostream& s, PartitionType partition_type)
    {
        return s << toString(partition_type);
    }
}


BOOST_AUTO_TEST_CASE(test_to_string)
{
    BOOST_CHECK_EQUAL(toString(PartitionType::PRIMARY), "primary");

    BOOST_CHECK_THROW(toString(PartitionType(-1)), IndexOutOfRangeException);
    BOOST_CHECK_THROW(toString(PartitionType(+4)), IndexOutOfRangeException);
}


BOOST_AUTO_TEST_CASE(test_to_value)
{
    PartitionType type;

    BOOST_CHECK(toValue("primary", type));
    BOOST_CHECK_EQUAL(type, PartitionType::PRIMARY);

    BOOST_CHECK(!toValue("hyperbolic", type));
}


BOOST_AUTO_TEST_CASE(test_to_value_with_fallback)
{
    BOOST_CHECK_EQUAL(toValueWithFallback("primary", PartitionType::LOGICAL), PartitionType::PRIMARY);

    BOOST_CHECK_EQUAL(toValueWithFallback("hyperbolic", PartitionType::LOGICAL), PartitionType::LOGICAL);
}
