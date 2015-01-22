

#include "storage/Devices/EncryptionImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Encryption::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
    }


    void
    Encryption::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);
    }


    void
    Encryption::Impl::add_create_actions(Actiongraph& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::FormatEncryption(get_sid()));
	actions.push_back(new Action::OpenEncryption(get_sid()));

	actiongraph.add_chain(actions);
    }


    bool
    Encryption::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return true;
    }


    void
    Encryption::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);
    }


    void
    Encryption::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);
    }


    namespace Action
    {

	Text
	FormatEncryption::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device = actiongraph.get_devicegraph(RHS)->find_device(sid);

	    return sformat(_("Create encryption on %1$s"), device->get_displayname().c_str());
	}


	Text
	OpenEncryption::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device = actiongraph.get_devicegraph(RHS)->find_device(sid);

	    return sformat(_("Open encryption on %1$s"), device->get_displayname().c_str());
	}

    }

}
