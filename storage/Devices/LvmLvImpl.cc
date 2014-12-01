

#include "storage/Devices/LvmLvImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


using namespace std;


namespace storage
{

    void
    LvmLv::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);
    }

}
