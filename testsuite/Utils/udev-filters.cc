
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include <iostream>

#include "storage/Utils/UdevFilters.h"


using namespace std;
using namespace storage;


namespace std
{
    ostream& operator<<(ostream& s, const vector<string>& names)
    {
        s << "{";
        for (vector<string>::const_iterator it = names.begin(); it != names.end(); ++it)
            s << (it == names.begin() ? " " : ", ") << *it;
        s << " }";

        return s;
    }
}


BOOST_AUTO_TEST_CASE(test1)
{
    const vector<string> patterns = { "scsi-*", "ata-*" };

    vector<string> names = { "ata-456", "ata-123", "scsi-123", "scsi-123.0", "lol-123" };
    udev_filter(names, patterns);

    BOOST_CHECK_EQUAL(names, vector<string>({ "scsi-123", "scsi-123.0", "ata-456", "ata-123" }));
}


BOOST_AUTO_TEST_CASE(test2)
{
    const vector<string> patterns = { "ata-*" };

    vector<string> names = { "ata-123", "lol-123", "ata-123", "lol-123" };
    udev_filter(names, patterns);

    BOOST_CHECK_EQUAL(names, vector<string>({ "ata-123", "ata-123" }));
}


BOOST_AUTO_TEST_CASE(test3)
{
    const vector<string> patterns = { "scsi-ata-*", "scsi-*" };

    vector<string> names = { "scsi-ata-123", "lol-123" };
    udev_filter(names, patterns);

    BOOST_CHECK_EQUAL(names, vector<string>({ "scsi-ata-123" }));
}


BOOST_AUTO_TEST_CASE(test4)
{
    const vector<string> patterns = { "scsi.*" };

    vector<string> names = { "scsi.123", "lol-123" };
    udev_filter(names, patterns);

    BOOST_CHECK_EQUAL(names, vector<string>({ "scsi.123" }));
}


BOOST_AUTO_TEST_CASE(test5)
{
    const vector<string> patterns = { "scsi.*" };

    vector<string> names = { "scsi.123", "lol-123" };
    udev_filter(names, patterns);

    BOOST_CHECK_EQUAL(names, vector<string>({ "scsi.123" }));
}


BOOST_AUTO_TEST_CASE(test6)
{
    const vector<string> patterns = { "scsi-[123]ATA-*" };

    vector<string> names = { "scsi-1ATA-123", "scsi-4ATA-123", "lol-123" };
    udev_filter(names, patterns);

    BOOST_CHECK_EQUAL(names, vector<string>({ "scsi-1ATA-123" }));
}


BOOST_AUTO_TEST_CASE(test7)
{
    const vector<string> patterns = { "scsi-?ATA-*" };

    vector<string> names = { "scsi-2ATA-123", "scsi-1ATA-123", "lol-123" };
    udev_filter(names, patterns);

    BOOST_CHECK_EQUAL(names, vector<string>({ "scsi-2ATA-123", "scsi-1ATA-123" }));
}


BOOST_AUTO_TEST_CASE(test8)
{
    const vector<string> patterns = { "ata-*-x" };

    vector<string> names = { "ata-123-x", "ata-123-y", "sata-123-x" };
    udev_filter(names, patterns);

    BOOST_CHECK_EQUAL(names, vector<string>({ "ata-123-x" }));
}
