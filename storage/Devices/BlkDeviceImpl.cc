

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/Filesystem.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


namespace storage
{

    BlkDevice::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), name(), size_k(0), major_minor(0)
    {
	if (!getChildValue(node, "name", name))
	    throw runtime_error("no name");

	getChildValue(node, "sysfs-name", sysfs_name);
	getChildValue(node, "sysfs-path", sysfs_path);

	getChildValue(node, "size-k", size_k);

	unsigned int major = 0, minor = 0;
	if (getChildValue(node, "major", major) && getChildValue(node, "minor", minor))
	    major_minor = makedev(major, minor);

	getChildValue(node, "udev-path", udev_path);
	getChildValue(node, "udev-id", udev_ids);
    }


    void
    BlkDevice::Impl::probe(SystemInfo& systeminfo)
    {
	Device::Impl::probe(systeminfo);

	const CmdUdevadmInfo& cmdudevadminfo = systeminfo.getCmdUdevadmInfo(name);

	sysfs_name = cmdudevadminfo.get_name();
	sysfs_path = cmdudevadminfo.get_path();

	// TODO read "sysfs_path + /size" and drop ProcParts?
	if (!systeminfo.getProcParts().getSize(sysfs_name, size_k))
	    throw;

	major_minor = cmdudevadminfo.get_majorminor();

	if (!cmdudevadminfo.get_by_path_links().empty())
	{
	    udev_path = cmdudevadminfo.get_by_path_links().front();
	    process_udev_path(udev_path);
	}

	if (!cmdudevadminfo.get_by_id_links().empty())
	{
	    udev_ids = cmdudevadminfo.get_by_id_links();
	    process_udev_ids(udev_ids);
	}
    }


    void
    BlkDevice::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "name", name);

	setChildValueIf(node, "sysfs-name", sysfs_name, !sysfs_name.empty());
	setChildValueIf(node, "sysfs-path", sysfs_path, !sysfs_path.empty());

	setChildValueIf(node, "size-k", size_k, size_k > 0);

	setChildValueIf(node, "major", gnu_dev_major(major_minor), major_minor != 0);
	setChildValueIf(node, "minor", gnu_dev_minor(major_minor), major_minor != 0);

	setChildValueIf(node, "udev-path", udev_path, !udev_path.empty());
	setChildValueIf(node, "udev-id", udev_ids, !udev_ids.empty());
    }


    void
    BlkDevice::Impl::set_name(const string& name)
    {
	Impl::name = name;
    }


    void
    BlkDevice::Impl::set_size_k(unsigned long long size_k)
    {
	Impl::size_k = size_k;
    }


    string
    BlkDevice::Impl::get_size_string() const
    {
	return byte_to_humanstring(1024 * get_size_k(), false, 2, false);
    }


    bool
    BlkDevice::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return name == rhs.name && sysfs_name == rhs.sysfs_name && sysfs_path == rhs.sysfs_path &&
	    size_k == rhs.size_k && major_minor == rhs.major_minor;
    }


    void
    BlkDevice::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "name", name, rhs.name);

	storage::log_diff(log, "sysfs-name", sysfs_name, rhs.sysfs_name);
	storage::log_diff(log, "sysfs-path", sysfs_path, rhs.sysfs_path);

	storage::log_diff(log, "size_k", size_k, rhs.size_k);
	storage::log_diff(log, "major", get_major(), rhs.get_major());
	storage::log_diff(log, "minor", get_minor(), rhs.get_minor());
    }


    void
    BlkDevice::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	out << " name:" << get_name();

	if (!sysfs_name.empty())
	    out << " sysfs-name:" << sysfs_name;

	if (!sysfs_path.empty())
	    out << " sysfs-path:" << sysfs_path;

	if (get_size_k() != 0)
	    out << " size_k:" << get_size_k();

	if (get_majorminor() != 0)
	    out << " major:" << get_major() << " minor:" << get_minor();

	if (!udev_path.empty())
	    out << " udev-path:" << udev_path;

	if (!udev_ids.empty())
	    out << " udev-ids:" << udev_ids;
    }


    Filesystem*
    BlkDevice::Impl::get_filesystem()
    {
	if (get_device()->num_children() != 1)
	    throw runtime_error("blkdevice has no children");

	const Devicegraph* devicegraph = get_devicegraph();

	Device* child = devicegraph->get_impl().graph[devicegraph->get_impl().child(get_vertex())].get();

	return dynamic_cast<Filesystem*>(child);
    }


    const Filesystem*
    BlkDevice::Impl::get_filesystem() const
    {
	if (get_device()->num_children() != 1)
	    throw runtime_error("blkdevice has no children");

	const Devicegraph* devicegraph = get_devicegraph();

	const Device* child = devicegraph->get_impl().graph[devicegraph->get_impl().child(get_vertex())].get();

	return dynamic_cast<const Filesystem*>(child);
    }


    void
    BlkDevice::Impl::wait_for_device() const
    {
	string cmd_line(UDEVADMBIN " settle --timeout=20");
	SystemCmd cmd(cmd_line);

	bool exist = access(name.c_str(), R_OK) == 0;
	y2mil("name:" << name << " exist:" << exist);

	if (!exist)
	{
	    for (int count = 0; count < 500; ++count)
	    {
		usleep(10000);
		exist = access(name.c_str(), R_OK) == 0;
		if (exist)
		    break;
	    }
	    y2mil("name:" << name << " exist:" << exist);
	}

	if (!exist)
	    throw runtime_error("wait_for_device failed");
    }

}
