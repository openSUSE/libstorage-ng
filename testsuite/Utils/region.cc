
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_output)
{
    Region region(2048, 1603584, 512);

    ostringstream out;
    out << region;

    BOOST_CHECK_EQUAL(out.str(), "[2048, 1603584, 512 B]");
}


BOOST_AUTO_TEST_CASE(test_block_size_512)
{
    Region region(2048, 1603584, 512);

    BOOST_CHECK(!region.empty());

    BOOST_CHECK_EQUAL(region.get_start(), 2048);
    BOOST_CHECK_EQUAL(region.get_length(), 1603584);
    BOOST_CHECK_EQUAL(region.get_end(), 1605631);

    BOOST_CHECK_EQUAL(region.get_block_size(), 512);

    BOOST_CHECK_EQUAL(region.to_kb(region.get_start()), 1024);
    BOOST_CHECK_EQUAL(region.to_kb(region.get_length()), 801792);
    BOOST_CHECK_EQUAL(region.to_kb(region.get_end()), 802815);

    BOOST_CHECK_EQUAL(region.to_value(1024), region.get_start());
}


BOOST_AUTO_TEST_CASE(test_block_size_4096)
{
    Region region(256, 65280, 4096);

    BOOST_CHECK(!region.empty());

    BOOST_CHECK_EQUAL(region.get_start(), 256);
    BOOST_CHECK_EQUAL(region.get_length(), 65280);
    BOOST_CHECK_EQUAL(region.get_end(), 65535);

    BOOST_CHECK_EQUAL(region.get_block_size(), 4096);

    BOOST_CHECK_EQUAL(region.to_kb(region.get_start()), 1024);
    BOOST_CHECK_EQUAL(region.to_kb(region.get_length()), 261120);
    BOOST_CHECK_EQUAL(region.to_kb(region.get_end()), 262140);

    BOOST_CHECK_EQUAL(region.to_value(1024), region.get_start());
}


BOOST_AUTO_TEST_CASE(test_set_values)
{
    Region region(0, 1024, 32);

    BOOST_CHECK_EQUAL(region.get_start(), 0);
    BOOST_CHECK_EQUAL(region.get_length(), 1024);
    BOOST_CHECK_EQUAL(region.get_end(), 1023);
    BOOST_CHECK_EQUAL(region.get_block_size(), 32);

    // setting the start does not change the length but the end

    region.set_start(256);

    BOOST_CHECK_EQUAL(region.get_start(), 256);
    BOOST_CHECK_EQUAL(region.get_length(), 1024);
    BOOST_CHECK_EQUAL(region.get_end(), 1279);
    BOOST_CHECK_EQUAL(region.get_block_size(), 32);

    // setting the length does not change the start but the end

    region.set_length(512);

    BOOST_CHECK_EQUAL(region.get_start(), 256);
    BOOST_CHECK_EQUAL(region.get_length(), 512);
    BOOST_CHECK_EQUAL(region.get_end(), 767);
    BOOST_CHECK_EQUAL(region.get_block_size(), 32);

    // setting the block-size does not change the start, length nor end

    region.set_block_size(64);

    BOOST_CHECK_EQUAL(region.get_start(), 256);
    BOOST_CHECK_EQUAL(region.get_length(), 512);
    BOOST_CHECK_EQUAL(region.get_end(), 767);
    BOOST_CHECK_EQUAL(region.get_block_size(), 64);
}


BOOST_AUTO_TEST_CASE(test_comparisons)
{
    // Four regions with block layout:
    //
    // aaaaaaaaaa
    //           bbbbbbbbbb
    //      cccccccccc
    // dddddddddddddddddddd

    Region a(0, 10, 1);
    Region b(10, 10, 1);
    Region c(5, 10, 1);
    Region d(0, 20, 1);

    BOOST_CHECK(a == a);
    BOOST_CHECK(a != b);

    BOOST_CHECK(a < b);
    BOOST_CHECK(c > d);

    BOOST_CHECK(a.inside(d));
    BOOST_CHECK(!a.inside(c));

    BOOST_CHECK(a.intersect(c));
    BOOST_CHECK(!a.intersect(b));

    BOOST_CHECK(a.intersection(c) == Region(5, 5, 1));
    BOOST_CHECK_THROW(a.intersection(b), NoIntersection);
}


BOOST_AUTO_TEST_CASE(test_copy_constructor)
{
    Region a(0, 10, 1);

    Region b(a);
    b.set_start(1);

    BOOST_CHECK_EQUAL(a.get_start(), 0);
    BOOST_CHECK_EQUAL(b.get_start(), 1);
}


BOOST_AUTO_TEST_CASE(test_copy_assignment)
{
    Region a(0, 10, 1);

    Region b;
    (b = a).set_start(1);

    BOOST_CHECK_EQUAL(a.get_start(), 0);
    BOOST_CHECK_EQUAL(b.get_start(), 1);
}


BOOST_AUTO_TEST_CASE(test_invalid_block_size)
{
    BOOST_CHECK_THROW(Region a(1, 2, 0), InvalidBlockSize);
}


BOOST_AUTO_TEST_CASE(test_different_block_size)
{
    Region a(0, 100, 1);
    Region b(0, 50, 2);

    BOOST_CHECK_THROW((void)(a == b), DifferentBlockSizes);
}


BOOST_AUTO_TEST_CASE(test_big_kb)
{
    Region r(0, 0, 512);

    // currently an intermediate result is in bytes and 64 bit so larger
    // numbers are not possible

    BOOST_CHECK_EQUAL(r.to_value(1ULL << 53) - 1, (1ULL << 54) - 1);
    BOOST_CHECK_EQUAL(r.to_kb(1ULL << 54) - 1, (1ULL << 53) - 1);
}


BOOST_AUTO_TEST_CASE(test_big_numbers)
{
    unsigned long long EiB = 1ULL << (10 * 6);

    Region kb(0, 1ULL << (10 * 5), 1ULL << (10 * 1));

    BOOST_CHECK_EQUAL(kb.to_kb(kb.get_length()), EiB / 1024);
    BOOST_CHECK_EQUAL(byte_to_humanstring(kb.to_kb(kb.get_length()) * 1024, true, 1, false), "1.0 EiB");

    Region mb(0, 1ULL << (10 * 4), 1ULL << (10 * 2));

    BOOST_CHECK_EQUAL(mb.to_kb(mb.get_length()), EiB / 1024);
    BOOST_CHECK_EQUAL(byte_to_humanstring(mb.to_kb(mb.get_length()) * 1024, true, 1, false), "1.0 EiB");
}
