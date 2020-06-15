
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Utils/StorageTmpl.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test1)
{
    vector<string> v = { "123", "a4", "a1", "a3", "a2", "1" };

    std::function<unsigned int(string)> key_fnc = [](const string& s) { return s.size(); };

    v = sort_by_key(v, key_fnc);

    BOOST_CHECK_EQUAL(boost::join(v, " "), "1 a4 a1 a3 a2 123");
}
