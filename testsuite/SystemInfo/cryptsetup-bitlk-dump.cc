
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
    Mockup::set_command({ CRYPTSETUP_BIN, "bitlkDump", name }, input);

    CmdCryptsetupBitlkDump cmd_cryptsetup_bitlk_dump(name);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_cryptsetup_bitlk_dump;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"Info for BITLK device /dev/sda3.",
	"Version:      	2",
	"GUID:         	dc2541db-275b-4c84-85e0-0a743b3ff229",
	"Sector size:  	512 [bytes]",
	"Created:      	Fri May 13 07:23:46 2022",
	"Description:  	DESKTOP-5F3VKFP C: 13.05.2022",
	"Cipher name:  	aes",
	"Cipher mode:  	xts-plain64",
	"Cipher key:   	256 bits",
	"",
	"Keyslots:",
	" 0: VMK",
	"	GUID:       	c754819a-f03a-4b68-96d3-d6523bf9f5d1",
	"	Protection: 	VMK protected with passphrase",
	"	Salt:       	cb1e911a89c690ebff0caeeb07273300",
	"	Key data size:	44 [bytes]",
	" 1: VMK",
	"	GUID:       	86eb87a4-263b-4f5e-a919-0c29dc067d72",
	"	Protection: 	VMK protected with recovery passphrase",
	"	Salt:       	3da0dca14bed555433b31e917e026088",
	"	Key data size:	44 [bytes]",
	" 2: FVEK",
	"	Key data size:	44 [bytes]",
	"",
	"Metadata segments:",
	" 0: FVE metadata area",
	"	Offset: 	45330432 [bytes]",
	"	Size:   	65536 [bytes]",
	" 1: FVE metadata area",
	"	Offset: 	3331559424 [bytes]",
	"	Size:   	65536 [bytes]",
	" 2: FVE metadata area",
	"	Offset: 	3331624960 [bytes]",
	"	Size:   	65536 [bytes]",
	" 3: Volume header",
	"	Offset: 	45395968 [bytes]",
	"	Size:   	8192 [bytes]",
	"	Cipher: 	aes-xts-plain64"
    };
    
    vector<string> output = {
	"name:/dev/sda3 uuid:dc2541db-275b-4c84-85e0-0a743b3ff229 cipher:aes-xts-plain64 key-size:32"
    };
    
    check("/dev/sda3", input, output);
}
