
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdUdevadm.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const string& file, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(UDEVADMBIN " info " + quote(file), input);

    CmdUdevadmInfo cmdudevadminfo(file);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmdudevadminfo;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"P: /devices/pci0000:00/0000:00:1f.2/ata1/host0/target0:0:0/0:0:0:0/block/sda/sda1",
	"N: sda1",
	"S: disk/by-id/ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683-part1",
	"S: disk/by-id/scsi-0ATA_WDC_WD10EADS-00M_WD-WCAV52321683-part1",
	"S: disk/by-id/scsi-1ATA_WDC_WD10EADS-00M2B0_WD-WCAV52321683-part1",
	"S: disk/by-id/scsi-350014ee203733bb5-part1",
	"S: disk/by-id/scsi-SATA_WDC_WD10EADS-00M_WD-WCAV52321683-part1",
	"S: disk/by-id/scsi-SATA_WDC_WD10EADS-00_WD-WCAV52321683-part1",
	"S: disk/by-id/wwn-0x50014ee203733bb5-part1",
	"S: disk/by-label/BOOT",
	"S: disk/by-path/pci-0000:00:1f.2-ata-1.0-part1",
	"S: disk/by-uuid/14875716-b8e3-4c83-ac86-48c20682b63a",
	"E: DEVLINKS=/dev/disk/by-id/ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683-part1 /dev/disk/by-id/scsi-0ATA_WDC_WD10EADS-00M_WD-WCAV52321683-part1 /dev/disk/by-id/scsi-1ATA_WDC_WD10EADS-00M2B0_WD-WCAV52321683-part1 /dev/disk/by-id/scsi-350014ee203733bb5-part1 /dev/disk/by-id/scsi-SATA_WDC_WD10EADS-00M_WD-WCAV52321683-part1 /dev/disk/by-id/scsi-SATA_WDC_WD10EADS-00_WD-WCAV52321683-part1 /dev/disk/by-id/wwn-0x50014ee203733bb5-part1 /dev/disk/by-label/BOOT /dev/disk/by-path/pci-0000:00:1f.2-ata-1.0-part1 /dev/disk/by-uuid/14875716-b8e3-4c83-ac86-48c20682b63a",
	"E: DEVNAME=/dev/sda1",
	"E: DEVPATH=/devices/pci0000:00/0000:00:1f.2/ata1/host0/target0:0:0/0:0:0:0/block/sda/sda1",
	"E: DEVTYPE=partition",
	"E: ID_ATA=1",
	"E: ID_BUS=ata",
	"E: ID_FS_LABEL=BOOT",
	"E: ID_FS_LABEL_ENC=BOOT",
	"E: ID_FS_TYPE=ext3",
	"E: ID_FS_USAGE=filesystem",
	"E: ID_FS_UUID=14875716-b8e3-4c83-ac86-48c20682b63a",
	"E: ID_FS_UUID_ENC=14875716-b8e3-4c83-ac86-48c20682b63a",
	"E: ID_FS_VERSION=1.0",
	"E: ID_MODEL=WDC_WD10EADS-00M",
	"E: ID_MODEL_ENC=WDC\x20WD10EADS-00M",
	"E: ID_PART_ENTRY_DISK=8:0",
	"E: ID_PART_ENTRY_FLAGS=0x80",
	"E: ID_PART_ENTRY_NUMBER=1",
	"E: ID_PART_ENTRY_OFFSET=2048",
	"E: ID_PART_ENTRY_SCHEME=dos",
	"E: ID_PART_ENTRY_SIZE=2103296",
	"E: ID_PART_ENTRY_TYPE=0x83",
	"E: ID_PART_ENTRY_UUID=000c0a5a-01",
	"E: ID_PART_TABLE_TYPE=dos",
	"E: ID_PART_TABLE_UUID=000c0a5a",
	"E: ID_PATH=pci-0000:00:1f.2-ata-1.0",
	"E: ID_PATH_TAG=pci-0000_00_1f_2-ata-1_0",
	"E: ID_REVISION=0A01",
	"E: ID_SCSI=1",
	"E: ID_SCSI_COMPAT=SATA_WDC_WD10EADS-00M_WD-WCAV52321683",
	"E: ID_SCSI_COMPAT_TRUNCATED=SATA_WDC_WD10EADS-00_WD-WCAV52321683",
	"E: ID_SERIAL=WDC_WD10EADS-00M2B0_WD-WCAV52321683",
	"E: ID_SERIAL_SHORT=WD-WCAV52321683",
	"E: ID_TYPE=disk",
	"E: ID_VENDOR=ATA",
	"E: ID_VENDOR_ENC=ATA\x20\x20\x20\x20\x20",
	"E: ID_WWN=0x50014ee203733bb5",
	"E: ID_WWN_WITH_EXTENSION=0x50014ee203733bb5",
	"E: MAJOR=8",
	"E: MINOR=1",
	"E: MPATH_SBIN_PATH=/sbin",
	"E: SCSI_IDENT_LUN_ATA=WDC_WD10EADS-00M2B0_WD-WCAV52321683",
	"E: SCSI_IDENT_LUN_NAA=50014ee203733bb5",
	"E: SCSI_IDENT_LUN_T10=ATA_WDC_WD10EADS-00M2B0_WD-WCAV52321683",
	"E: SCSI_IDENT_LUN_VENDOR=WD-WCAV52321683",
	"E: SCSI_IDENT_SERIAL=WD-WCAV52321683",
	"E: SCSI_MODEL=WDC_WD10EADS-00M",
	"E: SCSI_MODEL_ENC=WDC\x20WD10EADS-00M",
	"E: SCSI_REVISION=0A01",
	"E: SCSI_TPGS=0",
	"E: SCSI_TYPE=disk",
	"E: SCSI_VENDOR=ATA",
	"E: SCSI_VENDOR_ENC=ATA\x20\x20\x20\x20\x20",
	"E: SUBSYSTEM=block",
	"E: TAGS=:systemd:",
	"E: USEC_INITIALIZED=759018",
	"",
    };

    vector<string> output = {
	"file:/dev/sda1 path:/devices/pci0000:00/0000:00:1f.2/ata1/host0/target0:0:0/0:0:0:0/block/sda/sda1 name:sda1 majorminor:8:1 by-path-links:<pci-0000:00:1f.2-ata-1.0-part1> by-id-links:<ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683-part1 scsi-0ATA_WDC_WD10EADS-00M_WD-WCAV52321683-part1 scsi-1ATA_WDC_WD10EADS-00M2B0_WD-WCAV52321683-part1 scsi-350014ee203733bb5-part1 scsi-SATA_WDC_WD10EADS-00M_WD-WCAV52321683-part1 scsi-SATA_WDC_WD10EADS-00_WD-WCAV52321683-part1 wwn-0x50014ee203733bb5-part1>"
    };

    check("/dev/sda1", input, output);
}
