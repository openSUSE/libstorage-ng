
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/DevAndSys.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


using namespace std;
using namespace storage;


void
check(const string& path, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(LSBIN " -1l --sort=none " + quote(path), input);

    UdevMap udevmap(path);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << udevmap;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"total 0",
	"lrwxrwxrwx 1 root root 10 Jan 13 15:10 wwn-0x50014ee203733bb5-part2 -> ../../sda2",
	"lrwxrwxrwx 1 root root 10 Jan 13 15:10 scsi-SATA_WDC_WD10EADS-00_WD-WCAV52321683-part2 -> ../../sda2",
	"lrwxrwxrwx 1 root root 10 Jan 13 15:10 ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683-part2 -> ../../sda2",
	"lrwxrwxrwx 1 root root 10 Jan 13 15:10 wwn-0x50014ee203733bb5-part1 -> ../../sda1",
	"lrwxrwxrwx 1 root root 10 Jan 13 15:10 scsi-SATA_WDC_WD10EADS-00_WD-WCAV52321683-part1 -> ../../sda1",
	"lrwxrwxrwx 1 root root 10 Jan 13 15:10 ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683-part1 -> ../../sda1",
	"lrwxrwxrwx 1 root root  9 Jan 13 15:10 wwn-0x50014ee203733bb5 -> ../../sda",
	"lrwxrwxrwx 1 root root  9 Jan 13 15:10 scsi-SATA_WDC_WD10EADS-00_WD-WCAV52321683 -> ../../sda",
	"lrwxrwxrwx 1 root root  9 Jan 13 15:10 ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683 -> ../../sda"
    };

    vector<string> output = {
	"path:/dev/disk/by-id",
	"data[sda] -> ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683 scsi-SATA_WDC_WD10EADS-00_WD-WCAV52321683 wwn-0x50014ee203733bb5",
	"data[sda1] -> ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683-part1 scsi-SATA_WDC_WD10EADS-00_WD-WCAV52321683-part1 wwn-0x50014ee203733bb5-part1",
	"data[sda2] -> ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683-part2 scsi-SATA_WDC_WD10EADS-00_WD-WCAV52321683-part2 wwn-0x50014ee203733bb5-part2"
    };

    check("/dev/disk/by-id", input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"total 0",
	"lrwxrwxrwx 1 root root 10 Jan 13 15:10 pci-0000:00:1f.2-ata-1.0-part2 -> ../../sda2",
	"lrwxrwxrwx 1 root root 10 Jan 13 15:10 pci-0000:00:1f.2-ata-1.0-part1 -> ../../sda1",
	"lrwxrwxrwx 1 root root  9 Jan 13 15:10 pci-0000:00:1f.2-ata-1.0 -> ../../sda"
    };

    vector<string> output = {
	"path:/dev/disk/by-path",
	"data[sda] -> pci-0000:00:1f.2-ata-1.0",
	"data[sda1] -> pci-0000:00:1f.2-ata-1.0-part1",
	"data[sda2] -> pci-0000:00:1f.2-ata-1.0-part2"
    };

    check("/dev/disk/by-path", input, output);
}


// TODO tests with strange characters in paths
