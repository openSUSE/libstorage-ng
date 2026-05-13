
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/XmlFile.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(load1)
{
    XmlFile xml("xml.xml");

    const xmlNode* root_node = xml.getRootElement();
    BOOST_CHECK(root_node);

    const xmlNode* a_node = getChildNode(root_node, "a");
    BOOST_CHECK(a_node);

    BOOST_CHECK_EQUAL(getChildNodes(a_node).size(), 5);
    BOOST_CHECK_EQUAL(getChildNodes(a_node, "b").size(), 4);
    BOOST_CHECK_EQUAL(getChildNodes(a_node, "c").size(), 1);

    vector<string> bs;
    getChildValue(a_node, "b", bs);
    BOOST_CHECK_EQUAL(bs.size(), 4);
    BOOST_CHECK_EQUAL(bs[0], "one");
    BOOST_CHECK_EQUAL(bs[1], "");
    BOOST_CHECK_EQUAL(bs[2], "two");
    BOOST_CHECK_EQUAL(bs[3], "");
}
