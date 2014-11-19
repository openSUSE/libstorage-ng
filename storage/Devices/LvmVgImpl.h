#ifndef LVM_VG_IMPL_H
#define LVM_VG_IMPL_H


#include "storage/Devices/LvmVg.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    class LvmVg::Impl : public Device::Impl
    {
    public:

	Impl(DeviceGraph& device_graph, const string& name)
	    : Device::Impl(device_graph), name(name) {}

	Impl(DeviceGraph& device_graph, const Impl& impl)
	    : Device::Impl(device_graph, impl), name(impl.name) {}

	virtual Impl* clone(DeviceGraph& device_graph) const override { return new Impl(device_graph, *this); }

	string name;

    };

}

#endif
