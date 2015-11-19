#ifndef STORAGE_LVM_LV_IMPL_H
#define STORAGE_LVM_LV_IMPL_H


#include "storage/Devices/LvmLv.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    class LvmLv::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& name)
	    : BlkDevice::Impl(name) {}

	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "LvmLv"; }

	virtual Impl* clone() const override { return new Impl(*this); }

	virtual void save(xmlNode* node) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;

	virtual void print(std::ostream& out) const override;

	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;

	virtual Text do_create_text(bool doing) const override;

	virtual Text do_rename_text(const Impl& lhs, bool doing) const;
	virtual void do_rename(const Impl& lhs) const;

    };


    namespace Action
    {

	class Rename : public Modify
	{
	public:

	    Rename(sid_t sid) : Modify(sid) {}

	    virtual Text text(const Actiongraph::Impl& actiongraph, bool doing) const override;
	    virtual void commit(const Actiongraph::Impl& actiongraph) const override;

	};

    }

}

#endif
