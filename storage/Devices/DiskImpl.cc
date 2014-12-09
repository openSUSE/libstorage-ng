

#include "storage/Devices/DiskImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    void
    Disk::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);
    }


    Disk::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
    }


    void
    Disk::Impl::add_create_actions(Actiongraph& actiongraph) const
    {
	throw runtime_error("cannot create disk");
    }


    void
    Disk::Impl::add_delete_actions(Actiongraph& actiongraph) const
    {
	throw runtime_error("cannot delete disk");
    }

}
