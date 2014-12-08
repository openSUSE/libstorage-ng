

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    BlkDevice::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), size_k(0), name()
    {
	if (!getChildValue(node, "name", name))
	    throw runtime_error("no name");
    }


    void
    BlkDevice::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "name", name);
    }


    unsigned long long
    BlkDevice::Impl::getSizeK() const
    {
	return size_k;
    }


    void
    BlkDevice::Impl::setSizeK(unsigned long long size_k)
    {
	Impl::size_k = size_k;
    }

}
