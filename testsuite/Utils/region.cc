
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

    BOOST_CHECK_EQUAL(region.to_bytes(region.get_start()), 1 * MiB);
    BOOST_CHECK_EQUAL(region.to_bytes(region.get_length()), 821035008);
    BOOST_CHECK_EQUAL(region.to_bytes(region.get_end()), 822083072);

    BOOST_CHECK_EQUAL(region.to_blocks(1 * MiB), region.get_start());
}


BOOST_AUTO_TEST_CASE(test_block_size_4096)
{
    Region region(256, 65280, 4096);

    BOOST_CHECK(!region.empty());

    BOOST_CHECK_EQUAL(region.get_start(), 256);
    BOOST_CHECK_EQUAL(region.get_length(), 65280);
    BOOST_CHECK_EQUAL(region.get_end(), 65535);

    BOOST_CHECK_EQUAL(region.get_block_size(), 4096);

    BOOST_CHECK_EQUAL(region.to_bytes(region.get_start()), 1 * MiB);
    BOOST_CHECK_EQUAL(region.to_bytes(region.get_length()), 261120 * KiB);
    BOOST_CHECK_EQUAL(region.to_bytes(region.get_end()), 262140 * KiB);

    BOOST_CHECK_EQUAL(region.to_blocks(1 * MiB), region.get_start());
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


BOOST_AUTO_TEST_CASE(test_big_numbers)
{
    Region kb(0, 1ULL << (10 * 5), 1 * KiB);

    BOOST_CHECK_EQUAL(kb.to_bytes(kb.get_length()), EiB);

    Region mb(0, 1ULL << (10 * 4), 1 * MiB);

    BOOST_CHECK_EQUAL(mb.to_bytes(mb.get_length()), EiB);
}


BOOST_AUTO_TEST_CASE(test_unused_regions1)
{
    Region region(0, 100, 1);

    vector<Region> unused_regions = region.unused_regions({
	Region(30, 50, 1),
    });

    BOOST_CHECK_EQUAL(unused_regions.size(), 2);

    BOOST_CHECK_EQUAL(unused_regions[0].get_start(), 0);
    BOOST_CHECK_EQUAL(unused_regions[0].get_length(), 30);

    BOOST_CHECK_EQUAL(unused_regions[1].get_start(), 80);
    BOOST_CHECK_EQUAL(unused_regions[1].get_length(), 20);
}


BOOST_AUTO_TEST_CASE(test_unused_regions2)
{
    Region region(0, 100, 1);

    vector<Region> unused_regions = region.unused_regions({
	Region(0, 10, 1),
	Region(90, 10, 1)
    });

    BOOST_CHECK_EQUAL(unused_regions.size(), 1);

    BOOST_CHECK_EQUAL(unused_regions[0].get_start(), 10);
    BOOST_CHECK_EQUAL(unused_regions[0].get_length(), 80);
}


BOOST_AUTO_TEST_CASE(test_unused_regions3)
{
    Region region(0, 100, 1);

    vector<Region> unused_regions = region.unused_regions({
	Region(90, 10, 1),	// sorted incorrectly with other used region
	Region(0, 10, 1)	// sorted incorrectly with other used region
    });

    BOOST_CHECK_EQUAL(unused_regions.size(), 1);

    BOOST_CHECK_EQUAL(unused_regions[0].get_start(), 10);
    BOOST_CHECK_EQUAL(unused_regions[0].get_length(), 80);
}


BOOST_AUTO_TEST_CASE(test_unused_regions4)
{
    Region region(0, 100, 1);

    vector<Region> unused_regions = region.unused_regions({
	Region(10, 10, 1),
	Region(40, 20, 1),
	Region(80, 15, 1)
    });

    BOOST_CHECK_EQUAL(unused_regions.size(), 4);

    BOOST_CHECK_EQUAL(unused_regions[0].get_start(), 0);
    BOOST_CHECK_EQUAL(unused_regions[0].get_length(), 10);

    BOOST_CHECK_EQUAL(unused_regions[1].get_start(), 20);
    BOOST_CHECK_EQUAL(unused_regions[1].get_length(), 20);

    BOOST_CHECK_EQUAL(unused_regions[2].get_start(), 60);
    BOOST_CHECK_EQUAL(unused_regions[2].get_length(), 20);

    BOOST_CHECK_EQUAL(unused_regions[3].get_start(), 95);
    BOOST_CHECK_EQUAL(unused_regions[3].get_length(), 5);
}


BOOST_AUTO_TEST_CASE(test_unused_regions5)
{
    Region region(10, 80, 1);

    vector<Region> unused_regions = region.unused_regions({
	Region(0, 10, 1),	// completely before region
	Region(40, 20, 1),
	Region(90, 10, 1)	// completely behind region
    });

    BOOST_CHECK_EQUAL(unused_regions.size(), 2);

    BOOST_CHECK_EQUAL(unused_regions[0].get_start(), 10);
    BOOST_CHECK_EQUAL(unused_regions[0].get_length(), 30);

    BOOST_CHECK_EQUAL(unused_regions[1].get_start(), 60);
    BOOST_CHECK_EQUAL(unused_regions[1].get_length(), 30);
}


BOOST_AUTO_TEST_CASE(test_unused_regions6)
{
    Region region(10, 80, 1);

    vector<Region> unused_regions = region.unused_regions({
	Region(0, 5, 1),	// completely before region
	Region(40, 20, 1),
	Region(95, 5, 1)	// completely behind region
    });

    BOOST_CHECK_EQUAL(unused_regions.size(), 2);

    BOOST_CHECK_EQUAL(unused_regions[0].get_start(), 10);
    BOOST_CHECK_EQUAL(unused_regions[0].get_length(), 30);

    BOOST_CHECK_EQUAL(unused_regions[1].get_start(), 60);
    BOOST_CHECK_EQUAL(unused_regions[1].get_length(), 30);
}


BOOST_AUTO_TEST_CASE(test_unused_regions7)
{
    Region region(10, 80, 1);

    vector<Region> unused_regions = region.unused_regions({
	Region(0, 20, 1),	// partly before region
	Region(40, 20, 1),
	Region(80, 20, 1)	// partly behind region
    });

    BOOST_CHECK_EQUAL(unused_regions.size(), 2);

    BOOST_CHECK_EQUAL(unused_regions[0].get_start(), 20);
    BOOST_CHECK_EQUAL(unused_regions[0].get_length(), 20);

    BOOST_CHECK_EQUAL(unused_regions[1].get_start(), 60);
    BOOST_CHECK_EQUAL(unused_regions[1].get_length(), 20);
}


BOOST_AUTO_TEST_CASE(test_unused_regions8)
{
    Region region(0, 100, 1);

    vector<Region> unused_regions = region.unused_regions({
	Region(20, 40, 1),	// overlapping with other used region
	Region(40, 40, 1)	// overlapping with other used region
    });

    BOOST_CHECK_EQUAL(unused_regions.size(), 2);

    BOOST_CHECK_EQUAL(unused_regions[0].get_start(), 0);
    BOOST_CHECK_EQUAL(unused_regions[0].get_length(), 20);

    BOOST_CHECK_EQUAL(unused_regions[1].get_start(), 80);
    BOOST_CHECK_EQUAL(unused_regions[1].get_length(), 20);
}


BOOST_AUTO_TEST_CASE(test_unused_regions9)
{
    Region region(0, 100, 1);

    vector<Region> unused_regions = region.unused_regions({
	Region(20, 60, 1),	// overlapping with other used region
	Region(40, 20, 1)	// overlapping with other used region
    });

    BOOST_CHECK_EQUAL(unused_regions.size(), 2);

    BOOST_CHECK_EQUAL(unused_regions[0].get_start(), 0);
    BOOST_CHECK_EQUAL(unused_regions[0].get_length(), 20);

    BOOST_CHECK_EQUAL(unused_regions[1].get_start(), 80);
    BOOST_CHECK_EQUAL(unused_regions[1].get_length(), 20);
}
