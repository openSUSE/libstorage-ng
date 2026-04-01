
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
    Mockup::set_command(BLKID_BIN " --version",
			RemoteCommand({ "blkid from util-linux 2.42  (libblkid 2.42, 01-Apr-2026)" }, {}, 0));
    Mockup::set_command(BLKID_BIN " --cache-file /dev/null --output json", input);
    Mockup::set_command({ UDEVADM_BIN_SETTLE }, {});

    Udevadm udevadm;

    CmdBlkid cmd_blkid(udevadm);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_blkid;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


void
check(const string& device, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(BLKID_BIN " --version",
			RemoteCommand({ "blkid from util-linux 2.42  (libblkid 2.42, 01-Apr-2026)" }, {}, 0));
    Mockup::set_command(BLKID_BIN " --cache-file /dev/null --output json " + device, input);
    Mockup::set_command({ UDEVADM_BIN_SETTLE }, {});

    Udevadm udevadm;

    CmdBlkid cmd_blkid(udevadm, device);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_blkid;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"{",
	"   \"blkid\": [",
	"      {",
	"         \"device\": \"/dev/mapper/system-root\",",
	"         \"uuid\": \"72e2918a-b8a8-494c-abd7-2090ebf34e64\",",
	"         \"block_size\": \"4096\",",
	"         \"type\": \"ext4\"",
	"      },{",
	"         \"device\": \"/dev/sda2\",",
	"         \"uuid\": \"Ni5zl4-XUxV-0DxM-aUe1-eQpN-JHId-RDj9fa\",",
	"         \"type\": \"LVM2_member\",",
	"         \"partuuid\": \"65867d0b-0150-4af6-966c-fe82cdb180cd\"",
	"      },{",
	"         \"device\": \"/dev/sda1\",",
	"         \"partuuid\": \"54b83199-0935-41b7-b982-08c11a7cc47a\"",
	"      }",
	"   ]",
	"}"
    };

    vector<string> output = {
	"data[/dev/mapper/system-root] -> is-fs:true fs-type:ext4 fs-uuid:72e2918a-b8a8-494c-abd7-2090ebf34e64",
	"data[/dev/sda2] -> is-lvm:true"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"{",
	"   \"blkid\": [",
	"      {",
	"         \"device\": \"/dev/sdc1\",",
	"         \"uuid\": \"43653208-6196-4d1d-8823-fb735b2c66e9\",",
	"         \"block_size\": \"4096\",",
	"         \"type\": \"ext4\",",
	"         \"partuuid\": \"dc2fd0b9-7957-4183-b357-f9d482d43b7d\"",
	"      }",
	"   ]",
	"}"
    };

    vector<string> output = {
	"data[/dev/sdc1] -> is-fs:true fs-type:ext4 fs-uuid:43653208-6196-4d1d-8823-fb735b2c66e9"
    };

    check("/dev/sdc1", input, output);
}
