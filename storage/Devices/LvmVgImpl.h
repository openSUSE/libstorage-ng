#ifndef STORAGE_LVM_VG_IMPL_H
#define STORAGE_LVM_VG_IMPL_H


#include "storage/Devices/LvmVg.h"
#include "storage/Devices/DeviceImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<LvmVg> { static const char* classname; };


    class LvmVg::Impl : public Device::Impl
    {
    public:

	Impl(const string& name)
	    : Device::Impl(), name(name) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return DeviceTraits<LvmVg>::classname; }

	virtual string get_displayname() const override { return get_name(); }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	const string& get_name() const { return name; }
	void set_name(const string& name) { Impl::name = name; }

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual Text do_create_text(bool doing) const override;

    private:

	string name;

    };

}

#endif
