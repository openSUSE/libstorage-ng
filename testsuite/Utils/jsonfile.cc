
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/JsonFile.h"
#include "storage/Utils/Exception.h"

using namespace storage;


BOOST_AUTO_TEST_CASE(good1)
{
    vector<string> lines = {
	"{}"
    };

    JsonFile json_file(lines);
}


BOOST_AUTO_TEST_CASE(good2)
{
    vector<string> lines = {
	"{ \"device\" : \"/dev/sda\" }"
    };

    JsonFile json_file(lines);

    string tmp;
    BOOST_CHECK(get_child_value(json_file.get_root(), "device", tmp));
    BOOST_CHECK_EQUAL(tmp, "/dev/sda");
}


BOOST_AUTO_TEST_CASE(good3)
{
    vector<string> lines = {
	"{ \"device\" : \"/dev/sda\" }  ",
	"  "
    };

    JsonFile json_file(lines);

    string tmp;
    BOOST_CHECK(get_child_value(json_file.get_root(), "device", tmp));
    BOOST_CHECK_EQUAL(tmp, "/dev/sda");
}


BOOST_AUTO_TEST_CASE(bad1)
{
    vector<string> lines = {
    };

    BOOST_CHECK_THROW({ JsonFile json_file(lines); }, Exception);
}


BOOST_AUTO_TEST_CASE(bad2)
{
    vector<string> lines = {
	"{"
    };

    BOOST_CHECK_THROW({ JsonFile json_file(lines); }, Exception);
}
