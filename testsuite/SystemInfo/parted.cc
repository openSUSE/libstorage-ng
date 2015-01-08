
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdParted.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const string& device, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(PARTEDBIN " -s " + quote(device) + " unit cyl print unit s print", input);

    Parted parted(device);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << parted;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"Model: ATA WDC WD10EADS-00M (scsi)",
	"Disk /dev/sda: 121601cyl",
	"Sector size (logical/physical): 512B/4096B",
	"BIOS cylinder,head,sector geometry: 121601,255,63.  Each cylinder is 8225kB.",
	"Partition Table: msdos",
	"",
	"Number  Start   End        Size       Type     File system  Flags",
	" 1      0cyl    131cyl     130cyl     primary  ext3         boot, type=83",
	" 2      131cyl  117618cyl  117487cyl  primary               lvm, type=8e",
	"",
	"Model: ATA WDC WD10EADS-00M (scsi)",
	"Disk /dev/sda: 1953525168s",
	"Sector size (logical/physical): 512B/4096B",
	"Partition Table: msdos",
	"",
	"Number  Start     End          Size         Type     File system  Flags",
	" 1      2048s     2105343s     2103296s     primary  ext3         boot, type=83",
	" 2      2105344s  1889548287s  1887442944s  primary               lvm, type=8e",
	""
    };

    vector<string> output = {
	"device:/dev/sda label:msdos geometry:[121601,255,63,512]",
	"num:1 cylRegion:[0,132] secRegion:[2048,2103296] type:primary id:131 boot",
	"num:2 cylRegion:[131,117488] secRegion:[2105344,1887442944] type:primary id:142"
    };

    check("/dev/sda", input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"Model: ATA ST3500320NS (scsi)",
	"Disk /dev/sda: 60801cyl",
	"Sector size (logical/physical): 512B/512B",
	"BIOS cylinder,head,sector geometry: 60801,255,63.  Each cylinder is 8225kB.",
	"Partition Table: gpt_sync_mbr",
	"",
	"Number  Start     End       Size      File system     Name     Flags",
	" 1      0cyl      63cyl     63cyl     fat16           primary",
	" 2      63cyl     127cyl    63cyl     ext3            primary  boot, legacy_boot",
	" 3      127cyl    18369cyl  18241cyl  ext3            primary",
	" 4      18369cyl  36610cyl  18241cyl                  primary",
	" 5      36610cyl  36872cyl  261cyl    linux-swap(v1)  primary",
	" 6      36872cyl  60801cyl  23929cyl  ext3            primary",
	"",
	"Model: ATA ST3500320NS (scsi)",
	"Disk /dev/sda: 976773168s",
	"Sector size (logical/physical): 512B/512B",
	"Partition Table: gpt_sync_mbr",
	"",
	"Number  Start       End         Size        File system     Name     Flags",
	" 1      2048s       1028095s    1026048s    fat16           primary",
	" 2      1028096s    2056191s    1028096s    ext3            primary  boot, legacy_boot",
	" 3      2056192s    295098367s  293042176s  ext3            primary",
	" 4      295098368s  588140543s  293042176s                  primary",
	" 5      588140544s  592349183s  4208640s    linux-swap(v1)  primary",
	" 6      592349184s  976773119s  384423936s  ext3            primary",
	""
    };

    vector<string> output = {
	"device:/dev/sdb label:gpt geometry:[60801,255,63,512]",
	"num:1 cylRegion:[0,64] secRegion:[2048,1026048] type:primary id:12",
	"num:2 cylRegion:[63,65] secRegion:[1028096,1028096] type:primary id:131 boot",
	"num:3 cylRegion:[127,18243] secRegion:[2056192,293042176] type:primary id:131",
	"num:4 cylRegion:[18369,18242] secRegion:[295098368,293042176] type:primary id:131",
	"num:5 cylRegion:[36610,263] secRegion:[588140544,4208640] type:primary id:130",
	"num:6 cylRegion:[36872,23929] secRegion:[592349184,384423936] type:primary id:131"
    };

    check("/dev/sdb", input, output);
}


