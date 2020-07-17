
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
check(const string& device, const vector<string>& stdout, const vector<string>& stderr,
      const vector<string>& result)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(PARTED_BIN " --script --machine " + quote(device) + " unit s print",
			RemoteCommand(stdout, stderr, 0));

    Parted parted(device);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << parted;

    string lhs = parsed.str();
    string rhs = boost::join(result, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


void
check(const string& device, const vector<string>& stdout, const vector<string>& result)
{
    check(device, stdout, vector<string>(), result);
}


void
check_exception(const string& device, const vector<string>& input)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(PARTED_BIN " --script --machine " + quote(device) + " unit s print", input);

    BOOST_CHECK_THROW({ Parted parted(device); }, ParseException);
}


BOOST_AUTO_TEST_CASE(parse_msdos_disk_label_good)
{
    vector<string> input = {
	"BYT;",
	"/dev/sdb:160086528s:scsi:512:512:msdos:Maxtor 6 Y080L0:;",
	"1:2048s:32016383s:32014336s:ext4::type=83;",
	"2:32016384s:128069631s:96053248s:::lba, type=0f;",
	"5:32018432s:64034815s:32016384s:linux-swap(v1)::type=82;",
	"6:64036864s:96051199s:32014336s:::raid, type=fd;",
	"7:96053248s:128069631s:32016384s:::lvm, type=8e;"
    };

    vector<string> output = {
	"device:/dev/sdb label:MS-DOS region:[0, 160086528, 512 B]",
	"number:1 region:[2048, 32014336, 512 B] type:primary id:0x83",
	"number:2 region:[32016384, 96053248, 512 B] type:extended id:0x0F",
	"number:5 region:[32018432, 32016384, 512 B] type:logical id:0x82",
	"number:6 region:[64036864, 32014336, 512 B] type:logical id:0xFD",
	"number:7 region:[96053248, 32016384, 512 B] type:logical id:0x8E"
    };

    check("/dev/sdb", input, output);
}


BOOST_AUTO_TEST_CASE(parse_gpt_good)
{
    vector<string> input = {
	"BYT;",
        "/dev/sda:976773168s:scsi:512:512:gpt_sync_mbr:ATA ST3500320NS:;",
	"1:2048s:1028095s:1026048s:fat32::msftdata;",
	"2:1028096s:2056191s:1028096s:ext3::legacy_boot;",
	"3:2056192s:295098367s:293042176s:ext3::;",
	"4:295098368s:588140543s:293042176s:::;",
	"5:588140544s:592349183s:4208640s:linux-swap(v1)::;",
	"6:592349184s:976773119s:384423936s:ext3::;"
    };

    vector<string> output = {
	"device:/dev/sda label:GPT region:[0, 976773168, 512 B]",
	"number:1 region:[2048, 1026048, 512 B] type:primary id:0x102",
	"number:2 region:[1028096, 1028096, 512 B] type:primary id:0x83 legacy-boot",
	"number:3 region:[2056192, 293042176, 512 B] type:primary id:0x83",
	"number:4 region:[295098368, 293042176, 512 B] type:primary id:0x83",
	"number:5 region:[588140544, 4208640, 512 B] type:primary id:0x83",
	"number:6 region:[592349184, 384423936, 512 B] type:primary id:0x83"
    };

    check("/dev/sda", input, output);
}


BOOST_AUTO_TEST_CASE(parse_gpt_with_pmbr_boot)
{
    vector<string> input = {
	"BYT;",
	"/dev/sdb:160086528s:scsi:512:512:gpt:Maxtor 6 Y080L0:pmbr_boot;",
	"1:2048s:32016383s:32014336s:::;",
	"2:32016384s:160086015s:128069632s:::;"
    };

    vector<string> output = {
	"device:/dev/sdb label:GPT region:[0, 160086528, 512 B] gpt-pmbr-boot",
	"number:1 region:[2048, 32014336, 512 B] type:primary id:0x83",
	"number:2 region:[32016384, 128069632, 512 B] type:primary id:0x83"
    };

    check("/dev/sdb", input, output);
}


BOOST_AUTO_TEST_CASE(parse_dasd_good1)
{
    vector<string> input = {
	"BYT;",
	"/dev/dasda:14424480s:dasd:512:4096:dasd:IBM S390 DASD drive:;",
	"1:192s:409727s:409536s:ext2::;",
	"2:409728s:2437247s:2027520s:linux-swap(v1)::;",
	"3:2437248s:14424479s:11987232s:ext4::;"
    };

    vector<string> output = {
	"device:/dev/dasda label:DASD region:[0, 1803060, 4096 B]",
	"number:1 region:[24, 51192, 4096 B] type:primary id:0x83",
	"number:2 region:[51216, 253440, 4096 B] type:primary id:0x83",
	"number:3 region:[304656, 1498404, 4096 B] type:primary id:0x83"
    };

    check("/dev/dasda", input, output);
}


