
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Utils/AlignmentImpl.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_grain)
{
    BOOST_CHECK_EQUAL(Alignment(Topology(0, 0 * B)).get_impl().calculate_grain(), 1 * MiB);
    BOOST_CHECK_EQUAL(Alignment(Topology(-512, 4 * KiB)).get_impl().calculate_grain(), 1 * MiB);

    BOOST_CHECK_EQUAL(Alignment(Topology(0, 32 * KiB)).get_impl().calculate_grain(), 1 * MiB);
    BOOST_CHECK_EQUAL(Alignment(Topology(0, 1 * MiB)).get_impl().calculate_grain(), 1 * MiB);
    BOOST_CHECK_EQUAL(Alignment(Topology(0, 8 * MiB)).get_impl().calculate_grain(), 8 * MiB);

    BOOST_CHECK_EQUAL(Alignment(Topology(0, 768 * KiB)).get_impl().calculate_grain(), 1536 * KiB);
}


BOOST_AUTO_TEST_CASE(test_align_block_in_place)
{
    const Alignment alignment(Topology(0, 0));

    unsigned long block_size = 512;

    unsigned long long block;

    // Location::START

    block = 0;
    BOOST_CHECK(alignment.get_impl().align_block_in_place(block, block_size, Alignment::Impl::Location::START));
    BOOST_CHECK_EQUAL(block, 0);

    block = 1;
    BOOST_CHECK(alignment.get_impl().align_block_in_place(block, block_size, Alignment::Impl::Location::START));
    BOOST_CHECK_EQUAL(block, 2048);

    block = 2048;
    BOOST_CHECK(alignment.get_impl().align_block_in_place(block, block_size, Alignment::Impl::Location::START));
    BOOST_CHECK_EQUAL(block, 2048);

    block = 2049;
    BOOST_CHECK(alignment.get_impl().align_block_in_place(block, block_size, Alignment::Impl::Location::START));
    BOOST_CHECK_EQUAL(block, 4096);

    // Location::END

    block = 2046;
    BOOST_CHECK(!alignment.get_impl().align_block_in_place(block, block_size, Alignment::Impl::Location::END));

    block = 2047;
    BOOST_CHECK(alignment.get_impl().align_block_in_place(block, block_size, Alignment::Impl::Location::END));
    BOOST_CHECK_EQUAL(block, 2047);

    block = 2048;
    BOOST_CHECK(alignment.get_impl().align_block_in_place(block, block_size, Alignment::Impl::Location::END));
    BOOST_CHECK_EQUAL(block, 2047);

    block = 4096;
    BOOST_CHECK(alignment.get_impl().align_block_in_place(block, block_size, Alignment::Impl::Location::END));
    BOOST_CHECK_EQUAL(block, 4095);
}


BOOST_AUTO_TEST_CASE(test_align_region_in_place)
{
    const Alignment alignment(Topology(0, 0));

    Region region;

    // AlignPolicy::ALIGN_END

    region = Region(0, 2047, 512);
    BOOST_CHECK(!alignment.get_impl().align_region_in_place(region, AlignPolicy::ALIGN_END));

    region = Region(1, 2048, 512);
    BOOST_CHECK(!alignment.get_impl().align_region_in_place(region, AlignPolicy::ALIGN_END));

    region = Region(0, 2048, 512);
    BOOST_CHECK(alignment.get_impl().align_region_in_place(region, AlignPolicy::ALIGN_END));
    BOOST_CHECK_EQUAL(region, Region(0, 2048, 512));

    // AlignPolicy::KEEP_SIZE

    region = Region(0, 2048, 512);
    BOOST_CHECK(alignment.get_impl().align_region_in_place(region, AlignPolicy::KEEP_SIZE));
    BOOST_CHECK_EQUAL(region, Region(0, 2048, 512));

    region = Region(1, 2048, 512);
    BOOST_CHECK(alignment.get_impl().align_region_in_place(region, AlignPolicy::KEEP_SIZE));
    BOOST_CHECK_EQUAL(region, Region(2048, 2048, 512));

    // AlignPolicy::KEEP_END

    region = Region(0, 2048, 512);
    BOOST_CHECK(alignment.get_impl().align_region_in_place(region, AlignPolicy::KEEP_END));
    BOOST_CHECK_EQUAL(region, Region(0, 2048, 512));

    region = Region(0, 2047, 512);
    BOOST_CHECK(alignment.get_impl().align_region_in_place(region, AlignPolicy::KEEP_END));
    BOOST_CHECK_EQUAL(region, Region(0, 2047, 512));

    region = Region(1, 2047, 512);
    BOOST_CHECK(!alignment.get_impl().align_region_in_place(region, AlignPolicy::KEEP_END));

    region = Region(1, 2048, 512);
    BOOST_CHECK(alignment.get_impl().align_region_in_place(region, AlignPolicy::KEEP_END));
    BOOST_CHECK_EQUAL(region, Region(2048, 1, 512));
}


