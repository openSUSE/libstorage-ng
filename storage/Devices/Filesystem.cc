

#include "storage/Devices/FilesystemImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    Filesystem::Filesystem(Impl* impl)
	: Device(impl)
    {
    }


    Filesystem::Impl&
    Filesystem::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Filesystem::Impl&
    Filesystem::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    const string&
    Filesystem::get_label() const
    {
	return get_impl().label;
    }


    void
    Filesystem::set_label(const string& label)
    {
	get_impl().label = label;
    }


    const string&
    Filesystem::get_uuid() const
    {
	return get_impl().uuid;
    }


    const vector<string>&
    Filesystem::get_mountpoints() const
    {
	return get_impl().mountpoints;
    }


    void
    Filesystem::add_mountpoint(const string& mountpoint)
    {
	return get_impl().mountpoints.push_back(mountpoint);
    }


    vector<Filesystem*>
    Filesystem::find_by_label(const Devicegraph* devicegraph, const string& label)
    {
	auto pred = [&label](const Filesystem* filesystem) {
	    return filesystem->get_label() == label;
	};

	return devicegraph->get_impl().getDevicesIf<Filesystem>(pred);
    }


    vector<Filesystem*>
    Filesystem::find_by_mountpoint(const Devicegraph* devicegraph, const string& mountpoint)
    {
	auto pred = [&mountpoint](const Filesystem* filesystem) {
	    return contains(filesystem->get_mountpoints(), mountpoint);
	};

	return devicegraph->get_impl().getDevicesIf<Filesystem>(pred);
    }


    vector<const BlkDevice*>
    Filesystem::get_blkdevices() const
    {
	return get_impl().get_blkdevices();
    }


    void
    Filesystem::print(std::ostream& out) const
    {
	Device::print(out);
    }

}
