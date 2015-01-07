
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include <stdlib.h>
#include <iostream>
#include <locale>

#include "storage/Utils/AppUtil.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_udev_encode)
{
    BOOST_CHECK_EQUAL(udevEncode("nothing"), "nothing");
    
    BOOST_CHECK_EQUAL(udevEncode("hello world"), "hello\\x20world");
    BOOST_CHECK_EQUAL(udevEncode("hello  world"), "hello\\x20\\x20world");
    
    BOOST_CHECK_EQUAL(udevEncode(" beginning"), "\\x20beginning");
    BOOST_CHECK_EQUAL(udevEncode("ending "), "ending\\x20");
    
    BOOST_CHECK_EQUAL(udevEncode("woody's"), "woody\\x27s");
}


BOOST_AUTO_TEST_CASE(test_udev_decode)
{
    BOOST_CHECK_EQUAL(udevDecode("nothing"), "nothing");
    
    BOOST_CHECK_EQUAL(udevDecode("hello\\x20world"), "hello world");
    BOOST_CHECK_EQUAL(udevDecode("hello\\x20\\x20world"), "hello  world");
    
    BOOST_CHECK_EQUAL(udevDecode("\\x20beginning"), " beginning");
    BOOST_CHECK_EQUAL(udevDecode("ending\\x20"), "ending ");
    
    BOOST_CHECK_EQUAL(udevDecode("woody\\x27s"), "woody's");
}
