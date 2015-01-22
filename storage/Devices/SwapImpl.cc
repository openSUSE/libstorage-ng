

#include "storage/Devices/SwapImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


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

	actions.push_back(new Action::FormatSwap(get_sid()));

	if (!get_mountpoints().empty())
	{
	    actions.push_back(new Action::Mount(get_sid(), "swap"));
	    actions.push_back(new Action::AddFstab(get_sid(), "swap"));
	}

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


    namespace Action
    {

	void
	FormatSwap::commit(const Actiongraph& actiongraph) const
	{
	    const BlkDevice* blkdevice = get_blkdevice(actiongraph);

	    ostringstream cmd_line;

	    cmd_line << MKSWAPBIN " -f " << quote(blkdevice->get_name());

	    cout << cmd_line.str() << endl;

	    SystemCmd cmd(cmd_line.str());
	    if (cmd.retcode() != 0)
		throw runtime_error("format swap failed");
	}

    }

}
