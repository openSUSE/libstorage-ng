
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Devices/Md.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test1)
{
    BOOST_CHECK_CLOSE(Md::calculate_underlying_size(MdLevel::RAID0, 4, 1 * TiB), 1.0 * TiB / 4 + 128.0 * MiB, 0.1);
    BOOST_CHECK_CLOSE(Md::calculate_underlying_size(MdLevel::RAID1, 2, 1 * TiB), 1.0 * TiB + 128.0 * MiB, 0.1);
    BOOST_CHECK_CLOSE(Md::calculate_underlying_size(MdLevel::RAID5, 4, 1 * TiB), 1.0 * TiB / 3 + 128.0 * MiB, 0.1);
    BOOST_CHECK_CLOSE(Md::calculate_underlying_size(MdLevel::RAID6, 4, 1 * TiB), 1.0 * TiB / 2 + 128.0 * MiB, 0.1);
    BOOST_CHECK_CLOSE(Md::calculate_underlying_size(MdLevel::RAID10, 4, 1 * TiB), 1.0 * TiB / 2 + 128.0 * MiB, 0.1);
}


BOOST_AUTO_TEST_CASE(test2)
{
    BOOST_CHECK_EXCEPTION(Md::calculate_underlying_size(MdLevel::UNKNOWN, 2, 1 * TiB), Exception,
	[](const Exception& e) { return e.what() == "illegal raid level"s; }
    );

    BOOST_CHECK_EXCEPTION(Md::calculate_underlying_size(MdLevel::RAID5, 2, 1 * TiB), Exception,
	[](const Exception& e) { return e.what() == "too few devices"s; }
    );
}
