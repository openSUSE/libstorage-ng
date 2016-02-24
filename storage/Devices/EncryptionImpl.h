#ifndef STORAGE_ENCRYPTION_IMPL_H
#define STORAGE_ENCRYPTION_IMPL_H


#include "storage/Utils/Enum.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Encryption> { static const char* classname; };

    template <> struct EnumTraits<EncryptionType> { static const vector<string> names; };


    class Encryption::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name)
	    : BlkDevice::Impl(name) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "Encryption"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;

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

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};

    }

}

#endif
