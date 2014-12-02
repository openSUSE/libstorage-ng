

#include "storage/Devices/LvmLvImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


using namespace std;


namespace storage
{

    LvmLv::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
    }


    void
    LvmLv::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);
    }

}
