
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/EtcFstab.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_fstab_encode)
{
    BOOST_CHECK_EQUAL(EtcFstab::encode("nothing"), "nothing");

    BOOST_CHECK_EQUAL(EtcFstab::encode("a space"), "a\\040space");
    BOOST_CHECK_EQUAL(EtcFstab::encode("a\ttab"), "a\\011tab");

    BOOST_CHECK_EQUAL(EtcFstab::encode("a\\backslash"), "a\\\\backslash");
}


BOOST_AUTO_TEST_CASE(test_fstab_decode)
{
    BOOST_CHECK_EQUAL(EtcFstab::decode("nothing"), "nothing");

    BOOST_CHECK_EQUAL(EtcFstab::decode("a\\040space"), "a space");
    BOOST_CHECK_EQUAL(EtcFstab::decode("a\\011tab"), "a\ttab");

    BOOST_CHECK_EQUAL(EtcFstab::decode("a\\\\backslash"), "a\\backslash");
    BOOST_CHECK_EQUAL(EtcFstab::decode("a\\134backslash"), "a\\backslash");

    BOOST_CHECK_EQUAL(EtcFstab::decode("a\\043hash"), "a#hash");
}
