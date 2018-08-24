
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdMultipath.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(MULTIPATHBIN " -d -v 2 -ll", input);

    CmdMultipath cmd_multipath;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_multipath;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"36005076305ffc73a00000000000013b5 dm-1 IBM,2107900",
	"size=1.0G features='1 queue_if_no_path' hwhandler='0' wp=rw",
	"`-+- policy='service-time 0' prio=1 status=active",
	"  |- 0:0:0:1085620243 sdb 8:16 active ready running",
	"  `- 0:0:1:1085620243 sdd 8:48 active ready running",
	"36005076305ffc73a00000000000013b4 dm-0 IBM,2107900",
	"size=1.0G features='1 queue_if_no_path' hwhandler='0' wp=rw",
	"`-+- policy='service-time 0' prio=1 status=active",
	"  |- 0:0:0:1085554707 sda 8:0  active ready running",
	"  `- 0:0:1:1085554707 sdc 8:32 active ready running"
    };

    vector<string> output = {
	"data[36005076305ffc73a00000000000013b4] -> vendor:IBM model:2107900 devices:</dev/sda /dev/sdc>",
	"data[36005076305ffc73a00000000000013b5] -> vendor:IBM model:2107900 devices:</dev/sdb /dev/sdd>"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"36005076305ffc73a00000000000013b4 dm-0 IBM,2107900",
	"size=1.0G features='1 retain_attached_hw_handler' hwhandler='1 alua' wp=rw",
	"`-+- policy='service-time 0' prio=50 status=active",
	"  |- 0:0:0:1085554707 sda 8:0  active ready running",
	"  `- 0:0:1:1085554707 sdc 8:32 active ready running",
	"some-alias (36005076305ffc73a00000000000013b5) dm-3 IBM,2107900",
	"size=1.0G features='1 retain_attached_hw_handler' hwhandler='1 alua' wp=rw",
	"`-+- policy='service-time 0' prio=50 status=active",
	"  |- 0:0:0:1085620243 sdb 8:16 active ready running",
	"  `- 0:0:1:1085620243 sdd 8:48 active ready running"
    };

    vector<string> output = {
	"data[36005076305ffc73a00000000000013b4] -> vendor:IBM model:2107900 devices:</dev/sda /dev/sdc>",
	"data[some-alias] -> vendor:IBM model:2107900 devices:</dev/sdb /dev/sdd>"
    };

    check(input, output);
}


// entries might be prefixed with ': '
BOOST_AUTO_TEST_CASE(parse3)
{
    vector<string> input = {
	": QEMU_HARDDISK_38ffda4567ec4e2785ab dm-0 ATA,QEMU HARDDISK",
	"size=60G features='0' hwhandler='0' wp=rw",
	"|-+- policy='service-time 0' prio=1 status=active",
	"| `- 0:0:0:0 sda 8:0  active ready running",
	"`-+- policy='service-time 0' prio=1 status=enabled",
	"  `- 0:0:1:0 sdb 8:16 active ready running"
    };

    vector<string> output = {
	"data[QEMU_HARDDISK_38ffda4567ec4e2785ab] -> vendor:ATA model:QEMU HARDDISK devices:</dev/sda /dev/sdb>"
    };

    check(input, output);
}


// entries might be prefixed with 'create: '
BOOST_AUTO_TEST_CASE(parse4)
{
    vector<string> input = {
	"create: QEMU_HARDDISK_38ffda4567ec4e2785ab dm-0 ATA,QEMU HARDDISK",
	"size=60G features='0' hwhandler='0' wp=rw",
	"|-+- policy='service-time 0' prio=1 status=active",
	"| `- 0:0:0:0 sda 8:0  active ready running",
	"`-+- policy='service-time 0' prio=1 status=enabled",
	"  `- 0:0:1:0 sdb 8:16 active ready running"
    };

    vector<string> output = {
	"data[QEMU_HARDDISK_38ffda4567ec4e2785ab] -> vendor:ATA model:QEMU HARDDISK devices:</dev/sda /dev/sdb>"
    };

    check(input, output);
}


// nvme entries are ignored
BOOST_AUTO_TEST_CASE(parse5)
{
    vector<string> input = {
	"nvme-subsys0:NQN:nqn.2014.08.org.nvmexpress:19e519e5032BEHFSH7001190    HWE36P43016M000N                         (nvme.19e5-30333242454846534837303031313930-48574533365034333031364d3030304e-00000001) [nvme]:nvme0n1 NVMe,HWE36P43016M000N,2.52",
	"size=3125627568 features='n/a' hwhandler='n/a' wp=rw",
	"`-+- policy='n/a' prio=n/a status=n/a",
	"QEMU_HARDDISK_38ffda4567ec4e2785ab dm-0 ATA,QEMU HARDDISK",
	"size=60G features='0' hwhandler='0' wp=rw",
	"|-+- policy='service-time 0' prio=1 status=active",
	"| `- 0:0:0:0 sda 8:0  active ready running",
	"`-+- policy='service-time 0' prio=1 status=enabled",
	"  `- 0:0:1:0 sdb 8:16 active ready running"
    };

    vector<string> output = {
	"data[QEMU_HARDDISK_38ffda4567ec4e2785ab] -> vendor:ATA model:QEMU HARDDISK devices:</dev/sda /dev/sdb>"
    };

    check(input, output);
}
