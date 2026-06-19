
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include <glob.h>

#include "storage/Utils/AppUtil.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(glob1)
{
    vector<string> tmp = glob("*.cc", GLOB_NOSORT);

    BOOST_CHECK_GT(tmp.size(), 10);
    BOOST_CHECK(find(tmp.begin(), tmp.end(), "glob.cc") != tmp.end());
}


BOOST_AUTO_TEST_CASE(glob2)
{
    vector<string> tmp = glob("no-no-no", 0);

    BOOST_CHECK_EQUAL(tmp.size(), 0);
}


BOOST_AUTO_TEST_CASE(glob3)
{
    vector<string> tmp = glob("no-no-no", GLOB_NOCHECK);

    BOOST_CHECK_EQUAL(tmp.size(), 1);
    BOOST_CHECK_EQUAL(tmp[0], "no-no-no");
}


BOOST_AUTO_TEST_CASE(glob_invalid_flags)
{
    vector<string> tmp = glob("*", 0xffff);

    BOOST_CHECK_EQUAL(tmp.size(), 0);
}
