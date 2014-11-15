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

	Impl(const string& name) : Device::Impl(), name(name) {}


    };

}

#endif
