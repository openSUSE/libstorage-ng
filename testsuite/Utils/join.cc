
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Text.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(test_zero)
{
    const vector<string> v;

    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 0).native, "");
    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 100).native, "");

    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 0).native, "");
    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 100).native, "");
}


BOOST_AUTO_TEST_CASE(test_one)
{
    const vector<string> v = { "1" };

    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 0).native, "1");
    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 100).native, "1");

    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 0).native, "1");
    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 100).native, "1");
}


BOOST_AUTO_TEST_CASE(test_two)
{
    const vector<string> v = { "1", "2" };

    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 0).native, "1\n2");
    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 2).native, "1\n2");
    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 100).native, "1\n2");

    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 0).native, "1 and 2");
    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 2).native, "1 and 2");
    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 100).native, "1 and 2");
}


BOOST_AUTO_TEST_CASE(test_five)
{
    const vector<string> v = { "1", "2", "3", "4", "5" };

    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 0).native, "1\n2\n3\n4\n5");
    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 1).native, "1\n4 more");
    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 2).native, "1\n4 more");
    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 3).native, "1\n2\n3 more");
    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 4).native, "1\n2\n3\n2 more");
    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 5).native, "1\n2\n3\n4\n5");
    BOOST_CHECK_EQUAL(join(v, JoinMode::NEWLINE, 100).native, "1\n2\n3\n4\n5");

    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 0).native, "1, 2, 3, 4 and 5");
    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 1).native, "1 and 4 more");
    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 2).native, "1 and 4 more");
    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 3).native, "1, 2 and 3 more");
    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 4).native, "1, 2, 3 and 2 more");
    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 5).native, "1, 2, 3, 4 and 5");
    BOOST_CHECK_EQUAL(join(v, JoinMode::COMMA, 100).native, "1, 2, 3, 4 and 5");
}
