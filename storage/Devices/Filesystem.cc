

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


    FsType
    Filesystem::get_type() const
    {
	return get_impl().get_type();
    }


    const string&
    Filesystem::get_label() const
    {
	return get_impl().get_label();
    }


    void
    Filesystem::set_label(const string& label)
    {
	get_impl().set_label(label);
    }


    const string&
    Filesystem::get_uuid() const
    {
	return get_impl().get_uuid();
    }


    const vector<string>&
    Filesystem::get_mountpoints() const
    {
	return get_impl().get_mountpoints();
    }


    void
    Filesystem::set_mountpoints(const vector<string>& mountpoints)
    {
	get_impl().set_mountpoints(mountpoints);
    }


    void
    Filesystem::add_mountpoint(const string& mountpoint)
    {
	return get_impl().add_mountpoint(mountpoint);
    }


    MountByType
    Filesystem::get_mount_by() const
    {
	return get_impl().get_mount_by();
    }


    void
    Filesystem::set_mount_by(MountByType mount_by)
    {
	get_impl().set_mount_by(mount_by);
    }


    const list<string>&
    Filesystem::get_fstab_options() const
    {
	return get_impl().get_fstab_options();
    }


    void
    Filesystem::set_fstab_options(const list<string>& fstab_options)
    {
	get_impl().set_fstab_options(fstab_options);
    }


    const string&
    Filesystem::get_mkfs_options() const
    {
	return get_impl().get_mkfs_options();
    }


    void
    Filesystem::set_mkfs_options(const string& mkfs_options)
    {
	get_impl().set_mkfs_options(mkfs_options);
    }


    const string&
    Filesystem::get_tune_options() const
    {
	return get_impl().get_tune_options();
    }


    void
    Filesystem::set_tune_options(const string& tune_options)
    {
	get_impl().set_tune_options(tune_options);
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

}
