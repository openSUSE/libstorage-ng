
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(valid_vg_name)
{
    BOOST_CHECK(LvmVg::is_valid_vg_name("test"));
    BOOST_CHECK(LvmVg::is_valid_vg_name("hello-world"));
    BOOST_CHECK(LvmVg::is_valid_vg_name("data123"));
}


BOOST_AUTO_TEST_CASE(invalid_vg_name)
{
    BOOST_CHECK(!LvmVg::is_valid_vg_name(""));
    BOOST_CHECK(!LvmVg::is_valid_vg_name(".."));
    BOOST_CHECK(!LvmVg::is_valid_vg_name("-test"));
    BOOST_CHECK(!LvmVg::is_valid_vg_name("hello world"));
    BOOST_CHECK(!LvmVg::is_valid_vg_name("schöne-neue-Welt"));
}


BOOST_AUTO_TEST_CASE(valid_lv_name)
{
    BOOST_CHECK(LvmLv::is_valid_lv_name("test"));
    BOOST_CHECK(LvmLv::is_valid_lv_name("hello-world"));
    BOOST_CHECK(LvmLv::is_valid_lv_name("data123"));
}


BOOST_AUTO_TEST_CASE(invalid_lv_name)
{
    BOOST_CHECK(!LvmLv::is_valid_lv_name(""));
    BOOST_CHECK(!LvmLv::is_valid_lv_name(".."));
    BOOST_CHECK(!LvmLv::is_valid_lv_name("-test"));
    BOOST_CHECK(!LvmLv::is_valid_lv_name("hello world"));
    BOOST_CHECK(!LvmLv::is_valid_lv_name("schöne-neue-Welt"));

    BOOST_CHECK(!LvmLv::is_valid_lv_name("snapshot"));

    BOOST_CHECK(!LvmLv::is_valid_lv_name("test_cdata"));
}
