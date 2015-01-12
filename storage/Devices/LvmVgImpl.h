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

	Impl(const string& name)
	    : Device::Impl(), name(name) {}

	Impl(const xmlNode* node);

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	string name;

    };

}

#endif
