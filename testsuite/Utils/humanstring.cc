
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include <stdlib.h>
#include <iostream>
#include <locale>

#include "storage/Utils/HumanString.h"
#include "storage/Utils/Exception.h"


using namespace std;
using namespace storage;


// Tests here must work when using double instead of long double in
// HumanString.cc.


class Fixture
{
public:

    Fixture()
    {
	const char* localedir = getenv("LOCALEDIR");
	if (localedir)
	    bindtextdomain("libstorage-ng", localedir);
    }

};


BOOST_GLOBAL_FIXTURE(Fixture);


string
test(const char* loc, unsigned long long size, bool classic, int precision, bool omit_zeroes)
{
    locale::global(locale(loc));

    return byte_to_humanstring(size, classic, precision, omit_zeroes);
}


unsigned long long
test(const char* loc, const char* str, bool classic)
{
    locale::global(locale(loc));

    return humanstring_to_byte(str, classic);
}


BOOST_AUTO_TEST_CASE(test_byte_to_humanstring)
{
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 0, false, 2, true), "0 B");
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 0, false, 2, false), "0 B");

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 1023, true, 2, true), "1023 B");
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 1023, true, 2, false), "1023 B");

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 1024, true, 2, true), "1 KiB");
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 1024, true, 2, false), "1.00 KiB");

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 1025, true, 2, true), "1.00 KiB");
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 1025, true, 2, false), "1.00 KiB");

    BOOST_CHECK_EQUAL(test("de_DE.UTF-8", 123456789, true, 4, false), "117.7376 MiB");
    BOOST_CHECK_EQUAL(test("de_DE.UTF-8", 123456789, false, 4, false), "117,7376 MiB");

    BOOST_CHECK_EQUAL(test("fr_FR.UTF-8", 123456789, true, 4, false), "117.7376 MiB");
    BOOST_CHECK_EQUAL(test("fr_FR.UTF-8", 123456789, false, 4, false), "117,7376 Mio");

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 1000*1024, false, 2, false), "1,000.00 KiB");
    BOOST_CHECK_EQUAL(test("de_DE.UTF-8", 1000*1024, false, 2, false), "1.000,00 KiB");
    BOOST_CHECK_EQUAL(test("de_CH.UTF-8", 1000*1024, false, 2, false), "1'000.00 KiB");
    BOOST_CHECK_EQUAL(test("fr_FR.UTF-8", 1000*1024, false, 2, false), "1 000,00 Kio");

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 50 * MiB, false, 2, false), "50.00 MiB");
    BOOST_CHECK_EQUAL(test("de_DE.UTF-8", 50 * MiB, false, 2, false), "50,00 MiB");
    BOOST_CHECK_EQUAL(test("de_CH.UTF-8", 50 * MiB, false, 2, false), "50.00 MiB");
    BOOST_CHECK_EQUAL(test("fr_FR.UTF-8", 50 * MiB, false, 2, false), "50,00 Mio");
}


BOOST_AUTO_TEST_CASE(test_humanstring_to_byte)
{
    BOOST_CHECK_THROW(test("en_GB.UTF-8", "hello", true), ParseException);

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "0 B", true), 0);
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "-0 B", true), 0);
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "+0 B", true), 0);

    BOOST_CHECK_THROW(test("en_GB.UTF-8", "42", true), ParseException); // classic=true needs a suffix
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "42B", true), 42);
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "42 b", true), 42);

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "42", false), 42);
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "42b", false), 42);
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "42 B", false), 42);

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "12.4GB", true), 13314398618);
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "12.4 GB", true), 13314398618);

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "12.4GB", false), 13314398618);
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "12.4 gb", false), 13314398618);
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "12.4g", false), 13314398618);
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "12.4 G", false), 13314398618);

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "123,456 kB", false), 126418944);
    BOOST_CHECK_EQUAL(test("de_DE.UTF-8", "123.456 kB", false), 126418944);
    BOOST_CHECK_EQUAL(test("de_CH.UTF-8", "123'456 kB", false), 126418944);
    BOOST_CHECK_EQUAL(test("fr_FR.UTF-8", "123 456 ko", false), 126418944);

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "123,456.789kB", false), 126419752);
    BOOST_CHECK_EQUAL(test("de_DE.UTF-8", "123.456,789kB", false), 126419752);
    BOOST_CHECK_EQUAL(test("de_CH.UTF-8", "123'456.789kB", false), 126419752);
    BOOST_CHECK_EQUAL(test("fr_FR.UTF-8", "123 456,789ko", false), 126419752);

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "123,456.789 kB", false), 126419752);
    BOOST_CHECK_EQUAL(test("de_DE.UTF-8", "123.456,789 kB", false), 126419752);
    BOOST_CHECK_EQUAL(test("de_CH.UTF-8", "123'456.789 kB", false), 126419752);
    BOOST_CHECK_EQUAL(test("fr_FR.UTF-8", "123 456,789 ko", false), 126419752);

    BOOST_CHECK_EQUAL(test("fr_FR.UTF-8", "5Go", false), 5368709120);
    BOOST_CHECK_EQUAL(test("fr_FR.UTF-8", "5 Go", false), 5368709120);

    BOOST_CHECK_THROW(test("en_US.UTF-8", "5 G B", false), ParseException);
    BOOST_CHECK_THROW(test("de_DE.UTF-8", "12.34 kB", false), ParseException);
    BOOST_CHECK_THROW(test("de_DE.UTF-8", "12'34 kB", false), ParseException);
    BOOST_CHECK_THROW(test("fr_FR.UTF-8", "12 34 Go", false), ParseException);

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "3.14 G", false), 3371549327);
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "3.14 GB", false), 3371549327);
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "3.14 GiB", false), 3371549327);

    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "12345 GB", false), 13255342817280);
    BOOST_CHECK_EQUAL(test("de_DE.UTF-8", "12345 GB", false), 13255342817280);
    BOOST_CHECK_EQUAL(test("de_CH.UTF-8", "12345 GB", false), 13255342817280);
    BOOST_CHECK_EQUAL(test("fr_FR.UTF-8", "12345 Go", false), 13255342817280);
}


BOOST_AUTO_TEST_CASE(test_big_numbers)
{
    // 1 EiB - 1 B
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 1 * EiB - 1 * B, true, 2, true), "1024.00 PiB");

    // 1 EiB
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 1 * EiB, true, 2, true), "1 EiB");
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 1 * EiB, true, 2, false), "1.00 EiB");
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "1 EiB", true), 1 * EiB);
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "1.00 EiB", true), 1 * EiB);

    // 1 EiB + 1 B
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 1 * EiB + 1 * B, true, 2, true), "1.00 EiB");

    // 16 EiB - 1 B
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", 16 * EiB - 1 * B, true, 2, true), "16.00 EiB");
    BOOST_CHECK_EQUAL(test("en_GB.UTF-8", "18446744073709551615 B", true), 16 * EiB - 1 * B);

    // 16 EiB
    BOOST_CHECK_THROW(test("en_GB.UTF-8", "16 EiB", true), OverflowException);
    BOOST_CHECK_THROW(test("en_GB.UTF-8", "18446744073709551616 B", true), OverflowException);
}


BOOST_AUTO_TEST_CASE(test_negative_numbers)
{
    BOOST_CHECK_THROW(test("en_GB.UTF-8", "-1 B", false), OverflowException);
    BOOST_CHECK_THROW(test("en_GB.UTF-8", "-1.0 B", false), OverflowException);
}
