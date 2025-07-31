
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Udev.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_udev_encode)
{
    BOOST_CHECK_EQUAL(udev_encode("nothing"), "nothing");

    BOOST_CHECK_EQUAL(udev_encode("hello world"), "hello\\x20world");
    BOOST_CHECK_EQUAL(udev_encode("hello  world"), "hello\\x20\\x20world");

    BOOST_CHECK_EQUAL(udev_encode(" beginning"), "\\x20beginning");
    BOOST_CHECK_EQUAL(udev_encode("ending "), "ending\\x20");

    BOOST_CHECK_EQUAL(udev_encode("woody's"), "woody\\x27s");

    BOOST_CHECK_EQUAL(udev_encode("A/B image"), "A\\x2fB\\x20image");

    BOOST_CHECK_EQUAL(udev_encode("schön"), "schön");
}


BOOST_AUTO_TEST_CASE(test_udev_decode)
{
    BOOST_CHECK_EQUAL(udev_decode("nothing"), "nothing");

    BOOST_CHECK_EQUAL(udev_decode("hello\\x20world"), "hello world");
    BOOST_CHECK_EQUAL(udev_decode("hello\\x20\\x20world"), "hello  world");

    BOOST_CHECK_EQUAL(udev_decode("\\x20beginning"), " beginning");
    BOOST_CHECK_EQUAL(udev_decode("ending\\x20"), "ending ");

    BOOST_CHECK_EQUAL(udev_decode("woody\\x27s"), "woody's");
}
