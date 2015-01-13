

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/SystemInfo/SystemInfo.h"


namespace storage
{

    BlkDevice::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), name(), size_k(0), major_minor(0)
    {
	if (!getChildValue(node, "name", name))
	    throw runtime_error("no name");

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

	if (!systeminfo.getProcParts().getSize(name, size_k))
	    throw;

	major_minor = systeminfo.getMajorMinor(name).getMajorMinor();

	const UdevMap& by_path = systeminfo.getUdevMap("/dev/disk/by-path");
	UdevMap::const_iterator it1 = by_path.find(name.substr(5)); // TODO
	if (it1 != by_path.end())
	{
	    udev_path = it1->second.front();
	    process_udev_path(udev_path);
	}

	const UdevMap& by_id = systeminfo.getUdevMap("/dev/disk/by-id");
	UdevMap::const_iterator it2 = by_id.find(name.substr(5)); // TODO
	if (it2 != by_id.end())
	{
	    udev_ids = it2->second;
	    process_udev_ids(udev_ids);
	}
    }


    void
    BlkDevice::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "name", name);

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

	return name == rhs.name && size_k == rhs.size_k && major_minor == rhs.major_minor;
    }


    void
    BlkDevice::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "name", name, rhs.name);
	storage::log_diff(log, "size_k", size_k, rhs.size_k);
	storage::log_diff(log, "major", get_major(), rhs.get_major());
	storage::log_diff(log, "minor", get_minor(), rhs.get_minor());
    }


    void
    BlkDevice::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	out << " name:" << get_name();

	if (get_size_k() != 0)
	    out << " size_k:" << get_size_k();

	if (get_majorminor() != 0)
	    out << " major:" << get_major() << " minor:" << get_minor();

	if (!udev_path.empty())
	    out << " udev-path:" << udev_path;

	if (!udev_ids.empty())
	    out << " udev-ids:" << udev_ids;
    }

}
