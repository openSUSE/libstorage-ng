
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/EtcCrypttab.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_crypttab_encode)
{
    BOOST_CHECK_EQUAL(EtcCrypttab::encode("nothing"), "nothing");

    BOOST_CHECK_EQUAL(EtcCrypttab::encode("a space"), "a\\040space");

    BOOST_CHECK_EQUAL(EtcCrypttab::encode("a#hash"), "a#hash");
}


BOOST_AUTO_TEST_CASE(test_crypttab_decode)
{
    BOOST_CHECK_EQUAL(EtcCrypttab::decode("nothing"), "nothing");

    BOOST_CHECK_EQUAL(EtcCrypttab::decode("a\\040space"), "a space");

    BOOST_CHECK_EQUAL(EtcCrypttab::decode("a#hash"), "a#hash");
    BOOST_CHECK_EQUAL(EtcCrypttab::decode("a\\043hash"), "a#hash");
}
