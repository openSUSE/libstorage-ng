

#include "storage/Devices/LvmVgImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    using namespace std;


    LvmVg::Impl::Impl(const xmlNode* node)
	: Device::Impl(node)
    {
	if (!getChildValue(node, "name", name))
	    throw runtime_error("no name");
    }


    void
    LvmVg::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "name", name);
    }

}
