

#include <iostream>

#include "storage/Devices/LvmVgImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    LvmVg::LvmVg(const string& name)
	: Device(new LvmVg::Impl(name))
    {
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
