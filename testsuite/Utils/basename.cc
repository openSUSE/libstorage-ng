
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/AppUtil.h"

using namespace storage;


BOOST_AUTO_TEST_CASE(test_basename)
{
    BOOST_CHECK_EQUAL(basename("/hello/world"), "world");
    BOOST_CHECK_EQUAL(basename("hello/world"), "world");
    BOOST_CHECK_EQUAL(basename("/hello"), "hello");
    BOOST_CHECK_EQUAL(basename("hello"), "hello");
    BOOST_CHECK_EQUAL(basename("/"), "");
    BOOST_CHECK_EQUAL(basename(""), "");
    BOOST_CHECK_EQUAL(basename("."), ".");
    BOOST_CHECK_EQUAL(basename(".."), "..");
    BOOST_CHECK_EQUAL(basename("../.."), "..");
}
