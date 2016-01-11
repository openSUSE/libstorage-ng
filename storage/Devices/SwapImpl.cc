

#include <iostream>

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/SwapImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Swap>::classname = "Swap";


    Swap::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node)
    {
    }


    void
    Swap::Impl::do_create() const
    {
	const BlkDevice* blk_device = get_blk_device();

	blk_device->get_impl().wait_for_device();

	string cmd_line = MKSWAPBIN " -f " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create swap failed"));
    }


    void
    Swap::Impl::do_mount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = SWAPONBIN " --fixpgsz " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("swapon failed"));
    }


    void
    Swap::Impl::do_umount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = SWAPOFFBIN " " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("swapoff failed"));
    }

}
