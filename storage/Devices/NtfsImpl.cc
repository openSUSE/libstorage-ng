

#include <iostream>
#include <boost/algorithm/string.hpp>

#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/NtfsImpl.h"
#include "storage/FreeInfo.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Ntfs>::classname = "Ntfs";


    Ntfs::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node)
    {
    }


    ResizeInfo
    Ntfs::Impl::detect_resize_info_pure() const
    {
	if (!get_devicegraph()->get_impl().is_probed())
	    ST_THROW(Exception("function called on wrong device"));

	const BlkDevice* blk_device = get_blk_device();

	blk_device->get_impl().wait_for_device();

	ResizeInfo resize_info;

	// TODO filesystem must not be mounted

	SystemCmd cmd(NTFSRESIZEBIN " --force --info " + quote(blk_device->get_name()));
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("ntfsresize --info failed"));

	string fstr = " might resize at ";
	string::size_type pos;
	string stdout = boost::join(cmd.stdout(), "\n");
	if ((pos = stdout.find(fstr)) != string::npos)
	{
	    resize_info.resize_ok = true;

	    y2mil("pos:" << pos);
	    pos = stdout.find_first_not_of(" \t\n", pos + fstr.size());
	    y2mil("pos:" << pos);
	    string number = stdout.substr(pos, stdout.find_first_not_of("0123456789", pos));
	    number >> resize_info.min_size_k;
	    resize_info.min_size_k /= 1024;

	    // see ntfsresize(8) for += 100MiB
	    resize_info.min_size_k = min(resize_info.min_size_k + 100 * MiB, blk_device->get_size_k());

	    resize_info.max_size_k = 256 * TiB;
	}

	y2mil("resize-info:" << resize_info);

	return resize_info;
    }


    ContentInfo
    Ntfs::Impl::detect_content_info_pure() const
    {
	const BlkDevice* blk_device = get_blk_device();

	blk_device->get_impl().wait_for_device();

	// TODO filesystem must be mounted

	if (get_mountpoints().empty())
	    throw;

	ContentInfo content_info;

	content_info.is_windows = detect_is_windows(get_mountpoints().front());

	return content_info;
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


    void
    Ntfs::Impl::do_resize(ResizeMode resize_mode) const
    {
	const BlkDevice* blk_device = get_blk_device();

	blk_device->get_impl().wait_for_device();

	string cmd_line = "echo y | " NTFSRESIZEBIN " --force";
	if (resize_mode == ResizeMode::SHRINK)
	    cmd_line += " --size " + to_string(blk_device->get_size_k()) + "k";
	cmd_line += " " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("resize ntfs failed"));
    }

}
