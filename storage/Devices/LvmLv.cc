

#include "storage/Devices/LvmLvImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


using namespace std;


namespace storage
{

    LvmLv::LvmLv(DeviceGraph& device_graph, const string& name)
	: BlkDevice(device_graph, new LvmLv::Impl(device_graph, name))
    {
    }


    LvmLv::LvmLv(Impl* impl)
	: BlkDevice(impl)
    {
    }


    LvmLv::LvmLv(DeviceGraph& device_graph, Impl* impl)
	: BlkDevice(device_graph, impl)
    {
    }


    LvmLv*
    LvmLv::clone(DeviceGraph& device_graph) const
    {
	return new LvmLv(getImpl().clone(device_graph));
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
