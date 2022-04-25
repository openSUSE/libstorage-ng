
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdDumpe2fs.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


using namespace std;
using namespace storage;


void
check(const string& device, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(DUMPE2FS_BIN " -h " + quote(device), input);

    CmdDumpe2fs cmd_dumpe2fs(device);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_dumpe2fs;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"Filesystem volume name:   <none>",
	"Last mounted on:          /test",
	"Filesystem UUID:          f94d6ea9-edbc-4ac5-9fc0-0f7efef05b38",
	"Filesystem magic number:  0xEF53",
	"Filesystem revision #:    1 (dynamic)",
	"Filesystem features:      has_journal ext_attr resize_inode dir_index filetype extent 64bit flex_bg sparse_super large_file huge_file dir_nlink extra_isize metadata_csum",
	"Filesystem flags:         signed_directory_hash ",
	"Default mount options:    user_xattr acl",
	"Filesystem state:         clean",
	"Errors behavior:          Continue",
	"Filesystem OS type:       Linux",
	"Inode count:              1831424",
	"Block count:              7325440",
	"Reserved block count:     366272",
	"Free blocks:              7166435",
	"Free inodes:              1831412",
	"First block:              0",
	"Block size:               4096",
	"Fragment size:            4096",
	"Group descriptor size:    64",
	"Reserved GDT blocks:      1024",
	"Blocks per group:         32768",
	"Fragments per group:      32768",
	"Inodes per group:         8176",
	"Inode blocks per group:   511",
	"Flex block group size:    16",
	"Filesystem created:       Mon Nov 18 15:20:08 2019",
	"Last mount time:          Mon Nov 18 18:57:56 2019",
	"Last write time:          Mon Nov 18 18:58:13 2019",
	"Mount count:              3",
	"Maximum mount count:      -1",
	"Last checked:             Mon Nov 18 15:20:08 2019",
	"Check interval:           0 (<none>)",
	"Lifetime writes:          451 MB",
	"Reserved blocks uid:      0 (user root)",
	"Reserved blocks gid:      0 (group root)",
	"First inode:              11",
	"Inode size:	          256",
	"Required extra isize:     32",
	"Desired extra isize:      32",
	"Journal inode:            8",
	"Default directory hash:   half_md4",
	"Directory Hash Seed:      a482ef6a-0316-492c-825f-c734dad88125",
	"Journal backup:           inode blocks",
	"Checksum type:            crc32c",
	"Checksum:                 0xcdfb0399",
	"Journal features:         journal_64bit journal_checksum_v3",
	"Journal size:             128M",
	"Journal length:           32768",
	"Journal sequence:         0x0000000c",
	"Journal start:            0",
	"Journal checksum type:    crc32c",
	"Journal checksum:         0x196bfe83",
	""
    };

    vector<string> output = {
	"device:/dev/sdc1 block-size:4096 feature-64bit:true"
    };

    check("/dev/sdc1", input, output);
}
