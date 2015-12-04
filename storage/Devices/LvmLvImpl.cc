

#include "storage/Devices/LvmLvImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


using namespace std;


namespace storage
{

    const char* DeviceTraits<LvmLv>::classname = "LvmLv";


    LvmLv::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
    }


    void
    LvmLv::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);
    }


    bool
    LvmLv::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return true;
    }


    void
    LvmLv::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);
    }


    void
    LvmLv::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);
    }


    void
    LvmLv::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (get_name() != lhs.get_name())
	{
	    Action::Base* action = new Action::Rename(get_sid());
	    actiongraph.add_vertex(action);
	    action->first = action->last = true;
	}
    }


    Text
    LvmLv::Impl::do_create_text(bool doing) const
    {
	return sformat(_("Create logical volume %1$s (%2$s)"), get_displayname().c_str(),
		       get_size_string().c_str());
    }


    Text
    LvmLv::Impl::do_rename_text(const Impl& lhs, bool doing) const
    {
        return sformat(_("Rename %1$s to %2$s"), lhs.get_displayname().c_str(),
		       get_displayname().c_str());
    }


    void
    LvmLv::Impl::do_rename(const Impl& lhs) const
    {
    }


    namespace Action
    {

	Text
	Rename::text(const Actiongraph::Impl& actiongraph, bool doing) const
	{
	    const LvmLv* lhs_lvm_lv = to_lvm_lv(device_lhs(actiongraph));
	    const LvmLv* rhs_lvm_lv = to_lvm_lv(device_rhs(actiongraph));
	    return rhs_lvm_lv->get_impl().do_rename_text(lhs_lvm_lv->get_impl(), doing);
	}

	void
	Rename::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const LvmLv* lhs_lvm_lv = to_lvm_lv(device_lhs(actiongraph));
	    const LvmLv* rhs_lvm_lv = to_lvm_lv(device_rhs(actiongraph));
	    return rhs_lvm_lv->get_impl().do_rename(lhs_lvm_lv->get_impl());
	}

    }

}
