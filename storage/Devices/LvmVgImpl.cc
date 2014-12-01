

#include "storage/Devices/LvmVgImpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    using namespace std;


    void
    LvmVg::Impl::save(xmlNode* node) const
    {
	setChildValue(node, "name", name);
    }

}
