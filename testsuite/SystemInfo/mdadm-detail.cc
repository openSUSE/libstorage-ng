
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdMdadm.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const string& device, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command({ MDADM_BIN, "--detail", device, "--export" }, input);

    CmdMdadmDetail cmd_mdadm_detail(device);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_mdadm_detail;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"MD_LEVEL=raid1",
	"MD_DEVICES=2",
	"MD_METADATA=1.0",
	"MD_UUID=35dd06d4:b4e9e248:9262c3ad:02b61654",
	"MD_NAME=linux:0",
	"MD_DEVICE_sda1_ROLE=0",
	"MD_DEVICE_sda1_DEV=/dev/sda1",
	"MD_DEVICE_sdb1_ROLE=1",
	"MD_DEVICE_sdb1_DEV=/dev/sdb1",
    };

    vector<string> output = {
	"device:/dev/md0 uuid:35dd06d4:b4e9e248:9262c3ad:02b61654 devname: metadata:1.0 level:RAID1 roles:</dev/sda1:0 /dev/sdb1:1>"
    };

    check("/dev/md0", input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"MD_LEVEL=raid10",
	"MD_DEVICES=4",
	"MD_METADATA=1.2",
	"MD_UUID=bc9eb9bb:cba04711:7e4155f7:808630e7",
	"MD_DEVNAME=test",
	"MD_NAME=kassandra:test",
	"MD_DEVICE_ev_sdd2_ROLE=3",
	"MD_DEVICE_ev_sdd2_DEV=/dev/sdd2",
	"MD_DEVICE_ev_sdc2_ROLE=2",
	"MD_DEVICE_ev_sdc2_DEV=/dev/sdc2",
	"MD_DEVICE_ev_sdd3_ROLE=1",
	"MD_DEVICE_ev_sdd3_DEV=/dev/sdd3",
	"MD_DEVICE_ev_sdc3_ROLE=spare",
	"MD_DEVICE_ev_sdc3_DEV=/dev/sdc3",
	"MD_DEVICE_ev_sdc1_ROLE=0",
	"MD_DEVICE_ev_sdc1_DEV=/dev/sdc1",
    };

    vector<string> output = {
	"device:/dev/md/test uuid:bc9eb9bb:cba04711:7e4155f7:808630e7 devname:test metadata:1.2 level:RAID10 roles:</dev/sdc1:0 /dev/sdc2:2 /dev/sdc3:spare /dev/sdd2:3 /dev/sdd3:1>"
    };

    check("/dev/md/test", input, output);
}
