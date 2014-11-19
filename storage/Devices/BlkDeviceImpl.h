#ifndef BLK_DEVICE_IMPL_H
#define BLK_DEVICE_IMPL_H


#include "storage/Devices/BlkDevice.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    // abstract class

    class BlkDevice::Impl : public Device::Impl
    {
    public:

	string name;

	// size
	// major and minor
	// udev_id and udev_path

    protected:

	Impl(DeviceGraph& device_graph, const string& name)
	    : Device::Impl(device_graph), name(name) {}

	// TODO not nice that all members must be initialized individual
	Impl(DeviceGraph& device_graph, const Impl& impl)
	    : Device::Impl(device_graph, impl), name(impl.name) {}

    };

}

#endif
