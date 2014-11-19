

#include <iostream>

#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    using namespace std;


    BlkDevice::BlkDevice(Impl* impl)
	: Device(impl)
    {
    }



    BlkDevice::BlkDevice(DeviceGraph& device_graph, Impl* impl)
	: Device(device_graph, impl)
    {
    }


    BlkDevice::Impl&
    BlkDevice::getImpl()
    {
	return dynamic_cast<Impl&>(Device::getImpl());
    }


    const BlkDevice::Impl&
    BlkDevice::getImpl() const
    {
	return dynamic_cast<const Impl&>(Device::getImpl());
    }


    void
    BlkDevice::setName(const string& name)
    {
	getImpl().name = name;
    }


    const string&
    BlkDevice::getName() const
    {
	return getImpl().name;
    }


    void
    BlkDevice::check() const
    {
	if (getName().empty())
	    cerr << "blk device has no name" << endl;
    }

}
