
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

    BOOST_CHECK_EXCEPTION({ JsonFile json_file(lines); }, Exception, [](const Exception& e) {
	return strcmp(e.what(), "json parser failed: runaway") == 0;
    });
}


BOOST_AUTO_TEST_CASE(bad2)
{
    vector<string> lines = {
	"{"
    };

    BOOST_CHECK_EXCEPTION({ JsonFile json_file(lines); }, Exception, [](const Exception& e) {
	return strcmp(e.what(), "json parser failed: runaway") == 0;
    });
}


BOOST_AUTO_TEST_CASE(bad3)
{
    vector<string> lines = {
	"{ \"fun\": true, \"tonight\": maybe }"
    };

    BOOST_CHECK_EXCEPTION({ JsonFile json_file(lines); }, Exception, [](const Exception& e) {
	return strcmp(e.what(), "json parser failed: unexpected character") == 0;
    });
}
