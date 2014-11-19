#ifndef ENCRYPTION_IMPL_H
#define ENCRYPTION_IMPL_H


#include "storage/Devices/Encryption.h"
#include "storage/Devices/BlkDeviceImpl.h"


namespace storage
{

    using namespace std;


    class Encryption::Impl : public BlkDevice::Impl
    {
    public:

	Impl(DeviceGraph& device_graph, const string& name)
	    : BlkDevice::Impl(device_graph, name) {}

	Impl(DeviceGraph& device_graph, const Impl& impl)
	    : BlkDevice::Impl(device_graph, impl) {}

	virtual Impl* clone(DeviceGraph& device_graph) const override { return new Impl(device_graph, *this); }

	virtual void add_create_actions(ActionGraph& action_graph) const override;

	// password
	// mount-by for crypttab

    };

}

#endif
