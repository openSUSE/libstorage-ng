

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage_bgl
{

    BlkDevice::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), name(), size_k(0)
    {
	if (!getChildValue(node, "name", name))
	    throw runtime_error("no name");

	getChildValue(node, "size-k", size_k);
    }


    void
    BlkDevice::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "name", name);

	setChildValueIf(node, "size-k", size_k, size_k > 0);
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

}
