

#include "storage/Devices/NtfsImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    Ntfs*
    Ntfs::create(Devicegraph* devicegraph)
    {
	Ntfs* ret = new Ntfs(new Ntfs::Impl());
	ret->Device::create(devicegraph);
	return ret;
    }


    Ntfs*
    Ntfs::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Ntfs* ret = new Ntfs(new Ntfs::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Ntfs::Ntfs(Impl* impl)
	: Filesystem(impl)
    {
    }


    Ntfs*
    Ntfs::clone() const
    {
	return new Ntfs(get_impl().clone());
    }


    Ntfs::Impl&
    Ntfs::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Ntfs::Impl&
    Ntfs::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    bool
    is_ntfs(const Device* device)
    {
	return is_device_of_type<const Ntfs>(device);
    }


    Ntfs*
    to_ntfs(Device* device)
    {
	return to_device_of_type<Ntfs>(device);
    }


    const Ntfs*
    to_ntfs(const Device* device)
    {
	return to_device_of_type<const Ntfs>(device);
    }

}
