

#include "storage/Devices/LvmLvImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    LvmLv*
    LvmLv::create(DeviceGraph* device_graph, const string& name)
    {
	LvmLv* ret = new LvmLv(new LvmLv::Impl(name));
	ret->Device::create(device_graph);
	return ret;
    }


    LvmLv*
    LvmLv::load(DeviceGraph* device_graph, const xmlNode* node)
    {
	LvmLv* ret = new LvmLv(new LvmLv::Impl(node));
	ret->Device::load(device_graph);
	return ret;
    }


    LvmLv::LvmLv(Impl* impl)
	: BlkDevice(impl)
    {
    }


    LvmLv*
    LvmLv::clone() const
    {
	return new LvmLv(getImpl().clone());
    }


    LvmLv::Impl&
    LvmLv::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const LvmLv::Impl&
    LvmLv::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }


    void
    LvmLv::check() const
    {
	if (getName().empty())
	    cerr << "logical volume has no name" << endl;
    }

}
