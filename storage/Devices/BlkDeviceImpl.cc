

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
    }


    void
    BlkDevice::Impl::probe(SystemInfo& systeminfo)
    {
	Device::Impl::probe(systeminfo);

	if (!systeminfo.getProcParts().getSize(name, size_k))
	    throw;

	major_minor = systeminfo.getMajorMinor(name).getMajorMinor();
    }


    void
    BlkDevice::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "name", name);

	setChildValueIf(node, "size-k", size_k, size_k > 0);

	setChildValueIf(node, "major", gnu_dev_major(major_minor), major_minor != 0);
	setChildValueIf(node, "minor", gnu_dev_minor(major_minor), major_minor != 0);
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

}
