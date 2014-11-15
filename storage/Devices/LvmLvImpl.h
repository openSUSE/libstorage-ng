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

	Impl(const string& name) : BlkDevice::Impl(name) {}

	virtual Impl* clone() const override { return new Impl(*this); }

    };

}

#endif
