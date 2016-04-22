
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Utils/Topology.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_output)
{
    Topology topology(-512, 4096);

    ostringstream out;
    out << topology;

    BOOST_CHECK_EQUAL(out.str(), "[-512 B, 4096 B]");
}


BOOST_AUTO_TEST_CASE(test_grain)
{
    BOOST_CHECK_EQUAL(Topology(0, 0).calculate_grain(), 1024 * 1024);
    BOOST_CHECK_EQUAL(Topology(-512, 4096).calculate_grain(), 1024 * 1024);

    BOOST_CHECK_EQUAL(Topology(0, 32 * 1024).calculate_grain(), 1024 * 1024);
    BOOST_CHECK_EQUAL(Topology(0, 1024 * 1024).calculate_grain(), 1024 * 1024);
    BOOST_CHECK_EQUAL(Topology(0, 8 * 1024 * 1024).calculate_grain(), 8 * 1024 * 1024);

    BOOST_CHECK_EQUAL(Topology(0, 768 * 1024).calculate_grain(), 1536 * 1024);
}


BOOST_AUTO_TEST_CASE(test_align1)
{
    Topology topology(0, 0);

    BOOST_CHECK_EQUAL(topology.align(Region(0, 10000, 512), AlignPolicy::ALIGN_END), Region(0, 8192, 512));
    BOOST_CHECK_EQUAL(topology.align(Region(1, 10000, 512), AlignPolicy::ALIGN_END), Region(2048, 6144, 512));

    BOOST_CHECK_EQUAL(topology.align(Region(2048, 10239, 512), AlignPolicy::ALIGN_END), Region(2048, 8192, 512));
    BOOST_CHECK_EQUAL(topology.align(Region(2048, 10240, 512), AlignPolicy::ALIGN_END), Region(2048, 10240, 512));
    BOOST_CHECK_EQUAL(topology.align(Region(2048, 10241, 512), AlignPolicy::ALIGN_END), Region(2048, 10240, 512));

    BOOST_CHECK_EQUAL(topology.align(Region(0, 10000, 512), AlignPolicy::KEEP_END), Region(0, 10000, 512));
    BOOST_CHECK_EQUAL(topology.align(Region(1, 10000, 512), AlignPolicy::KEEP_END), Region(2048, 7953, 512));

    BOOST_CHECK_EQUAL(topology.align(Region(0, 10000, 512), AlignPolicy::KEEP_SIZE), Region(0, 10000, 512));
    BOOST_CHECK_EQUAL(topology.align(Region(1, 10000, 512), AlignPolicy::KEEP_SIZE), Region(2048, 10000, 512));
}


BOOST_AUTO_TEST_CASE(test_align2)
{
    Topology topology(0, 0);

    BOOST_CHECK_EQUAL(topology.can_be_aligned(Region(1, 4094, 512), AlignPolicy::ALIGN_END), false);
    BOOST_CHECK_THROW(topology.align(Region(1, 4094, 512), AlignPolicy::ALIGN_END), AlignError);

    BOOST_CHECK_EQUAL(topology.can_be_aligned(Region(1, 4095, 512), AlignPolicy::ALIGN_END), true);
    BOOST_CHECK_EQUAL(topology.align(Region(1, 4095, 512), AlignPolicy::ALIGN_END), Region(2048, 2048, 512));
}



BOOST_AUTO_TEST_CASE(test_align_with_offset)
{
    Topology topology(4 * KiB - 512 * B, 0);

    BOOST_CHECK_EQUAL(topology.align(Region(0, 10000, 512), AlignPolicy::KEEP_SIZE), Region(7, 10000, 512));
    BOOST_CHECK_EQUAL(topology.align(Region(7, 10000, 512), AlignPolicy::KEEP_SIZE), Region(7, 10000, 512));
}
