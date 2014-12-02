

#include <iostream>

#include "storage/Devices/LvmVgImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    LvmVg*
    LvmVg::create(DeviceGraph* device_graph, const string& name)
    {
	LvmVg* ret = new LvmVg(new LvmVg::Impl(name));
	ret->addToDeviceGraph(device_graph);
	return ret;
    }


    LvmVg*
    LvmVg::load(DeviceGraph* device_graph, const xmlNode* node)
    {
	LvmVg* ret = new LvmVg(new LvmVg::Impl(node));
	ret->addToDeviceGraph(device_graph);
	return ret;
    }


    LvmVg::LvmVg(Impl* impl)
	: Device(impl)
    {
    }


    LvmVg*
    LvmVg::clone() const
    {
	return new LvmVg(getImpl().clone());
    }


    LvmVg::Impl&
    LvmVg::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const LvmVg::Impl&
    LvmVg::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }


    const string&
    LvmVg::getName() const
    {
	return getImpl().name;
    }


    void
    LvmVg::setName(const string& name)
    {
	getImpl().name = name;
    }


    void
    LvmVg::check() const
    {
	if (getName().empty())
	    cerr << "volume group has no name" << endl;
    }

}
