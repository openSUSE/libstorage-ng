

#include "storage/Devices/DeviceImpl.h"
#include "storage/DeviceGraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Device::Device(Impl* impl)
	: impl(impl)
    {
	if (!impl)
	    throw runtime_error("impl is nullptr");
    }


    Device::~Device()
    {
    }


    Device::Impl&
    Device::getImpl()
    {
	return *impl;
    }


    const Device::Impl&
    Device::getImpl() const
    {
	return *impl;
    }


    sid_t
    Device::getSid() const
    {
	return getImpl().sid;
    }


    void
    Device::check() const
    {
    }

}
