

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/FilesystemImpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/FreeInfo.h"


namespace storage
{


    const char* DeviceTraits<BlkDevice>::classname = "BlkDevice";


    BlkDevice::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), name(), size_k(0)
    {
	if (!getChildValue(node, "name", name))
	    throw runtime_error("no name");

	getChildValue(node, "sysfs-name", sysfs_name);
	getChildValue(node, "sysfs-path", sysfs_path);

	getChildValue(node, "size-k", size_k);

	getChildValue(node, "udev-path", udev_path);
	getChildValue(node, "udev-id", udev_ids);
    }


    void
    BlkDevice::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Device::Impl::probe_pass_1(probed, systeminfo);

	const CmdUdevadmInfo& cmdudevadminfo = systeminfo.getCmdUdevadmInfo(name);

	sysfs_name = cmdudevadminfo.get_name();
	sysfs_path = cmdudevadminfo.get_path();

	// TODO read "sysfs_path + /size" and drop ProcParts?
	if (!systeminfo.getProcParts().getSize(sysfs_name, size_k))
	    throw;

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


    ResizeInfo
    BlkDevice::Impl::detect_resize_info() const
    {
	// TODO handle all types of children

	const Filesystem* filesystem = get_filesystem();

	ResizeInfo resize_info = filesystem->get_impl().detect_resize_info();

	return resize_info;
    }


    void
    BlkDevice::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	Device::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (get_size_k() != lhs.get_size_k())
	{
	    ResizeMode resize_mode = get_size_k() < lhs.get_size_k() ? ResizeMode::SHRINK : ResizeMode::GROW;

	    vector<Action::Base*> actions;

	    if (resize_mode == ResizeMode::GROW)
	    {
		actions.push_back(new Action::Resize(get_sid(), resize_mode));
	    }

	    // TODO handle children that cannot be resized, filesystems that
	    // are created must not be resized, encryption between partition
	    // and filesystem, ...

	    try
	    {
		const Filesystem* filesystem = get_filesystem();
		actions.push_back(new Action::Resize(filesystem->get_sid(), resize_mode));
	    }
	    catch (const Exception&)
	    {
	    }

	    if (resize_mode == ResizeMode::SHRINK)
	    {
		actions.push_back(new Action::Resize(get_sid(), resize_mode));
	    }

	    actiongraph.add_chain(actions);
	}
    }


    bool
    BlkDevice::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return name == rhs.name && sysfs_name == rhs.sysfs_name && sysfs_path == rhs.sysfs_path &&
	    size_k == rhs.size_k;
    }


    void
    BlkDevice::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "name", name, rhs.name);

	storage::log_diff(log, "sysfs-name", sysfs_name, rhs.sysfs_name);
	storage::log_diff(log, "sysfs-path", sysfs_path, rhs.sysfs_path);

	storage::log_diff(log, "size-k", size_k, rhs.size_k);
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
	    out << " size-k:" << get_size_k();

	if (!udev_path.empty())
	    out << " udev-path:" << udev_path;

	if (!udev_ids.empty())
	    out << " udev-ids:" << udev_ids;
    }


    Filesystem*
    BlkDevice::Impl::get_filesystem()
    {
	return get_single_child_of_type<Filesystem>();
    }


    const Filesystem*
    BlkDevice::Impl::get_filesystem() const
    {
	return get_single_child_of_type<const Filesystem>();
    }


    void
    BlkDevice::Impl::wait_for_device() const
    {
	SystemCmd(UDEVADMBIN_SETTLE);

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
	    ST_THROW(Exception("wait_for_device failed"));
    }


    void
    BlkDevice::Impl::wipe_device() const
    {
	string cmd_line = WIPEFSBIN " --all " + quote(get_name());

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("wipefs failed"));
    }

}
