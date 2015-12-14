

#include <iostream>

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/BtrfsImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Btrfs>::classname = "Btrfs";


    Btrfs::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node)
    {
    }


    void
    Btrfs::Impl::do_create() const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	blkdevice->get_impl().wait_for_device();

	string cmd_line = MKFSBTRFSBIN " -f " + quote(blkdevice->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create btrfs failed"));
    }


    void
    Btrfs::Impl::do_set_label() const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	// TODO handle mounted

        string cmd_line = BTRFSBIN " filesystem label " + quote(blkdevice->get_name()) + " " +
	    quote(get_label());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-label btrfs failed"));
    }

}
