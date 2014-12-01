

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    void
    BlkDevice::Impl::save(xmlNode* node) const
    {
	setChildValue(node, "name", name);
    }

}
