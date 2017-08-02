
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/EtcMdadm.h"
#include "storage/Utils/Mockup.h"


using namespace std;
using namespace storage;


void
setup(const vector<string>& etc_mdadm)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);

    Mockup::set_file("/etc/mdadm.conf", etc_mdadm);
}


void
check(const vector<string>& etc_mdadm)
{
    Mockup::File file = Mockup::get_file("/etc/mdadm.conf");

    string lhs = boost::join(file.content, "\n");
    string rhs = boost::join(etc_mdadm, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(has1)
{
    setup({
	"DEVICE containers partitions",
	"ARRAY /dev/md0 UUID=0a278ebc:9aea4c40:554a5f39:b52224a7",
	"ARRAY /dev/md1 UUID=0a1750eb:b5efbc17:b0bb6de2:b707a04f"
    });

    EtcMdadm etc_mdadm;

    BOOST_CHECK(etc_mdadm.has_entry("0a278ebc:9aea4c40:554a5f39:b52224a7"));
    BOOST_CHECK(!etc_mdadm.has_entry("deadbeef:9aea4c40:554a5f39:b52224a7"));
}


BOOST_AUTO_TEST_CASE(update1)
{
    setup({ });

    EtcMdadm etc_mdadm;

    EtcMdadm::Entry entry;
    entry.device = "/dev/md0";
    entry.uuid = "0a278ebc:9aea4c40:554a5f39:b52224a7";

    etc_mdadm.update_entry(entry);

    check({
	"ARRAY /dev/md0 UUID=0a278ebc:9aea4c40:554a5f39:b52224a7"
    });
}


BOOST_AUTO_TEST_CASE(update2)
{
    setup({ });

    EtcMdadm etc_mdadm;

    EtcMdadm::Entry entry1;
    entry1.uuid = "9087e240:1a2f2dfe:85189535:b0c0ebc5";
    entry1.metadata = "imsm";

    EtcMdadm::Entry entry2;
    entry2.device = "/dev/md/126";
    entry2.uuid = "24d20d34:3c2dc232:37f18a24:76282016";
    entry2.container_uuid = "9087e240:1a2f2dfe:85189535:b0c0ebc5";
    entry2.container_member = "0";

    etc_mdadm.update_entry(entry1);
    etc_mdadm.update_entry(entry2);

    check({
	"ARRAY metadata=imsm UUID=9087e240:1a2f2dfe:85189535:b0c0ebc5",
	"ARRAY /dev/md/126 container=9087e240:1a2f2dfe:85189535:b0c0ebc5 member=0 UUID=24d20d34:3c2dc232:37f18a24:76282016"
    });
}


BOOST_AUTO_TEST_CASE(remove1)
{
    setup({
        "DEVICE containers partitions",
        "ARRAY /dev/md0 UUID=0a278ebc:9aea4c40:554a5f39:b52224a7",
        "ARRAY /dev/md1 UUID=0a1750eb:b5efbc17:b0bb6de2:b707a04f"
    });

    EtcMdadm etc_mdadm;

    etc_mdadm.remove_entry("0a278ebc:9aea4c40:554a5f39:b52224a7");

    check({
        "DEVICE containers partitions",
        "ARRAY /dev/md1 UUID=0a1750eb:b5efbc17:b0bb6de2:b707a04f"
    });
}