BOOST_AUTO_TEST_CASE(test_align1)
{
    const Alignment alignment(Topology(0, 0));

    BOOST_CHECK_EQUAL(alignment.align(Region(0, 10000, 512), AlignPolicy::ALIGN_END), Region(0, 8192, 512));
    BOOST_CHECK_EQUAL(alignment.align(Region(1, 10000, 512), AlignPolicy::ALIGN_END), Region(2048, 6144, 512));

    BOOST_CHECK_EQUAL(alignment.align(Region(2048, 10239, 512), AlignPolicy::ALIGN_END), Region(2048, 8192, 512));
    BOOST_CHECK_EQUAL(alignment.align(Region(2048, 10240, 512), AlignPolicy::ALIGN_END), Region(2048, 10240, 512));
    BOOST_CHECK_EQUAL(alignment.align(Region(2048, 10241, 512), AlignPolicy::ALIGN_END), Region(2048, 10240, 512));

    BOOST_CHECK_EQUAL(alignment.align(Region(0, 10000, 512), AlignPolicy::KEEP_END), Region(0, 10000, 512));
    BOOST_CHECK_EQUAL(alignment.align(Region(1, 10000, 512), AlignPolicy::KEEP_END), Region(2048, 7953, 512));

    BOOST_CHECK_EQUAL(alignment.align(Region(0, 10000, 512), AlignPolicy::KEEP_SIZE), Region(0, 10000, 512));
    BOOST_CHECK_EQUAL(alignment.align(Region(1, 10000, 512), AlignPolicy::KEEP_SIZE), Region(2048, 10000, 512));
}


BOOST_AUTO_TEST_CASE(test_align2)
{
    const Alignment alignment(Topology(0, 0));

    BOOST_CHECK(!alignment.can_be_aligned(Region(1, 4094, 512), AlignPolicy::ALIGN_END));
    BOOST_CHECK_THROW(alignment.align(Region(1, 4094, 512), AlignPolicy::ALIGN_END), AlignError);

    BOOST_CHECK(alignment.can_be_aligned(Region(1, 4095, 512), AlignPolicy::ALIGN_END));
    BOOST_CHECK_EQUAL(alignment.align(Region(1, 4095, 512), AlignPolicy::ALIGN_END), Region(2048, 2048, 512));
}


BOOST_AUTO_TEST_CASE(test_align_with_offset)
{
    const Alignment alignment(Topology(4 * KiB - 512 * B, 0));

    BOOST_CHECK_EQUAL(alignment.align(Region(0, 10000, 512), AlignPolicy::KEEP_SIZE), Region(7, 10000, 512));
    BOOST_CHECK_EQUAL(alignment.align(Region(7, 10000, 512), AlignPolicy::KEEP_SIZE), Region(7, 10000, 512));
}
