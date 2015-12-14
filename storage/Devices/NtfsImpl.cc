

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
	const BlkDevice* blkdevice = get_blkdevice();

	blkdevice->get_impl().wait_for_device();

	string cmd_line = MKFSNTFSBIN " --fast --with-uuid " + quote(blkdevice->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    throw runtime_error("create ntfs failed");
    }


    void
    Ntfs::Impl::do_set_label() const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	string cmd_line = NTFSLABELBIN " " + quote(blkdevice->get_name()) + " " + quote(get_label());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    throw runtime_error("set-label ntfs failed");
    }

}
