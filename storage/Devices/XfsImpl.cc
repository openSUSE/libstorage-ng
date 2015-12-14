

#include <iostream>

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/XfsImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Xfs>::classname = "Xfs";


    Xfs::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node)
    {
    }


    void
    Xfs::Impl::do_create() const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	blkdevice->get_impl().wait_for_device();

	string cmd_line = MKFSXFSBIN " -q -f -m crc=1 " + quote(blkdevice->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create xfs failed"));
    }


    void
    Xfs::Impl::do_set_label() const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	string cmd_line = XFSADMINBIN " -L " + quote(get_label().empty() ? "--" : get_label()) + " " +
	    quote(blkdevice->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-label xfs failed"));
    }

}
