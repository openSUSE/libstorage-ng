

#include <iostream>

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/VfatImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Vfat>::classname = "Vfat";


    Vfat::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node)
    {
    }


    void
    Vfat::Impl::do_create() const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	blkdevice->get_impl().wait_for_device();

	string cmd_line = MKFSFATBIN " " + quote(blkdevice->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create vfat failed"));
    }


    void
    Vfat::Impl::do_set_label() const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	string cmd_line = FATLABELBIN " " + quote(blkdevice->get_name()) + " " + quote(get_label());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set-label vfat failed"));
    }

}
