
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/AppUtil.h"

using namespace storage;


BOOST_AUTO_TEST_CASE(test_regex_escape1)
{
    BOOST_CHECK_EQUAL(regex_escape("test", regex::extended), "test");
    BOOST_CHECK_EQUAL(regex_escape("test []", regex::extended), "test \\[]");
}


/*
 * Check that for single char strings 1. the regex compilation with
 * the escaped string works and that 2. the string only matches
 * itself. The checks are also done when the escaped string is placed
 * inside rounded brackets.
 */
BOOST_AUTO_TEST_CASE(test_regex_escape2)
{
    vector<string> v;

    for (unsigned char c1 = 1; c1 < 255; ++c1)
        v.push_back(string(1, c1));

    for (const string& s1 : v)
    {
	string es1 = regex_escape(s1, regex::extended);

	regex rx;

	BOOST_CHECK_NO_THROW(rx.assign(es1, regex::extended));

	for (const string& s2 : v)
	    BOOST_CHECK_EQUAL(regex_match(s2, rx), s1 == s2);

	BOOST_CHECK_NO_THROW(rx.assign("(" + es1 + ")", regex::extended));

	for (const string& s2 : v)
	    BOOST_CHECK_EQUAL(regex_match(s2, rx), s1 == s2);
    }
}
