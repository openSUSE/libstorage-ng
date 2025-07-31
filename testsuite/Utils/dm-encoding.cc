
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Dm.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_dm_encode)
{
    BOOST_CHECK_EQUAL(dm_encode("nothing"), "nothing");

    BOOST_CHECK_EQUAL(dm_encode("Gimme! $"), "Gimme\\x21\\x20\\x24");
}
