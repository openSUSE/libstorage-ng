
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Graphviz.h"

using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test1)
{
    GraphvizFlags f1 = GraphvizFlags::PRETTY_CLASSNAME;
    GraphvizFlags f2 = GraphvizFlags::NAME;
    GraphvizFlags f3 = GraphvizFlags::SIZE;

    BOOST_CHECK_EQUAL((unsigned int) f1, 0x02);
    BOOST_CHECK_EQUAL((unsigned int) f2, 0x04);
    BOOST_CHECK_EQUAL((unsigned int) f3, 0x10);

    GraphvizFlags f = f1 | f2 | f3;

    BOOST_CHECK_EQUAL((unsigned int) f, 0x16);
}


BOOST_AUTO_TEST_CASE(test2)
{
    GraphvizFlags f = (GraphvizFlags)(0x16);

    BOOST_CHECK(f && GraphvizFlags::PRETTY_CLASSNAME);
    BOOST_CHECK(!(f && GraphvizFlags::CLASSNAME));
    BOOST_CHECK(f && GraphvizFlags::NAME);
    BOOST_CHECK(!(f && GraphvizFlags::SID));
    BOOST_CHECK(f && GraphvizFlags::SIZE);
}