BOOST_AUTO_TEST_CASE(parse3)
{
    vector<string> input = {
	"Model: IBM S390 DASD drive (dasd)",
	"Disk /dev/dasda: 178079cyl",
	"Sector size (logical/physical): 512B/4096B",
	"BIOS cylinder,head,sector geometry: 178080,15,12.  Each cylinder is 92.2kB.",
	"Partition Table: dasd",
	"Disk Flags: ",
	"",
	"Number  Start     End        Size       File system     Flags",
	" 1      1cyl      23303cyl   23302cyl   linux-swap(v1)",
	" 2      23304cyl  25583cyl   2280cyl    ext2",
	" 3      25584cyl  178079cyl  152496cyl  ext4",
	"",
	"Model: IBM S390 DASD drive (dasd)",
	"Disk /dev/dasda: 32054400s",
	"Sector size (logical/physical): 512B/4096B",
	"Partition Table: dasd",
	"Disk Flags: ",
	"",
	"Number  Start     End        Size       File system     Flags",
	" 1      192s      4194719s   4194528s   linux-swap(v1)",
	" 2      4194720s  4605119s   410400s    ext2",
	" 3      4605120s  32054399s  27449280s  ext4",
	""
    };

    vector<string> output = {
	"device:/dev/dasda label:dasd geometry:[178079,15,12,512]",
	"num:1 cylRegion:[1,23303] secRegion:[192,4194528] type:primary id:130",
	"num:2 cylRegion:[23304,2280] secRegion:[4194720,410400] type:primary id:131",
	"num:3 cylRegion:[25584,152495] secRegion:[4605120,27449280] type:primary id:131"
    };

    check("/dev/dasda", input, output);
}


BOOST_AUTO_TEST_CASE(parse4)
{
    vector<string> input = {
	"Model: Maxtor 6 Y080L0 (scsi)",
	"Disk /dev/sdb: 9964cyl",
	"Sector size (logical/physical): 512B/512B",
	"BIOS cylinder,head,sector geometry: 9964,255,63.  Each cylinder is 8225kB.",
	"Partition Table: loop",
	"",
	"Number  Start  End      Size     File system  Flags",
	" 1      0cyl   9964cyl  9964cyl  xfs",
	"",
	"Model: Maxtor 6 Y080L0 (scsi)",
	"Disk /dev/sdb: 160086528s",
	"Sector size (logical/physical): 512B/512B",
	"Partition Table: loop",
	"",
	"Number  Start  End         Size        File system  Flags",
	" 1      0s     160086527s  160086528s  xfs",
	""
    };

    vector<string> output = {
	"device:/dev/sdb label:loop geometry:[9964,255,63,512]"
    };

    check("/dev/sdb", input, output);
}


BOOST_AUTO_TEST_CASE(parse5)
{
    vector<string> input = {
	"Model: IBM S390 DASD drive (dasd)",
	"Disk /dev/dasdc: 244cyl",
	"Sector size (logical/physical): 512B/512B",
	"BIOS cylinder,head,sector geometry: 244,16,128.  Each cylinder is 1049kB.",
	"Partition Table: dasd",
	"Disk Flags: implicit_partition_table",
	"",
	"Number  Start  End     Size    File system  Flags",
	" 1      0cyl   244cyl  244cyl",
	"",
	"Model: IBM S390 DASD drive (dasd)",
	"Disk /dev/dasdc: 500000s",
	"Sector size (logical/physical): 512B/512B",
	"Partition Table: dasd",
	"Disk Flags: implicit_partition_table",
	"",
	"Number  Start  End      Size     File system  Flags",
	" 1      2s     499999s  499998s"
    };

    vector<string> output = {
	"device:/dev/dasdc label:dasd geometry:[244,16,128,512] implicit",
	"num:1 cylRegion:[0,244] secRegion:[2,499998] type:primary id:131"
    };

    check("/dev/dasdc", input, output);
}
