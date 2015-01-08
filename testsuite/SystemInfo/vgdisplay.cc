
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdLvm.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


using namespace std;
using namespace storage;


void
check(const string& name, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(VGDISPLAYBIN " --units k --verbose " + quote(name), input);

    CmdVgdisplay cmdvgdisplay(name);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmdvgdisplay;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"  --- Volume group ---",
	"  VG Name               system",
	"  System ID             ",
	"  Format                lvm2",
	"  Metadata Areas        1",
	"  Metadata Sequence No  4",
	"  VG Access             read/write",
	"  VG Status             resizable",
	"  MAX LV                0",
	"  Cur LV                3",
	"  Open LV               3",
	"  Max PV                0",
	"  Cur PV                1",
	"  Act PV                1",
	"  VG Size               16359424.00 KiB",
	"  PE Size               4096.00 KiB",
	"  Total PE              3994",
	"  Alloc PE / Size       3993 / 16355328.00 KiB",
	"  Free  PE / Size       1 / 4096.00 KiB",
	"  VG UUID               0CjwWr-FrTK-wpFX-jagq-hdqS-cLbX-QNLQyH",
	"   ",
	"  --- Logical volume ---",
	"  LV Path                /dev/system/home",
	"  LV Name                home",
	"  VG Name                system",
	"  LV UUID                RhARFY-6Ygt-fD6z-sfce-guO0-wF3W-VVDOuz",
	"  LV Write Access        read/write",
	"  LV Creation host, time linux, 2014-02-07 11:13:13 +0000",
	"  LV Status              available",
	"  # open                 1",
	"  LV Size                8536064.00 KiB",
	"  Current LE             2084",
	"  Segments               1",
	"  Allocation             inherit",
	"  Read ahead sectors     auto",
	"  - currently set to     1024",
	"  Block device           253:2",
	"   ",
	"  --- Logical volume ---",
	"  LV Path                /dev/system/root",
	"  LV Name                root",
	"  VG Name                system",
	"  LV UUID                7tqhRv-uE2c-Nepc-6qd8-Uz80-kyMW-5Ce7Pm",
	"  LV Write Access        read/write",
	"  LV Creation host, time linux, 2014-02-07 11:13:13 +0000",
	"  LV Status              available",
	"  # open                 1",
	"  LV Size                6291456.00 KiB",
	"  Current LE             1536",
	"  Segments               1",
	"  Allocation             inherit",
	"  Read ahead sectors     auto",
	"  - currently set to     1024",
	"  Block device           253:0",
	"   ",
	"  --- Logical volume ---",
	"  LV Path                /dev/system/swap",
	"  LV Name                swap",
	"  VG Name                system",
	"  LV UUID                hTHP5T-d1Au-s79B-OYBH-vn1U-i4v0-kvBAzO",
	"  LV Write Access        read/write",
	"  LV Creation host, time linux, 2014-02-07 11:13:14 +0000",
	"  LV Status              available",
	"  # open                 2",
	"  LV Size                1527808.00 KiB",
	"  Current LE             373",
	"  Segments               1",
	"  Allocation             inherit",
	"  Read ahead sectors     auto",
	"  - currently set to     1024",
	"  Block device           253:1",
	"   ",
	"  --- Physical volumes ---",
	"  PV Name               /dev/sda2     ",
	"  PV UUID               xzyv9B-emKN-tx32-ySlg-EId1-pbHW-D5XkdZ",
	"  PV Status             allocatable",
	"  Total PE / Free PE    3994 / 1",
	"   ",
    };

    vector<string> output = {
	"name:system uuid:0CjwWr-FrTK-wpFX-jagq-hdqS-cLbX-QNLQyH status:resizable pe_size:4096 num_pe:3994 free_pe:1",
	"lv name:home uuid:RhARFY-6Ygt-fD6z-sfce-guO0-wF3W-VVDOuz status:available num_le:2084",
	"lv name:root uuid:7tqhRv-uE2c-Nepc-6qd8-Uz80-kyMW-5Ce7Pm status:available num_le:1536",
	"lv name:swap uuid:hTHP5T-d1Au-s79B-OYBH-vn1U-i4v0-kvBAzO status:available num_le:373",
	"pv device:/dev/sda2 uuid:xzyv9B-emKN-tx32-ySlg-EId1-pbHW-D5XkdZ status:allocatable num_pe:3994 free_pe:1"
    };

    check("system", input, output);
}
