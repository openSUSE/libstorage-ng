
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdCryptsetup.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


using namespace std;
using namespace storage;


void
check(const string& name, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(CRYPTSETUPBIN " status " + quote(name), input);

    CmdCryptsetupStatus cmd_cryptsetup_status(name);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_cryptsetup_status;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"/dev/mapper/cr-test is active and is in use.",
	"  type:    LUKS1",
	"  cipher:  aes-cbc-essiv:sha256",
	"  keysize: 256 bits",
	"  device:  /dev/sda2",
	"  offset:  4096 sectors",
	"  size:    487348224 sectors",
	"  mode:    read/write",
    };

    vector<string> output = {
	"name:cr-test encryption-type:luks1"
    };

    check("cr-test", input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"/dev/mapper/cr-test is active.",
	"  type:    LUKS2",
	"  cipher:  aes-xts-plain64",
	"  keysize: 512 bits",
	"  key location: keyring",
	"  device:  /dev/sdc1",
	"  sector size:  512",
	"  offset:  32768 sectors",
	"  size:    20938752 sectors",
	"  mode:    read/write"
    };

    vector<string> output = {
	"name:cr-test encryption-type:luks2"
    };

    check("cr-test", input, output);
}
