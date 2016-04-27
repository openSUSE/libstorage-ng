
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/ProcMdstat.h"
#include "storage/Utils/Mockup.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_file("/proc/mdstat", input);

    ProcMdstat procmdstat;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << procmdstat;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"Personalities : [raid0] [raid1] [raid10] [raid6] [raid5] [raid4] ",
	"md0 : active raid1 sdb1[1] sda1[0]",
	"      8387520 blocks super 1.0 [2/2] [UU]",
	"      [====>................]  resync = 23.4% (1963200/8387520) finish=4.6min speed=23178K/sec",
	"      bitmap: 1/1 pages [4KB], 65536KB chunk",
	"",
	"unused devices: <none>"
    };

    vector<string> output = {
	"data[md0] -> md-level:RAID1 super:1.0 size:8588820480 devices:</dev/sda1 /dev/sdb1>"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"Personalities : [raid1] [raid0] ",
	"md125 : active (auto-read-only) raid1 sda[1] sdb[0]",
	"      4194304 blocks super external:/md127/0 [2/2] [UU]",
	"      ",
	"md126 : active raid0 sda[1] sdb[0]",
	"      8378790 blocks super external:/md127/1 128k chunks",
	"      ",
	"md127 : inactive sdb[1](S) sda[0](S)",
	"      5104 blocks super external:imsm",
	"       ",
	"unused devices: <none>",
    };

    vector<string> output = {
	"data[md125] -> md-level:RAID1 super:external:/md127/0 size:4294967296 readonly devices:</dev/sda /dev/sdb> has-container container-name:md127 container-member:0",
	"data[md126] -> md-level:RAID0 super:external:/md127/1 chunk-size:131072 size:8579880960 devices:</dev/sda /dev/sdb> has-container container-name:md127 container-member:1",
	"data[md127] -> md-level:unknown super:external:imsm size:5226496 readonly inactive devices:</dev/sda(S) /dev/sdb(S)> is-container"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse3)
{
    vector<string> input = {
	"Personalities : [raid1] [raid0] ",
	"md125 : active raid1 sda[1] sdb[0]",
	"      7355904 blocks super external:/md127/1 [2/2] [UU]",
	"      ",
	"md126 : active raid0 sda[1] sdb[0]",
	"      1999872 blocks super external:/md127/0 512k chunks",
	"      ",
	"md127 : inactive sdb[1](S) sda[0](S)",
	"      65536 blocks super external:ddf",
	"       ",
	"unused devices: <none>",
    };

    vector<string> output = {
	"data[md125] -> md-level:RAID1 super:external:/md127/1 size:7532445696 devices:</dev/sda /dev/sdb> has-container container-name:md127 container-member:1",
	"data[md126] -> md-level:RAID0 super:external:/md127/0 chunk-size:524288 size:2047868928 devices:</dev/sda /dev/sdb> has-container container-name:md127 container-member:0",
	"data[md127] -> md-level:unknown super:external:ddf size:67108864 readonly inactive devices:</dev/sda(S) /dev/sdb(S)> is-container"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse4)
{
    vector<string> input = {
	"Personalities : [raid1] [raid0] [raid6] [raid5] [raid4] [raid10] [multipath] ",
	"md0 : active raid5 sdd[4](F) sdc[3] sdb[1] sda[0]",
	"      33521664 blocks super 1.2 level 5, 512k chunk, algorithm 2 [3/3] [UUU]",
	"      ",
	"unused devices: <none>"
    };

    vector<string> output = {
	"data[md0] -> md-level:RAID5 md-parity:left-symmetric super:1.2 chunk-size:524288 size:34326183936 devices:</dev/sda /dev/sdb /dev/sdc /dev/sdd(F)>",
    };

    check(input, output);
}
