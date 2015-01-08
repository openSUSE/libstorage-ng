
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/ProcParts.h"
#include "storage/Utils/Mockup.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_file("/proc/partitions", input);

    ProcParts procparts;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << procparts;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"major minor  #blocks  name",
	"",
	"   8        0  976762584 sda",
	"   8        1    1051648 sda1",
	"   8        2  943721472 sda2",
	" 253        0   31457280 dm-0",
	" 253        1   52428800 dm-1",
	" 253        2   16777216 dm-2",
	" 253        3    2097152 dm-3",
	" 253        4  314572800 dm-4",
	" 253        5    5242880 dm-5",
	" 253        6    2097152 dm-6",
	" 253        7    1048576 dm-7",
	"  11        0    1048575 sr0"
    };

    vector<string> output = {
	"data[/dev/dm-0] -> 31457280",
	"data[/dev/dm-1] -> 52428800",
	"data[/dev/dm-2] -> 16777216",
	"data[/dev/dm-3] -> 2097152",
	"data[/dev/dm-4] -> 314572800",
	"data[/dev/dm-5] -> 5242880",
	"data[/dev/dm-6] -> 2097152",
	"data[/dev/dm-7] -> 1048576",
	"data[/dev/sda] -> 976762584",
	"data[/dev/sda1] -> 1051648",
	"data[/dev/sda2] -> 943721472",
	"data[/dev/sr0] -> 1048575"
    };

    check(input, output);
}
