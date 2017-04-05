
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/AppUtil.h"

using namespace storage;


BOOST_AUTO_TEST_CASE(test_dirname)
{
    BOOST_CHECK_EQUAL(dirname("/hello/world"), "/hello");
    BOOST_CHECK_EQUAL(dirname("hello/world"), "hello");
    BOOST_CHECK_EQUAL(dirname("/hello"), "/");
    BOOST_CHECK_EQUAL(dirname("hello"), ".");
    BOOST_CHECK_EQUAL(dirname("/"), "/");
    BOOST_CHECK_EQUAL(dirname(""), ".");
    BOOST_CHECK_EQUAL(dirname("."), ".");
    BOOST_CHECK_EQUAL(dirname(".."), ".");
    BOOST_CHECK_EQUAL(dirname("../.."), "..");
}