BOOST_AUTO_TEST_CASE(parse_dasd_good2)
{
    vector<string> input = {
	"BYT;",
	"/dev/dasda:4808160s:dasd:512:4096:dasd:IBM S390 DASD drive:;",
	"1:192s:1442399s:1442208s:::raid;",
	"2:1442400s:2884895s:1442496s:::lvm;",
	"3:2884896s:4327295s:1442400s:linux-swap(v1)::swap;"
    };

    vector<string> output = {
	"device:/dev/dasda label:DASD region:[0, 601020, 4096 B]",
	"number:1 region:[24, 180276, 4096 B] type:primary id:0xFD",
	"number:2 region:[180300, 180312, 4096 B] type:primary id:0x8E",
	"number:3 region:[360612, 180300, 4096 B] type:primary id:0x82"
    };

    check("/dev/dasda", input, output);
}


BOOST_AUTO_TEST_CASE(parse_dasd_good3)
{
    vector<string> input = {
	"BYT;",
        "/dev/dasda:144540s:dasd:512:1024:dasd:IBM S390 DASD drive:;",
        "1:6s:144539s:144534s:::;"
    };

    vector<string> output = {
        "device:/dev/dasda label:DASD region:[0, 72270, 1024 B]",
        "number:1 region:[3, 72267, 1024 B] type:primary id:0x83"
    };

    check("/dev/dasda", input, output);
}


BOOST_AUTO_TEST_CASE(parse_loop_good)
{
    vector<string> input = {
	"BYT;",
	"/dev/sdc:160086528s:scsi:512:512:loop:Maxtor 6 Y080L0:;",
	"1:0s:160086527s:160086528s:xfs::;"
    };

    vector<string> output = {
	"device:/dev/sdb label:loop region:[0, 160086528, 512 B]"
    };

    check("/dev/sdb", input, output);
}


BOOST_AUTO_TEST_CASE(parse_dasd_implicit_good)
{
    vector<string> input = {
	"BYT;",
	"/dev/dasdc:20000s:dasd:512:512:dasd:IBM S390 DASD drive:implicit_partition_table;",
	"1:2s:19999s:19998s:::;"
    };

    vector<string> output = {
	"device:/dev/dasdc label:DASD region:[0, 20000, 512 B] implicit",
	"number:1 region:[2, 19998, 512 B] type:primary id:0x83"
    };

    check("/dev/dasdc", input, output);
}


BOOST_AUTO_TEST_CASE(parse_wiped_disk_good)
{
    vector<string> input = {
	"BYT;",
	"/dev/sdb:160086528s:scsi:512:512:unknown:Maxtor 6 Y080L0:;"
    };

    vector<string> output = {
	"device:/dev/sdb label:unknown region:[0, 160086528, 512 B]"
    };

    check("/dev/sdb", input, output);
}


BOOST_AUTO_TEST_CASE(parse_gpt_of_windows)
{
    vector<string> input = {
	"BYT;",
	"/dev/vda:104857600s:virtblk:512:512:gpt:Virtio Block Device:;",
	"1:2048s:923647s:921600s:ntfs:Basic data partition:hidden, diag;",
	"2:923648s:1126399s:202752s:fat32:EFI system partition:boot, esp;",
	"3:1126400s:1159167s:32768s::Microsoft reserved partition:msftres;",
	"4:1159168s:104855551s:103696384s:ntfs:Basic data partition:msftdata;"
    };

    vector<string> output = {
	"device:/dev/vda label:GPT region:[0, 104857600, 512 B]",
	"number:1 region:[2048, 921600, 512 B] type:primary id:0x12 name:Basic data partition",
	"number:2 region:[923648, 202752, 512 B] type:primary id:0xEF name:EFI system partition",
	"number:3 region:[1126400, 32768, 512 B] type:primary id:0x103 name:Microsoft reserved partition",
	"number:4 region:[1159168, 103696384, 512 B] type:primary id:0x102 name:Basic data partition"
    };

    check("/dev/vda", input, output);
}


BOOST_AUTO_TEST_CASE(parse_gpt_undersized)
{
    vector<string> stdout = {
	"BYT;",
	"/dev/sdc:160086528s:scsi:512:512:gpt_sync_mbr:Maxtor 6 Y080L0:;"
    };

    vector<string> stderr = {
	"Warning: Not all of the space available to /dev/sdc appears to be used, you can fix the GPT to use all of the space (an extra 562240 blocks) or continue with the current setting? "
    };

    vector<string> output = {
	"device:/dev/sdc label:GPT region:[0, 160086528, 512 B] gpt-undersized"
    };

    check("/dev/sdc", stdout, stderr, output);
}


BOOST_AUTO_TEST_CASE(parse_uncommon_ordering)
{
    vector<string> input = {
	"BYT;",
	"/dev/sdb:160086528s:scsi:512:512:gpt:Maxtor 6 Y080L0:;",
	"2:2048s:32016383s:32014336s:::;",
	"1:32016384s:80044031s:48027648s:::;"
    };

    vector<string> output = {
	"device:/dev/sdb label:GPT region:[0, 160086528, 512 B]",
	"number:1 region:[32016384, 48027648, 512 B] type:primary id:0x83",
	"number:2 region:[2048, 32014336, 512 B] type:primary id:0x83"
    };

    check("/dev/sdb", input, output);
}


