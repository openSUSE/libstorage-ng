
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Format.h"

using namespace storage;


BOOST_AUTO_TEST_CASE(test_string)
{
    BOOST_CHECK_EQUAL(sformat("a %1$s %2$s d", "b", "c"s), "a b c d");
    BOOST_CHECK_EQUAL(sformat("a %2$s %1$s d", "b", "c"s), "a c b d");

    BOOST_CHECK_EQUAL(sformat("0x%1$02X", 10), "0x0A");
    BOOST_CHECK_EQUAL(sformat("0x%1$02X", 32), "0x20");

    BOOST_CHECK_EQUAL(sformat("/dev/block/%d:%d", 1234, 5678), "/dev/block/1234:5678");
}


BOOST_AUTO_TEST_CASE(test_string_locale)
{
    locale::global(locale("de_DE.UTF-8"));

    BOOST_CHECK_EQUAL(sformat("%d", 1000), "1000");
}


BOOST_AUTO_TEST_CASE(test_string_errors)
{
    // bad_format_string
    BOOST_CHECK_NO_THROW(sformat("%"));

    // too_few_args
    BOOST_CHECK_NO_THROW(sformat("%d"));

    // too_many_args
    BOOST_CHECK_NO_THROW(sformat("", 1));
}


BOOST_AUTO_TEST_CASE(test_text)
{
    Text t1("good %s", "Guten %s");
    Text t2("morning", "Tag");

    Text t3 = sformat(t1, t2);

    BOOST_CHECK_EQUAL(t3.native, "good morning");
    BOOST_CHECK_EQUAL(t3.translated, "Guten Tag");
}


BOOST_AUTO_TEST_CASE(test_text_locale)
{
    locale::global(locale("de_DE.UTF-8"));

    UntranslatedText t1("%d %.1f%%");

    Text t2 = sformat(t1, 1024, 50.0);

    BOOST_CHECK_EQUAL(t2.native, "1024 50.0%");
    BOOST_CHECK_EQUAL(t2.translated, "1.024 50,0%");
}
