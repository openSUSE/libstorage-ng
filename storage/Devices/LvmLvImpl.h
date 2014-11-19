#ifndef LVM_LV_IMPL_H
#define LVM_LV_IMPL_H


#include "storage/Devices/LvmLv.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    using namespace std;


    class LvmLv::Impl : public BlkDevice::Impl
    {
    public:

	Impl(DeviceGraph& device_graph, const string& name)
	    : BlkDevice::Impl(device_graph, name) {}

	Impl(DeviceGraph& device_graph, const Impl& impl)
	    : BlkDevice::Impl(device_graph, impl) {}

	virtual Impl* clone(DeviceGraph& device_graph) const override { return new Impl(device_graph, *this); }

    };

}

#endif
