#ifndef ENCRYPTION_IMPL_H
#define ENCRYPTION_IMPL_H


#include "storage/Devices/Encryption.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    class Encryption::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name)
	    : BlkDevice::Impl(name) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "Encryption"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void add_create_actions(Actiongraph& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual Text do_create_text(bool doing) const override;

	virtual Text do_open_text(bool doing) const;

    private:

	// password
	// mount-by for crypttab

    };


    namespace Action
    {

	class OpenEncryption : public Modify
	{
	public:

	    OpenEncryption(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph& actiongraph, bool doing) const override;

	};

    }

}

#endif
