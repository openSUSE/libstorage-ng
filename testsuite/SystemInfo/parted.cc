
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
    Mockup::set_command(PARTEDBIN " --script " + quote(device) + " unit s print", input);

    Parted parted(device);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << parted;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


void
check_parse_exception(const string& device, const vector<string>& input)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(PARTEDBIN " --script " + quote(device) + " unit s print", input);

    BOOST_CHECK_THROW({ Parted parted(device); }, ParseException);
}


BOOST_AUTO_TEST_CASE(parse_msdos_disk_label_good)
{
    vector<string> input = {
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
	"device:/dev/sda label:MS-DOS region:[0, 1953525168, 512 B]",
	"num:1 region:[2048, 2103296, 512 B] type:primary id:131 boot",
	"num:2 region:[2105344, 1887442944, 512 B] type:primary id:142"
    };

    check("/dev/sda", input, output);
}


BOOST_AUTO_TEST_CASE(parse_gpt_good)
{
    vector<string> input = {
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
	"device:/dev/sda label:GPT region:[0, 976773168, 512 B]",
	"num:1 region:[2048, 1026048, 512 B] type:primary id:12",
	"num:2 region:[1028096, 1028096, 512 B] type:primary id:131 legacy-boot",
	"num:3 region:[2056192, 293042176, 512 B] type:primary id:131",
	"num:4 region:[295098368, 293042176, 512 B] type:primary id:131",
	"num:5 region:[588140544, 4208640, 512 B] type:primary id:130",
	"num:6 region:[592349184, 384423936, 512 B] type:primary id:131"
    };

    check("/dev/sda", input, output);
}


// TO DO: add test cases "parse_gpt_enlarge_good" and "parse_gpt_fix_backup_good"


BOOST_AUTO_TEST_CASE(parse_gpt_with_pmbr_boot)
{
    vector<string> input = {
	"Model: Maxtor 6 Y080L0 (scsi)",
	"Disk /dev/sdb: 160086528s",
	"Sector size (logical/physical): 512B/512B",
	"Partition Table: gpt",
	"Disk Flags: pmbr_boot",
	"",
	"Number  Start      End         Size        File system  Name  Flags",
	" 1      2048s      32016383s   32014336s",
	" 2      32016384s  160086015s  128069632s",
	""
    };

    vector<string> output = {
	"device:/dev/sdb label:GPT region:[0, 160086528, 512 B] gpt-pmbr-boot",
	"num:1 region:[2048, 32014336, 512 B] type:primary id:131",
	"num:2 region:[32016384, 128069632, 512 B] type:primary id:131"
    };

    check("/dev/sdb", input, output);
}


BOOST_AUTO_TEST_CASE(parse_dasd_good)
{
    vector<string> input = {
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
	"device:/dev/dasda label:DASD region:[0, 32054400, 512 B]",
	"num:1 region:[192, 4194528, 512 B] type:primary id:130",
	"num:2 region:[4194720, 410400, 512 B] type:primary id:131",
	"num:3 region:[4605120, 27449280, 512 B] type:primary id:131"
    };

    check("/dev/dasda", input, output);
}


BOOST_AUTO_TEST_CASE(parse_loop_good)
{
    vector<string> input = {
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
	"device:/dev/sdb label:loop region:[0, 160086528, 512 B]"
    };

    check("/dev/sdb", input, output);
}


BOOST_AUTO_TEST_CASE(parse_dasd_implicit_good)
{
    vector<string> input = {
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
	"device:/dev/dasdc label:DASD region:[0, 500000, 512 B] implicit",
	"num:1 region:[2, 499998, 512 B] type:primary id:131"
    };

    check("/dev/dasdc", input, output);
}


BOOST_AUTO_TEST_CASE(parse_wiped_disk_good)
{
    // Disk with no partition table at all (brand new or after 'wipefs')
    vector<string> input = {
	"Error: /dev/sdb: unrecognised disk label",
	"Model: Maxtor 6 Y080L0 (scsi)",
	"Disk /dev/sdb: 160086528s",
	"Sector size (logical/physical): 512B/512B",
	"Partition Table: unknown",
	"Disk Flags: "
    };

    vector<string> output = { "device:/dev/sdb label:unknown region:[0, 160086528, 512 B]" };

    check("/dev/sdb", input, output);
}


BOOST_AUTO_TEST_CASE(parse_bad_sector_size_line)
{
    vector<string> input = {
	"Sector size (logical/physical): 512B, 4096B",
	"Partition Table: msdos",
	"",
	"Number  Start     End          Size         Type     File system  Flags",
	" 1      2048s     2105343s     2103296s     primary  ext3         boot, type=83",
	" 2      2105344s  1889548287s  1887442944s  primary               lvm, type=8e",
	""
    };

    check_parse_exception("/dev/sda", input);
}


BOOST_AUTO_TEST_CASE(parse_bad_msdos_part_entry_2)
{
    vector<string> input = {
	"Model: ATA WDC WD10EADS-00M (scsi)",
	"Disk /dev/sda: 1953525168s",
	"Sector size (logical/physical): 512B/4096B",
	"Partition Table: msdos",
	"",
	"Number  Start    End         Size        Type     File system  Flags",
	" 1      2048     2105343     2103296     primary  ext3         boot, type=83", // no "s" units
	" 2      2105344  1889548287  1887442944  primary               lvm, type=8e",  // no "s" units
	""
    };

    check_parse_exception("/dev/sda", input);
}


BOOST_AUTO_TEST_CASE(parse_two_partition_table)
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

    check_parse_exception("/dev/sda", input);
}
