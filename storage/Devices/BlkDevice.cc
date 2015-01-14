

#include <iostream>

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"
#include "storage/Holders/Using.h"
#include "storage/Utils/AppUtil.h"


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

	throw DeviceNotFound(sformat("device not found, name:%s", name.c_str()));
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
	    throw runtime_error("BlkDevice has children");

	Filesystem* ret = nullptr;

	switch (fs_type)
	{
	    case EXT4:
		ret = Ext4::create(get_impl().get_devicegraph());
		break;

	    case SWAP:
		ret = Swap::create(get_impl().get_devicegraph());
		break;

	    default:
		// TODO
		break;
	}

	if (!ret)
	    throw runtime_error("unknown filesystem type");

	Using::create(get_impl().get_devicegraph(), this, ret);

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

}