BOOST_AUTO_TEST_CASE(parse_jetson_nano)
{
    // GPT of Jetson Nano image with only 15 partition slots.

    vector<string> input = {
	"BYT;",
	"/dev/mmcblk1:31116288s:sd/mmc:512:512:gpt:SD SL16G::15;",
	"1:40s:29360167s:29360128s:ext4:APP:;",
	"2:29360168s:29360423s:256s::TBC:msftdata;",
	"3:29360424s:29361319s:896s::RP1:msftdata;",
	"4:29361320s:29362471s:1152s::EBT:msftdata;",
	"5:29362472s:29362599s:128s::WB0:msftdata;",
	"6:29362600s:29362983s:384s::BPF:msftdata;",
	"7:29362984s:29363751s:768s::BPF-DTB:msftdata;",
	"8:29363752s:29363879s:128s::FX:msftdata;",
	"9:29363880s:29364775s:896s::TOS:msftdata;",
	"10:29364776s:29365671s:896s::DTB:msftdata;",
	"11:29365672s:29367207s:1536s::LNX:msftdata;",
	"12:29367208s:29367335s:128s::EKS:msftdata;",
	"13:29367336s:29367495s:160s::BMP:msftdata;",
	"14:29367496s:29367751s:256s::RP4:msftdata;",
	"15:29367752s:29629895s:262144s:fat16:EFI:boot, esp;"
    };

    vector<string> output = {
	"device:/dev/mmcblk1 label:GPT region:[0, 31116288, 512 B] primary-slots:15",
	"number:1 region:[40, 29360128, 512 B] type:primary id:0x83 name:APP",
	"number:2 region:[29360168, 256, 512 B] type:primary id:0x102 name:TBC",
	"number:3 region:[29360424, 896, 512 B] type:primary id:0x102 name:RP1",
	"number:4 region:[29361320, 1152, 512 B] type:primary id:0x102 name:EBT",
	"number:5 region:[29362472, 128, 512 B] type:primary id:0x102 name:WB0",
	"number:6 region:[29362600, 384, 512 B] type:primary id:0x102 name:BPF",
	"number:7 region:[29362984, 768, 512 B] type:primary id:0x102 name:BPF-DTB",
	"number:8 region:[29363752, 128, 512 B] type:primary id:0x102 name:FX",
	"number:9 region:[29363880, 896, 512 B] type:primary id:0x102 name:TOS",
	"number:10 region:[29364776, 896, 512 B] type:primary id:0x102 name:DTB",
	"number:11 region:[29365672, 1536, 512 B] type:primary id:0x102 name:LNX",
	"number:12 region:[29367208, 128, 512 B] type:primary id:0x102 name:EKS",
	"number:13 region:[29367336, 160, 512 B] type:primary id:0x102 name:BMP",
	"number:14 region:[29367496, 256, 512 B] type:primary id:0x102 name:RP4",
	"number:15 region:[29367752, 262144, 512 B] type:primary id:0xEF name:EFI"
    };

    check("/dev/mmcblk1", input, output);
}


BOOST_AUTO_TEST_CASE(parse_gpt_backup_broken)
{
    vector<string> stdout = {
	"BYT;",
	"/dev/sdc:160086528s:scsi:512:512:gpt:Maxtor 6 Y080L0:;"
    };

    vector<string> stderr = {
	"Error: The backup GPT table is corrupt, but the primary appears OK, so that will be used."
    };

    vector<string> output = {
	"device:/dev/sdc label:GPT region:[0, 160086528, 512 B] gpt-backup-broken"
    };

    check("/dev/sdc", stdout, stderr, output);
}


BOOST_AUTO_TEST_CASE(parse_name_with_colon)
{
    // see bsc #1066467

    vector<string> input = {
	"BYT;",
	"/dev/md/ozelot\\:0:67100416s:md:512:4096:loop:Linux Software RAID Array:;",
	"1:0s:67100415s:67100416s:ext3::;"
    };

    vector<string> output = {
	"device:/dev/md/ozelot:0 label:loop region:[0, 67100416, 512 B]"
    };

    check("/dev/md/ozelot:0", input, output);
}


BOOST_AUTO_TEST_CASE(parse_missing_byt)
{
    vector<string> input = {
	"/dev/sdc:160086528s:scsi:512:512:gpt:Maxtor 6 Y080L0:;",
	"1:2048s:923647s:921600s:ext4::;"
    };

    check_exception("/dev/sdc", input);
}


BOOST_AUTO_TEST_CASE(parse_missing_semicolon)
{
    vector<string> input = {
	"BYT;",
        "/dev/sdc:160086528s:scsi:512:512:gpt:Maxtor 6 Y080L0:;"
	"1:2048s:923647s:921600s:ext4::",
    };

    check_exception("/dev/sdc", input);
}
