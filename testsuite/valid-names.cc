
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Md.h"
#include "storage/Devices/BlkDevice.h"


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


BOOST_AUTO_TEST_CASE(valid_md_name)
{
    BOOST_CHECK(Md::is_valid_name("/dev/md0"));
    BOOST_CHECK(Md::is_valid_name("/dev/md/0"));

    BOOST_CHECK(Md::is_valid_name("/dev/md100000"));
    BOOST_CHECK(Md::is_valid_name("/dev/md/100000"));

    BOOST_CHECK(Md::is_valid_name("/dev/md/007"));

    BOOST_CHECK(Md::is_valid_name("/dev/md/test"));
    BOOST_CHECK(Md::is_valid_name("/dev/md/test-test"));
}


BOOST_AUTO_TEST_CASE(invalid_md_name)
{
    BOOST_CHECK(!Md::is_valid_name("/dev/md"));

    BOOST_CHECK(!Md::is_valid_name("/dev/md/a/b"));

    BOOST_CHECK(!Md::is_valid_name("/dev/md12345678901234567890"));
    BOOST_CHECK(!Md::is_valid_name("/dev/md/12345678901234567890"));
}


BOOST_AUTO_TEST_CASE(valid_dm_table_name)
{
    BOOST_CHECK(BlkDevice::is_valid_dm_table_name("cr-1"));
}


BOOST_AUTO_TEST_CASE(invalid_dm_table_name)
{
    BOOST_CHECK(!BlkDevice::is_valid_dm_table_name("cr/1"));
}
