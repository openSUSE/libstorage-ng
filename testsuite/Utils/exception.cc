
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/ExceptionImpl.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(code_location)
{
    int line = __LINE__;

    try
    {
	ST_THROW(Exception("test"));
    }
    catch (const Exception& e)
    {
	BOOST_CHECK_EQUAL(e.msg(), "test");

	const CodeLocation& code_location = e.where();
	BOOST_CHECK_EQUAL(code_location.file(), __BASE_FILE__);
	BOOST_CHECK_EQUAL(code_location.line(), line + 4);
    }
}


BOOST_AUTO_TEST_CASE(null_pointer)
{
    void* ptr = nullptr;

    BOOST_CHECK_EXCEPTION(ST_CHECK_PTR(ptr), NullPointerException, [](const NullPointerException& e) {
	return e.msg() == "Null pointer";
    });
}
