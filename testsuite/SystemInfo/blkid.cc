
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdBlkid.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(BLKIDBIN " -c '/dev/null'", input);

    Blkid blkid;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << blkid;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"/dev/sda1: LABEL=\"BOOT\" UUID=\"14875716-b8e3-4c83-ac86-48c20682b63a\" TYPE=\"ext3\" PTTYPE=\"dos\" ",
	"/dev/sda2: UUID=\"qquP1O-WWoh-Ofas-Rbx0-y72T-0sNe-Wnyc33\" TYPE=\"LVM2_member\" ",
	"/dev/mapper/system-abuild: LABEL=\"ABUILD\" UUID=\"16337c60-fc2a-4b87-8199-4f511fa06c65\" TYPE=\"ext4\" ",
	"/dev/mapper/system-btrfs: LABEL=\"BTRFS\" UUID=\"946de1e3-ab5a-49d2-8c9d-057f1613d395\" UUID_SUB=\"8fd5c226-d060-4049-90e6-1df5c865fdf4\" TYPE=\"btrfs\" ",
	"/dev/mapper/system-giant: LABEL=\"GIANT\" UUID=\"0857a01f-c58e-464a-b74b-cd46992873e7\" TYPE=\"xfs\" ",
	"/dev/mapper/system-root: LABEL=\"ROOT\" UUID=\"31e381c9-5b35-4045-8d01-9274a30e1298\" TYPE=\"ext3\" ",
	"/dev/mapper/system-swap: LABEL=\"SWAP\" UUID=\"fd39c3f9-2990-435d-8eed-e56b6dc2e592\" TYPE=\"swap\" "
    };

    vector<string> output = {
	"data[/dev/mapper/system-abuild] -> is_fs:true fs_type:ext4 fs_uuid:16337c60-fc2a-4b87-8199-4f511fa06c65 fs_label:ABUILD",
	"data[/dev/mapper/system-btrfs] -> is_fs:true fs_type:btrfs fs_uuid:946de1e3-ab5a-49d2-8c9d-057f1613d395 fs_label:BTRFS",
	"data[/dev/mapper/system-giant] -> is_fs:true fs_type:xfs fs_uuid:0857a01f-c58e-464a-b74b-cd46992873e7 fs_label:GIANT",
	"data[/dev/mapper/system-root] -> is_fs:true fs_type:ext3 fs_uuid:31e381c9-5b35-4045-8d01-9274a30e1298 fs_label:ROOT",
	"data[/dev/mapper/system-swap] -> is_fs:true fs_type:swap fs_uuid:fd39c3f9-2990-435d-8eed-e56b6dc2e592 fs_label:SWAP",
	"data[/dev/sda1] -> is_fs:true fs_type:ext3 fs_uuid:14875716-b8e3-4c83-ac86-48c20682b63a fs_label:BOOT",
	"data[/dev/sda2] -> is_lvm:true"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"/dev/sda1: UUID=\"fc5985ee-e069-4bb4-a36c-24d8f6022f7a\" TYPE=\"ext4\" ",
	"/dev/sda2: UUID=\"f98def5a-6553-49a1-940d-df55a77d7d28\" TYPE=\"crypto_LUKS\" ",
	"/dev/mapper/tmp: UUID=\"wunWKW-nDeG-SUnC-CxDO-5UNt-BA4Y-Se7xVC\" TYPE=\"LVM2_member\" ",
	"/dev/mapper/system-home: UUID=\"7e9e18c3-b743-47d8-9f33-464f466fc517\" UUID_SUB=\"4f40002c-125e-43c1-8a38-b5bf29f5319c\" TYPE=\"btrfs\" ",
	"/dev/mapper/system-root: UUID=\"9fca85ff-4278-4f49-932e-4060726cf0d6\" UUID_SUB=\"ab28e20a-9c11-4ea6-a46d-0fe412fe0e19\" TYPE=\"btrfs\" ",
	"/dev/mapper/system-swap: UUID=\"d0f020a1-9847-4ee5-a22e-fe0cdd4aa905\" TYPE=\"swap\" "
    };

    vector<string> output = {
	"data[/dev/mapper/system-home] -> is_fs:true fs_type:btrfs fs_uuid:7e9e18c3-b743-47d8-9f33-464f466fc517",
	"data[/dev/mapper/system-root] -> is_fs:true fs_type:btrfs fs_uuid:9fca85ff-4278-4f49-932e-4060726cf0d6",
	"data[/dev/mapper/system-swap] -> is_fs:true fs_type:swap fs_uuid:d0f020a1-9847-4ee5-a22e-fe0cdd4aa905",
	"data[/dev/mapper/tmp] -> is_lvm:true",
	"data[/dev/sda1] -> is_fs:true fs_type:ext4 fs_uuid:fc5985ee-e069-4bb4-a36c-24d8f6022f7a",
	"data[/dev/sda2] -> is_luks:true luks_uuid:f98def5a-6553-49a1-940d-df55a77d7d28"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse3)
{
    vector<string> input = {
	"/dev/dasda1: UUID=\"ab43b144-37e2-4e9c-8369-6df220f055b0\" SEC_TYPE=\"ext2\" TYPE=\"ext4\" "
    };

    vector<string> output = {
	"data[/dev/dasda1] -> is_fs:true fs_type:ext4 fs_uuid:ab43b144-37e2-4e9c-8369-6df220f055b0"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse4)
{
    vector<string> input = {
	"/dev/sdb: UUID=\"2013-06-14-19-02-28-00\" LABEL=\"SLES-11-SP3-MINI-ISO-x86_640703\" TYPE=\"iso9660\" ",
	"/dev/sdc1: SEC_TYPE=\"msdos\" LABEL=\"BOOT\" UUID=\"65CB-FEB0\" TYPE=\"vfat\" PARTUUID=\"a893698d-01\" ",
	"/dev/sdc2: UUID=\"2013-11-06-15-55-09-00\" LABEL=\"openSUSE 13.1 KDE Live\" TYPE=\"udf\" PARTUUID=\"a893698d-02\" "
    };

    vector<string> output = {
	"data[/dev/sdb] -> is_fs:true fs_type:iso9660 fs_uuid:2013-06-14-19-02-28-00 fs_label:SLES-11-SP3-MINI-ISO-x86_640703",
	"data[/dev/sdc1] -> is_fs:true fs_type:vfat fs_uuid:65CB-FEB0 fs_label:BOOT",
	"data[/dev/sdc2] -> is_fs:true fs_type:udf fs_uuid:2013-11-06-15-55-09-00 fs_label:openSUSE 13.1 KDE Live"
    };

    check(input, output);
}
