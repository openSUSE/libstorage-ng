

#include "storage/Devices/Ext4Impl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Ext4*
    Ext4::create(Devicegraph* devicegraph)
    {
	Ext4* ret = new Ext4(new Ext4::Impl());
	ret->Device::create(devicegraph);
	return ret;
    }


    Ext4*
    Ext4::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Ext4* ret = new Ext4(new Ext4::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Ext4::Ext4(Impl* impl)
	: Filesystem(impl)
    {
    }


    Ext4*
    Ext4::clone() const
    {
	return new Ext4(get_impl().clone());
    }


    Ext4::Impl&
    Ext4::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Ext4::Impl&
    Ext4::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }

}
