

#include <iostream>

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/NtfsImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Ntfs>::classname = "Ntfs";


    Ntfs::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node)
    {
    }


    void
    Ntfs::Impl::do_create() const
    {
	const BlkDevice* blk_device = get_blk_device();

	blk_device->get_impl().wait_for_device();

	string cmd_line = MKFSNTFSBIN " --fast --with-uuid " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create ntfs failed"));
    }


    void
    Ntfs::Impl::do_set_label() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = NTFSLABELBIN " " + quote(blk_device->get_name()) + " " +
	    quote(get_label());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-label ntfs failed"));
    }

}
