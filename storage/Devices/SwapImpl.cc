

#include "storage/Devices/SwapImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Swap::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node)
    {
    }


    void
    Swap::Impl::save(xmlNode* node) const
    {
	Filesystem::Impl::save(node);
    }


    void
    Swap::Impl::add_create_actions(Actiongraph& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Format(get_sid()));
	actions.push_back(new Action::Mount(get_sid(), "swap"));
	actions.push_back(new Action::AddFstab(get_sid(), "swap"));

	actiongraph.add_chain(actions);
    }


    bool
    Swap::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Filesystem::Impl::equal(rhs))
	    return false;

	return true;
    }


    void
    Swap::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Filesystem::Impl::log_diff(log, rhs);
    }


    void
    Swap::Impl::print(std::ostream& out) const
    {
	Filesystem::Impl::print(out);
    }

}
