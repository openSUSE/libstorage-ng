
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Algorithm.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_first_missing_number)
{
    struct Tmp
    {
	Tmp(unsigned int number) : number(number) {}

	unsigned int get_number() const { return number; }

	const unsigned int number;

	static
	vector<shared_ptr<Tmp>>
	make_vector(const vector<unsigned int>& x)
	{
	    vector<shared_ptr<Tmp>> ret;

	    for (unsigned int i : x)
		ret.push_back(make_shared<Tmp>(i));

	    return ret;
	}
    };

    // start = 0

    BOOST_CHECK_EQUAL(first_missing_number(Tmp::make_vector({ }), 0), 0);
    BOOST_CHECK_EQUAL(first_missing_number(Tmp::make_vector({ 0 }), 0), 1);
    BOOST_CHECK_EQUAL(first_missing_number(Tmp::make_vector({ 1 }), 0), 0);
    BOOST_CHECK_EQUAL(first_missing_number(Tmp::make_vector({ 0, 1, 3 }), 0), 2);

    // start = 1

    BOOST_CHECK_EQUAL(first_missing_number(Tmp::make_vector({ }), 1), 1);
    BOOST_CHECK_EQUAL(first_missing_number(Tmp::make_vector({ 1 }), 1), 2);
    BOOST_CHECK_EQUAL(first_missing_number(Tmp::make_vector({ 2 }), 1), 1);
    BOOST_CHECK_EQUAL(first_missing_number(Tmp::make_vector({ 1, 2, 4 }), 1), 3);
}
