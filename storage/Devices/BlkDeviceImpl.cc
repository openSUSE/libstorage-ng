

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    BlkDevice::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), name()
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


}
