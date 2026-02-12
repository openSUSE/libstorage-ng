
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
    Mockup::set_command(BLKID_BIN " -c /dev/null", input);
    Mockup::set_command({ UDEVADM_BIN_SETTLE }, {});

    Udevadm udevadm;

    Blkid blkid(udevadm);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << blkid;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


void
check(const string& device, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(BLKID_BIN " -c /dev/null " + device, input);
    Mockup::set_command({ UDEVADM_BIN_SETTLE }, {});

    Udevadm udevadm;

    Blkid blkid(udevadm, device);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << blkid;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


void
check_split_line( const string & input, const string & output )
{
    string result = boost::join( Blkid::split_line( input ), "|" );

    BOOST_CHECK_EQUAL( result, output );
}


BOOST_AUTO_TEST_CASE(split_line)
{
    check_split_line( "aaa bbb ccc", "aaa|bbb|ccc" );
    check_split_line( "  aaa   bbb   ccc  ", "aaa|bbb|ccc" );
    check_split_line( "", "" );
    check_split_line( "aa=\"xxx\" bb=\"yyy\" cc=\"zzz\"", "aa=\"xxx\"|bb=\"yyy\"|cc=\"zzz\"" );
    check_split_line( "  aa=\"xxx\"   bb=\"yyy\"   cc=\"zzz\"  ", "aa=\"xxx\"|bb=\"yyy\"|cc=\"zzz\"" );

    // Whitespace in quoted strings
    check_split_line( "aa=\"x  x x\" bb=\"yy y\" cc=\"zzz\"", "aa=\"x  x x\"|bb=\"yy y\"|cc=\"zzz\"" );
    check_split_line( "aa=\"x  x x\" bb=\"yy y", "aa=\"x  x x\"|bb=\"yy y" );

    // Escaped quote in string
    check_split_line( "aa=\"x\\\"xx\" bb=\"yyy\"", "aa=\"x\\\"xx\"|bb=\"yyy\"");

    // Escaped quote in string and at the end and not properly terminated
    check_split_line( "aa=\"x\\\"xx\" bb=\"yyy\\\"", "aa=\"x\\\"xx\"|bb=\"yyy\\\"");
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
	"/dev/mapper/system-swap: LABEL=\"SWAP\" UUID=\"fd39c3f9-2990-435d-8eed-e56b6dc2e592\" TYPE=\"swap\" ",
	"/dev/mapper/docker-254:3-266193-pool: UUID=\"b8965f68-1e7c-4fac-982b-5859dca91de5\" TYPE=\"ext4\" "
    };

    vector<string> output = {
	"data[/dev/mapper/docker-254:3-266193-pool] -> is-fs:true fs-type:ext4 fs-uuid:b8965f68-1e7c-4fac-982b-5859dca91de5",
	"data[/dev/mapper/system-abuild] -> is-fs:true fs-type:ext4 fs-uuid:16337c60-fc2a-4b87-8199-4f511fa06c65 fs-label:ABUILD",
	"data[/dev/mapper/system-btrfs] -> is-fs:true fs-type:btrfs fs-uuid:946de1e3-ab5a-49d2-8c9d-057f1613d395 fs-label:BTRFS fs-sub-uuid:8fd5c226-d060-4049-90e6-1df5c865fdf4",
	"data[/dev/mapper/system-giant] -> is-fs:true fs-type:xfs fs-uuid:0857a01f-c58e-464a-b74b-cd46992873e7 fs-label:GIANT",
	"data[/dev/mapper/system-root] -> is-fs:true fs-type:ext3 fs-uuid:31e381c9-5b35-4045-8d01-9274a30e1298 fs-label:ROOT",
	"data[/dev/mapper/system-swap] -> is-fs:true fs-type:swap fs-uuid:fd39c3f9-2990-435d-8eed-e56b6dc2e592 fs-label:SWAP",
	"data[/dev/sda1] -> is-fs:true fs-type:ext3 fs-uuid:14875716-b8e3-4c83-ac86-48c20682b63a fs-label:BOOT",
	"data[/dev/sda2] -> is-lvm:true"
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
	"data[/dev/mapper/system-home] -> is-fs:true fs-type:btrfs fs-uuid:7e9e18c3-b743-47d8-9f33-464f466fc517 fs-sub-uuid:4f40002c-125e-43c1-8a38-b5bf29f5319c",
	"data[/dev/mapper/system-root] -> is-fs:true fs-type:btrfs fs-uuid:9fca85ff-4278-4f49-932e-4060726cf0d6 fs-sub-uuid:ab28e20a-9c11-4ea6-a46d-0fe412fe0e19",
	"data[/dev/mapper/system-swap] -> is-fs:true fs-type:swap fs-uuid:d0f020a1-9847-4ee5-a22e-fe0cdd4aa905",
	"data[/dev/mapper/tmp] -> is-lvm:true",
	"data[/dev/sda1] -> is-fs:true fs-type:ext4 fs-uuid:fc5985ee-e069-4bb4-a36c-24d8f6022f7a",
	"data[/dev/sda2] -> is-luks:true luks-uuid:f98def5a-6553-49a1-940d-df55a77d7d28"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse3)
{
    vector<string> input = {
	"/dev/dasda1: UUID=\"ab43b144-37e2-4e9c-8369-6df220f055b0\" SEC_TYPE=\"ext2\" TYPE=\"ext4\" "
    };

    vector<string> output = {
	"data[/dev/dasda1] -> is-fs:true fs-type:ext4 fs-uuid:ab43b144-37e2-4e9c-8369-6df220f055b0"
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
	"data[/dev/sdb] -> is-fs:true fs-type:iso9660 fs-uuid:2013-06-14-19-02-28-00 fs-label:SLES-11-SP3-MINI-ISO-x86_640703",
	"data[/dev/sdc1] -> is-fs:true fs-type:vfat fs-uuid:65CB-FEB0 fs-label:BOOT",
	"data[/dev/sdc2] -> is-fs:true fs-type:udf fs-uuid:2013-11-06-15-55-09-00 fs-label:openSUSE 13.1 KDE Live"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse5)
{
    vector<string> input = {
	"/dev/sdb1: UUID=\"db2702b8-7ae4-6bea-1849-341d8ecd7367\" UUID_SUB=\"5b8ceee8-7757-7ce9-d003-a50c09c88b02\" LABEL=\"thalassa:0\" TYPE=\"linux_raid_member\" PARTUUID=\"995ef2e3-cae4-4515-adf0-525729463b0e\"",
	"/dev/sdb2: UUID=\"db2702b8-7ae4-6bea-1849-341d8ecd7367\" UUID_SUB=\"e1385e7d-40c2-6ec7-f610-ab5bbfb5df8d\" LABEL=\"thalassa:0\" TYPE=\"linux_raid_member\" PARTUUID=\"460714f6-8eed-4ffe-9de0-8dfa2c6fff40\""
    };

    vector<string> output = {
	"data[/dev/sdb1] -> is-md:true",
	"data[/dev/sdb2] -> is-md:true"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse6)
{
    vector<string> input = {
	"/dev/sdb: UUID=\"86fa0e25-c962-4410-9113-cd23bfad08a6\" TYPE=\"bcache\"",
	"/dev/sdc: UUID=\"5f5da36e-b312-45e5-b86c-059275cfe1de\" TYPE=\"bcache\"",
	"/dev/bcache0: UUID=\"38986ecf-78d1-450b-8ad4-f071a33a7984\" TYPE=\"xfs\"",
    };

    vector<string> output = {
	"data[/dev/bcache0] -> is-fs:true fs-type:xfs fs-uuid:38986ecf-78d1-450b-8ad4-f071a33a7984",
	"data[/dev/sdb] -> is-bcache:true bcache-uuid:86fa0e25-c962-4410-9113-cd23bfad08a6",
	"data[/dev/sdc] -> is-bcache:true bcache-uuid:5f5da36e-b312-45e5-b86c-059275cfe1de"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse7)
{
    vector<string> input = {
	"/dev/sda1: LABEL=\"BOOT\" UUID=\"14875716-b8e3-4c83-ac86-48c20682b63a\" TYPE=\"ext3\" PTTYPE=\"dos\" "
    };

    vector<string> output = {
	"data[/dev/sda1] -> is-fs:true fs-type:ext3 fs-uuid:14875716-b8e3-4c83-ac86-48c20682b63a fs-label:BOOT"
    };

    check("/dev/sda1", input, output);
}


BOOST_AUTO_TEST_CASE(parse_external_journal)
{
    vector<string> input = {
	"/dev/sdc1: UUID=\"6ed5af86-99f7-4ffc-aeef-1e9da94c8f10\" EXT_JOURNAL=\"d1cdcace-86b4-4f36-aaf3-38897c95108d\" TYPE=\"ext4\"",
	"/dev/sdc2: UUID=\"d1cdcace-86b4-4f36-aaf3-38897c95108d\" LOGUUID=\"d1cdcace-86b4-4f36-aaf3-38897c95108d\" TYPE=\"jbd\"",
	"/dev/sdc3: UUID=\"3937b487-0ea8-4605-a2b1-69504a79ad02\" TYPE=\"xfs\"",
	"/dev/sdc4: LOGUUID=\"3937b487-0ea8-4605-a2b1-69504a79ad02\" TYPE=\"xfs_external_log\""
    };

    vector<string> output = {
	"data[/dev/sdc1] -> is-fs:true fs-type:ext4 fs-uuid:6ed5af86-99f7-4ffc-aeef-1e9da94c8f10 fs-journal-uuid:d1cdcace-86b4-4f36-aaf3-38897c95108d",
	"data[/dev/sdc2] -> is-journal:true journal-uuid:d1cdcace-86b4-4f36-aaf3-38897c95108d",
	"data[/dev/sdc3] -> is-fs:true fs-type:xfs fs-uuid:3937b487-0ea8-4605-a2b1-69504a79ad02",
	"data[/dev/sdc4] -> is-journal:true journal-uuid:3937b487-0ea8-4605-a2b1-69504a79ad02"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse_isw)
{
    vector<string> input = {
	"/dev/sdb: TYPE=\"isw_raid_member\"",
	"/dev/sdc: TYPE=\"isw_raid_member\"",
	"/dev/sdd: TYPE=\"isw_raid_member\"",
	"/dev/md126: UUID=\"aa211544-31d9-491a-b770-2295381acb3b\" TYPE=\"ext4\""
    };

    vector<string> output = {
	"data[/dev/md126] -> is-fs:true fs-type:ext4 fs-uuid:aa211544-31d9-491a-b770-2295381acb3b",
	"data[/dev/sdb] -> is-md:true",
	"data[/dev/sdc] -> is-md:true",
	"data[/dev/sdd] -> is-md:true"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse_ddf)
{
    vector<string> input = {
	"/dev/sdb: UUID=\"Linux-MDM-^M--M->M-o\" TYPE=\"ddf_raid_member\"",
	"/dev/sdc: UUID=\"Linux-MDM-^M--M->M-o\" TYPE=\"ddf_raid_member\"",
	"/dev/sdd: UUID=\"Linux-MDM-^M--M->M-o\" TYPE=\"ddf_raid_member\"",
	"/dev/md126: UUID=\"b6da62bd-6057-4315-a58c-b7b4a9a5e693\" TYPE=\"ext4\""
    };

    vector<string> output = {
	"data[/dev/md126] -> is-fs:true fs-type:ext4 fs-uuid:b6da62bd-6057-4315-a58c-b7b4a9a5e693",
	"data[/dev/sdb] -> is-md:true",
	"data[/dev/sdc] -> is-md:true",
	"data[/dev/sdd] -> is-md:true"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse_luks)
{
    vector<string> input = {
	"/dev/sdb1: UUID=\"b329b40b-e5f0-4f8e-814d-b6afb7f0ce64\" TYPE=\"crypto_LUKS\" PARTUUID=\"02cabc90-ca73-4302-928e-a924cda495bc\"",
	"/dev/sdb2: UUID=\"332cd185-9d1b-479c-ade6-a9fb6e4e536d\" LABEL=\"master-plan\" TYPE=\"crypto_LUKS\" PARTUUID=\"20e9945b-1b57-4f46-89d8-b6321be05df3\""
    };

    vector<string> output = {
	"data[/dev/sdb1] -> is-luks:true luks-uuid:b329b40b-e5f0-4f8e-814d-b6afb7f0ce64",
	"data[/dev/sdb2] -> is-luks:true luks-uuid:332cd185-9d1b-479c-ade6-a9fb6e4e536d luks-label:master-plan"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(split_weird_uuid_line)
{
    // bsc#1102572
    string input  = "/dev/sdb: UUID=\"LSI     M-^@M-^FM-!^F^W4^R\\\"HM-^@M- ^XM-.kwM-T\" TYPE=\"ddf_raid_member\"";
    string output = "/dev/sdb:|UUID=\"LSI     M-^@M-^FM-!^F^W4^R\\\"HM-^@M- ^XM-.kwM-T\"|TYPE=\"ddf_raid_member\"";

    check_split_line( input, output );
}


BOOST_AUTO_TEST_CASE(weird_uuid)
{
     // bsc#1102572
    vector<string> input = {
        "/dev/sdb: UUID=\"LSI     M-^@M-^FM-!^F^W4^R\\\"HM-^@M- ^XM-.kwM-T\" TYPE=\"ddf_raid_member\"",
        "/dev/sda: UUID=\"LSI     M-^@M-^FM-!^F^W4^R\\\"HM-^@M- ^XM-.kwM-T\" TYPE=\"ddf_raid_member\"",
        "/dev/md126: PTUUID=\"361b9912\" PTTYPE=\"dos\"",
        "/dev/md126p1: LABEL=\"LINSERV\" UUID=\"88B5-20E8\" TYPE=\"vfat\" PARTUUID=\"361b9912-01\"",
        "/dev/md126p2: LABEL=\"RAID1\" UUID=\"BAE2-F35E\" TYPE=\"vfat\" PARTUUID=\"361b9912-02\""
    };

    vector<string> output = {
        // Alpha-sorted by map key, thus the different order than in the input
	"data[/dev/md126p1] -> is-fs:true fs-type:vfat fs-uuid:88B5-20E8 fs-label:LINSERV",
	"data[/dev/md126p2] -> is-fs:true fs-type:vfat fs-uuid:BAE2-F35E fs-label:RAID1",
        // No output for /dev/md126 because it's irrelevant (partitionable device, nothing else)
	"data[/dev/sda] -> is-md:true",
	"data[/dev/sdb] -> is-md:true"
    };

    check(input, output);
}
