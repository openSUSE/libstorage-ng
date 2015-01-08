
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

    CmdCryptsetup cmdcryptsetup(name);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmdcryptsetup;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"/dev/mapper/cr_test is active and is in use.",
	"  type:    LUKS1",
	"  cipher:  aes-cbc-essiv:sha256",
	"  keysize: 256 bits",
	"  device:  /dev/sda2",
	"  offset:  4096 sectors",
	"  size:    487348224 sectors",
	"  mode:    read/write",
    };

    vector<string> output = {
	"name:cr_test encrypt_type:luks"
    };

    check("cr_test", input, output);
}
