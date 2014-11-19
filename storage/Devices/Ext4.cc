

#include "storage/Devices/Ext4Impl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Ext4::Ext4(DeviceGraph& device_graph)
	: Filesystem(device_graph, new Ext4::Impl(device_graph))
    {
    }


    Ext4::Ext4(Impl* impl)
	: Filesystem(impl)
    {
    }


/*
    Ext4::Ext4(DeviceGraph& device_graph, Impl* impl)
	: Filesystem(device_graph, impl)
    {
    }
*/


    Ext4*
    Ext4::clone(DeviceGraph& device_graph) const
    {
	return new Ext4(getImpl().clone(device_graph));
    }


    Ext4::Impl&
    Ext4::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const Ext4::Impl&
    Ext4::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }

}
