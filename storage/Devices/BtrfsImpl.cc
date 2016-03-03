

#include <iostream>

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/BtrfsImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/FreeInfo.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Btrfs>::classname = "Btrfs";


    Btrfs::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node)
    {
    }


    ResizeInfo
    Btrfs::Impl::detect_resize_info() const
    {
	ResizeInfo resize_info = Filesystem::Impl::detect_resize_info();

	resize_info.combine(ResizeInfo(256 * MiB, 16 * EiB));

	return resize_info;
    }


    void
    Btrfs::Impl::do_create() const
    {
	const BlkDevice* blk_device = get_blk_device();

	blk_device->get_impl().wait_for_device();

	string cmd_line = MKFSBTRFSBIN " -f " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create btrfs failed"));
    }


    void
    Btrfs::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	// TODO handle mounted

        string cmd_line = BTRFSBIN " filesystem label " + quote(blk_device->get_name()) + " " +
	    quote(get_label());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-label btrfs failed"));
    }

}
