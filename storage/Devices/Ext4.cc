

#include "storage/Devices/Ext4Impl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Ext4*
    Ext4::create(DeviceGraph* device_graph)
    {
	Ext4* ret = new Ext4(new Ext4::Impl());
	ret->Device::create(device_graph);
	return ret;
    }


    Ext4*
    Ext4::load(DeviceGraph* device_graph, const xmlNode* node)
    {
	Ext4* ret = new Ext4(new Ext4::Impl(node));
	ret->Device::load(device_graph);
	return ret;
    }


    Ext4::Ext4(Impl* impl)
	: Filesystem(impl)
    {
    }


    Ext4*
    Ext4::clone() const
    {
	return new Ext4(getImpl().clone());
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
