

#include <iostream>

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Btrfs.h"
#include "storage/Devices/Xfs.h"
#include "storage/Devices/Swap.h"
#include "storage/Holders/User.h"
#include "storage/Utils/AppUtil.h"
#include "storage/Utils/Enum.h"


namespace storage
{

    using namespace std;


    BlkDevice::BlkDevice(Impl* impl)
	: Device(impl)
    {
    }


    BlkDevice::Impl&
    BlkDevice::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const BlkDevice::Impl&
    BlkDevice::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    BlkDevice*
    BlkDevice::find(const Devicegraph* devicegraph, const string& name)
    {
	for (Devicegraph::Impl::vertex_descriptor v : devicegraph->get_impl().vertices())
	{
	    BlkDevice* blk_device = dynamic_cast<BlkDevice*>(devicegraph->get_impl().graph[v].get());
	    if (blk_device && blk_device->get_name() == name)
		return blk_device;
	}

	ST_THROW(DeviceNotFound(name));
	// TODO, compiler does not understand that ST_THROW with doThrow = true
	// always does a throw.
	__builtin_unreachable();
    }


    void
    BlkDevice::set_name(const string& name)
    {
	get_impl().set_name(name);
    }


    const string&
    BlkDevice::get_name() const
    {
	return get_impl().get_name();
    }


    unsigned long long
    BlkDevice::get_size_k() const
    {
	return get_impl().get_size_k();
    }


    void
    BlkDevice::set_size_k(unsigned long long size_k)
    {
	get_impl().set_size_k(size_k);
    }


    string
    BlkDevice::get_size_string() const
    {
	return get_impl().get_size_string();
    }


    dev_t
    BlkDevice::get_majorminor() const
    {
	return get_impl().get_majorminor();
    }


    unsigned int
    BlkDevice::get_major() const
    {
	return get_impl().get_major();
    }


    unsigned int
    BlkDevice::get_minor() const
    {
	return get_impl().get_minor();
    }


    const std::string&
    BlkDevice::get_udev_path() const
    {
	return get_impl().get_udev_path();
    }


    const std::vector<std::string>&
    BlkDevice::get_udev_ids() const
    {
	return get_impl().get_udev_ids();
    }


    void
    BlkDevice::check() const
    {
	if (get_name().empty())
	    cerr << "blk device has no name" << endl;
    }


    Filesystem*
    BlkDevice::create_filesystem(FsType fs_type)
    {
	if (num_children() != 0)
	    ST_THROW(WrongNumberOfChildren(num_children(), 0));

	Filesystem* ret = nullptr;

	switch (fs_type)
	{
	    case EXT4:
		ret = Ext4::create(get_impl().get_devicegraph());
		break;

	    case BTRFS:
		ret = Btrfs::create(get_impl().get_devicegraph());
		break;

	    case XFS:
		ret = Xfs::create(get_impl().get_devicegraph());
		break;

	    case SWAP:
		ret = Swap::create(get_impl().get_devicegraph());
		break;

	    default:
		ST_THROW(NotImplementedException("unimplemented filesystem type " + toString(fs_type)));
	}

	User::create(get_impl().get_devicegraph(), this, ret);

	return ret;
    }


    Filesystem*
    BlkDevice::get_filesystem()
    {
	return get_impl().get_filesystem();
    }


    const Filesystem*
    BlkDevice::get_filesystem() const
    {
	return get_impl().get_filesystem();
    }


    bool
    is_blkdevice(const Device* device)
    {
	return dynamic_cast<const BlkDevice*>(device);
    }


    BlkDevice*
    to_blkdevice(Device* device)
    {
	return to_device_of_type<BlkDevice>(device);
    }


    const BlkDevice*
    to_blkdevice(const Device* device)
    {
	return to_device_of_type<const BlkDevice>(device);
    }

}
